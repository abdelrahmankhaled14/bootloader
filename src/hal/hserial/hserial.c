#include "mcal/rcc/rcc.h"
#include "hal/hserial/hserial.h"
#include "mcal/gpio/gpio.h"
#include "mcal/uart/uart.h"
#include "macros.h"
#include "mcal/nvic/nvic.h"

/* ============================================================
 * CHANNEL DATA
 * ============================================================ */
typedef struct
{
    uint8_t  tx_buffer[1024];
    uint8_t  rx_buffer[1024];
    uint16_t tx_count;
    uint16_t rx_count;
    uint16_t tx_length;
    uint16_t rx_length;
    uint8_t  in_use;
    uint8_t  tx_complete;
    uint8_t  rx_complete;
} Channel_data_t;

static Channel_data_t UartChannelData[3];

static rxCallback_t rx_callback = NULL;
static txCallback_t tx_callback = NULL;

extern const HSerial_Config_t HSerial_ConfigTable[HSERIAL_CHANNEL_COUNT];

/* ============================================================
 * HELPER FUNCTION PROTOTYPES
 * ============================================================ */
static HSerial_Status_t HSERIAL_EnableRccPeripheral(HSerial_Channel_t channel);
static HSerial_Status_t HSERIAL_EnableNvicInterrupt(HSerial_Channel_t channel,
                                                   HSerial_Mode_t mode);
static HSerial_Status_t HSERIAL_InitPeripheral(HSerial_Channel_t channel);

static void HSERIAL_UART_IRQHANDLER(Uart_t uart);

/* UART – Polling */
static void UART_Polling_TransmitBuffer(HSerial_Channel_t channel,
                                        const uint8_t *buffer,
                                        uint16_t length);
static void UART_Polling_ReceiveBuffer(HSerial_Channel_t channel,
                                       uint8_t *buffer,
                                       uint16_t length);
static void UART_Polling_SendByte(HSerial_Channel_t channel, uint8_t byte);
static void UART_Polling_ReceiveByte(HSerial_Channel_t channel, uint8_t *byte);

/* UART – Interrupt */
static void UART_IT_TransmitBuffer(HSerial_Channel_t channel,
                                   const uint8_t *buffer,
                                   uint16_t length);
static void UART_IT_ReceiveBuffer(HSerial_Channel_t channel,
                                  uint8_t *buffer,
                                  uint16_t length);

/* ============================================================
 * MAIN INIT FUNCTION
 * ============================================================ */
HSerial_Status_t HSerial_Init(HSerial_Channel_t channel,
                             HSerial_Interface_t *interface,
                             rxCallback_t rx,
                             txCallback_t tx)
{
    if (channel >= HSERIAL_CHANNEL_COUNT || interface == NULL)
    {
        return HSERIAL_ERROR;
    }

    const HSerial_Config_t *config = &HSerial_ConfigTable[channel];

    /* Enable RCC */
    if (HSERIAL_EnableRccPeripheral(channel) != HSERIAL_OK)
    {
        return HSERIAL_ERROR;
    }

    /* Enable NVIC (only for interrupt mode) */
    if (HSERIAL_EnableNvicInterrupt(channel, config->mode) != HSERIAL_OK)
    {
        return HSERIAL_ERROR;
    }

    /* Init UART peripheral */
    if (HSERIAL_InitPeripheral(channel) != HSERIAL_OK)
    {
        return HSERIAL_ERROR;
    }

    /* Setup interface */
    if (config->periph_type != HSERIAL_PERIPH_UART)
    {
        return HSERIAL_ERROR;
    }

    switch (config->mode)
    {
    case HSERIAL_MODE_POLLING:
        interface->TransmitBuffer = UART_Polling_TransmitBuffer;
        interface->ReceiveBuffer  = UART_Polling_ReceiveBuffer;
        interface->SendByte       = UART_Polling_SendByte;
        interface->ReceiveByte    = UART_Polling_ReceiveByte;
        break;

    case HSERIAL_MODE_INTERRUPT:
        rx_callback = rx;
        tx_callback = tx;

        UART_SetCallback(channel, HSERIAL_UART_IRQHANDLER);

        interface->TransmitBuffer = UART_IT_TransmitBuffer;
        interface->ReceiveBuffer  = UART_IT_ReceiveBuffer;
        interface->SendByte       = NULL;
        interface->ReceiveByte    = NULL;
        break;

    default:
        return HSERIAL_ERROR;
    }

    return HSERIAL_OK;
}

/* ============================================================
 * RCC ENABLE
 * ============================================================ */
static HSerial_Status_t HSERIAL_EnableRccPeripheral(HSerial_Channel_t channel)
{
    switch (channel)
    {
    case HSERIAL_UART_1:
        rcc_clk_perpheral_enable(APB2_BUS, USART1_EN);
        break;

    case HSERIAL_UART_2:
        rcc_clk_perpheral_enable(APB1_BUS, USART2_EN);
        break;

    case HSERIAL_UART_6:
        rcc_clk_perpheral_enable(APB2_BUS, USART6_EN);
        break;

    default:
        return HSERIAL_ERROR;
    }

    return HSERIAL_OK;
}

