#include "mcal/dma/dma.h"
#include "mcal/dma/dma_private.h"
#include "mcal/rcc/rcc.h"

DMA_Registers_t *Dmax[2] = {DMA1, DMA2};
static DMA_Status_t g_Dma_state[2][8] = {{DMA_OK, DMA_OK, DMA_OK, DMA_OK, DMA_OK, DMA_OK, DMA_OK, DMA_OK},
                                         {DMA_OK, DMA_OK, DMA_OK, DMA_OK, DMA_OK, DMA_OK, DMA_OK, DMA_OK}};

static DMA_Callback_t g_DMA_Callbacks[2][8] = {
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
/* ================= DMA Helper Function ================= */

/* ================= DMA Init Functions ================== */
static void DMA_EnableClock(uint8_t Dma)
{
    if (Dma == DMA1_NUM)
    {
        rcc_clk_perpheral_enable(AHB1_BUS, DMA1_EN);
    }
    else if (Dma == DMA2_NUM)
    {
        rcc_clk_perpheral_enable(AHB1_BUS, DMA2_EN);
    }
}
static void DMA_FlowController(uint8_t Dma, uint8_t Stream , DMA_FlowController_t FlowControl)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(1 << PFCTRL);
    Dmax[Dma]->Streamx[Stream].CR |= (FlowControl << PFCTRL);
}
static void DMA_ChannalSelection(uint8_t Channal, uint8_t Dma, uint8_t Stream)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(0x7 << CHSEL);
    Dmax[Dma]->Streamx[Stream].CR |= (Channal << CHSEL);
}
static void DMA_EnableDmaStream(uint8_t Dma, uint8_t Stream)
{
    Dmax[Dma]->Streamx[Stream].CR |= (1 << EN);
}
static DMA_Status_t DMA_DisableDmaStream(uint8_t Dma, uint8_t Stream)
{
    volatile uint32_t timeout = 1000;
    Dmax[Dma]->Streamx[Stream].CR &= ~(1 << EN);
    while ((Dmax[Dma]->Streamx[Stream].CR & (1 << EN)) && timeout)
    {
        timeout--;
    }
    return (timeout > 0) ? DMA_OK : DMA_ERROR;
}
static void DMA_ModeSelection(DMA_Mode_t mode, uint8_t Dma, uint8_t Stream)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(1 << CIRC);
    Dmax[Dma]->Streamx[Stream].CR |= (mode << CIRC);
}

static void DMA_SetDataDirection(uint8_t Dma, uint8_t Stream, DMA_Direction_t direction)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(0x3 << DIR);
    Dmax[Dma]->Streamx[Stream].CR |= (direction << DIR);
}
static void DMA_SetStreamPriority(uint8_t Dma, uint8_t Stream, DMA_Priority_t Priority)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(0x3 << PL);
    Dmax[Dma]->Streamx[Stream].CR |= (Priority << PL);
}
static void DMA_SetStreamPeriphralDataWitdth(uint8_t Dma, uint8_t Stream, DMA_DataWidth_t PeripheralDataWidth)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(0x3 << PSIZE);
    Dmax[Dma]->Streamx[Stream].CR |= (PeripheralDataWidth << PSIZE);
}
static void DMA_SetStreamMemoryDataWitdth(uint8_t Dma, uint8_t Stream, DMA_DataWidth_t MemoryDataWidth)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(0x3 << MSIZE);
    Dmax[Dma]->Streamx[Stream].CR |= (MemoryDataWidth << MSIZE);
}
static void DMA_SetStreamPeriphralIncMode(uint8_t Dma, uint8_t Stream, DMA_IncrementMode_t PerphralInc)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(1 << PINC);
    Dmax[Dma]->Streamx[Stream].CR |= (PerphralInc << PINC);
}
static void DMA_SetStreamMemoryIncMode(uint8_t Dma, uint8_t Stream, DMA_IncrementMode_t MemoryInc)
{
    Dmax[Dma]->Streamx[Stream].CR &= ~(1 << MINC);
    Dmax[Dma]->Streamx[Stream].CR |= (MemoryInc << MINC);
}

static void DMA_SetDataLength(uint8_t Dma, uint8_t Stream, uint16_t length)
{
    Dmax[Dma]->Streamx[Stream].NDTR = length;
}
static void DMA_SetPerphralAddress(uint8_t Dma, uint8_t Stream, uint32_t *Address)
{
    Dmax[Dma]->Streamx[Stream].PAR = (uint32_t)Address;
}
static void DMA_SetMemoryAddress(uint8_t Dma, uint8_t Stream, uint32_t *Address)
{
    Dmax[Dma]->Streamx[Stream].M0AR = (uint32_t)Address;
}

