#include <main.h>

#include "errors.h"
#include "emmiters.h"

void Error_InitRadio(void)
{
    for (uint_fast8_t i = 0; i < 3; i++) {
        Buzzer_SetState(1);
        HAL_Delay(300);
        Buzzer_SetState(0);
        HAL_Delay(100);
    }
}

void Error_InitTemperature(void)
{

}
