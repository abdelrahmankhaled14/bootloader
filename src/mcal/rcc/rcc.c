#include "mcal/rcc/rcc.h"
#include "macros.h"


#define RCC_BASE_ADDR 0x40023800
typedef struct{
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    uint32_t RESERVED0[2];
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    uint32_t RESERVED2[2];
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    uint32_t RESERVED3[2];
    volatile uint32_t AHB1LPENR;
    volatile uint32_t AHB2LPENR;
    uint32_t RESERVED4[2];
    volatile uint32_t APB1LPENR;
    volatile uint32_t APB2LPENR;
    uint32_t RESERVED5[2];
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    uint32_t RESERVED6[2];
    volatile uint32_t SSCGR;
    volatile uint32_t PLLI2SCFGR;
    uint32_t RESERVED7;
    volatile uint32_t DCKCFGR;
} RCC_Registers_t;


RCC_Registers_t* const RCC_Registers = (RCC_Registers_t*)RCC_BASE_ADDR;










void rcc_init(void) 
{
    /* Enable HSE */
    rcc_ctrl_sysclk(HSI,ENABLE);

    /* Configure PLL: PLLM=8, PLLN=336, PLLP=2, PLLQ=7, Source=HSE */
    rcc_pll_config(PLL_M,PLL_N,PLL_P,PLL_Q,PLL_SOURCE);

    /* Enable PLL */
    rcc_ctrl_sysclk(PLL,ENABLE);

    /* Set Flash latency to 5 wait states */
    
    *(uint32_t*)((0x40000000UL + 0x00020000UL) + 0x3C00UL) &= ~0b111;
    *(uint32_t*)((0x40000000UL + 0x00020000UL) + 0x3C00UL) |= (2 & 0b111);
    /* Select PLL as system clock source */
    rcc_set_sysclk(PLL);
    while(((RCC_Registers -> CFGR) >> 2 & 0x3) != PLL); 
    
}           


void rcc_pll_config(uint32_t pllm, uint32_t plln, uint32_t pllp, uint32_t pllq,Pll_Src_t clk_source) 
{
    if(clk_source == Src_HSE)
    {
        SET_BIT(RCC_Registers->PLLCFGR,22); /*Set PLL Source to HSE */
    }
    else
    {
        CLEAR_BIT(RCC_Registers->PLLCFGR,22); /*Set PLL Source to HSI */
    }
/* Set PLLM (bits 5:0) */
RCC_Registers->PLLCFGR = (RCC_Registers->PLLCFGR & ~(0x3F << 0)) | ((pllm & 0x3F) << 0);

/* Set PLLN (bits 14:6) */
RCC_Registers->PLLCFGR = (RCC_Registers->PLLCFGR & ~(0x1FF << 6)) | ((plln & 0x1FF) << 6);

/* Set PLLP (bits 17:16) */
RCC_Registers->PLLCFGR = (RCC_Registers->PLLCFGR & ~(0x3 << 16)) | ((pllp & 0x3) << 16);

/* Set PLLQ (bits 27:24) */
RCC_Registers->PLLCFGR = (RCC_Registers->PLLCFGR & ~(0xF << 24)) | ((pllq & 0xF) << 24);

}
void rcc_set_sysclk(sysclk_source_t sysclk_source) 
{
    RCC_Registers->CFGR = ((RCC_Registers -> CFGR & ~(0x00000003 << 0)) | ((sysclk_source &  0x00000003) << 0 )); /*Set SysClk */
}
void rcc_ctrl_sysclk(sysclk_source_t sysclk_source,  state_t state)
{
    switch (sysclk_source)
    {
    case HSE:
        if(state == ENABLE)
        {
            SET_BIT(RCC_Registers->CR,16); 
            while(READ_BIT(RCC_Registers->CR,17) == 0); 
        }
        else
        {
            CLEAR_BIT(RCC_Registers->CR,16);
            
        }
        break;
    case HSI:
        if(state == ENABLE)
        {
            SET_BIT(RCC_Registers->CR,0);
            while(READ_BIT(RCC_Registers->CR,1) == 0);
        }
        else
        {
            CLEAR_BIT(RCC_Registers->CR,0); 
        }
        break;
    case PLL:
        if(state == ENABLE)
        {
            SET_BIT(RCC_Registers->CR,24); 
            while(READ_BIT(RCC_Registers->CR,25) == 0);
        }
        else
        {
            CLEAR_BIT(RCC_Registers->CR,24); 
        }
        break;
    default:
        break;
    }
}
void rcc_clk_perpheral_disable(bus_t bus, RCC_Peripheral_t periph) {
    volatile int index = 0;
    if(periph >31)
    {
         index = periph % 32;
    }
    else
    {
         index = periph;
    }
    switch (bus) 
    {
        case AHB1_BUS: 
            CLEAR_BIT(RCC_Registers->AHB1ENR, index);
            break;
        case AHB2_BUS: 
            CLEAR_BIT(RCC_Registers->AHB2ENR, index);        
            break;
        case APB1_BUS: 
            CLEAR_BIT(RCC_Registers->APB1ENR, index);
            break;
        case APB2_BUS:
            CLEAR_BIT(RCC_Registers->APB2ENR, index);
            break;
        default:
            break;
    }
}  
void rcc_clk_perpheral_enable(bus_t bus, RCC_Peripheral_t periph) 
{
    volatile int index = 0;
    if(periph > 31)
    {
         index = periph % 32;
    }
    else
    {
         index = periph;
    }
    switch (bus) 
    {
        case AHB1_BUS:
            SET_BIT(RCC_Registers->AHB1ENR, index);
            break;
        case AHB2_BUS:
            SET_BIT(RCC_Registers->AHB2ENR, index);
            break;
        case APB1_BUS:
            SET_BIT(RCC_Registers->APB1ENR, index);
            break;
        case APB2_BUS:
            SET_BIT(RCC_Registers->APB2ENR, index);
            break;
        default:
            break;
    }
}
void rcc_reset_peripheral(uint8_t bus, RCC_Peripheral_t periph) 
{
    volatile int index = 0;
    if(periph >31)
    {
         index = periph % 32;
    }
    else
    {
         index = periph;
    }
    switch (bus) {
        case AHB1_BUS: // AHB1
            SET_BIT(RCC_Registers->AHB1RSTR, index);
            CLEAR_BIT(RCC_Registers->AHB1RSTR, index);
            break;
        case AHB2_BUS: // AHB2
            SET_BIT(RCC_Registers->AHB2RSTR, index);
            CLEAR_BIT(RCC_Registers->AHB2RSTR, index);        
            break;
        case APB1_BUS: // APB1             
            SET_BIT(RCC_Registers->APB1RSTR, index);
            CLEAR_BIT(RCC_Registers->APB1RSTR, index);
            break;
        case APB2_BUS: // APB2
            SET_BIT(RCC_Registers->APB2RSTR, index);
            CLEAR_BIT(RCC_Registers->APB2RSTR, index);
            break;
        default:
            break;
    }
}    
