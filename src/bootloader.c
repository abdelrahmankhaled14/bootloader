#include "bootloader.h"
#include "mcal/flashdriver/flashdriver.h"
#include "hal/hserial/hserial.h"

HSerial_Interface_t serial_interface;
Bootloader_Sequence_t bootloader_sequence = BOOTLOADER_IDLE;
bootloader_t bootloader_data;
uint8_t buffer[256];

/* Protocol constants */
#define BOOTLOADER_START_OF_FRAME_VALUE 0x55
#define BOOTLOADER_ACK_VALUE 0xAA
#define BOOTLOADER_NACK_VALUE 0xFF
#define BOOTLOADER_MAX_DATA_SIZE 256

/* Commands */
#define BOOTLOADER_CMD_READ_MEMORY 0x00
#define BOOTLOADER_CMD_WRITE_MEMORY 0x01
#define BOOTLOADER_CMD_ERASE_MEMORY 0x02
#define BOOTLOADER_CMD_GET_VERSION 0x03
#define BOOTLOADER_CMD_DONE 0x04

/* RCC Register definitions */
#define RCC_BASE 0x40023800
#define RCC_CSR (*(volatile unsigned int *)(RCC_BASE + 0x74))

/* NVIC Register definitions */
#define NVIC_BASE 0xE000E000
#define AIRCR (*(volatile unsigned int *)(NVIC_BASE + 0xD0C))

/* RCC_CSR bits */
#define RCC_CSR_SFTRSTF (1 << 24) /* Software reset flag */
#define RCC_CSR_WDGRSTF (1 << 29) /* Watchdog reset flag */
#define RCC_CSR_PINRSTF (1 << 26) /* PIN reset flag */
#define RCC_CSR_BORRSTF (1 << 25) /* Brown-out reset flag */

/* AIRCR bits */
#define AIRCR_VECTKEY 0x05FA0000   /* Vector Key (required for write) */
#define AIRCR_SYSRESETREQ (1 << 2) /* System reset request */

/* Expected buffer sizes */
#define EXPECTED_SOF_SIZE 1
#define EXPECTED_CMD_SIZE 1
#define EXPECTED_LENGTH_SIZE 1
#define EXPECTED_ADDRESS_SIZE 4

/* State definitions */
#define BOOTLOADER_IDLE 0
#define BOOTLOADER_START_OF_FRAME 1
#define BOOTLOADER_DATA_LENGTH 2
#define BOOTLOADER_DATA_ADDRESS 3
#define BOOTLOADER_ERASE_SECTOR 4
#define BOOTLOADER_DATA 5

/* Flash marker definitions */
#define MARKER_ADDRESS 0x0800C000
#define MARKER_VALUE 0xDEAD
#define SECTOR_NUMBER 2

/* ===================================
   PROTOCOL FLOW:
   1. Host sends SOF -> MCU ACKs
   2. Host sends CMD -> MCU ACKs
   3. Host sends parameters (LENGTH, ADDRESS, DATA)
   4. MCU executes command (may take time for flash ops)
   5. MCU sends final ACK when done, then NACK if error

   For READ: MCU sends data without ACK before, then ACK after
   For GET_VERSION: MCU sends version byte only
   For DONE: MCU writes marker then triggers system reset
            After reset, bootloader detects marker and jumps to app
=================================== */

/**
 * Write marker to flash indicating valid application exists
 */
void write_marker(void)
{
    flash_status_t status;
    uint8_t marker_bytes[4];

    /* Convert 32-bit marker to byte array (little endian) */
    marker_bytes[0] = (MARKER_VALUE >> 0) & 0xFF;
    marker_bytes[1] = (MARKER_VALUE >> 8) & 0xFF;
    marker_bytes[2] = 0x00;
    marker_bytes[3] = 0x00;

    /* Erase the sector first */
    status = flash_sector_erase(SECTOR_NUMBER);
    if (status != FLASH_OK) {
        while (1);
    }

    /* Program the marker */
    status = flash_program(MARKER_ADDRESS, marker_bytes, sizeof(marker_bytes));
    if (status != FLASH_OK) {
        while (1);
    }
}

