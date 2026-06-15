#include "hal/hserial/hserial.h"

/* ============================================================
 * HSERIAL CHANNEL CONFIGURATIONS
 * ============================================================ */
const HSerial_Config_t HSerial_ConfigTable[HSERIAL_CHANNEL_COUNT] =
{
    [HSERIAL_UART_1] =
    {
        .channel     = HSERIAL_UART_1,
        .periph_type = HSERIAL_PERIPH_UART,
        .mode        = HSERIAL_MODE_INTERRUPT,

        .rx_callback = NULL,
        .tx_callback = NULL,

        .device_config =
        {
            .uart =
            {
                .Uart       = UART_1,
                .Baud_rate = 115200,
                .Parity_bit = NO_PARTITY,
                .stop_bit   = _1BIT,
                .mode       = INTERRUPT   /* UART driver mode */
            }
        }
    }
};
