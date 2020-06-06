#include <main.h>
#include <tim.h>

#include "distance.h"
#include "errors.h"
#include "debug.h"

#define MIN_DELAY_BETWEEN_MEASUREMENTS_US   51
#define TRIGGER_PULSE_WIDTH_US    11
#define ECHO_RESPONSE_TIMEOUT_US  500  /* взято с запасом */

/* Для 150 см, более нам не требуется, т.к. квант планировщика равен 10 мс. */
#define ECHO_MAX_PULSE_WIDTH_US   8700

/*
 * Максимальное расстояние от кормы робота до поверхности,
 * при котором движение назад не блокируется.
 */
#define MAX_SAFE_TO_FALL_DISTANCE_CM    12

/*
 * Минимальное расстояние от кормы робота до поверхности,
 * при котором считается, что препятствия сзади нет.
 */
#define MIN_NO_OBSTACLE_DISTANCE_CM    5

typedef struct {
    GPIO_TypeDef *gport;
    uint32_t gpin_trig;
    uint32_t gpin_echo;
} HCSR04_s;

static int _Distance_MeasureCm(HCSR04_s *sonar)
{
    delay_us(MIN_DELAY_BETWEEN_MEASUREMENTS_US);

    LL_GPIO_SetOutputPin(sonar->gport, sonar->gpin_trig);
    delay_us(TRIGGER_PULSE_WIDTH_US);
    LL_GPIO_ResetOutputPin(sonar->gport, sonar->gpin_trig);

    LL_TIM_SetCounter(TIM3, 0);
    while (!LL_GPIO_IsInputPinSet(sonar->gport, sonar->gpin_echo)) {
        if (LL_TIM_GetCounter(TIM3) > ECHO_RESPONSE_TIMEOUT_US)
            return -1;
    }

    LL_TIM_SetCounter(TIM3, 0);
    while (LL_GPIO_IsInputPinSet(sonar->gport, sonar->gpin_echo)) {
        if (LL_TIM_GetCounter(TIM3) > ECHO_MAX_PULSE_WIDTH_US)
            break;
    }

    return LL_TIM_GetCounter(TIM3) / 58;
}

Distance_e Distance_GetBack(void)
{
    static HCSR04_s sternSonar = {
        .gport = SONAR_GPIO_Port,
        .gpin_trig = SONAR_TRIG_Pin,
        .gpin_echo = SONAR_ECHO_Pin
    };
    int distanceCm = _Distance_MeasureCm(&sternSonar);

    if (distanceCm < 0) {
        debug_logs("distance error\n");
        return DIST_ERROR;
    }
    else if (distanceCm > MAX_SAFE_TO_FALL_DISTANCE_CM) {
        return DIST_CLIFF;
    }
    else if (distanceCm < MIN_NO_OBSTACLE_DISTANCE_CM) {
        return DIST_OBSTACLE;
    }
    else {
        return DIST_NOTHING;
    }
}