/**
 * System Reset via NVIC AIRCR register
 * This resets EVERYTHING:
 * - All peripherals (UART, SPI, I2C, etc.)
 * - All registers
 * - Clock system
 * - All interrupts
 * - MCU restarts from reset vector (0x08000000)
 */
void system_reset(void)
{
    /* Disable all interrupts */
    __asm volatile("cpsid i");

    /* Trigger system reset using AIRCR register */
    /* Write VECTKEY (0x05FA) + SYSRESETREQ bit to AIRCR */
    AIRCR = AIRCR_VECTKEY | AIRCR_SYSRESETREQ;

    /* Wait for reset to take effect */
    while (1);
}

/**
 * Jump to application based on marker in flash
 * Called from main() after reset
 */
void bootloader_jump_to_application(void)
{
    /* Application's vector table base address */
    uint32_t *vector_table = (uint32_t *)0x08020000;
    uint32_t reset_handler_address;
    uint32_t app_stack_pointer;

    /* Read app stack pointer from vector table[0] */
    app_stack_pointer = vector_table[0];

    /* Read Reset_Handler address from vector table[1] */
    reset_handler_address = vector_table[1];

    /* Validate the addresses are not erased flash (0xFFFFFFFF) */
    if (reset_handler_address == 0xFFFFFFFF || app_stack_pointer == 0xFFFFFFFF) {
        /* Invalid app - return to bootloader */
        return;
    }

    /* Disable all interrupts before context switch */
    __asm volatile("cpsid i");

    /* Set main stack pointer to app's stack */
    __asm volatile("msr msp, %0" : : "r"(app_stack_pointer) :);

    /* Jump to Reset_Handler using BX (Branch Exchange) instruction */
    __asm volatile("bx %0" : : "r"(reset_handler_address) :);

    /* Should never reach here */
    while (1);
}

