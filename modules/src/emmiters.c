#include "emmiters.h"
#include "main.h"

void led_set(_Bool state)
{
    if (state)
        LL_GPIO_ResetOutputPin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
    else
        LL_GPIO_SetOutputPin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
}

void lights_set(_Bool state)
{
    if (state)
        LL_GPIO_SetOutputPin(LIGHTS_GPIO_Port, LIGHTS_Pin);
    else
        LL_GPIO_ResetOutputPin(LIGHTS_GPIO_Port, LIGHTS_Pin);
}

void buzzer_set(_Bool state)
{
    if (state)
        LL_GPIO_SetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);
    else
        LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);
}