/* =================== Read flags ========================= */

uint8_t DMA_ReadTransferCompleteFlag(uint8_t dma_num, uint8_t stream_num)
{
    volatile uint32_t status_reg;
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        status_reg = Dmax[dma_num]->LISR;
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
    }
    else
    {
        status_reg = Dmax[dma_num]->HISR;
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
    }

    return (status_reg >> (bit_offset + 5)) & 0x01;
}

uint8_t DMA_ReadHalfTransferCompleteFlag(uint8_t dma_num, uint8_t stream_num)
{
    volatile uint32_t status_reg;
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        status_reg = Dmax[dma_num]->LISR;
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
    }
    else
    {
        status_reg = Dmax[dma_num]->HISR;
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
    }

    return (status_reg >> (bit_offset + 4)) & 0x01;
}

uint8_t DMA_ReadTransferErrorFlag(uint8_t dma_num, uint8_t stream_num)
{
    volatile uint32_t status_reg;
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        status_reg = Dmax[dma_num]->LISR;
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
    }
    else
    {
        status_reg = Dmax[dma_num]->HISR;
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
    }

    return (status_reg >> (bit_offset + 3)) & 0x01;
}

uint8_t DMA_ReadDirectModeErrorFlag(uint8_t dma_num, uint8_t stream_num)
{
    volatile uint32_t status_reg;
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        status_reg = Dmax[dma_num]->LISR;
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
    }
    else
    {
        status_reg = Dmax[dma_num]->HISR;
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
    }

    return (status_reg >> (bit_offset + 2)) & 0x01;
}

uint8_t DMA_ReadFIFOErrorFlag(uint8_t dma_num, uint8_t stream_num)
{
    volatile uint32_t status_reg;
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        status_reg = Dmax[dma_num]->LISR;
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
    }
    else
    {
        status_reg = Dmax[dma_num]->HISR;
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
    }

    return (status_reg >> (bit_offset + 0)) & 0x01;
}

/* ====================== Clear Flags ======================= */
void DMA_ClearTransferCompleteFlag(uint8_t dma_num, uint8_t stream_num)
{
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
        Dmax[dma_num]->LIFCR = (1 << (bit_offset + 5));
    }
    else
    {
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
        Dmax[dma_num]->HIFCR = (1 << (bit_offset + 5));
    }
}

void DMA_ClearHalfTransferCompleteFlag(uint8_t dma_num, uint8_t stream_num)
{
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
        Dmax[dma_num]->LIFCR = (1 << (bit_offset + 4));
    }
    else
    {
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
        Dmax[dma_num]->HIFCR = (1 << (bit_offset + 4));
    }
}

void DMA_ClearTransferErrorFlag(uint8_t dma_num, uint8_t stream_num)
{
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
        Dmax[dma_num]->LIFCR = (1 << (bit_offset + 3));
    }
    else
    {
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
        Dmax[dma_num]->HIFCR = (1 << (bit_offset + 3));
    }
}

void DMA_ClearDirectModeErrorFlag(uint8_t dma_num, uint8_t stream_num)
{
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
        Dmax[dma_num]->LIFCR = (1 << (bit_offset + 2));
    }
    else
    {
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
        Dmax[dma_num]->HIFCR = (1 << (bit_offset + 2));
    }
}

void DMA_ClearFIFOErrorFlag(uint8_t dma_num, uint8_t stream_num)
{
    uint8_t bit_offset;

    if (stream_num < 4)
    {
        bit_offset = stream_num * 6;
        if (stream_num >= 2)
            bit_offset += 4;
        Dmax[dma_num]->LIFCR = (1 << (bit_offset + 0));
    }
    else
    {
        bit_offset = (stream_num - 4) * 6;
        if (stream_num >= 6)
            bit_offset += 4;
        Dmax[dma_num]->HIFCR = (1 << (bit_offset + 0));
    }
}

/* ==================== DMA Inable and disable inturrupt ====================== */
void DMA_EnableTransferCompleteInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR |= (1 << TCIE);
}

void DMA_DisableTransferCompleteInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR &= ~(1 << TCIE);
}

void DMA_EnableHalfTransferInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR |= (1 << HTIE);
}

void DMA_DisableHalfTransferInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR &= ~(1 << HTIE);
}

void DMA_EnableTransferErrorInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR |= (1 << TEIE);
}

void DMA_DisableTransferErrorInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR &= ~(1 << TEIE);
}

void DMA_EnableDirectModeErrorInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR |= (1 << DMEIE);
}

void DMA_DisableDirectModeErrorInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR &= ~(1 << DMEIE);
}

void DMA_EnableFIFOErrorInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].FCR |= (1 << FEIE);
}

