#ifndef __DMA_PRIVATE
#define __DMA_PRIVATE

/******************************************************************************
 * DMA Interrupt Status Register (LISR/HISR) Bit Definitions
 ******************************************************************************/
#define TCIE   4   // Transfer Complete Interrupt Enable
#define TEIE   2   // Transfer Error Interrupt Enable

/******************************************************************************
 * DMA Interrupt Flag Status Register (LISR/HISR) Bit Definitions
 ******************************************************************************/
#define TCIF0  5   // Transfer Complete Interrupt Flag Stream 0
#define TEIF0  3   // Transfer Error Interrupt Flag Stream 0
#define TCIF4  5   // Transfer Complete Interrupt Flag Stream 4
#define TEIF4  3   // Transfer Error Interrupt Flag Stream 4

/******************************************************************************
 * DMA Interrupt Flag Clear Register (LIFCR/HIFCR) Bit Definitions
 ******************************************************************************/
#define CTCIF0 5   // Clear Transfer Complete Interrupt Flag Stream 0
#define CTEIF0 3   // Clear Transfer Error Interrupt Flag Stream 0
#define CTCIF4 5   // Clear Transfer Complete Interrupt Flag Stream 4
#define CTEIF4 3   // Clear Transfer Error Interrupt Flag Stream 4

/******************************************************************************
 * DMA Streamx Configuration Register (CR) Bit Definitions
 ******************************************************************************/
#define EN       0   /**< Stream enable */
#define DMEIE    1   /**< Direct mode error interrupt enable */
#define TEIE     2   /**< Transfer error interrupt enable */
#define HTIE     3   /**< Half transfer interrupt enable */
#define TCIE     4   /**< Transfer complete interrupt enable */
#define PFCTRL   5   /**< Peripheral flow controller */
#define DIR      6   /**< Data transfer direction bit 0 */
#define CIRC     8   /**< Circular mode enable */
#define PINC     9   /**< Peripheral increment mode */
#define MINC    10   /**< Memory increment mode */
#define PSIZE   11   /**< Peripheral data size bit 0 */
#define MSIZE   13   /**< Memory data size bit 0 */
#define PL      15   /**< Priority level bit 0 */
#define DBM     18   /**< Double buffer mode */
#define CT      19   /**< Current target (double buffer) */
#define CHSEL   25   /**< Channel selection bit 0 */

/******************************************************************************
 * DMA Streamx FIFO Control Register (FCR) Bit Definitions
 ******************************************************************************/
#define FTH0      0   /**< FIFO threshold bit 0 */
#define FTH1      1   /**< FIFO threshold bit 1 */
#define DMDIS     2   /**< Direct mode disable */
#define FS0       3   /**< FIFO status bit 0 */
#define FS1       4   /**< FIFO status bit 1 */
#define FS2       5   /**< FIFO status bit 2 */
#define FEIE      7   /**< FIFO error interrupt enable */

/******************************************************************************
 * DMA Register Structures
 ******************************************************************************/
typedef struct {
    volatile uint32_t CR;       // Configuration register
    volatile uint32_t NDTR;     // Number of data register
    volatile uint32_t PAR;      // Peripheral address register
    volatile uint32_t M0AR;     // Memory 0 address register
    volatile uint32_t M1AR;     // Memory 1 address register
    volatile uint32_t FCR;      // FIFO control register
} DMA_Stream_Regs_t;

typedef struct {
    volatile uint32_t LISR;     // 0x00 - Low interrupt status register
    volatile uint32_t HISR;     // 0x04 - High interrupt status register
    volatile uint32_t LIFCR;    // 0x08 - Low interrupt flag clear register
    volatile uint32_t HIFCR;    // 0x0C - High interrupt flag clear register
    DMA_Stream_Regs_t Streamx[8];
} DMA_Registers_t;

/******************************************************************************
 * DMA Base Addresses
 ******************************************************************************/
// Base addresses
#define DMA1_BASE   0x40026000UL
#define DMA2_BASE   0x40026400UL 

#define DMA1        ((DMA_Registers_t*)DMA1_BASE)
#define DMA2        ((DMA_Registers_t*)DMA2_BASE)

// DMA pointers array
extern DMA_Registers_t * Dmax[2];

#endif /* __DMA_PRIVATE */