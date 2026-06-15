#include "mcal/spi/spi_private.h"
#include "mcal/spi/spi.h"
#include "macros.h"
Spi_Register_t *spix[4] = {SPI1, SPI2, SPI3, SPI4};
static void (*g_SpiCallback[4])(spi_t spi) = {0};

void SPI_Init(Spi_Config_t *config)
{
    if (config == NULL)
        return;

    spi_t spi = config->Spi_Number;

    /* 1. Disable SPI first */
    SPI_DisableSpi(spi);

    /* 2. Configure MASTER / SLAVE */
    SPI_EnableSpiMode(spi, config->mode);

    /* 3. Configure baud rate */
    SPI_ConfigureClock(spi, config->baud_rate);

    /* 4. Data size */
    SPI_ConfigureDataSize(spi, config->data_size);

    /* 5. Frame format (MSB / LSB) */
    SPI_ConfigureFrameFormat(spi, config->frame_format);

    /* 6. Clock polarity & phase */
    SPI_ConfigureClockPolarity(spi, config->clock_polarity);
    SPI_ConfigureClockPhase(spi, config->clock_phase);

    /* 7. CRC */
    if (config->crc_state == CRC_ENABLED)
        SPI_EnableCrc(spi);
    else
        SPI_DisableCrc(spi);

    /* 8. MASTER MODE CRITICAL STEP (SSM + SSI) */
    if (config->mode == MASTER_SPI)
    {
        /* Your API does not expose SSM/SSI,
           so SPI_EnableSpiMode() MUST handle this internally.
           If it doesn’t → THAT IS A BUG IN YOUR DRIVER */
    }

    /* 9. Transaction mode */
    switch (config->transtion_mode)
    {
    case SPI_NORMAL:
        SPI_DisableTxDma(spi);
        SPI_DisableRxDma(spi);
        SPI_DisableTxInterrupt(spi);
        SPI_DisableRxInterrupt(spi);
        break;

    case SPI_INTERRUPT:
        SPI_DisableTxDma(spi);
        SPI_DisableRxDma(spi);
        SPI_DisableTxInterrupt(spi);
        SPI_DisableRxInterrupt(spi);
        break;

    case SPI_DMA:
        SPI_EnableTxDma(spi);
        SPI_EnableRxDma(spi);
        SPI_DisableTxInterrupt(spi);
        SPI_DisableRxInterrupt(spi);
        break;
    }

    /* 10. Enable SPI LAST */
    SPI_EnableSpi(spi);
}

/*CR1*/
void SPI_EnableSpi(spi_t spi)
{
    SPI1->CR1 |= SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_SSM; // Enable SPI
}

void SPI_DisableSpi(spi_t spi)
{
    spix[spi]->CR1 &= ~SPI_CR1_SPE;
}
void SPI_EnableSpiMode(spi_t spi, spi_mode_t mode)
{
    if (mode == MASTER_SPI)
    {
        spix[spi]->CR1 |= SPI_CR1_MSTR;
    }
    else
    {
        spix[spi]->CR1 &= ~SPI_CR1_MSTR;
    }
}
void SPI_ConfigureClock(spi_t spi, baud_rate_t baud_rate)
{
    /* Configure the clock settings */
    spix[spi]->CR1 &= ~SPI_CR1_BR;
    spix[spi]->CR1 |= (baud_rate << 3);
}
void SPI_ConfigureFrameFormat(spi_t spi, frame_format_t frame_format)
{
    if (frame_format == MSB)
    {
        spix[spi]->CR1 &= ~SPI_CR1_LSBFIRST;
    }
    else
    {
        spix[spi]->CR1 |= SPI_CR1_LSBFIRST;
    }
}
void SPI_ConfigureClockPolarity(spi_t spi, clock_polarity_t clock_polarity)
{
    if (clock_polarity == IDLE_ON_LOW)
    {
        spix[spi]->CR1 &= ~SPI_CR1_CPOL;
    }
    else
    {
        spix[spi]->CR1 |= SPI_CR1_CPOL;
    }
}
void SPI_ConfigureClockPhase(spi_t spi, clock_phase_t clock_phase)
{
    if (clock_phase == FIRST_EDGE)
    {
        spix[spi]->CR1 &= ~SPI_CR1_CPHA;
    }
    else
    {
        spix[spi]->CR1 |= SPI_CR1_CPHA;
    }
}

