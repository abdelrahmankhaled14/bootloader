#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include "std_types.h"



typedef enum
{
    /* AHB1ENR */
    GPIOA_EN = 0,
    GPIOB_EN,
    GPIOC_EN,
    GPIOD_EN,
    GPIOE_EN,
    RESERVED_AHB1_5,
    RESERVED_AHB1_6,
    GPIOH_EN,
    RESERVED_AHB1_8,
    RESERVED_AHB1_9,
    RESERVED_AHB1_10,
    RESERVED_AHB1_11,
    CRC_EN,
    RESERVED_AHB1_13,
    RESERVED_AHB1_14,
    RESERVED_AHB1_15,
    RESERVED_AHB1_16,
    RESERVED_AHB1_17,
    RESERVED_AHB1_18,
    RESERVED_AHB1_19,
    RESERVED_AHB1_20,
    DMA1_EN,      
    DMA2_EN,
    RESERVED_AHB1_23,
    RESERVED_AHB1_24,
    RESERVED_AHB1_25,
    RESERVED_AHB1_26,
    RESERVED_AHB1_27,
    RESERVED_AHB1_28,
    RESERVED_AHB1_29,
    RESERVED_AHB1_30,
    RESERVED_AHB1_31,

    /* AHB2ENR */
    RESERVED_AHB2_0 = 32,
    RESERVED_AHB2_1,
    RESERVED_AHB2_2,
    RESERVED_AHB2_3,
    RESERVED_AHB2_4,
    RESERVED_AHB2_5,
    RESERVED_AHB2_6,
    OTGFS_EN,
    RESERVED_AHB2_8,
    RESERVED_AHB2_9,
    RESERVED_AHB2_10,
    RESERVED_AHB2_11,
    RESERVED_AHB2_12,
    RESERVED_AHB2_13,
    RESERVED_AHB2_14,
    RESERVED_AHB2_15,
    RESERVED_AHB2_16,
    RESERVED_AHB2_17,
    RESERVED_AHB2_18,
    RESERVED_AHB2_19,
    RESERVED_AHB2_20,
    RESERVED_AHB2_21,
    RESERVED_AHB2_22,
    RESERVED_AHB2_23,
    RESERVED_AHB2_24,
    RESERVED_AHB2_25,
    RESERVED_AHB2_26,
    RESERVED_AHB2_27,
    RESERVED_AHB2_28,
    RESERVED_AHB2_29,
    RESERVED_AHB2_30,
    RESERVED_AHB2_31,

    /* APB1ENR */
    TIM2_EN = 64,
    TIM3_EN,
    TIM4_EN,
    TIM5_EN,
    TIM6_EN,
    RESERVED_APB1_6,
    RESERVED_APB1_7,
    RESERVED_APB1_8,
    RESERVED_APB1_9,
    RESERVED_APB1_10,
    WWDG_EN,
    RESERVED_APB1_12,
    RESERVED_APB1_13,
    SPI2_EN,
    SPI3_EN,
    USART2_EN,
    RESERVED_APB1_17,
    RESERVED_APB1_18,
    RESERVED_APB1_19,
    RESERVED_APB1_20,
    I2C1_EN,
    I2C2_EN,
    I2C3_EN,
    RESERVED_APB1_24,
    RESERVED_APB1_25,
    RESERVED_APB1_26,
    RESERVED_APB1_27,
    PWR_EN,
    RESERVED_APB1_29,
    RESERVED_APB1_30,
    RESERVED_APB1_31,
    

    /* APB2ENR */
    TIM1_EN = 96,
    RESERVED_APB2_1,
    RESERVED_APB2_2,
    RESERVED_APB2_3,
    USART1_EN,
    USART6_EN,
    RESERVED_APB2_6,
    RESERVED_APB2_7,
    ADC1_EN,
    RESERVED_APB2_9,
    RESERVED_APB2_10,
    SDIO_EN,
    SPI1_EN,
    SPI4_EN,
    SYSCFG_EN,
    RESERVED_APB2_15,
    TIM9_EN,
    TIM10_EN,
    TIM11_EN,
    RESERVED_APB2_19,
    RESERVED_APB2_20,
    RESERVED_APB2_21,
    RESERVED_APB2_22,
    RESERVED_APB2_23,
    RESERVED_APB2_24,
    RESERVED_APB2_25,
    RESERVED_APB2_26,
    RESERVED_APB2_27,
    RESERVED_APB2_28,
    RESERVED_APB2_29,
    RESERVED_APB2_30,
    RESERVED_APB2_31

} RCC_Peripheral_t;

typedef enum
{
    Src_HSI,Src_HSE
}Pll_Src_t;
typedef enum
{
    HSI,HSE,PLL
}sysclk_source_t;
typedef enum
{
    AHB1_BUS,AHB2_BUS,APB1_BUS,APB2_BUS
}bus_t;
/***************Configration***************/
#define SYSCLK_SOURCE    PLL
#define PLL_SOURCE       Src_HSI
#define PLL_M            8
#define PLL_N            64    
#define PLL_P            3   /* 0:2 ,1:4,2:6,3:8 */
#define PLL_Q            7
/****************Prototype*************/
void rcc_set_sysclk(sysclk_source_t sysclk_source);
void rcc_ctrl_sysclk(sysclk_source_t clk, state_t state);
void rcc_pll_config(uint32_t pllm, uint32_t plln, uint32_t pllp, uint32_t pllq,Pll_Src_t clk_source);
void rcc_clk_perpheral_enable(bus_t bus, RCC_Peripheral_t periph);
void rcc_clk_perpheral_disable(bus_t bus, RCC_Peripheral_t periph);
void rcc_reset_peripheral(bus_t bus, RCC_Peripheral_t periph);
void rcc_init(void);
/*void rcc_setcfg(uint32_t ahb_prescaler, uint32_t apb1_prescaler, uint32_t apb2_prescaler);*/


#endif // RCC_H