void rx_callback_function(uint8_t *rx_data, uint16_t length)
{
    static uint16_t data_index = 0;
    uint16_t i;

    if (rx_data == NULL || length == 0)
        return;

    for (i = 0; i < length; i++) {
        uint8_t byte = rx_data[i];

        switch (bootloader_sequence) {
        
        /* ============================================
           STATE 1: IDLE - Wait for SOF
           ============================================ */
        case BOOTLOADER_IDLE:
            if (byte == BOOTLOADER_START_OF_FRAME_VALUE) {
                bootloader_data.start_of_frame = byte;
                send_ack();
                bootloader_sequence = BOOTLOADER_START_OF_FRAME;
                data_index = 0;

                serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_CMD_SIZE);
            }
            break;

        /* ============================================
           STATE 2: START_OF_FRAME - Receive command
           ============================================ */
        case BOOTLOADER_START_OF_FRAME:
            bootloader_data.command = byte;

            if (!is_valid_command(bootloader_data.command)) {
                send_nack();
                bootloader_sequence = BOOTLOADER_IDLE;
                data_index = 0;
                serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);
                break;
            }

            send_ack();

            switch (bootloader_data.command) {
            case BOOTLOADER_CMD_READ_MEMORY:
            case BOOTLOADER_CMD_WRITE_MEMORY:
                bootloader_sequence = BOOTLOADER_DATA_LENGTH;
                data_index = 0;
                bootloader_data.data_length = 0;
                bootloader_data.data_address = 0;

                serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_LENGTH_SIZE);
                break;

            case BOOTLOADER_CMD_ERASE_MEMORY:
                bootloader_sequence = BOOTLOADER_ERASE_SECTOR;
                data_index = 0;

                serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, 1);
                break;

            case BOOTLOADER_CMD_GET_VERSION:
                /* Execute version immediately (sends version byte) */
                execute_bootloader_command();

                bootloader_sequence = BOOTLOADER_IDLE;
                data_index = 0;
                serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);
                break;

            case BOOTLOADER_CMD_DONE:
                /* Write marker and trigger system reset */
                /* After reset, bootloader will detect marker and jump to app */
                execute_bootloader_command();
                /* Never returns - system resets */
                break;

            default:
                send_nack();
                bootloader_sequence = BOOTLOADER_IDLE;
                serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);
                break;
            }
            break;

        /* ============================================
           STATE 3B: ERASE_SECTOR
           ============================================ */
        case BOOTLOADER_ERASE_SECTOR:
            bootloader_data.data_length = byte;

            /* Execute erase */
            execute_bootloader_command();

            bootloader_sequence = BOOTLOADER_IDLE;
            data_index = 0;
            serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);
            break;

        /* ============================================
           STATE 3: DATA_LENGTH
           ============================================ */
        case BOOTLOADER_DATA_LENGTH:
            bootloader_data.data_length = byte;

            if (bootloader_data.data_length == 0 ||
                bootloader_data.data_length > BOOTLOADER_MAX_DATA_SIZE) {
                send_nack();
                bootloader_sequence = BOOTLOADER_IDLE;
                data_index = 0;
                serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);
                break;
            }

            send_ack();

            bootloader_sequence = BOOTLOADER_DATA_ADDRESS;
            data_index = 0;
            bootloader_data.data_address = 0;

            serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_ADDRESS_SIZE);
            break;

        /* ============================================
           STATE 4: DATA_ADDRESS - Accumulate 4 bytes
           ============================================ */
        case BOOTLOADER_DATA_ADDRESS:
            bootloader_data.data_address <<= 8;
            bootloader_data.data_address |= byte;
            data_index++;

            if (data_index == 4) {
                data_index = 0;

                if (!is_valid_address(bootloader_data.data_address)) {
                    send_nack();
                    bootloader_sequence = BOOTLOADER_IDLE;
                    serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);
                } else {
                    send_ack();

                    bootloader_sequence = BOOTLOADER_DATA;

                    serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer,
                                                   bootloader_data.data_length);
                }
            }
            break;

        /* ============================================
           STATE 5: DATA - Accumulate N bytes
           ============================================ */
        case BOOTLOADER_DATA:
            if (data_index < bootloader_data.data_length) {
                bootloader_data.data[data_index] = byte;
                data_index++;
            }

            if (data_index == bootloader_data.data_length) {
                data_index = 0;
                send_ack();

                /* Execute the command NOW */
                execute_bootloader_command();

                /* MCU sends ACK/NACK from execute function */

                bootloader_sequence = BOOTLOADER_IDLE;

                serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);
            }
            break;

        default:
            bootloader_sequence = BOOTLOADER_IDLE;
            serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);
            break;
        }
    }
}

void tx_callback_function(void)
{
}

/* ===================================
   Helper Functions
=================================== */

uint8_t is_valid_command(uint8_t command)
{
    switch (command) {
    case BOOTLOADER_CMD_READ_MEMORY:
    case BOOTLOADER_CMD_WRITE_MEMORY:
    case BOOTLOADER_CMD_ERASE_MEMORY:
    case BOOTLOADER_CMD_GET_VERSION:
    case BOOTLOADER_CMD_DONE:
        return 1;
    default:
        return 0;
    }
}

uint8_t is_valid_address(uint32_t address)
{
#define BOOTLOADER_MIN_ADDRESS 0x08001000 /* App starts at 0x08001000 */
#define BOOTLOADER_MAX_ADDRESS 0x0803FFFF

    return (address >= BOOTLOADER_MIN_ADDRESS &&
            address < BOOTLOADER_MAX_ADDRESS);
}

void send_ack(void)
{
    uint8_t ack = BOOTLOADER_ACK_VALUE;
    serial_interface.TransmitBuffer(HSERIAL_UART_1, &ack, 1);
}

void send_nack(void)
{
    uint8_t nack = BOOTLOADER_NACK_VALUE;
    serial_interface.TransmitBuffer(HSERIAL_UART_1, &nack, 1);
}

/* ===================================
   Command Execution
=================================== */

