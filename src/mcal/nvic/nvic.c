#include "mcal/nvic/nvic.h"
#include "mcal/nvic/nvic_private.h"


status_t NVIC_EnableIRQ(IRQn_t IRQn)
{
    status_t status = STATUS_OK;
    if (IRQn > 239)
    {
        status =  STATUS_ERROR;
    }
    else
    {
        NVIC->NVIC_ICPR[IRQn / 32] = (1 << (IRQn % 32));
        NVIC->NVIC_ISER[IRQn / 32] = (1 << (IRQn % 32));
    }
    return status;
   
}
/*Enables an interrupt or exception.*/
status_t NVIC_DisableIRQ(IRQn_t IRQn)
{
    status_t status = STATUS_OK;
    if (IRQn > 239)
    {
        status = STATUS_ERROR;
    }
    else
    {
        NVIC->NVIC_ICER[IRQn / 32] = (1 << (IRQn % 32));
    }
    return status;
}
status_t NVIC_SetPendingIRQ(IRQn_t IRQn)
{
    status_t status = STATUS_OK;
    if (IRQn > 239)
    {
        status = STATUS_ERROR;
    }
    else
    {
        NVIC->NVIC_ISPR[IRQn / 32] = (1 << (IRQn % 32));
    }
    return status;
}

status_t NVIC_ClearPendingIRQ(IRQn_t IRQn)
{
    status_t status = STATUS_OK;
    if (IRQn > 239)
    {
        status = STATUS_ERROR;
    }
    else
    {
        NVIC->NVIC_ICPR[IRQn / 32] = (1 << (IRQn % 32));
    }
    return status;
}
status_t NVIC_GetPendingIRQ(IRQn_t IRQn,uint32_t* pendingIRQ)
{
    status_t status = STATUS_OK;
    if(pendingIRQ == NULL)
    {
        status = STATUS_ERROR;
    }
    else if (IRQn > 239)
    {
        status = STATUS_ERROR;
    }
    else
    {
        *pendingIRQ = (NVIC->NVIC_ISPR[IRQn / 32] & (1 << (IRQn % 32))) != 0;
    }
    return status;
}
status_t NVIC_SetPriority(IRQn_t IRQn, Priority_t priority)
{
    status_t status = STATUS_OK;
    if (IRQn > 239)
    {
        status = STATUS_ERROR;
    }
    else if (priority > PRIORITY_15)
    {
        status = STATUS_ERROR;
    }
    else
    {
        NVIC->NVIC_IPR[IRQn] = (uint8_t)(priority << 4);
    }
    return status;
}
status_t NVIC_GetPriority(IRQn_t IRQn, Priority_t* priority)
{
    status_t status = STATUS_OK;
    if(priority == NULL)
    {
        status = STATUS_ERROR;
    }
    else if (IRQn > 239)
    {
        status = STATUS_ERROR;
    }
    else
    {
        *priority = (Priority_t)(NVIC->NVIC_IPR[IRQn] >> 4);
    }
    return status;
}