void DMA_DisableFIFOErrorInterrupt(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].FCR &= ~(1 << FEIE);
}

void DMA_EnableAllInterrupts(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR |= (1 << TCIE) | (1 << HTIE) | (1 << TEIE) | (1 << DMEIE);
    Dmax[dma_num]->Streamx[stream_num].FCR |= (1 << FEIE);
}

void DMA_DisableAllInterrupts(uint8_t dma_num, uint8_t stream_num)
{
    Dmax[dma_num]->Streamx[stream_num].CR &= ~((1 << TCIE) | (1 << HTIE) | (1 << TEIE) | (1 << DMEIE));
    Dmax[dma_num]->Streamx[stream_num].FCR &= ~(1 << FEIE);
}

/********************************************************
 ================= APIs Implementation =================
 ******************************************************* */

void DMA_Init(DMA_Config_t *config)
{
    DMA_Status_t DMA_Status;
    uint8_t Dma;
    uint8_t Stream;
    uint8_t Channal;
    if (config == NULL)
    {
        /*Do nothing*/
    }
    else
    {
        Dma = (config->Request >> 6) & 0x01;
        Stream = (config->Request >> 3) & 0x07;
        Channal = (config->Request & 0x07);
        DMA_EnableClock(Dma);
        DMA_Status = DMA_DisableDmaStream(Dma, Stream);
        if (DMA_Status == DMA_ERROR)
        {
            return;
        }

        DMA_ChannalSelection(Channal, Dma, Stream);
        DMA_SetDataDirection(Dma, Stream, config->Direction);
        DMA_SetStreamPriority(Dma, Stream, config->Priority);
        DMA_SetStreamPeriphralDataWitdth(Dma, Stream, config->PeripheralDataWidth);
        DMA_SetStreamMemoryDataWitdth(Dma, Stream, config->MemoryDataWidth);
        DMA_SetStreamPeriphralIncMode(Dma, Stream, config->PeripheralInc);
        DMA_SetStreamMemoryIncMode(Dma, Stream, config->MemoryInc);
        DMA_ModeSelection(config->Mode, Dma, Stream);
        DMA_FlowController(Dma, Stream , config->FlowController);

    }
}

DMA_Status_t DMA_Start(DMA_Config_t *config)
{
    uint8_t Dma;
    uint8_t Stream;
    if (config == NULL)
    {
        return DMA_ERROR;
    }
    else if (g_Dma_state[(config->Request >> 6) & 0x01][(config->Request >> 3) & 0x07] == DMA_BUSY)
    {
        return DMA_BUSY;
    }
    else if (config->Request == DMA_MEM2MEM_DMA2_STREAM1_CHANNEL1)
    {
        Dma = DMA2_NUM;
        Stream = STREAM1_NUM;
        DMA_EnableDmaStream(Dma, Stream);
        DMA_EnableTransferCompleteInterrupt(Dma, Stream);
        g_Dma_state[Dma][Stream] = DMA_BUSY;
        return DMA_OK;
    }
    else
    {
        Dma = (config->Request >> 6) & 0x01;
        Stream = (config->Request >> 3) & 0x07;
        DMA_SetMemoryAddress(Dma, Stream, config->MemoryAddress);
        DMA_SetPerphralAddress(Dma, Stream, config->PeripheralAddress);
        DMA_SetDataLength(Dma, Stream, config->DataLength);
        DMA_EnableDmaStream(Dma, Stream);
        DMA_EnableTransferCompleteInterrupt(Dma, Stream);
        g_Dma_state[Dma][Stream] = DMA_BUSY;
        return DMA_OK;
    }
}

signed int DMA_GetTransferDataNum(DMA_Config_t *config)
{
    uint8_t Dma;
    uint8_t Stream;
    if (config == NULL)
    {
        return -1;
    }
    else
    {
        Dma = (config->Request >> 6) & 0x01;
        Stream = (config->Request >> 3) & 0x07;
        return (Dmax[Dma]->Streamx[Stream].NDTR);
    }
}

DMA_Status_t DMA_GetTransferStatus(DMA_Config_t *config)
{
    uint8_t Dma;
    uint8_t Stream;
    if (config == NULL)
    {
        return DMA_ERROR;
    }
    else
    {
        Dma = (config->Request >> 6) & 0x01;
        Stream = (config->Request >> 3) & 0x07;
        if (g_Dma_state[Dma][Stream] == DMA_BUSY)
        {
            return DMA_BUSY;
        }
        else
        {
            return DMA_OK;
        }
    }
}