void execute_bootloader_command(void)
{
    Bootloader_Status_t status = BOOTLOADER_OK;

    switch (bootloader_data.command) {
    case BOOTLOADER_CMD_READ_MEMORY:
        status = bootloader_read_memory();
        /* No ACK/NACK sent - data is sent directly */
        break;

    case BOOTLOADER_CMD_WRITE_MEMORY:
        status = bootloader_write_memory();
        /* Send ACK/NACK AFTER write completes */
        if (status == BOOTLOADER_OK)
            send_ack();
        else
            send_nack();
        break;

    case BOOTLOADER_CMD_ERASE_MEMORY:
        status = bootloader_erase_memory();
        /* Send ACK/NACK AFTER erase completes */
        if (status == BOOTLOADER_OK)
            send_ack();
        else
            send_nack();
        break;

    case BOOTLOADER_CMD_GET_VERSION:
        /* Sends version byte, no ACK */
        bootloader_get_version();
        break;

    case BOOTLOADER_CMD_DONE:
        /* 
         * WORKFLOW:
         * 1. Write marker (0xDEAD) to flash
         * 2. Send ACK to host
         * 3. Trigger system reset
         * 4. After reset, main() detects marker
         * 5. Bootloader jumps to app
         * 6. All peripherals reset, fresh start
         */
        send_ack();
        write_marker();
        system_reset();
        /* Never returns */
        break;

    default:
        send_nack();
        break;
    }
}

/* ===================================
   Command Implementations
=================================== */

Bootloader_Status_t bootloader_read_memory(void)
{
    flash_status_t flash_status;

    flash_status = flash_read(
        bootloader_data.data_address,
        bootloader_data.data,
        bootloader_data.data_length);

    if (flash_status != FLASH_OK) {
        return BOOTLOADER_ERROR;
    }

    /* Send data back to host */
    serial_interface.TransmitBuffer(HSERIAL_UART_1,
                                    bootloader_data.data,
                                    bootloader_data.data_length);

    return BOOTLOADER_OK;
}

Bootloader_Status_t bootloader_write_memory(void)
{
    flash_status_t flash_status;

    flash_status = flash_program(
        bootloader_data.data_address,
        bootloader_data.data,
        bootloader_data.data_length);

    if (flash_status != FLASH_OK) {
        return BOOTLOADER_WRITE_ERROR;
    }

    return BOOTLOADER_OK;
}

Bootloader_Status_t bootloader_erase_memory(void)
{
    flash_status_t flash_status;

    flash_status = flash_sector_erase(bootloader_data.data_length);

    if (flash_status != FLASH_OK) {
        return BOOTLOADER_ERASE_ERROR;
    }

    return BOOTLOADER_OK;
}

Bootloader_Status_t bootloader_get_version(void)
{
#define BOOTLOADER_VERSION 0x02

    uint8_t version = BOOTLOADER_VERSION;

    serial_interface.TransmitBuffer(HSERIAL_UART_1, &version, 1);

    return BOOTLOADER_OK;
}

/**
 * Check reset source - for debugging/diagnostics
 */
void check_reset_source(void)
{
    volatile unsigned int reset_flags = RCC_CSR;

    /* Check which reset occurred */
    if (reset_flags & RCC_CSR_SFTRSTF) {
        /* Software reset - bootloader triggered it via AIRCR */
        /* This happens after BOOTLOADER_CMD_DONE is executed */
    }
    else if (reset_flags & RCC_CSR_PINRSTF) {
        /* External pin reset - user pressed reset button */
    }
    else if (reset_flags & RCC_CSR_BORRSTF) {
        /* Brown-out reset - power supply issue */
    }
    else if (reset_flags & RCC_CSR_WDGRSTF) {
        /* Watchdog reset - timeout occurred */
    }

    /* Clear the reset flags by writing 1 to RMVF bit */
    RCC_CSR |= (1 << 24); /* RMVF - Remove reset flags */
}

/* ===================================
   Initialization
=================================== */

Bootloader_Status_t Bootloader_Init(void)
{
    flash_status_t flash_status;

    flash_status = flash_init();
    if (flash_status != FLASH_OK) {
        return BOOTLOADER_ERROR;
    }

    HSerial_Init(HSERIAL_UART_1, &serial_interface,
                 rx_callback_function, tx_callback_function);

    serial_interface.ReceiveBuffer(HSERIAL_UART_1, buffer, EXPECTED_SOF_SIZE);

    bootloader_sequence = BOOTLOADER_IDLE;

    return BOOTLOADER_OK;
}
