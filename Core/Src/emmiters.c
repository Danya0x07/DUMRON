#include <main.h>

#include "emmiters.h"

void Led_SetState(bool state)
{
    if (state)
        LL_GPIO_ResetOutputPin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);
    else
        LL_GPIO_SetOutputPin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);
}

void Lights_SetState(bool state)
{
    if (state)
        LL_GPIO_SetOutputPin(LIGHTS_GPIO_Port, LIGHTS_Pin);
    else
        LL_GPIO_ResetOutputPin(LIGHTS_GPIO_Port, LIGHTS_Pin);
}

void Buzzer_SetState(bool state)
{
    if (state)
        LL_GPIO_SetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);
    else
        LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);
}