DMA_Status_t DMA_MemCpy(uint32_t *dest, uint32_t *src, uint16_t length)
{
    if ((dest == NULL) || (src == NULL) || (length == 0))
    {
        return DMA_ERROR;
    }
    else if (dest == src)
    {
        return DMA_OK;
    }
    else if (g_Dma_state[DMA2_NUM][STREAM1_NUM] == DMA_BUSY)
    {
        return DMA_BUSY;
    }
    else
    {

        DMA_Config_t dma_config;
        dma_config.Direction = DMA_DIR_MEM_TO_MEM;
        dma_config.MemoryDataWidth = DMA_DATA_WIDTH_32BIT;
        dma_config.PeripheralDataWidth = DMA_DATA_WIDTH_32BIT;
        dma_config.MemoryInc = DMA_INC_ENABLE;
        dma_config.PeripheralInc = DMA_INC_ENABLE;
        dma_config.Priority = DMA_PRIORITY_HIGH;
        dma_config.Mode = DMA_MODE_NORMAL;
        dma_config.MemoryAddress = dest;
        dma_config.PeripheralAddress = src;
        dma_config.DataLength = length;
        dma_config.Request = DMA_MEM2MEM_DMA2_STREAM1_CHANNEL1;
        dma_config.FlowController = DMA_FLOW_CTRL_DMA;

        DMA_Init(&dma_config);
        return DMA_Start(&dma_config);
    }
}

/* ================= DMA IRQ Handler ================= */


void DMA_SetIRQHANDLER(uint8_t Dma, uint8_t Stream, DMA_Callback_t callback)
{
    if (Dma < 2 && Stream < 8)
    {
        g_DMA_Callbacks[Dma][Stream] = callback;
    }
}



/* ================= Real IRQ =====================*/
void DMA1_Stream0_IRQHandler(void)
{
    g_DMA_Callbacks[DMA1_NUM][STREAM0_NUM](DMA1_NUM, STREAM0_NUM);
}
void DMA1_Stream1_IRQHandler(void)
{
    g_DMA_Callbacks[DMA1_NUM][STREAM1_NUM](DMA1_NUM, STREAM1_NUM);
}

void DMA1_Stream2_IRQHandler(void)
{
    g_DMA_Callbacks[DMA1_NUM][STREAM2_NUM](DMA1_NUM, STREAM2_NUM);}

void DMA1_Stream3_IRQHandler(void)
{
    g_DMA_Callbacks[DMA1_NUM][STREAM3_NUM](DMA1_NUM, STREAM3_NUM);
}

void DMA1_Stream4_IRQHandler(void)
{
    g_DMA_Callbacks[DMA1_NUM][STREAM4_NUM](DMA1_NUM, STREAM4_NUM);
}

void DMA1_Stream5_IRQHandler(void)
{
    g_DMA_Callbacks[DMA1_NUM][STREAM5_NUM](DMA1_NUM, STREAM5_NUM);
}

void DMA1_Stream6_IRQHandler(void)
{
    g_DMA_Callbacks[DMA1_NUM][STREAM6_NUM](DMA1_NUM, STREAM6_NUM);
}

void DMA1_Stream7_IRQHandler(void)
{
    g_DMA_Callbacks[DMA1_NUM][STREAM7_NUM](DMA1_NUM, STREAM7_NUM);
}

void DMA2_Stream0_IRQHandler(void)
{
    g_DMA_Callbacks[DMA2_NUM][STREAM0_NUM](DMA2_NUM, STREAM0_NUM);
}
void DMA2_Stream1_IRQHandler(void)
{
    g_DMA_Callbacks[DMA2_NUM][STREAM1_NUM](DMA2_NUM, STREAM1_NUM);
}

void DMA2_Stream2_IRQHandler(void)
{
    g_DMA_Callbacks[DMA2_NUM][STREAM2_NUM](DMA2_NUM, STREAM2_NUM);
}

void DMA2_Stream3_IRQHandler(void)
{
    g_DMA_Callbacks[DMA2_NUM][STREAM3_NUM](DMA2_NUM, STREAM3_NUM);
}

void DMA2_Stream4_IRQHandler(void)
{
    g_DMA_Callbacks[DMA2_NUM][STREAM4_NUM](DMA2_NUM, STREAM4_NUM);
}

void DMA2_Stream5_IRQHandler(void)
{
    g_DMA_Callbacks[DMA2_NUM][STREAM5_NUM](DMA2_NUM, STREAM5_NUM);
}

void DMA2_Stream6_IRQHandler(void)
{
    g_DMA_Callbacks[DMA2_NUM][STREAM6_NUM](DMA2_NUM, STREAM6_NUM);
}

void DMA2_Stream7_IRQHandler(void)
{
    g_DMA_Callbacks[DMA2_NUM][STREAM7_NUM](DMA2_NUM, STREAM7_NUM);
}
