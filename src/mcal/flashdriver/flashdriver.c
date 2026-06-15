#include "mcal/flashdriver/flashdriver.h"

static flash_regs_t *flash_regs = (flash_regs_t *)FLASH_BASE_ADDR;
static uint32_t flash_timeout;

/* Verify flash controller is accessible */
static bool_t flash_controller_valid(void) {
    /* Check if pointer is in valid memory range */
    if ((uint32_t)flash_regs < 0x40000000 || (uint32_t)flash_regs > 0x50000000) {
        return FALSE;
    }
    return TRUE;
}

/**
 * @brief Validate address is within flash memory range
 * @param address Address to validate
 * @return true if valid, false otherwise
 */
static bool_t flash_address_valid(uint32_t address) {
    return (address >= FLASH_MEM_START && address < FLASH_MEM_END);
}

/**
 * @brief Check flash busy status
 * @return true if busy, false otherwise
 */
bool_t flash_check_busy(void) {
    return (flash_regs->SR & FLASH_SR_BSY) ? TRUE : FALSE;
}

/**
 * @brief Get current flash status
 * @return flash_status_t status code
 */
flash_status_t flash_get_status(void) {
    uint32_t status = flash_regs->SR;
    
    if (status & FLASH_SR_BSY) {
        return FLASH_BUSY;
    }
    
    if (status & FLASH_SR_OPERR) {
        return FLASH_ERROR;
    }
    
    if (status & FLASH_SR_PROGERR) {
        return FLASH_ERROR;
    }
    
    if (status & FLASH_SR_WRPERR) {
        return FLASH_LOCKED;
    }
    
    if (status & FLASH_SR_EOP) {
        return FLASH_OK;
    }
    
    return FLASH_OK;
}

/**
 * @brief Poll until flash operation completes
 * @return flash_status_t final status
 */
flash_status_t flash_poll_busy(void) {
    flash_timeout = FLASH_TIMEOUT_MS;
    
    while (flash_check_busy()) {
        if (--flash_timeout == 0) {
            return FLASH_TIMEOUT;
        }
    }
    
    return flash_get_status();
}

/**
 * @brief Unlock flash memory for write/erase operations
 * @return flash_status_t status
 */
flash_status_t flash_unlock(void) {
    /* Validate controller */
    if (!flash_controller_valid()) {
        return FLASH_ERROR;
    }
    
    /* Check if already unlocked */
    if (!(flash_regs->CR & FLASH_CR_LOCK)) {
        return FLASH_OK;
    }
    
    /* Write unlock keys in sequence */
    flash_regs->KEYR = FLASH_KEY1;
    flash_regs->KEYR = FLASH_KEY2;
    
    /* Verify unlock was successful */
    if (flash_regs->CR & FLASH_CR_LOCK) {
        return FLASH_ERROR;
    }
    
    return FLASH_OK;
}

/**
 * @brief Lock flash memory
 * @return flash_status_t status
 */
flash_status_t flash_lock(void) {
    flash_regs->CR |= FLASH_CR_LOCK;
    
    /* Verify lock was successful */
    if (!(flash_regs->CR & FLASH_CR_LOCK)) {
        return FLASH_ERROR;
    }
    
    return FLASH_OK;
}

/**
 * @brief Read from flash memory
 * @param address Start address (must be within valid flash range)
 * @param data Pointer to buffer to store read data
 * @param size Number of bytes to read
 * @return flash_status_t status
 */
flash_status_t flash_read(uint32_t address, uint8_t *data, uint32_t size) {
    uint32_t i;
    volatile uint8_t *flash_addr;
    
    /* Validate inputs */
    if (data == NULL || size == 0) {
        return FLASH_ERROR;
    }
    
    /* Validate start address */
    if (!flash_address_valid(address)) {
        return FLASH_INVALID_ADDRESS;
    }
    
    /* Validate end address */
    if (!flash_address_valid(address + size - 1)) {
        return FLASH_INVALID_ADDRESS;
    }
    
    /* Flash read does not require unlock/lock */
    /* Simply copy data from flash memory to buffer */
    flash_addr = (volatile uint8_t *)address;
    
    for (i = 0; i < size; i++) 
    {
        data[i] = flash_addr[i];
    }
    
    return FLASH_OK;
}

/**
 * @brief Perform complete mass erase (Bank 1)
 * @return flash_status_t final status
 */
