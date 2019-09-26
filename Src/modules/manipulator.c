#include "modules/manipulator.h"
#include "main.h"

#define SERVO_DEGREE_0      125
#define SERVO_DEGREE_180    250

#define ARM_MIN_PULSE   150
#define ARM_MAX_PULSE   240
#define ARM_PULSE_FADE  5

#define CLAW_MIN_PULSE   160
#define CLAW_MAX_PULSE   230
#define CLAW_PULSE_FADE  4

static void constrain_pulse(uint32_t* pulse, uint32_t max, uint32_t min)
{
    if (*pulse > max)
        *pulse = max;
    else if (*pulse < min)
        *pulse = min;
}

void manipulator_arm_move(ArmServoDirection dir)
{
    uint32_t pulse = LL_TIM_ReadReg(SERVO_TIM, SERVO_ARM_PWM_Reg);
    pulse += ARM_PULSE_FADE * dir;
    constrain_pulse(&pulse, ARM_MAX_PULSE, ARM_MIN_PULSE);
    LL_TIM_WriteReg(SERVO_TIM, SERVO_ARM_PWM_Reg, pulse);
}

void manipulator_claw_move(ClawServoDirection dir)
{
    uint32_t pulse = LL_TIM_ReadReg(SERVO_TIM, SERVO_CLAW_PWM_Reg);
    pulse += CLAW_PULSE_FADE * dir;
    constrain_pulse(&pulse, CLAW_MAX_PULSE, CLAW_MIN_PULSE);
    LL_TIM_WriteReg(SERVO_TIM, SERVO_CLAW_PWM_Reg, pulse);
}
