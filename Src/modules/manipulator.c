#include "modules/manipulator.h"
#include "main.h"

#define SERVO_DEGREE_0
#define SERVO_DEGREE_180    248

#define ARM_MIN_PULSE   92
#define ARM_MAX_PULSE   272
#define ARM_PULSE_FADE  2

#define CLAW_MIN_PULSE   160
#define CLAW_MAX_PULSE   230
#define CLAW_PULSE_FADE  1

static void constrain_pulse(uint32_t* pulse, uint32_t maxval, uint32_t minval)
{
    if (*pulse > maxval)
        *pulse = maxval;
    else if (*pulse < minval)
        *pulse = minval;
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
