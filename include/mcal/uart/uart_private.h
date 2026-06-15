#ifndef _UART_PRIVATE_H
#define _UART_PRIVATE_H

#include "std_types.h"
typedef struct 
{
    uint32_t SR;
    uint32_t DR;
    uint32_t BRR;
    uint32_t CR1;
    uint32_t CR2;
    uint32_t CR3;
    uint32_t GTPR;
}Uart_Register_t;

#define UART1_BASE      (0x40011000)  
#define UART2_BASE      (0x40004400)
#define UART6_BASE      (0x40011400) 


#define UART1 ((Uart_Register_t *) UART1_BASE)
#define UART2 ((Uart_Register_t *) UART2_BASE)
#define UART6 ((Uart_Register_t *) UART6_BASE)

/******************************************************************************
 * UART Status Register (SR) Bit Definitions
 ******************************************************************************/
#define PE      0   /**< Parity Error */
#define FE      1   /**< Framing Error */
#define NF      2   /**< Noise Detected Flag */
#define ORE     3   /**< Overrun Error */
#define IDLE    4   /**< IDLE Line Detected */
#define RXNE    5   /**< Read Data Register Not Empty */
#define TC      6   /**< Transmission Complete */
#define TXE     7   /**< Transmit Data Register Empty */
#define LBD     8   /**< LIN Break Detection Flag */
#define CTS     9   /**< CTS Flag */

/******************************************************************************
 * UART Control Register 1 (CR1) Bit Definitions
 ******************************************************************************/
#define SBK     0   /**< Send Break */
#define RWU     1   /**< Receiver Wakeup */
#define RE      2   /**< Receiver Enable */
#define TE      3   /**< Transmitter Enable */
#define IDLEIE  4   /**< IDLE Interrupt Enable */
#define RXNEIE  5   /**< RXNE Interrupt Enable */
#define TCIE    6   /**< Transmission Complete Interrupt Enable */
#define TXEIE   7   /**< TXE Interrupt Enable */
#define PEIE    8   /**< PE Interrupt Enable */
#define PS      9   /**< Parity Selection */
#define PCE    10   /**< Parity Control Enable */
#define WAKE   11   /**< Wakeup method */
#define M      12   /**< Word length */
#define UE     13   /**< USART Enable */
#define OVER8  15   /**< Oversampling mode */


/******************************************************************************
 * UART Control Register 2 (CR2) Bit Definitions
 ******************************************************************************/
#define ADD       0  /**< Address of the USART node (bits 0-3) */
#define LBDL      5  /**< LIN Break Detection Length */
#define LBDIE     6  /**< LIN Break Detection Interrupt Enable */
#define LBCL      8  /**< Last Bit Clock pulse */
#define CPHA      9  /**< Clock Phase */
#define CPOL     10  /**< Clock Polarity */
#define CLKEN    11  /**< Clock Enable */
#define STOP     12  /**< Stop Bits (bits 12-13) */
#define LINEN    14  /**< LIN Mode Enable */


/******************************************************************************
 * UART Control Register 3 (CR3) Bit Definitions
 ******************************************************************************/
#define EIE       0  /**< Error Interrupt Enable */
#define IREN      1  /**< IrDA mode Enable */
#define IRLP      2  /**< IrDA Low-Power */
#define HDSEL     3  /**< Half-Duplex Selection */
#define NACK      4  /**< Smartcard NACK enable */
#define SCEN      5  /**< Smartcard mode enable */
#define DMAR      6  /**< DMA Enable Receiver */
#define DMAT      7  /**< DMA Enable Transmitter */
#define RTSE      8  /**< RTS Enable */
#define CTSE      9  /**< CTS Enable */
#define CTSIE    10  /**< CTS Interrupt Enable */
#define ONEBIT   11  /**< One sample bit method enable */



#endif