#ifndef _NIVC_PRIVATE
#define _NIVC_PRIVATE
#include "std_types.h"




typedef struct 
{
    volatile uint32_t NVIC_ISER[8];     
    uint32_t RESERVED0[24];             

    volatile uint32_t NVIC_ICER[8];     
    uint32_t RESERVED1[24];             

    volatile uint32_t NVIC_ISPR[8];     
    uint32_t RESERVED2[24];            

    volatile uint32_t NVIC_ICPR[8];     
    uint32_t RESERVED3[24];             

    volatile uint32_t NVIC_IABR[8];     
    uint32_t RESERVED4[56];             

    volatile uint8_t  NVIC_IPR[240];    
    uint32_t RESERVED5[644];            
} NVIC_Registers_t;

#define NVIC_BASE 0xE000E100UL
#define NVIC ((NVIC_Registers_t*) NVIC_BASE)





#endif






