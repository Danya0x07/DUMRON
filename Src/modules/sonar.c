#include "modules/sonar.h"

#define TRIGGER_PULSE_WIDTH    11
#define ECHO_RESPONSE_TIMEOUT  60
#define MAX_ECHO_PULSE_WIDTH   300

#define sonar_trig_1()     LL_GPIO_SetOutputPin(SONAR_GPIO_Port, SONAR_TRIG_Pin)
#define sonar_trig_0()     LL_GPIO_ResetOutputPin(SONAR_GPIO_Port, SONAR_TRIG_Pin)
#define sonar_echo_is_1()  LL_GPIO_IsInputPinSet(SONAR_GPIO_Port, SONAR_ECHO_Pin)

#define sonar_timer_reset()  LL_TIM_SetCounter(TIM3, 0)
#define sonar_timer_get()    LL_TIM_GetCounter(TIM3)

uint16_t sonar_scan(void)
{
    sonar_trig_1();
    delay_us(TRIGGER_PULSE_WIDTH);
    sonar_trig_0();
    sonar_timer_reset();
    while (!sonar_echo_is_1() && sonar_timer_get() < ECHO_RESPONSE_TIMEOUT)
        ;
    sonar_timer_reset();
    while (sonar_echo_is_1() && sonar_timer_get() < MAX_ECHO_PULSE_WIDTH)
        ;
    return sonar_timer_get() / 58;
}
