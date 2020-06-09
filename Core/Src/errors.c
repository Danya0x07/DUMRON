#include "main.h"

#include "errors.h"
#include "emmiters.h"

/** Если ошибка инициализации радиомодуля, */
void ErrorShow_InitRadio(void)
{
    /* 3 раза пищим. */
    for (uint_fast8_t i = 0; i < 3; i++) {
        Buzzer_SetState(1);
        HAL_Delay(300);
        Buzzer_SetState(0);
        HAL_Delay(100);
    }
}

/** Если ошибка инициализации датчиков температуры, */
void ErrorShow_InitTemperature(void)
{
    /* 2 раза моргаем фарами. */
    for (uint_fast8_t i = 0; i < 2; i++) {
        Lights_SetState(1);
        HAL_Delay(300);
        Lights_SetState(0);
        HAL_Delay(100);
    }
}

/** Если обнаружено переполнение стека, */
void ErrorCritical_StackOverflow(void)
{
    bool buzzerState = false;

    /* издаём звуковые сигналы при включённом светодиоде. */
    Led_SetState(1);
    for (;;) {
        Buzzer_SetState((buzzerState = !buzzerState));
        HAL_Delay(200);
    }
}

/** Если не хватило памяти в куче, */
void ErrorCritical_MallocFailed(void)
{
    bool buzzerState = false;

    /* издаём звуковые сигналы при выключённом светодиоде. */
    Led_SetState(0);
    for (;;) {
        Buzzer_SetState((buzzerState = !buzzerState));
        HAL_Delay(500);
    }
}
