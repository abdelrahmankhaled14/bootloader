#ifndef STM32_FLASH_DRIVER_H
#define STM32_FLASH_DRIVER_H
#include "std_types.h"

/* Flash Control Register Base Address - STM32L4 */
#define FLASH_BASE_ADDR 0x40023C00

/* Flash Status Codes */
typedef enum {
    FLASH_OK = 0,
    FLASH_BUSY = 1,
    FLASH_ERROR = 2,
    FLASH_INVALID_SECTOR = 3,
    FLASH_INVALID_ADDRESS = 4,
    FLASH_LOCKED = 5,
    FLASH_TIMEOUT = 6
} flash_status_t;

/* Flash Control Registers */
typedef struct {
    volatile uint32_t ACR;      /* 0x00 Access Control Register */
    volatile uint32_t KEYR;     /* 0x04 Key Register */
    volatile uint32_t OPTKEYR;  /* 0x08 Option Key Register */
    volatile uint32_t SR;       /* 0x0C Status Register */
    volatile uint32_t CR;       /* 0x10 Control Register */
    volatile uint32_t OPTR;     /* 0x14 Option Register */
} flash_regs_t;

/* Flash Status Register Bits (at 0x0C) */
#define FLASH_SR_BSY     (1 << 16)   /* Busy */
#define FLASH_SR_EOP     (1 << 0)    /* End of Operation */
#define FLASH_SR_OPERR   (1 << 2)    /* Operation Error */
#define FLASH_SR_PROGERR (1 << 3)    /* Programming Error */
#define FLASH_SR_WRPERR  (1 << 4)    /* Write Protection Error */
#define FLASH_SR_PGAERR  (1 << 5)    /* Programming Alignment Error */
#define FLASH_SR_PGPERR  (1 << 6)    /* Programming Parity Error */
#define FLASH_SR_PGSERR  (1 << 7)    /* Programming Sequence Error */
#define FLASH_SR_RDERR   (1 << 9)    /* PCROP Read Error */

/* Flash Control Register Bits (at 0x10) */
#define FLASH_CR_PG           (1 << 0)    /* Programming */
#define FLASH_CR_SER          (1 << 1)    /* Sector Erase */
#define FLASH_CR_MER          (1 << 2)    /* Mass Erase */
#define FLASH_CR_SNB          (0xF << 3)  /* Sector Number */
#define FLASH_CR_PSIZE        (3 << 8)    /* Program Size */
#define FLASH_CR_PSIZE_BYTE   (0 << 8)    /* Byte programming */
#define FLASH_CR_PSIZE_HALF   (1 << 8)    /* Half-word programming */
#define FLASH_CR_PSIZE_WORD   (2 << 8)    /* Word programming */
#define FLASH_CR_PSIZE_DWORD  (3 << 8)    /* Double-word programming */
#define FLASH_CR_STRT         (1 << 16)   /* Start */
#define FLASH_CR_ERRIE        (1 << 24)   /* Error Interrupt Enable */
#define FLASH_CR_EOPIE        (1 << 25)   /* End of Program Interrupt Enable */
#define FLASH_CR_LOCK         (1 << 31)   /* Lock */

/* Flash Key Values */
#define FLASH_KEY1 0x45670123
#define FLASH_KEY2 0xCDEF89AB

/* Timeout in milliseconds */
#define FLASH_TIMEOUT_MS 1000

/* Address validation macros */
#define FLASH_MEM_START 0x08000000
#define FLASH_MEM_END   0x0803FFFF

/**
 * @brief Read from flash memory
 * @param address Start address
 * @param data Pointer to buffer to store read data
 * @param size Number of bytes to read
 * @return flash_status_t status
 */
flash_status_t flash_read(uint32_t address, uint8_t *data, uint32_t size);

/**
 * @brief Unlock the flash memory
 * @return flash_status_t status
 */
flash_status_t flash_unlock(void);

/**
 * @brief Lock the flash memory
 * @return flash_status_t status
 */
flash_status_t flash_lock(void);

/**
 * @brief Check if flash is busy
 * @return true if busy, false otherwise
 */
bool_t flash_check_busy(void);

/**
 * @brief Get operation status
 * @return flash_status_t current status
 */
flash_status_t flash_get_status(void);

/**
 * @brief Poll for operation completion
 * @return flash_status_t status
 */
flash_status_t flash_poll_busy(void);

/**
 * @brief Perform mass erase
 * @return flash_status_t status
 */
flash_status_t flash_mass_erase(void);

/**
 * @brief Erase a sector
 * @param sector_num Sector number (0-255)
 * @return flash_status_t status
 */
flash_status_t flash_sector_erase(uint32_t sector_num);

/**
 * @brief Program flash memory
 * @param address Start address
 * @param data Pointer to data
 * @param size Number of bytes to program
 * @return flash_status_t status
 */
flash_status_t flash_program(uint32_t address, const uint8_t *data, uint32_t size);

/**
 * @brief Initialize flash driver
 * @return flash_status_t status
 */
flash_status_t flash_init(void);

#endif /* STM32_FLASH_DRIVER_H */