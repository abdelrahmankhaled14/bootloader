/****************************************************************************
 * @file uart.c
 * @brief UART driver implementation for STM32F4 (Blackpill) - Refactored
 * @author Abdelrahman Khaled
 * @version 1.2
 ****************************************************************************/

#include "mcal/gpio/gpio.h"
#include "mcal/uart/uart.h"
#include "mcal/uart/uart_private.h"
#include "mcal/rcc/rcc.h"
#include "macros.h"

/* ==========================================================================
 * STATIC GLOBAL VARIABLES (REFACTORED WITH ARRAYS)
 * ========================================================================== */

/* Busy flags - Array indexed by UART number [UART_1, UART_2, UART_6] */
static uint8_t g_UartSendFlagBusy[3] = {0, 0, 0};
static uint8_t g_UartReceiveFlagBusy[3] = {0, 0, 0};

/* Callbacks - Array indexed by UART number */

static void (*g_Callback[3])(Uart_t) = {NULL, NULL, NULL};
/* ==========================================================================
 * UART REGISTER BASE POINTERS
 * ========================================================================== */
Uart_Register_t *Uartx[] = {UART1, UART2, UART6};

/* ==========================================================================
 * LOW-LEVEL SEND/READ FUNCTIONS
 * ========================================================================== */

/******************************************************************************
 * @brief Write a byte to UART Data Register
 * @param uart: UART peripheral (UART_1, UART_2, or UART_6)
 * @param byte: Byte to write
 * @return None
 ******************************************************************************/
void UART_WriteDR(Uart_t uart, uint8_t byte)
{
    Uartx[uart]->DR = byte;
}

/******************************************************************************
 * @brief Read a byte from UART Data Register
 * @param uart: UART peripheral (UART_1, UART_2, or UART_6)
 * @return uint8_t: Received byte
 ******************************************************************************/
uint8_t UART_ReadDR(Uart_t uart)
{
    return (uint8_t)(Uartx[uart]->DR & 0xFF);
}

/* ==========================================================================
 * GPIO AND CONFIGURATION FUNCTIONS
 * ========================================================================== */

/******************************************************************************
 * @brief Configure GPIO pins for UART functionality
 * @param Uart: UART peripheral selection
 * @return None
 ******************************************************************************/
void UART_GpioInit(Uart_t Uart)
{
    GPIO_Configuration_t gpio_config[2]; /* Array for TX and RX pins */

    /* Configure GPIO based on selected UART peripheral */
    switch (Uart)
    {
    case UART_1:
        /********** Configure UART1: PA9 (TX), PA10 (RX) **********/
        rcc_clk_perpheral_enable(AHB1_BUS, GPIOA_EN);

        gpio_config[0].port = PORTA;
        gpio_config[0].pin = PIN9;
        gpio_config[0].direction = ALTERNATE_FUNCTION;
        gpio_config[0].output_type = PUSH_PULL;
        gpio_config[0].speed = VERY_HIGH_SPEED;
        gpio_config[0].resistor = PULL_UP;
        gpio_config[0].output_value = LOW;
        gpio_config[0].mode = AF_7;


        gpio_config[1].port = PORTA;
        gpio_config[1].pin = PIN10;
        gpio_config[1].direction = ALTERNATE_FUNCTION;
        gpio_config[1].output_type = PUSH_PULL;
        gpio_config[1].speed = VERY_HIGH_SPEED;
        gpio_config[1].resistor = PULL_UP;
        gpio_config[1].output_value = LOW;
        gpio_config[1].mode = AF_7;

        GPIO_Init(gpio_config, 2);
        rcc_clk_perpheral_enable(APB2_BUS, USART1_EN);
        break;

    case UART_2:
        /********** Configure UART2: PA2 (TX), PA3 (RX) **********/
        rcc_clk_perpheral_enable(AHB1_BUS, GPIOA_EN);

        gpio_config[0].port = PORTA;
        gpio_config[0].pin = PIN2;
        gpio_config[0].direction = ALTERNATE_FUNCTION;
        gpio_config[0].mode = AF_7;
        gpio_config[0].output_type = PUSH_PULL;
        gpio_config[0].speed = VERY_HIGH_SPEED;
        gpio_config[0].resistor = PULL_UP;
        gpio_config[0].output_value = LOW;

        gpio_config[1].port = PORTA;
        gpio_config[1].pin = PIN3;
        gpio_config[1].direction = ALTERNATE_FUNCTION;
        gpio_config[1].mode = AF_7;
        gpio_config[1].output_type = PUSH_PULL;
        gpio_config[1].speed = VERY_HIGH_SPEED;
        gpio_config[1].resistor = PULL_UP;
        gpio_config[1].output_value = LOW;

        GPIO_Init(gpio_config, 2);
        rcc_clk_perpheral_enable(APB1_BUS, USART2_EN);
        break;

    case UART_6:
        /********** Configure UART6: PC6 (TX), PC7 (RX) **********/
        rcc_clk_perpheral_enable(AHB1_BUS, GPIOC_EN);

        gpio_config[0].port = PORTC;
        gpio_config[0].pin = PIN6;
        gpio_config[0].direction = ALTERNATE_FUNCTION;
        gpio_config[0].mode = AF_8;
        gpio_config[0].output_type = PUSH_PULL;
        gpio_config[0].speed = VERY_HIGH_SPEED;
        gpio_config[0].resistor = PULL_UP;
        gpio_config[0].output_value = LOW;

        gpio_config[1].port = PORTC;
        gpio_config[1].pin = PIN7;
        gpio_config[1].direction = ALTERNATE_FUNCTION;
        gpio_config[1].mode = AF_8;
        gpio_config[1].output_type = PUSH_PULL;
        gpio_config[1].speed = VERY_HIGH_SPEED;
        gpio_config[1].resistor = PULL_UP;
        gpio_config[1].output_value = LOW;

        GPIO_Init(gpio_config, 2);
        rcc_clk_perpheral_enable(APB2_BUS, USART6_EN);
        break;

    default:
        break;
    }
}

