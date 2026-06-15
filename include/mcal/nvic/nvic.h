#ifndef NIVC_H
#define NIVC_H
#include "std_types.h"
#include "mcal/nvic/stm32f401cc.h"



status_t NVIC_EnableIRQ(IRQn_t IRQn);
 /*Enables an interrupt or exception.*/
status_t NVIC_DisableIRQ(IRQn_t IRQn); 
/*Disables an interrupt or exception.*/
status_t NVIC_SetPendingIRQ(IRQn_t IRQn);
/*Sets the pending status of interrupt or exception to 1.*/
status_t NVIC_ClearPendingIRQ(IRQn_t IRQn);
/*Clears the pending status of interrupt or exception to 0.*/
status_t NVIC_GetPendingIRQ(IRQn_t IRQn, uint32_t* pendingIRQ);
/*Reads the pending status of interrupt orexception.
 This function returns non-zero value if the pending status is set to 1.*/
status_t NVIC_SetPriority(IRQn_t IRQn, Priority_t priority);
/*Sets the priority of an interrupt or exception with configurable priority level to 1.*/
status_t NVIC_GetPriority(IRQn_t IRQn, Priority_t* priority);
/*Reads the priority of an interrupt orexception with configurable prioritylevel.
 This function return the currentpriority level.*/


 /* void NIVC_Configure_SubGroup_Bits(Priority_Group_t Periority_Group);*/














#endif // NIVC_H