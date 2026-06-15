#ifndef _GPIO_PRIVATE_H
#define _GPIO_PRIVATE_H


#include "std_types.h"
/*************************************************************************************************************************
 *                                          GPIO Type Defintion                                                        *
 *************************************************************************************************************************/
typedef struct 
{
        volatile uint32_t MODER0  :2;
        volatile uint32_t MODER1  :2;   
        volatile uint32_t MODER2  :2;   
        volatile uint32_t MODER3  :2;   
        volatile uint32_t MODER4  :2;   
        volatile uint32_t MODER5  :2;   
        volatile uint32_t MODER6  :2;
        volatile uint32_t MODER7  :2;   
        volatile uint32_t MODER8  :2;   
        volatile uint32_t MODER9  :2;   
        volatile uint32_t MODER10 :2;   
        volatile uint32_t MODER11 :2;   
        volatile uint32_t MODER12 :2;   
        volatile uint32_t MODER13 :2;   
        volatile uint32_t MODER14 :2;   
        volatile uint32_t MODER15 :2;       
        
}MODER_Bits_t;

typedef struct 
{
        volatile uint32_t OT0  :1;
        volatile uint32_t OT1  :1;   
        volatile uint32_t OT2  :1;
        volatile uint32_t OT3  :1;   
        volatile uint32_t OT4  :1;   
        volatile uint32_t OT5  :1;
        volatile uint32_t OT6  :1;   
        volatile uint32_t OT7  :1;   
        volatile uint32_t OT8  :1;   
        volatile uint32_t OT9  :1;   
        volatile uint32_t OT10 :1;   
        volatile uint32_t OT11 :1;   
        volatile uint32_t OT12 :1;   
        volatile uint32_t OT13 :1;   
        volatile uint32_t OT14 :1;   
        volatile uint32_t OT15 :1; 
        volatile uint32_t reserved :16;          
        
}OTYPER_Bits_t;

typedef struct 
{
    volatile uint32_t OSPEEDR0  :2;
    volatile uint32_t OSPEEDR1  :2;   
    volatile uint32_t OSPEEDR2  :2;   
    volatile uint32_t OSPEEDR3  :2;
    volatile uint32_t OSPEEDR4  :2;   
    volatile uint32_t OSPEEDR5  :2;
    volatile uint32_t OSPEEDR6  :2;   
    volatile uint32_t OSPEEDR7  :2;
    volatile uint32_t OSPEEDR8  :2;   
    volatile uint32_t OSPEEDR9  :2;
    volatile uint32_t OSPEEDR10 :2;   
    volatile uint32_t OSPEEDR11 :2;   
    volatile uint32_t OSPEEDR12 :2;         
    volatile uint32_t OSPEEDR13 :2;   
    volatile uint32_t OSPEEDR14 :2;   
    volatile uint32_t OSPEEDR15 :2;
}OSPEEDR_bits_t;

typedef struct 
{
    volatile uint32_t PUPDR0  :2;
    volatile uint32_t PUPDR1  :2;   
    volatile uint32_t PUPDR2  :2;   
    volatile uint32_t PUPDR3  :2;
    volatile uint32_t PUPDR4  :2;   
    volatile uint32_t PUPDR5  :2;
    volatile uint32_t PUPDR6  :2;   
    volatile uint32_t PUPDR7  :2;
    volatile uint32_t PUPDR8  :2;   
    volatile uint32_t PUPDR9  :2;
    volatile uint32_t PUPDR10 :2;   
    volatile uint32_t PUPDR11 :2;   
    volatile uint32_t PUPDR12 :2;
    volatile uint32_t PUPDR13 :2;   
    volatile uint32_t PUPDR14 :2;   
    volatile uint32_t PUPDR15 :2;   
}PUPDR_bits_t;

