#include "main.h"
#include "hcsr04.h"

#define TRIGGER_PULSE_WIDTH_US    12
#define ECHO_RESPONSE_TIMEOUT_US  500
#define MAX_ECHO_PULSE_WIDTH_US   11600

#define sonar_trig_1()     LL_GPIO_SetOutputPin(SONAR_GPIO_Port, SONAR_TRIG_Pin)
#define sonar_trig_0()     LL_GPIO_ResetOutputPin(SONAR_GPIO_Port, SONAR_TRIG_Pin)
#define sonar_echo_is_1()  LL_GPIO_IsInputPinSet(SONAR_GPIO_Port, SONAR_ECHO_Pin)

#define tim_us_reset_counter()  LL_TIM_SetCounter(TIM3, 0)
#define tim_us_get_counter()    LL_TIM_GetCounter(TIM3)

/**
 * @brief  Опрашивает дальномер.
 * @retval Расстояние до объекта в см.
 */
uint16_t hcsr04_measure(void)
{
    sonar_trig_1();
    delay_us(TRIGGER_PULSE_WIDTH_US);
    sonar_trig_0();
    tim_us_reset_counter();
    while (!sonar_echo_is_1() && tim_us_get_counter() < ECHO_RESPONSE_TIMEOUT_US)
        ;
    tim_us_reset_counter();
    while (sonar_echo_is_1() && tim_us_get_counter() < MAX_ECHO_PULSE_WIDTH_US)
        ;
    return tim_us_get_counter() / 58;
}
