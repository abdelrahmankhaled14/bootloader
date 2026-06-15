#ifndef LED_H
#define LED_H
#include "std_types.h"
#include "hal/led/led_cfg.h"
typedef enum
{
    LED_PORTA,LED_PORTB,LED_PORTC,LED_PORTD,LED_PORTE,LED_PORTH
}Led_Port_t;
typedef enum
{
    LED_PIN0,LED_PIN1,LED_PIN2,LED_PIN3,LED_PIN4,LED_PIN5,LED_PIN6,LED_PIN7,LED_PIN8,LED_PIN9,LED_PIN10,LED_PIN11,LED_PIN12,LED_PIN13,LED_PIN14,LED_PIN15
}Led_Pin_t;
typedef enum
{
    LED_LOW,LED_HIGH
}Led_Output_Value_t;
typedef enum
{
    LED_PUSH_PULL,LED_OPEN_DRAIN
}Led_Output_Type_t;
typedef enum
{
    ACTIVE_HIGH,
    ACTIVE_LOW
}Led_Active_State_t;

typedef struct 
{
    Led_Port_t port;
    Led_Pin_t pin;
    Led_Output_Value_t output_value;
    Led_Output_Type_t output_type;
    Led_Active_State_t active_State;

}LED_Configuration_t;

/*************************************************************************************************************************
 *                                               GPIO Function Prototype                                            *
 *************************************************************************************************************************/
void LED_Init(LED_Configuration_t* LED_Configuration);
void LED_On(LED_Names_t led);
void LED_Off(LED_Names_t led);
void LED_Toggle(LED_Names_t led);
#endif /* LED_H */