typedef struct 
{
    volatile uint32_t IDR0  :1;
    volatile uint32_t IDR1  :1;
    volatile uint32_t IDR2  :1;
    volatile uint32_t IDR3  :1;
    volatile uint32_t IDR4  :1;
    volatile uint32_t IDR5  :1;
    volatile uint32_t IDR6  :1;
    volatile uint32_t IDR7  :1;
    volatile uint32_t IDR8  :1;
    volatile uint32_t IDR9  :1;
    volatile uint32_t IDR10 :1;
    volatile uint32_t IDR11 :1;
    volatile uint32_t IDR12 :1;
    volatile uint32_t IDR13 :1;
    volatile uint32_t IDR14 :1;
    volatile uint32_t IDR15 :1;
    volatile uint32_t reserved:16;
}IDR_bits_t;

typedef struct 
{
    volatile uint32_t ODR0  :1;
    volatile uint32_t ODR1  :1;
    volatile uint32_t ODR2  :1;
    volatile uint32_t ODR3  :1;
    volatile uint32_t ODR4  :1;
    volatile uint32_t ODR5  :1;
    volatile uint32_t ODR6  :1;
    volatile uint32_t ODR7  :1;
    volatile uint32_t ODR8  :1;
    volatile uint32_t ODR9  :1;
    volatile uint32_t ODR10 :1;
    volatile uint32_t ODR11 :1;
    volatile uint32_t ODR12 :1;
    volatile uint32_t ODR13 :1;
    volatile uint32_t ODR14 :1;
    volatile uint32_t ODR15 :1;
    volatile uint32_t reserved:16;
}ODR_bits_t;

typedef struct 
{
    volatile uint32_t BS0  :1;
    volatile uint32_t BS1  :1;
    volatile uint32_t BS2  :1;
    volatile uint32_t BS3  :1;
    volatile uint32_t BS4  :1;    
    volatile uint32_t BS5  :1;
    volatile uint32_t BS6  :1;
    volatile uint32_t BS7  :1;
    volatile uint32_t BS8  :1;    
    volatile uint32_t BS9  :1;
    volatile uint32_t BS10 :1;
    volatile uint32_t BS11 :1;
    volatile uint32_t BS12 :1;    
    volatile uint32_t BS13 :1;
    volatile uint32_t BS14 :1;
    volatile uint32_t BS15 :1;
    volatile uint32_t BR0  :1;
    volatile uint32_t BR1  :1;
    volatile uint32_t BR2  :1;
    volatile uint32_t BR3  :1;
    volatile uint32_t BR4  :1;    
    volatile uint32_t BR5  :1;
    volatile uint32_t BR6  :1;
    volatile uint32_t BR7  :1;
    volatile uint32_t BR8  :1;    
    volatile uint32_t BR9  :1;
    volatile uint32_t BR10 :1;
    volatile uint32_t BR11 :1;
    volatile uint32_t BR12 :1;    
    volatile uint32_t BR13 :1;
    volatile uint32_t BR14 :1;
    volatile uint32_t BR15 :1;
}BSRR_bits_t;

typedef struct 
{
    volatile uint32_t LCK0  :1;
    volatile uint32_t LCK1  :1;
    volatile uint32_t LCK2  :1;
    volatile uint32_t LCK3  :1;
    volatile uint32_t LCK4  :1;    
    volatile uint32_t LCK5  :1;
    volatile uint32_t LCK6  :1;
    volatile uint32_t LCK7  :1;
    volatile uint32_t LCK8  :1;    
    volatile uint32_t LCK9  :1;
    volatile uint32_t LCK10 :1;
    volatile uint32_t LCK11 :1;
    volatile uint32_t LCK12 :1;    
    volatile uint32_t LCK13 :1;
    volatile uint32_t LCK14 :1;
    volatile uint32_t LCK15 :1;
    volatile uint32_t LCKK  :1;
    volatile uint32_t reserved:15;
}LCKR_bits_t;
typedef struct 
{
    volatile uint32_t AFRL0  :4;
    volatile uint32_t AFRL1  :4;   
    volatile uint32_t AFRL2  :4;   
    volatile uint32_t AFRL3  :4;
    volatile uint32_t AFRL4  :4;   
    volatile uint32_t AFRL5  :4;
    volatile uint32_t AFRL6  :4;   
    volatile uint32_t AFRL7  :4;   
}AFRL_bits_t;
    
