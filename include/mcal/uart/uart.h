#ifndef _UART_H
#define _UART_H

#include "std_types.h"

/*************************************************************************
 ****************************   Data type   ******************************
 *************************************************************************
*/

typedef enum
{
    NO_PARTITY,ODD_PARITY,EVEN_PARITY
}Parity_bit_t;

typedef enum
{
    _1BIT,_0_5BIT,_2BIT,_1_5BIT
}stop_bit_t;
typedef enum
{
    NORMAL,INTERRUPT,DMA
}mode_t;

typedef enum
{
    UART_1,UART_2,UART_6
}Uart_t;

typedef struct 
{ 
   uint32_t Baud_rate;
   Parity_bit_t Parity_bit;
   stop_bit_t stop_bit;
   mode_t mode; 
   Uart_t Uart;
   
}Uart_Config_t;

typedef enum
{
    SEND,                                      // Check send busy flag
    RECEIVE                                    // Check receive busy flag
} send_receiveflag_t;
/********************************************************************** 
***************************Function Prototype***************************
***********************************************************************/
/* ==========================================================================
 * INITIALIZATION
 * ========================================================================== */

void UART_Init(Uart_Config_t *Uart_Config);

/* ==========================================================================
 * DATA REGISTER ACCESS
 * ========================================================================== */

void    UART_WriteDR(Uart_t uart, uint8_t byte);
uint8_t UART_ReadDR(Uart_t uart);

/* ==========================================================================
 * FLAG READ FUNCTIONS
 * ========================================================================== */

uint8_t UART_ReadParityErrorFlag(Uart_t uart);
uint8_t UART_ReadFramingErrorFlag(Uart_t uart);
uint8_t UART_ReadNoiseErrorFlag(Uart_t uart);
uint8_t UART_ReadOverrunErrorFlag(Uart_t uart);
uint8_t UART_ReadIdleLineFlag(Uart_t uart);
uint8_t UART_ReadRXNotEmptyFlag(Uart_t uart);
uint8_t UART_ReadTransmitCompleteFlag(Uart_t uart);
uint8_t UART_ReadTXEmptyFlag(Uart_t uart);
uint8_t UART_ReadLINBreakFlag(Uart_t uart);
uint8_t UART_ReadCTSFlag(Uart_t uart);
uint8_t UART_IsTransmitInterruptEnabled(Uart_t uart);
uint8_t UART_IsReceiveInterruptEnabled(Uart_t uart);

/* ==========================================================================
 * CONTROL REGISTER 1
 * ========================================================================== */

void UART_EnableTransmit(Uart_t uart);
void UART_DisableTransmit(Uart_t uart);
void UART_EnableReceive(Uart_t uart);
void UART_DisableReceive(Uart_t uart);
void UART_EnableUart(Uart_t uart);
void UART_DisableUart(Uart_t uart);

void UART_ParityEnable(Uart_t uart);
void UART_ParityDisable(Uart_t uart);
void UART_OddParity(Uart_t uart);
void UART_EvenParity(Uart_t uart);

void UART_ParityErrorInterruptEnable(Uart_t uart);
void UART_ParityErrorInterruptDisable(Uart_t uart);

void UART_TransmitEmptyInterruptEnable(Uart_t uart);
void UART_TransmitEmptyInterruptDisable(Uart_t uart);

void UART_TransmitCompleteInterruptEnable(Uart_t uart);
void UART_TransmitCompleteInterruptDisable(Uart_t uart);

void UART_ReceiveInterruptEnable(Uart_t uart);
void UART_ReceiveCompleteInterruptDisable(Uart_t uart);

void UART_IDLEInterruptEnable(Uart_t uart);
void UART_IDLECompleteInterruptDisable(Uart_t uart);

/* ==========================================================================
 * CONTROL REGISTER 2
 * ========================================================================== */

void UART_SetAddress(Uart_t uart, uint8_t address);
void UART_LINBreakDetectionLengthEnable(Uart_t uart);
void UART_LINBreakDetectionLengthDisable(Uart_t uart);
void UART_LINBreakDetectionInterruptEnable(Uart_t uart);
void UART_LINBreakDetectionInterruptDisable(Uart_t uart);
void UART_LastBitClockPulseEnable(Uart_t uart);
void UART_LastBitClockPulseDisable(Uart_t uart);
void UART_ClockPhaseFirstEdge(Uart_t uart);
void UART_ClockPhaseSecondEdge(Uart_t uart);
void UART_ClockPolarityLow(Uart_t uart);
void UART_ClockPolarityHigh(Uart_t uart);
void UART_ClockEnable(Uart_t uart);
void UART_ClockDisable(Uart_t uart);
void UART_StopBit(Uart_t uart, stop_bit_t stop_bit);
void UART_LINModeEnable(Uart_t uart);
void UART_LINModeDisable(Uart_t uart);

/* ==========================================================================
 * CONTROL REGISTER 3
 * ========================================================================== */

void UART_ErrorInterruptEnable(Uart_t uart);
void UART_ErrorInterruptDisable(Uart_t uart);
void UART_IrDAModeEnable(Uart_t uart);
void UART_IrDAModeDisable(Uart_t uart);
void UART_IrDALowPowerEnable(Uart_t uart);
void UART_IrDALowPowerDisable(Uart_t uart);
void UART_HalfDuplexEnable(Uart_t uart);
void UART_HalfDuplexDisable(Uart_t uart);
void UART_SmartcardNACKEnable(Uart_t uart);
void UART_SmartcardNACKDisable(Uart_t uart);
void UART_SmartcardModeEnable(Uart_t uart);
void UART_SmartcardModeDisable(Uart_t uart);
void UART_DMAReceiverEnable(Uart_t uart);
void UART_DMAReceiverDisable(Uart_t uart);
void UART_DMATransmitterEnable(Uart_t uart);
void UART_DMATransmitterDisable(Uart_t uart);
void UART_RTSEnable(Uart_t uart);
void UART_RTSDisable(Uart_t uart);
void UART_CTSEnable(Uart_t uart);
void UART_CTSDisable(Uart_t uart);
void UART_CTSInterruptEnable(Uart_t uart);
void UART_CTSInterruptDisable(Uart_t uart);
void UART_OneBitSamplingEnable(Uart_t uart);
void UART_OneBitSamplingDisable(Uart_t uart);

/* ==========================================================================
 * CONFIGURATION
 * ========================================================================== */

void UART_Configure_BaudRate(Uart_t uart, uint32_t BaudRate);
void UART_Configure_parity(Uart_t uart, Parity_bit_t parity);

/* ==========================================================================
 * CALLBACKS
 * ========================================================================== */

void UART_SetCallback(Uart_t uart, void (*Callback)(Uart_t));

/* ==========================================================================
 * BUSY FLAGS
 * ========================================================================== */

uint8_t UART_GetSendFlagBusy(Uart_t uart);
uint8_t UART_GetReceiveFlagBusy(Uart_t uart);
void    UART_SetSendFlagBusy(Uart_t uart);
void    UART_ClearSendFlagBusy(Uart_t uart);
void    UART_SetReceiveFlagBusy(Uart_t uart);
void    UART_ClearReceiveFlagBusy(Uart_t uart);

#endif /* UART_H */