/* ==========================================================================
 * STATUS REGISTER FLAG FUNCTIONS
 * ========================================================================== */

uint8_t UART_ReadParityErrorFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, PE);
}

uint8_t UART_ReadFramingErrorFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, FE);
}

uint8_t UART_ReadNoiseErrorFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, NF);
}

uint8_t UART_ReadOverrunErrorFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, ORE);
}

uint8_t UART_ReadIdleLineFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, IDLE);
}

uint8_t UART_ReadRXNotEmptyFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, RXNE);
}

uint8_t UART_ReadTransmitCompleteFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, TC);
}

uint8_t UART_ReadTXEmptyFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, TXE);
}

uint8_t UART_ReadLINBreakFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, LBD);
}

uint8_t UART_ReadCTSFlag(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->SR, CTS);
}

uint8_t UART_IsTransmitInterruptEnabled(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->CR1, TXEIE);
}
uint8_t UART_IsReceiveInterruptEnabled(Uart_t uart)
{
    return READ_BIT(Uartx[uart]->CR1, RXNEIE);
}
/* ==========================================================================
 * CONTROL REGISTER 1 FUNCTIONS
 * ========================================================================== */

void UART_EnableTransmit(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << TE);
}

void UART_DisableTransmit(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << TE);
}

void UART_EnableReceive(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << RE);
}

void UART_DisableReceive(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << RE);
}

void UART_EnableUart(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << UE);
}

void UART_DisableUart(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << UE);
}

void UART_ParityEnable(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << PCE);
}

void UART_ParityDisable(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << PCE);
}

void UART_OddParity(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << PS);
}

void UART_EvenParity(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << PS);
}

void UART_ParityErrorInterruptEnable(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << PEIE);
}

void UART_ParityErrorInterruptDisable(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << PEIE);
}

void UART_TransmitEmptyInterruptEnable(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << TXEIE);
}

void UART_TransmitEmptyInterruptDisable(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << TXEIE);
}

void UART_TransmitCompleteInterruptEnable(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << TCIE);
}

void UART_TransmitCompleteInterruptDisable(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << TCIE);
}

void UART_ReceiveInterruptEnable(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << RXNEIE);
}

void UART_ReceiveCompleteInterruptDisable(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << RXNEIE);
}

void UART_IDLEInterruptEnable(Uart_t uart)
{
    Uartx[uart]->CR1 |= (1 << IDLEIE);
}

void UART_IDLECompleteInterruptDisable(Uart_t uart)
{
    Uartx[uart]->CR1 &= ~(1 << IDLEIE);
}

/* ==========================================================================
 * CONTROL REGISTER 2 FUNCTIONS
 * ========================================================================== */