typedef struct 
{
    volatile uint32_t AFRH8  :4;
    volatile uint32_t AFRH9  :4;   
    volatile uint32_t AFRH10 :4;   
    volatile uint32_t AFRH11 :4;
    volatile uint32_t AFRH12 :4;   
    volatile uint32_t AFRH13 :4;
    volatile uint32_t AFRH14 :4;   
    volatile uint32_t AFRH15 :4;
}AFRH_bits_t;











typedef union 
{
    ODR_bits_t Bits;
    volatile uint32_t  ODR;
}GPIO_ODR_t;

typedef union 
{
    IDR_bits_t Bits;
    volatile uint32_t  IDR;
}GPIO_IDR_t;

typedef union
{
    MODER_Bits_t Bits;
    volatile uint32_t  MODER;
}GPIO_MODER_t;

typedef union 
{
    OTYPER_Bits_t Bits;
    volatile uint32_t  OTYPER;
}GPIO_OTYPER_t;
typedef union 
{
    OSPEEDR_bits_t Bits;
    volatile uint32_t  OSPEEDR;
}GPIO_OSPEEDR_t;
typedef union 
{
    PUPDR_bits_t Bits;
    volatile uint32_t  PUPDR;
}GPIO_PUPDR_t;
typedef union 
{
    BSRR_bits_t Bits;
    volatile uint32_t  BSRR;
}GPIO_BSRR_t;
typedef union 
{
    LCKR_bits_t Bits;
    volatile uint32_t  LCKR;
}GPIO_LCKR_t;
typedef union 
{
    AFRL_bits_t Bits;
    volatile uint32_t  AFRL;
}GPIO_AFRL_t;
typedef union 
{
    AFRH_bits_t Bits;
    volatile uint32_t  AFRH;
}GPIO_AFRH_t;
typedef struct 
{
    GPIO_MODER_t       GPIO_MODER;
    GPIO_OTYPER_t      GPIO_OTYPER;
    GPIO_OSPEEDR_t     GPIO_OSPEEDR;
    GPIO_PUPDR_t       GPIO_PUPDR;
    GPIO_IDR_t         GPIO_IDR;
    GPIO_ODR_t         GPIO_ODR;
    GPIO_BSRR_t        GPIO_BSRR;
    GPIO_LCKR_t        GPIO_LCKR;    
    GPIO_AFRL_t        GPIO_AFRL;
    GPIO_AFRH_t        GPIO_AFRH;
}GPIOX_regsters_t;

/*************************************************************************************************************************
 *                                          GPIO Base Addresses                                                        *
 *************************************************************************************************************************/
#define GPIOA_BASE   (0x40020000)
#define GPIOB_BASE   (0x40020400)
#define GPIOC_BASE   (0x40020800)
#define GPIOD_BASE   (0x40020C00)
#define GPIOE_BASE   (0x40021000)
#define GPIOH_BASE   (0x40021C00)
/*************************************************************************************************************************
 *                                          GPIO Base Address Definition                                        *
 *************************************************************************************************************************/
#define GPIOA   ((GPIOX_regsters_t *) GPIOA_BASE)
#define GPIOB   ((GPIOX_regsters_t *) GPIOB_BASE)
#define GPIOC   ((GPIOX_regsters_t *) GPIOC_BASE)
#define GPIOD   ((GPIOX_regsters_t *) GPIOD_BASE)
#define GPIOE   ((GPIOX_regsters_t *) GPIOE_BASE)
#define GPIOH   ((GPIOX_regsters_t *) GPIOH_BASE)

#endif