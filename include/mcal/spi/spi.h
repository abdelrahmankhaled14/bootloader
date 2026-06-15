#ifndef SPI_H
#define SPI_H
#include "std_types.h"


/* ================= Types ================= */ 

typedef enum
{
    CRC_DISABLED = 0,
    CRC_ENABLED
} Spi_Crc_t;
typedef enum
{
    _8_BIT,
    _16_BIT
} Spi_DataSize_t;

typedef enum
{
    BAUD_RATE_2,
    BAUD_RATE_4,
    BAUD_RATE_8,
    BAUD_RATE_16,
    BAUD_RATE_32,
    BAUD_RATE_64,
    BAUD_RATE_128,
    BAUD_RATE_256
} baud_rate_t;
typedef enum
{
    SLAVE_SPI,
    MASTER_SPI
}spi_mode_t;

typedef enum
{
    MSB,
    LSB
}frame_format_t;
typedef enum
{
    IDLE_ON_LOW,
    IDLE_ON_HIGH
}clock_polarity_t;
typedef enum
{
    FIRST_EDGE,
    SECOND_EDGE
}clock_phase_t;
typedef enum
{
    SPI_1 = 0,
    SPI_2,
    SPI_3,
    SPI_4
}spi_t;

typedef enum
{
    SPI_NORMAL,SPI_INTERRUPT,SPI_DMA
}transtion_mode_t;
typedef struct
{
    Spi_Crc_t crc_state;
    Spi_DataSize_t data_size;
    baud_rate_t baud_rate;
    spi_mode_t mode;
    frame_format_t frame_format;
    clock_polarity_t clock_polarity;
    clock_phase_t clock_phase;
    spi_t Spi_Number;
    transtion_mode_t transtion_mode;
    
}Spi_Config_t;

/* ================= APIs ================= */
void SPI_Init(Spi_Config_t *config);

/* ================= CR1 Control ================= */
void SPI_EnableSpi(spi_t spi);
void SPI_DisableSpi(spi_t spi);
void SPI_EnableSpiMode(spi_t spi, spi_mode_t mode);
void SPI_ConfigureClock(spi_t spi, baud_rate_t baud_rate);
void SPI_ConfigureFrameFormat(spi_t spi, frame_format_t frame_format);
void SPI_ConfigureClockPolarity(spi_t spi, clock_polarity_t clock_polarity);
void SPI_ConfigureClockPhase(spi_t spi, clock_phase_t clock_phase);
void SPI_ConfigureDataSize(spi_t spi, Spi_DataSize_t data_size);
void SPI_EnableCrc(spi_t spi);
void SPI_DisableCrc(spi_t spi);

/* ================= CR2 Control ================= */
void SPI_EnableTxDma(spi_t spi);
void SPI_DisableTxDma(spi_t spi);
void SPI_EnableRxDma(spi_t spi);
void SPI_DisableRxDma(spi_t spi);
void SPI_EnableTxInterrupt(spi_t spi);
void SPI_DisableTxInterrupt(spi_t spi);
void SPI_EnableRxInterrupt(spi_t spi);
void SPI_DisableRxInterrupt(spi_t spi);

/* ================= Status Flags ================= */
uint8_t SPI_IsReceiveBufferNotEmpty(spi_t spi);
uint8_t SPI_IsTransmitBufferEmpty(spi_t spi);
uint8_t SPI_IsRxInterruptEnabled(spi_t spi);
uint8_t SPI_IsTxInterruptEnabled(spi_t spi);

/* ================= Data Transfer ================= */
void SPI_SendData(spi_t spi, uint16_t data);
uint16_t SPI_ReceiveData(spi_t spi);

#endif /* SPI_H */