void UART_SetAddress(Uart_t uart, uint8_t address)
{
    Uartx[uart]->CR2 = (Uartx[uart]->CR2 & ~(0x0F << ADD)) | ((address & 0x0F) << ADD);
}

void UART_LINBreakDetectionLengthEnable(Uart_t uart)
{
    Uartx[uart]->CR2 |= (1 << LBDL);
}

void UART_LINBreakDetectionLengthDisable(Uart_t uart)
{
    Uartx[uart]->CR2 &= ~(1 << LBDL);
}

void UART_LINBreakDetectionInterruptEnable(Uart_t uart)
{
    Uartx[uart]->CR2 |= (1 << LBDIE);
}

void UART_LINBreakDetectionInterruptDisable(Uart_t uart)
{
    Uartx[uart]->CR2 &= ~(1 << LBDIE);
}

void UART_LastBitClockPulseEnable(Uart_t uart)
{
    Uartx[uart]->CR2 |= (1 << LBCL);
}

void UART_LastBitClockPulseDisable(Uart_t uart)
{
    Uartx[uart]->CR2 &= ~(1 << LBCL);
}

void UART_ClockPhaseFirstEdge(Uart_t uart)
{
    Uartx[uart]->CR2 &= ~(1 << CPHA);
}

void UART_ClockPhaseSecondEdge(Uart_t uart)
{
    Uartx[uart]->CR2 |= (1 << CPHA);
}

void UART_ClockPolarityLow(Uart_t uart)
{
    Uartx[uart]->CR2 &= ~(1 << CPOL);
}

void UART_ClockPolarityHigh(Uart_t uart)
{
    Uartx[uart]->CR2 |= (1 << CPOL);
}

void UART_ClockEnable(Uart_t uart)
{
    Uartx[uart]->CR2 |= (1 << CLKEN);
}

void UART_ClockDisable(Uart_t uart)
{
    Uartx[uart]->CR2 &= ~(1 << CLKEN);
}

void UART_StopBit(Uart_t uart, stop_bit_t stop_bit)
{
    Uartx[uart]->CR2 = (Uartx[uart]->CR2 & ~(0b11 << STOP)) | ((stop_bit & 0b11) << STOP);
}

void UART_LINModeEnable(Uart_t uart)
{
    Uartx[uart]->CR2 |= (1 << LINEN);
}

void UART_LINModeDisable(Uart_t uart)
{
    Uartx[uart]->CR2 &= ~(1 << LINEN);
}

/* ==========================================================================
 * CONTROL REGISTER 3 FUNCTIONS
 * ========================================================================== */

void UART_ErrorInterruptEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << EIE);
}

void UART_ErrorInterruptDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << EIE);
}

void UART_IrDAModeEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << IREN);
}

void UART_IrDAModeDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << IREN);
}

void UART_IrDALowPowerEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << IRLP);
}

void UART_IrDALowPowerDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << IRLP);
}

void UART_HalfDuplexEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << HDSEL);
}

void UART_HalfDuplexDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << HDSEL);
}

void UART_SmartcardNACKEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << NACK);
}

void UART_SmartcardNACKDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << NACK);
}

void UART_SmartcardModeEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << SCEN);
}

void UART_SmartcardModeDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << SCEN);
}

void UART_DMAReceiverEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << DMAR);
}

void UART_DMAReceiverDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << DMAR);
}

void UART_DMATransmitterEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << DMAT);
}

void UART_DMATransmitterDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << DMAT);
}

void UART_RTSEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << RTSE);
}

void UART_RTSDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << RTSE);
}

void UART_CTSEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << CTSE);
}

void UART_CTSDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << CTSE);
}

void UART_CTSInterruptEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << CTSIE);
}

void UART_CTSInterruptDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << CTSIE);
}

void UART_OneBitSamplingEnable(Uart_t uart)
{
    Uartx[uart]->CR3 |= (1 << ONEBIT);
}

void UART_OneBitSamplingDisable(Uart_t uart)
{
    Uartx[uart]->CR3 &= ~(1 << ONEBIT);
}

/* ==========================================================================
 * CONFIGURATION FUNCTIONS
 * ========================================================================== */