void SPI_ConfigureDataSize(spi_t spi, Spi_DataSize_t data_size)
{
    if (data_size == _8_BIT)
    {
        spix[spi]->CR1 &= ~SPI_CR1_DFF;
    }
    else
    {
        spix[spi]->CR1 |= SPI_CR1_DFF;
    }
}
void SPI_EnableCrc(spi_t spi)
{
    spix[spi]->CR1 |= SPI_CR1_CRCEN;
}
void SPI_DisableCrc(spi_t spi)
{
    spix[spi]->CR1 &= ~SPI_CR1_CRCEN;
}
/*CR2*/
void SPI_EnableTxDma(spi_t spi)
{
    spix[spi]->CR2 |= SPI_CR2_TXDMAEN;
}
void SPI_DisableTxDma(spi_t spi)
{
    spix[spi]->CR2 &= ~SPI_CR2_TXDMAEN;
}
void SPI_EnableRxDma(spi_t spi)
{
    spix[spi]->CR2 |= SPI_CR2_RXDMAEN;
}
void SPI_DisableRxDma(spi_t spi)
{
    spix[spi]->CR2 &= ~SPI_CR2_RXDMAEN;
}
void SPI_EnableTxInterrupt(spi_t spi)
{
    spix[spi]->CR2 |= SPI_CR2_TXEIE;
}
void SPI_DisableTxInterrupt(spi_t spi)
{
    spix[spi]->CR2 &= ~SPI_CR2_TXEIE;
}
void SPI_EnableRxInterrupt(spi_t spi)
{
    spix[spi]->CR2 |= SPI_CR2_RXNEIE;
}
void SPI_DisableRxInterrupt(spi_t spi)
{
    spix[spi]->CR2 &= ~SPI_CR2_RXNEIE;
}

/* Read flags*/
uint8_t SPI_IsReceiveBufferNotEmpty(spi_t spi)
{
     int x = READ_BIT(spix[spi]->SR, SPI_SR_RXNE_Pos);
    return x;
}
uint8_t SPI_IsTransmitBufferEmpty(spi_t spi)
{
    volatile int x = READ_BIT(spix[spi]->SR, SPI_SR_TXE_Pos);
    return x;
}

/* Send and receive data */
void SPI_SendData(spi_t spi, uint16_t data)
{
    spix[spi]->DR = data;
}
uint16_t SPI_ReceiveData(spi_t spi)
{
    return (uint16_t)(spix[spi]->DR);
}


uint8_t SPI_IsRxInterruptEnabled(spi_t spi)
{
    int x =  READ_BIT(spix[spi]->CR2, SPI_CR2_RXNEIE_Pos);
    return x;

}
uint8_t SPI_IsTxInterruptEnabled(spi_t spi)
{
    return READ_BIT(spix[spi]->CR2, SPI_CR2_TXEIE_Pos);
}
void SPI_SetCallback(spi_t spi, void (*Callback)(spi_t))
{
    g_SpiCallback[spi] = Callback;
}

void SPI1_IRQHandler(void)
{
    if (g_SpiCallback[SPI_1] != NULL)
    {
        g_SpiCallback[SPI_1](SPI_1);
    }
}
void SPI2_IRQHandler(void)
{
    if (g_SpiCallback[SPI_2] != NULL)
    {
        g_SpiCallback[SPI_2](SPI_2);
    }
}
void SPI3_IRQHandler(void)
{
    if (g_SpiCallback[SPI_3] != NULL)
    {
        g_SpiCallback[SPI_3](SPI_3);
    }
}