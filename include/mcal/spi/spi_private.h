/**
 ******************************************************************************
 * @file    stm32_spi_registers.h
 * @brief   STM32 SPI Register Definitions and Bit Masks
 * @author  Your Name
 * @date    December 2025
 ******************************************************************************
 */

#ifndef __STM32_SPI_REGISTERS_H
#define __STM32_SPI_REGISTERS_H

#include "std_types.h"

/* ============================================================================ */
/*                         SPI Base Addresses                                   */
/* ============================================================================ */

#define SPI1_BASE       0x40013000  // APB2
#define SPI2_BASE       0x40003800  // APB1
#define SPI3_BASE       0x40003C00  // APB1
#define SPI4_BASE       0x40013400

/* ============================================================================ */
/*                         SPI Register Structure                               */
/* ============================================================================ */

typedef struct {
    volatile uint32_t CR1;      // Control Register 1            (Offset: 0x00)
    volatile uint32_t CR2;      // Control Register 2            (Offset: 0x04)
    volatile uint32_t SR;       // Status Register               (Offset: 0x08)
    volatile uint32_t DR;       // Data Register                 (Offset: 0x0C)
    volatile uint32_t CRCPR;    // CRC Polynomial Register       (Offset: 0x10)
    volatile uint32_t RXCRCR;   // RX CRC Register               (Offset: 0x14)
    volatile uint32_t TXCRCR;   // TX CRC Register               (Offset: 0x18)
} Spi_Register_t;

/* ============================================================================ */
/*                         SPI Peripheral Pointers                              */
/* ============================================================================ */

#define SPI1            (( Spi_Register_t *) SPI1_BASE)
#define SPI2            (( Spi_Register_t *) SPI2_BASE)
#define SPI3            (( Spi_Register_t *) SPI3_BASE)
#define SPI4            (( Spi_Register_t *) SPI4_BASE)

/* ============================================================================ */
/*                    SPI_CR1 - Control Register 1 Bits                         */
/* ============================================================================ */

// Bit positions
#define SPI_CR1_CPHA_Pos        0
#define SPI_CR1_CPOL_Pos        1
#define SPI_CR1_MSTR_Pos        2
#define SPI_CR1_BR_Pos          3
#define SPI_CR1_SPE_Pos         6
#define SPI_CR1_LSBFIRST_Pos    7
#define SPI_CR1_SSI_Pos         8
#define SPI_CR1_SSM_Pos         9
#define SPI_CR1_RXONLY_Pos      10
#define SPI_CR1_DFF_Pos         11
#define SPI_CR1_CRCNEXT_Pos     12
#define SPI_CR1_CRCEN_Pos       13
#define SPI_CR1_BIDIOE_Pos      14
#define SPI_CR1_BIDIMODE_Pos    15

// Bit masks
#define SPI_CR1_CPHA            (1U << SPI_CR1_CPHA_Pos)        // Clock Phase
#define SPI_CR1_CPOL            (1U << SPI_CR1_CPOL_Pos)        // Clock Polarity
#define SPI_CR1_MSTR            (1U << SPI_CR1_MSTR_Pos)        // Master Selection
#define SPI_CR1_BR              (0x7U << SPI_CR1_BR_Pos)        // Baud Rate Control
#define SPI_CR1_SPE             (1U << SPI_CR1_SPE_Pos)         // SPI Enable
#define SPI_CR1_LSBFIRST        (1U << SPI_CR1_LSBFIRST_Pos)    // Frame Format
#define SPI_CR1_SSI             (1U << SPI_CR1_SSI_Pos)         // Internal Slave Select
#define SPI_CR1_SSM             (1U << SPI_CR1_SSM_Pos)         // Software Slave Management
#define SPI_CR1_RXONLY          (1U << SPI_CR1_RXONLY_Pos)      // Receive Only
#define SPI_CR1_DFF             (1U << SPI_CR1_DFF_Pos)         // Data Frame Format
#define SPI_CR1_CRCNEXT         (1U << SPI_CR1_CRCNEXT_Pos)     // CRC Transfer Next
#define SPI_CR1_CRCEN           (1U << SPI_CR1_CRCEN_Pos)       // Hardware CRC Enable
#define SPI_CR1_BIDIOE          (1U << SPI_CR1_BIDIOE_Pos)      // Output Enable in Bidi Mode
#define SPI_CR1_BIDIMODE        (1U << SPI_CR1_BIDIMODE_Pos)    // Bidirectional Data Mode

