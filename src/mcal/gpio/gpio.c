

#ifdef __cplusplus
namespace gpio{
extern "C"{
#endif


#include "mcal/gpio/gpio.h"
#include "mcal/gpio/gpio_private.h"
#include "macros.h"



 GPIOX_regsters_t *gpiox[]={GPIOA,GPIOB,GPIOC,GPIOD,GPIOE,GPIOH};

status_t GPIO_Init(GPIO_Configuration_t *GPIO_Configuration, uint8_t size)
{
    status_t ret = STATUS_OK;
    for (uint8_t i = 0; i < size; i++)
    {
        ret = GPIO_SetPinMode(GPIO_Configuration[i].port, GPIO_Configuration[i].pin, GPIO_Configuration[i].mode);
        ret = GPIO_SetPinDirection(GPIO_Configuration[i].port, GPIO_Configuration[i].pin, GPIO_Configuration[i].direction);
        ret = GPIO_SetPinValue(GPIO_Configuration[i].port, GPIO_Configuration[i].pin, GPIO_Configuration[i].output_value);
        
        ret = GPIO_SetPinRegister(GPIO_Configuration[i].port, GPIO_Configuration[i].pin, GPIO_Configuration[i].resistor);
        ret = GPIO_SetPinOutputType(GPIO_Configuration[i].port, GPIO_Configuration[i].pin, GPIO_Configuration[i].output_type);
        ret = GPIO_SetPinSpeed(GPIO_Configuration[i].port, GPIO_Configuration[i].pin, GPIO_Configuration[i].speed);
    }
    return ret;
}

status_t GPIO_SetPinDirection(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_pin_Mode_t Direction)
{
    status_t ret = STATUS_OK;
    if (PinNumber > PIN15)
    {
        ret = STATUS_ERROR;
    }
    else if(Port > PORTH)
    {
        ret = STATUS_ERROR;
    }
    else
    {
        gpiox[Port]->GPIO_MODER.MODER = (gpiox[Port]->GPIO_MODER.MODER & ~(0b11 << (PinNumber*2))) | (Direction << (PinNumber*2));
    }

    return ret;
}
status_t GPIO_SetPinValue(Gpio_Port_t Port, Gpio_Pin_t PinNumber,Gpio_Output_Value_t Value)
{
    status_t ret = STATUS_OK;
    if (PinNumber > PIN15)
    {
        ret = STATUS_ERROR;
    }   
    else if(Port > PORTH)
    {
        ret = STATUS_ERROR;
    }
    else
    {
        if(Value == HIGH)
        {
            SET_BIT(gpiox[Port]->GPIO_ODR.ODR, PinNumber);   
        }
        else
        {
            CLEAR_BIT(gpiox[Port]->GPIO_ODR.ODR, PinNumber);
        }
                       
    }
 
    return ret;
    
}
status_t GPIO_ReadPinValue(Gpio_Port_t Port, Gpio_Pin_t PinNumber, uint8_t* Value)
{
    status_t ret = STATUS_OK;
    if (PinNumber > PIN15 || Value == NULL)
    {
        ret = STATUS_ERROR;
    }
    else if (Port > PORTH)
    {
        ret = STATUS_ERROR;
    }
    else
    {
        *Value = READ_BIT(gpiox[Port]->GPIO_IDR.IDR, PinNumber);
    }
    
    return ret;
}
status_t GPIO_TogglePinValue(Gpio_Port_t Port, Gpio_Pin_t PinNumber)
{
    status_t ret = STATUS_OK;
    if (PinNumber > PIN15)
    {
        ret = STATUS_ERROR;
    } 
    else if (Port > PORTH)
    {
        ret = STATUS_ERROR;
    }
    else
    {
        TOGGLE_BIT(gpiox[Port]->GPIO_ODR.ODR, PinNumber);
    }
    return ret;
}
status_t GPIO_SetPinMode(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_Alter_Mode_t Mode)
{
    status_t ret = STATUS_OK;
    uint8_t afr_index = PinNumber / 8;
    uint8_t afr_position = (PinNumber % 8) * 4;

    if (PinNumber > PIN15)
    {
        ret = STATUS_ERROR;
    }   
    else if (Port > PORTH)
    {
        ret = STATUS_ERROR;
    }
    else
    {
        if (afr_index == 0)
        {
            gpiox[Port]->GPIO_AFRL.AFRL = (gpiox[Port]->GPIO_AFRL.AFRL & ~(0b1111 << afr_position)) | (Mode << afr_position);
        }
        else
        {
            gpiox[Port]->GPIO_AFRH.AFRH = (gpiox[Port]->GPIO_AFRH.AFRH & ~(0b1111 << afr_position)) | (Mode << afr_position);
        }
    
    }
    return ret;
}

status_t GPIO_SetPinRegister(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_Register_Value_t Register)
{
    status_t ret = STATUS_OK;
    if (PinNumber > PIN15)
    {
        ret = STATUS_ERROR;
    }   
    else if (Port > PORTH)
    {
        ret = STATUS_ERROR;
    }
    else
    {
     gpiox[Port]->GPIO_PUPDR.PUPDR = (gpiox[Port]->GPIO_PUPDR.PUPDR & ~(0b11 << (PinNumber*2))) | (    Register << (PinNumber*2));
    }
    return ret;
}
status_t GPIO_SetPinOutputType(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_Output_Type_t OutputType)
{
    status_t ret = STATUS_OK;
    if (PinNumber > PIN15)
    {
        ret = STATUS_ERROR;
    }   
    else if (Port > PORTH)
    {
        ret = STATUS_ERROR;
    }
    else
    {
        gpiox[Port]->GPIO_OTYPER.OTYPER = (gpiox[Port]->GPIO_OTYPER.OTYPER & ~(0b1 << PinNumber)) | (    OutputType << PinNumber);
    }
    return ret;
}
status_t GPIO_SetPinSpeed(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_Speed_t Speed)
{
    status_t ret = STATUS_OK;
    if (PinNumber > PIN15)
    {
        ret = STATUS_ERROR;
    }   
    else if (Port > PORTH)
    {
        ret = STATUS_ERROR;
    }
    else
    {
         gpiox[Port]->GPIO_OSPEEDR.OSPEEDR = (gpiox[Port]->GPIO_OSPEEDR.OSPEEDR & ~(0b11 << (PinNumber*2))) | (    Speed << (PinNumber*2));
    }
   
    return ret;
}       

#ifdef __cplusplus
}
}
#endif