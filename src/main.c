

// // #include "mcal/rcc/rcc.h"
// // #include "hal/led/led.h"

// // #include "hal/hserial/hserial.h"

// // uint8_t buffer[1024];
// // void rx_callback_function(uint8_t *rx_data, uint16_t length)
// // {
// //     for(int i=0;i<length;i++)
// //     {
// //         // Process received data
// //         buffer[i] = rx_data[i];
// //     }
// //     LED_Toggle(1);
// // }

// // void tx_callback_function(void)
// // {
// //     // Handle TX complete event
// //     LED_Toggle(2);
// // }
// // extern uint8_t buffer [1024];

// // HSerial_Interface_t serial_interface;

// // extern LED_Configuration_t LED_Config[LED_MAX];
// // int main(void)
// // {
// //     rcc_clk_perpheral_enable(APB2_BUS, GPIOA_EN);
// //     LED_Init(LED_Config);
// //     HSerial_Init(HSERIAL_UART_1, &serial_interface, rx_callback_function, tx_callback_function);
// //     serial_interface.TransmitBuffer(HSERIAL_UART_1, (uint8_t *)"Hello, salah! I love you so much", 32);
// //     serial_interface.ReceiveBuffer(HSERIAL_UART_1, (uint8_t *)buffer, 4);
// //     while (1)
// //     {

// //     }
// // }

// #include "mcal/flashdriver/flashdriver.h"

// /* Define flash test addresses */
// #define TEST_FLASH_ADDR 0x08010000
// #define TEST_SECTOR_NUM 4
// #define TEST_DATA_SIZE 10

// /* Test data to program (8-bit bytes for STM32L4) */
// uint8_t test_data[TEST_DATA_SIZE] = {
//     0x12, 0x34, 0x56, 0x78, 0x9A,
//     0xBC, 0xDE, 0xF0, 0xAA, 0xBB};

// /* Buffer to verify read data */
// uint8_t read_buffer[TEST_DATA_SIZE];

// /**
//  * @brief Verify data written to flash
//  */
// bool_t verify_flash_data(uint32_t addr, const uint8_t *expected, uint32_t size)
// {
//     uint32_t i;
//     volatile uint8_t *flash_ptr = (volatile uint8_t *)addr;

//     for (i = 0; i < size; i++)
//     {
//         read_buffer[i] = flash_ptr[i];
//         if (flash_ptr[i] != expected[i])
//         {
//             return FALSE;
//         }
//     }

//     return TRUE;
// }

// /**
//  * @brief Main test routine
//  */
// int main(void)
// {
//     flash_status_t status;

//     /* Initialize flash driver */
//     status = flash_init();
//     if (status != FLASH_OK)
//     {
//         return 1;
//     }

//     /* Erase target sector (sector number only, no address) */
//     status = flash_sector_erase(TEST_SECTOR_NUM);
//     if (status != FLASH_OK)
//     {
//         return 1;
//     }

//     /* Program flash with test data */
//     status = flash_program(TEST_FLASH_ADDR, test_data, TEST_DATA_SIZE);
//     if (status != FLASH_OK)
//     {
//         return 1;
//     }

//     /* Verify programmed data */
//     if (!verify_flash_data(TEST_FLASH_ADDR, test_data, TEST_DATA_SIZE))
//     {
//         return 1;
//     }

//     /* Test lock/unlock */
//     status = flash_unlock();
//     if (status != FLASH_OK)
//     {
//         return 2;
//     }

//     status = flash_lock();
//     if (status != FLASH_OK)
//     {
//         return 3;
//     }
//     read_buffer[1] = 0;
//     /* Mass erase test (uncomment to test) */
    
//     status = flash_mass_erase();
//     if (status != FLASH_OK) {
//         return 4;
//     }
    
//     while (1)
//     {
//         uint8_t *x = (uint8_t *)0x08010000;
//     }

//     return 0;
// }

#include "bootloader.h"

/* Simple main - bootloader handles everything via interrupts */
int main(void)
{
    uint32_t *marker = (uint32_t *)0x0800C000;
    if (*marker == 0x0000DEADU)
    {
        bootloader_jump_to_application();
    }
    Bootloader_Status_t status;

    /* Initialize bootloader - handles UART, Flash, and State Machine */
    status = Bootloader_Init();

    if (status != BOOTLOADER_OK)
    {
        while (1)
            ;
    }

    /* Main loop - bootloader runs in interrupt callbacks */
    while (1)
    {

        /* Do nothing - bootloader handles everything */
        /* UART interrupts will call rx_callback_function() */
    }

    return 0;
}