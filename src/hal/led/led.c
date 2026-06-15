#include "mcal/gpio/gpio.h"  
#include "hal/led/led.h"
extern LED_Configuration_t LED_Config[LED_MAX];

void LED_Init(LED_Configuration_t* LED_Configuration)
{
    GPIO_Configuration_t led_gpio_config;
    
    for (uint8_t i = 0; i < LED_MAX; i++)
    {
        led_gpio_config.port = LED_Config[i].port;
        led_gpio_config.pin = LED_Config[i].pin;
        led_gpio_config.direction = OUTPUT;
        led_gpio_config.output_type = LED_Config[i].output_type;
        led_gpio_config.output_value = (LED_Config[i].active_State == ACTIVE_HIGH) ? LOW : HIGH;
        led_gpio_config.resistor = NO_RESISTOR;
        led_gpio_config.mode = AF_0;
        led_gpio_config.speed = HIGH_SPEED;
        GPIO_Init(&led_gpio_config, 1);

        if (LED_Config[i].active_State == ACTIVE_HIGH)
        {
            GPIO_SetPinValue(LED_Config[i].port, LED_Config[i].pin, LOW);
        }
        else if (LED_Config[i].active_State == ACTIVE_LOW)
        {
            GPIO_SetPinValue(LED_Config[i].port, LED_Config[i].pin, HIGH);
        }
    }
}
void LED_On(LED_Names_t led)
{
    if (LED_Config[led].active_State == ACTIVE_HIGH)
    {
        GPIO_SetPinValue(LED_Config[led].port, LED_Config[led].pin, HIGH);
    }
    else if (LED_Config[led].active_State == ACTIVE_LOW)
    {
        GPIO_SetPinValue(LED_Config[led].port, LED_Config[led].pin, LOW);
    }
}
void LED_Off(LED_Names_t led)
{
    if (LED_Config[led].active_State == ACTIVE_HIGH)
    {
        GPIO_SetPinValue(LED_Config[led].port, LED_Config[led].pin, LOW);
    }
    else if (LED_Config[led].active_State == ACTIVE_LOW)
    {
        GPIO_SetPinValue(LED_Config[led].port, LED_Config[led].pin, HIGH);
    }
}

void LED_Toggle(LED_Names_t led)
{
    GPIO_TogglePinValue(LED_Config[led].port, LED_Config[led].pin);
}       