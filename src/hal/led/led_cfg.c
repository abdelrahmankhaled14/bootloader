#include "hal/led/led.h"

const LED_Configuration_t LED_Config[LED_MAX] = 
{
    [LED_1]{
        .port = LED_PORTA,
        .active_State = ACTIVE_HIGH,
        .pin = LED_PIN0,
        .output_value = LED_LOW,
        .output_type = LED_PUSH_PULL },  
         /* LED_1 */

       [LED_2]{
        .port = LED_PORTA,
        .active_State = ACTIVE_HIGH,
        .pin = LED_PIN1,
        .output_value = LED_LOW,
        .output_type = LED_PUSH_PULL,
     },   /* LED_2 */
     [LED_3]{
        .port = LED_PORTA,
        .active_State = ACTIVE_HIGH,
        .pin = LED_PIN2,
        .output_value = LED_LOW,
        .output_type = LED_PUSH_PULL },  
         /* LED_3 */

       [LED_4]{
        .port = LED_PORTA,
        .active_State = ACTIVE_HIGH,
        .pin = LED_PIN3,
        .output_value = LED_LOW,
        .output_type = LED_PUSH_PULL,
     } ,   /* LED_4 */
     [LED_5]{
        .port = LED_PORTA,
        .active_State = ACTIVE_HIGH,
        .pin = LED_PIN4,
        .output_value = LED_LOW,
        .output_type = LED_PUSH_PULL },  
         /* LED_5 */

       [LED_6]{
        .port = LED_PORTA,
        .active_State = ACTIVE_HIGH,
        .pin = LED_PIN5,
        .output_value = LED_LOW,
        .output_type = LED_PUSH_PULL,
     }    /* LED_6 */
};