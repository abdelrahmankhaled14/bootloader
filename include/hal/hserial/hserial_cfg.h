
typedef enum
{
    /* UART Channels */
    HSERIAL_UART_1 = 0,         /**< UART1 Channel */
    HSERIAL_UART_2,             /**< UART2 Channel */
    HSERIAL_UART_6,             /**< UART6 Channel */
    /* SPI Channels (Future expansion) */
    HSERIAL_SPI_1,              /**< SPI1 Channel */
    HSERIAL_SPI_2,              /**< SPI2 Channel */
    HSERIAL_SPI_3,              /**< SPI3 Channel */
    /* Total count */
    HSERIAL_CHANNEL_COUNT       /**< Total number of channels */
} HSerial_Channel_t;