// Baud Rate Prescaler Values
#define SPI_BR_DIV2             (0x0U << SPI_CR1_BR_Pos)        // fPCLK/2
#define SPI_BR_DIV4             (0x1U << SPI_CR1_BR_Pos)        // fPCLK/4
#define SPI_BR_DIV8             (0x2U << SPI_CR1_BR_Pos)        // fPCLK/8
#define SPI_BR_DIV16            (0x3U << SPI_CR1_BR_Pos)        // fPCLK/16
#define SPI_BR_DIV32            (0x4U << SPI_CR1_BR_Pos)        // fPCLK/32
#define SPI_BR_DIV64            (0x5U << SPI_CR1_BR_Pos)        // fPCLK/64
#define SPI_BR_DIV128           (0x6U << SPI_CR1_BR_Pos)        // fPCLK/128
#define SPI_BR_DIV256           (0x7U << SPI_CR1_BR_Pos)        // fPCLK/256

/* ============================================================================ */
/*                    SPI_CR2 - Control Register 2 Bits                         */
/* ============================================================================ */

// Bit positions
#define SPI_CR2_RXDMAEN_Pos     0
#define SPI_CR2_TXDMAEN_Pos     1
#define SPI_CR2_SSOE_Pos        2
#define SPI_CR2_FRF_Pos         4
#define SPI_CR2_ERRIE_Pos       5
#define SPI_CR2_RXNEIE_Pos      6
#define SPI_CR2_TXEIE_Pos       7

// Bit masks
#define SPI_CR2_RXDMAEN         (1U << SPI_CR2_RXDMAEN_Pos)     // RX Buffer DMA Enable
#define SPI_CR2_TXDMAEN         (1U << SPI_CR2_TXDMAEN_Pos)     // TX Buffer DMA Enable
#define SPI_CR2_SSOE            (1U << SPI_CR2_SSOE_Pos)        // SS Output Enable
#define SPI_CR2_FRF             (1U << SPI_CR2_FRF_Pos)         // Frame Format
#define SPI_CR2_ERRIE           (1U << SPI_CR2_ERRIE_Pos)       // Error Interrupt Enable
#define SPI_CR2_RXNEIE          (1U << SPI_CR2_RXNEIE_Pos)      // RXNE Interrupt Enable
#define SPI_CR2_TXEIE           (1U << SPI_CR2_TXEIE_Pos)       // TXE Interrupt Enable

/* ============================================================================ */
/*                    SPI_SR - Status Register Bits                             */
/* ============================================================================ */

// Bit positions
#define SPI_SR_RXNE_Pos         0
#define SPI_SR_TXE_Pos          1
#define SPI_SR_CHSIDE_Pos       2
#define SPI_SR_UDR_Pos          3
#define SPI_SR_CRCERR_Pos       4
#define SPI_SR_MODF_Pos         5
#define SPI_SR_OVR_Pos          6
#define SPI_SR_BSY_Pos          7

// Bit masks
#define SPI_SR_RXNE             (1U << SPI_SR_RXNE_Pos)         // Receive Buffer Not Empty
#define SPI_SR_TXE              (1U << SPI_SR_TXE_Pos)          // Transmit Buffer Empty
#define SPI_SR_CHSIDE           (1U << SPI_SR_CHSIDE_Pos)       // Channel Side (I2S)
#define SPI_SR_UDR              (1U << SPI_SR_UDR_Pos)          // Underrun Flag
#define SPI_SR_CRCERR           (1U << SPI_SR_CRCERR_Pos)       // CRC Error Flag
#define SPI_SR_MODF             (1U << SPI_SR_MODF_Pos)         // Mode Fault
#define SPI_SR_OVR              (1U << SPI_SR_OVR_Pos)          // Overrun Flag
#define SPI_SR_BSY              (1U << SPI_SR_BSY_Pos)          // Busy Flag







#endif /* __STM32_SPI_REGISTERS_H */






