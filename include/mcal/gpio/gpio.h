#ifndef GPIO_H
#define GPIO_H  

#include "std_types.h"

typedef enum
{
    PORTA,PORTB,PORTC,PORTD,PORTE,PORTH
}Gpio_Port_t;
typedef enum
{
    PIN0,PIN1,PIN2,PIN3,PIN4,PIN5,PIN6,PIN7,PIN8,PIN9,PIN10,PIN11,PIN12,PIN13,PIN14,PIN15
}Gpio_Pin_t;
typedef enum
{
    INPUT,OUTPUT,ALTERNATE_FUNCTION,ANALOG
}Gpio_pin_Mode_t;
typedef enum
{
    AF_0,AF_1,AF_2,AF_3,AF_4,AF_5,AF_6,AF_7,AF_8,AF_9,AF_10,AF_11,AF_12,AF_13,AF_14,AF_15
}Gpio_Alter_Mode_t;
typedef enum
{
    LOW,HIGH
}Gpio_Output_Value_t;
typedef enum
{
    NO_RESISTOR ,
    PULL_UP ,
    PULL_DOWN
}Gpio_Register_Value_t;
typedef enum
{
    LOW_SPEED,
    MEDIUM_SPEED,
    HIGH_SPEED,
    VERY_HIGH_SPEED
}Gpio_Speed_t;
typedef enum
{
    PUSH_PULL,OPEN_DRAIN
}Gpio_Output_Type_t;
typedef struct 
{
    Gpio_Port_t port;
    Gpio_Pin_t pin;
    Gpio_pin_Mode_t direction;
    Gpio_Output_Value_t output_value;
    Gpio_Register_Value_t resistor;
    Gpio_Alter_Mode_t mode;
    Gpio_Output_Type_t output_type;
    Gpio_Speed_t speed;

}GPIO_Configuration_t;
/*************************************************************************************************************************
 *                                               GPIO Function Prototype                                            *
 *************************************************************************************************************************/
status_t GPIO_SetPinMode(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_Alter_Mode_t Mode);
status_t GPIO_SetPinDirection(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_pin_Mode_t Direction);
status_t GPIO_SetPinValue(Gpio_Port_t Port, Gpio_Pin_t PinNumber,Gpio_Output_Value_t Value);
status_t GPIO_ReadPinValue(Gpio_Port_t Port, Gpio_Pin_t PinNumber, uint8_t* Value);
status_t GPIO_TogglePinValue(Gpio_Port_t Port, Gpio_Pin_t PinNumber);

status_t GPIO_SetPinRegister(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_Register_Value_t Register);
status_t GPIO_SetPinOutputType(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_Output_Type_t OutputType);
status_t GPIO_SetPinSpeed(Gpio_Port_t Port, Gpio_Pin_t PinNumber, Gpio_Speed_t Speed);
status_t GPIO_Init(GPIO_Configuration_t *GPIO_Configuration, uint8_t size);

#endif 