void UART_Configure_BaudRate(Uart_t uart, uint32_t BaudRate)
{
    uint32_t pclk = 16000000;
    float usartdiv;
    uint32_t mantissa;
    uint32_t fraction;

    usartdiv = (float)(pclk + (BaudRate / 2U)) / (16U * BaudRate);
    mantissa = usartdiv;
    fraction = ((pclk + (BaudRate / 2U)) / BaudRate) % 16U;
    int x = (mantissa << 4) | (fraction & 0xF);
    Uartx[uart]->BRR = x;
}

void UART_Configure_parity(Uart_t uart, Parity_bit_t parity)
{
    if (parity == NO_PARTITY)
    {
        UART_ParityDisable(uart);
    }
    else if (parity == ODD_PARITY)
    {
        UART_ParityEnable(uart);
        UART_OddParity(uart);
    }
    else
    {
        UART_ParityEnable(uart);
        UART_EvenParity(uart);
    }
}


/* ==========================================================================
 * INITIALIZATION FUNCTION
 * ========================================================================== */

void UART_Init(Uart_Config_t *Uart_Config)
{
    UART_GpioInit(Uart_Config->Uart);

    switch (Uart_Config->mode)
    {
    case NORMAL:
        UART_Configure_BaudRate(Uart_Config->Uart, Uart_Config->Baud_rate);
        UART_Configure_parity(Uart_Config->Uart, Uart_Config->Parity_bit);
        UART_StopBit(Uart_Config->Uart, Uart_Config->stop_bit);
        UART_EnableTransmit(Uart_Config->Uart);
        UART_EnableReceive(Uart_Config->Uart);
        UART_EnableUart(Uart_Config->Uart);
        break;

    case INTERRUPT:
        UART_Configure_BaudRate(Uart_Config->Uart, Uart_Config->Baud_rate);
        UART_Configure_parity(Uart_Config->Uart, Uart_Config->Parity_bit);
        UART_StopBit(Uart_Config->Uart, Uart_Config->stop_bit);
        UART_EnableTransmit(Uart_Config->Uart);
        UART_EnableReceive(Uart_Config->Uart);
        UART_EnableUart(Uart_Config->Uart);
        break;

    case DMA:
        UART_Configure_BaudRate(Uart_Config->Uart, Uart_Config->Baud_rate);
        UART_Configure_parity(Uart_Config->Uart, Uart_Config->Parity_bit);
        UART_StopBit(Uart_Config->Uart, Uart_Config->stop_bit);
        UART_EnableTransmit(Uart_Config->Uart);
        UART_EnableReceive(Uart_Config->Uart);
        UART_DMATransmitterEnable(Uart_Config->Uart);
        UART_DMAReceiverEnable(Uart_Config->Uart);
        UART_EnableUart(Uart_Config->Uart);
        break;

    default:
        break;
    }
}
/* ==========================================================================
 * CALLBACK SETTER FUNCTIONS
 * ========================================================================== */
void UART_SetCallback(Uart_t uart, void (*Callback)(Uart_t))
{
    g_Callback[uart] = Callback;
}
/* ==========================================================================
 * INTERRUPT HANDLER FUNCTIONS
 * ========================================================================== */    
void USART1_IRQHandler(void)
{
    if(g_Callback[UART_1] != NULL)
    {
            g_Callback[UART_1](UART_1);
    }
}
void USART2_IRQHandler(void)
{
    if(g_Callback[UART_2] != NULL)
    {
            g_Callback[UART_2](UART_2);
    }
}
void USART6_IRQHandler(void)
{
    if(g_Callback[UART_6] != NULL)
    {
            g_Callback[UART_6](UART_6);
    }
}

/* ==========================================================================
 * BUSY FLAG GETTER FUNCTIONS
 * ========================================================================== */
/*functions to send busy flag*/
uint8_t UART_GetSendFlagBusy(Uart_t uart)
{
    return g_UartSendFlagBusy[uart];
}
uint8_t UART_GetReceiveFlagBusy(Uart_t uart)
{
    return g_UartReceiveFlagBusy[uart];
}
void UART_SetSendFlagBusy(Uart_t uart)
{
    g_UartSendFlagBusy[uart]=1;
}
void UART_ClearSendFlagBusy(Uart_t uart)
{
    g_UartSendFlagBusy[uart]=0;
}
void UART_SetReceiveFlagBusy(Uart_t uart)
{
    g_UartSendFlagBusy[uart]=1;
}
void UART_ClearReceiveFlagBusy(Uart_t uart)
{
    g_UartReceiveFlagBusy[uart]=0;
}
/* ==========================================================================
 * END OF FILE
 * ========================================================================== */