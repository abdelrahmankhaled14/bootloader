/****************************************************************************
 * @file hserial.h
 * @brief Hardware Serial Layer Interface - UART Only
 * @author Generated
 * @version 2.1
 * @details This layer provides a high-level interface for UART communication
 *          supporting polling and interrupt-driven modes.
 ****************************************************************************/

#ifndef HSERIAL_H
#define HSERIAL_H

#include "std_types.h"
#include "mcal/uart/uart.h"
#include "hal/hserial/hserial_cfg.h"

/* ==========================================================================
 * TYPE DEFINITIONS
 * ========================================================================== */

/**
 * @brief Supported peripheral types
 * @note  SPI and DMA intentionally removed
 */
typedef enum
{
    HSERIAL_PERIPH_UART = 0
} HSerial_PeripheralType_t;

/**
 * @brief UART operating modes
 */
typedef enum
{
    HSERIAL_MODE_POLLING = 0,   /**< Blocking polling mode */
    HSERIAL_MODE_INTERRUPT      /**< Interrupt-driven mode */
} HSerial_Mode_t;

/**
 * @brief API return status
 */
typedef enum
{
    HSERIAL_OK = 0,
    HSERIAL_ERROR,
    HSERIAL_BUSY
} HSerial_Status_t;

/* ==========================================================================
 * CALLBACK TYPES
 * ========================================================================== */

typedef void (*rxCallback_t)(uint8_t *rx_data, uint16_t length);
typedef void (*txCallback_t)(void);

/* ==========================================================================
 * DEVICE CONFIGURATION
 * ========================================================================== */

/**
 * @brief UART-only device configuration
 */
typedef struct
{
    Uart_Config_t uart;
} device_config_t;

/**
 * @brief HSerial configuration structure
 */
typedef struct
{
    HSerial_Channel_t channel;             /**< UART channel */
    HSerial_PeripheralType_t periph_type;  /**< Always UART */
    HSerial_Mode_t mode;                   /**< Polling or Interrupt */

    rxCallback_t rx_callback;              /**< RX complete callback */
    txCallback_t tx_callback;              /**< TX complete callback */

    device_config_t device_config;         /**< UART configuration */
} HSerial_Config_t;

/* ==========================================================================
 * INTERFACE FUNCTION POINTERS
 * ========================================================================== */

typedef void (*HSerial_TransmitBuffer_t)(
    HSerial_Channel_t channel,
    const uint8_t *buffer,
    uint16_t length
);

typedef void (*HSerial_ReceiveBuffer_t)(
    HSerial_Channel_t channel,
    uint8_t *buffer,
    uint16_t length
);

typedef void (*HSerial_SendByte_t)(
    HSerial_Channel_t channel,
    uint8_t byte
);

typedef void (*HSerial_ReceiveByte_t)(
    HSerial_Channel_t channel,
    uint8_t *byte
);

/**
 * @brief UART interface abstraction
 */
typedef struct
{
    HSerial_TransmitBuffer_t TransmitBuffer;
    HSerial_ReceiveBuffer_t  ReceiveBuffer;
    HSerial_SendByte_t       SendByte;
    HSerial_ReceiveByte_t    ReceiveByte;
} HSerial_Interface_t;

/* ==========================================================================
 * FUNCTION PROTOTYPES
 * ========================================================================== */

/**
 * @brief Initialize UART serial channel
 *
 * @param channel   UART channel
 * @param interface Pointer to interface structure
 * @param rx        RX complete callback (interrupt mode)
 * @param tx        TX complete callback (interrupt mode)
 *
 * @return HSERIAL_OK or HSERIAL_ERROR
 */
HSerial_Status_t HSerial_Init(HSerial_Channel_t channel,
                             HSerial_Interface_t *interface,
                             rxCallback_t rx,
                             txCallback_t tx);

#endif /* HSERIAL_H */