/* ============================================================
 * NVIC ENABLE
 * ============================================================ */
static HSerial_Status_t HSERIAL_EnableNvicInterrupt(HSerial_Channel_t channel,
                                                   HSerial_Mode_t mode)
{
    if (mode != HSERIAL_MODE_INTERRUPT)
    {
        return HSERIAL_OK;
    }

    switch (channel)
    {
    case HSERIAL_UART_1:
        NVIC_EnableIRQ(USART1_IRQn);
        break;

    case HSERIAL_UART_2:
        NVIC_EnableIRQ(USART2_IRQn);
        break;

    case HSERIAL_UART_6:
        NVIC_EnableIRQ(USART6_IRQn);
        break;

    default:
        return HSERIAL_ERROR;
    }

    return HSERIAL_OK;
}

/* ============================================================
 * UART INIT
 * ============================================================ */
static HSerial_Status_t HSERIAL_InitPeripheral(HSerial_Channel_t channel)
{
    const HSerial_Config_t *config = &HSerial_ConfigTable[channel];

    UART_Init(&config->device_config.uart);
    return HSERIAL_OK;
}

/* ============================================================
 * UART POLLING
 * ============================================================ */
static void UART_Polling_TransmitBuffer(HSerial_Channel_t channel,
                                        const uint8_t *buffer,
                                        uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        UART_Polling_SendByte(channel, buffer[i]);
    }
}

static void UART_Polling_ReceiveBuffer(HSerial_Channel_t channel,
                                       uint8_t *buffer,
                                       uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        UART_Polling_ReceiveByte(channel, &buffer[i]);
    }
}

static void UART_Polling_SendByte(HSerial_Channel_t channel, uint8_t byte)
{
    while (UART_ReadTXEmptyFlag(channel) == 0)
        ;

    UART_WriteDR(channel, byte);

    while (UART_ReadTransmitCompleteFlag(channel) == 0)
        ;
}

static void UART_Polling_ReceiveByte(HSerial_Channel_t channel, uint8_t *byte)
{
    while (UART_ReadRXNotEmptyFlag(channel) == 0)
        ;

    *byte = UART_ReadDR(channel);
}

/* ============================================================
 * UART INTERRUPT TRANSMIT / RECEIVE
 * ============================================================ */
static void UART_IT_TransmitBuffer(HSerial_Channel_t channel,
                                   const uint8_t *buffer,
                                   uint16_t length)
{
    Channel_data_t *ch = &UartChannelData[channel];

    ch->tx_length   = length;
    ch->tx_count    = 0;
    ch->tx_complete = 0;
    ch->in_use      = 1;

    for (uint16_t i = 0; i < length; i++)
    {
        ch->tx_buffer[i] = buffer[i];
    }

    UART_WriteDR(channel, ch->tx_buffer[ch->tx_count++]);
    UART_TransmitEmptyInterruptEnable(channel);
}

static void UART_IT_ReceiveBuffer(HSerial_Channel_t channel,
                                  uint8_t *buffer,
                                  uint16_t length)
{
    Channel_data_t *ch = &UartChannelData[channel];

    ch->rx_length   = length;
    ch->rx_count    = 0;
    ch->rx_complete = 0;
    ch->in_use      = 1;

    (void)buffer; /* internal buffer used */

    UART_ReceiveInterruptEnable(channel);
}

/* ============================================================
 * UART IRQ HANDLER
 * ============================================================ */
void HSERIAL_UART_IRQHANDLER(Uart_t uart)
{
    Channel_data_t *ch = &UartChannelData[uart];

    /* ================= RX ================= */
    if (UART_IsReceiveInterruptEnabled(uart) &&
        UART_ReadRXNotEmptyFlag(uart))
    {
        uint8_t data = UART_ReadDR(uart);

        if (ch->rx_count < ch->rx_length)
        {
            ch->rx_buffer[ch->rx_count++] = data;

            if (ch->rx_count >= ch->rx_length)
            {
                UART_ReceiveCompleteInterruptDisable(uart);
                UART_ClearReceiveFlagBusy(uart);

                ch->rx_complete = 1;

                if (rx_callback)
                {
                    rx_callback(ch->rx_buffer, ch->rx_length);
                }

                if (ch->tx_complete)
                {
                    ch->in_use = 0;
                }
            }
        }
    }

    /* ================= TX ================= */
    if (UART_IsTransmitInterruptEnabled(uart) &&
        UART_ReadTXEmptyFlag(uart))
    {
        if (ch->tx_count < ch->tx_length)
        {
            UART_WriteDR(uart, ch->tx_buffer[ch->tx_count++]);
        }
        else
        {
            UART_TransmitEmptyInterruptDisable(uart);
            UART_ClearSendFlagBusy(uart);

            ch->tx_complete = 1;

            if (tx_callback)
            {
                tx_callback();
            }

            if (ch->rx_complete)
            {
                ch->in_use = 0;
            }
        }
    }
}