__attribute__((section(".ram_func")))
flash_status_t flash_mass_erase(void) {
    flash_status_t status;
    
    /* Unlock flash */
    status = flash_unlock();
    if (status != FLASH_OK) {
        return status;
    }
    
    /* Check if busy */
    if (flash_check_busy()) {
        flash_lock();
        return FLASH_BUSY;
    }
    
    /* Clear error flags */
    flash_regs->SR |= 0xFFFF;
    
    /* Set MER (Mass Erase) bit */
    flash_regs->CR |= FLASH_CR_MER;
    
    /* Set START bit */
    flash_regs->CR |= FLASH_CR_STRT;
    
    /* Poll until complete */
    status = flash_poll_busy();
    
    /* Clear error flags */
    flash_regs->SR |= 0xFFFF;
    
    /* Clear MER (Mass Erase) bit */
    flash_regs->CR &= ~FLASH_CR_MER;
    
    /* Lock flash */
    flash_lock();
    
    return status;
}

/**
 * @brief Erase a single sector
 * @param sector_num Sector number (0-5 for STM32L4)
 * @return flash_status_t status
 */
flash_status_t flash_sector_erase(uint32_t sector_num) {
    flash_status_t status;
    
    /* Validate sector number */
    if (sector_num > 5) {
        return FLASH_INVALID_SECTOR;
    }
    
    /* Check if busy */
    if (flash_check_busy()) {
        return FLASH_BUSY;
    }
    
    /* Unlock flash */
    status = flash_unlock();
    if (status != FLASH_OK) {
        return status;
    }
    
    /* Clear error flags */
    flash_regs->SR |= 0xFFFF;
    
    /* Set SER (Sector Erase) bit and sector number */
    flash_regs->CR |= FLASH_CR_SER;
    flash_regs->CR |= (sector_num << 3) & FLASH_CR_SNB;
    
    /* Set START bit */
    flash_regs->CR |= FLASH_CR_STRT;
    
    /* Poll until complete */
    status = flash_poll_busy();
    
    /* Clear SER and SNB bits */
    flash_regs->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB);
    
    /* Lock flash */
    flash_lock();
    
    return status;
}

/**
 * @brief Program flash memory with data
 * @param address Start address (must be byte-aligned)
 * @param data Pointer to data
 * @param size Number of bytes to program
 * @return flash_status_t status
 */
__attribute__((section(".ram_func")))
flash_status_t flash_program(uint32_t address, const uint8_t *data, uint32_t size) {
    flash_status_t status;
    uint32_t i;
    volatile uint8_t *flash_addr;
    
    if (data == NULL || size == 0) {
        return FLASH_ERROR;
    }
    
    /* Validate address */
    if (!flash_address_valid(address)) {
        return FLASH_INVALID_ADDRESS;
    }
    
    /* Validate end address */
    if (!flash_address_valid(address + size - 1)) {
        return FLASH_INVALID_ADDRESS;
    }
    
    /* Unlock flash */
    status = flash_unlock();
    if (status != FLASH_OK) {
        return status;
    }
    
    /* Check if busy */
    if (flash_check_busy()) {
        flash_lock();
        return FLASH_BUSY;
    }
    
    /* Clear error flags */
    flash_regs->SR |= 0xFFFF;
    
    /* Set programming bit and program size (byte programming) */
    flash_regs->CR |= FLASH_CR_PG;
    flash_regs->CR |= FLASH_CR_PSIZE_BYTE;
    
    /* Program each byte */
    flash_addr = (volatile uint8_t *)address;
    for (i = 0; i < size; i++) {
        /* Write data to flash */
        *flash_addr = data[i];
        
        /* Poll until write complete */
        status = flash_poll_busy();
        if (status != FLASH_OK) {
            flash_regs->CR &= ~FLASH_CR_PG;
            flash_lock();
            return status;
        }
        
        /* Move to next address */
        flash_addr++;
    }
    
    /* Clear programming bit */
    flash_regs->CR &= ~FLASH_CR_PG;
    
    /* Lock flash */
    flash_lock();
    
    return FLASH_OK;
}

/**
 * @brief Initialize flash driver
 * @return flash_status_t status
 */
flash_status_t flash_init(void) {
    flash_status_t status;
    
    /* Unlock flash for operations */
    status = flash_unlock();
    if (status != FLASH_OK) {
        return status;
    }
    
    /* Clear any pending status flags */
    flash_regs->SR |= 0xFFFF;
    
    /* Lock flash initially */
    status = flash_lock();
    
    return status;
}

