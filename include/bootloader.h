#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "std_types.h"

/* Application start address - define based on your flash layout */
#define BOOTLOADER_APP_START_ADDRESS 0x08010000

typedef enum
{
    BOOTLOADER_OK = 0,
    BOOTLOADER_ERROR,
    BOOTLOADER_INVALID_ADDRESS,
    BOOTLOADER_WRITE_ERROR,
    BOOTLOADER_ERASE_ERROR,
} Bootloader_Status_t;

typedef enum
{
    BOOTLOADER_CMD_READ_MEMORY = 0x00,
    BOOTLOADER_CMD_WRITE_MEMORY = 0x01,
    BOOTLOADER_CMD_ERASE_MEMORY = 0x02,
    BOOTLOADER_CMD_GET_VERSION = 0x03,
    BOOTLOADER_CMD_ACK = 0x04,
    BOOTLOADER_CMD_DONE = 0x05
} Bootloader_Command_t;

typedef enum
{
    BOOTLOADER_IDLE,
    BOOTLOADER_START_OF_FRAME,
    BOOTLOADER_CMD,
    BOOTLOADER_DATA_LENGTH,
    BOOTLOADER_DATA_ADDRESS,
    BOOTLOADER_DATA,
    BOOTLOADER_SECTOR_ERASE
} Bootloader_Sequence_t;

typedef struct 
{
    uint8_t start_of_frame;
    uint8_t command;
    uint16_t data_length;
    uint32_t data_address;
    uint8_t sector_number;  /* For erase operations */
    uint8_t data[256];      /* Reduced from 65535 to reasonable size */
} bootloader_t;

/* Main initialization function */
Bootloader_Status_t Bootloader_Init(void);

/* Jump to application - call this after receiving DONE command */
void bootloader_jump_to_application(void);

/* Callback functions */
void rx_callback_function(uint8_t *rx_data, uint16_t length);
void tx_callback_function(void);

/* Helper validation functions */
uint8_t is_valid_command(uint8_t command);
uint8_t is_valid_address(uint32_t address);

/* Command processing functions */
void process_bootloader_command(uint8_t command);
void execute_bootloader_command(void);

/* Command implementation functions */
Bootloader_Status_t bootloader_read_memory(void);
Bootloader_Status_t bootloader_write_memory(void);
Bootloader_Status_t bootloader_erase_memory(void);
Bootloader_Status_t bootloader_get_version(void);
Bootloader_Status_t bootloader_done(void);

/* Communication functions */
void send_ack(void);
void send_nack(void);

#endif /* BOOTLOADER_H */