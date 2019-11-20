#include "modules/manipulator.h"
#include "main.h"

// #define MG_SERVO_DEGREE_0      63
// #define MG_SERVO_DEGREE_180    248

#define ARM_MIN_PULSE   92
#define ARM_MAX_PULSE   272
#define ARM_PULSE_FADE  2

#define CLAW_MIN_PULSE   195
#define CLAW_MAX_PULSE   270
#define CLAW_PULSE_FADE  3

static ArmServoDirection arm_servo_dir = ARM_STOP;
static ClawServoDirection claw_servo_dir = CLAW_STOP;

static void manipulator_arm_move(ArmServoDirection dir);
static void manipulator_claw_move(ClawServoDirection dir);
static void constrain_pulse(uint32_t* pulse, uint32_t maxval, uint32_t minval);

void manipulator_set_directions(ArmServoDirection arm_dir, ClawServoDirection claw_dir)
{
    arm_servo_dir = arm_dir;
    claw_servo_dir = claw_dir;
}

void manipulator_move(void)
{
    manipulator_arm_move(arm_servo_dir);
    manipulator_claw_move(claw_servo_dir);
}

static void manipulator_arm_move(ArmServoDirection dir)
{
    uint32_t pulse = LL_TIM_ReadReg(SERVO_TIM, SERVO_ARM_PWM_Reg);
    pulse += ARM_PULSE_FADE * dir;
    constrain_pulse(&pulse, ARM_MAX_PULSE, ARM_MIN_PULSE);
    LL_TIM_WriteReg(SERVO_TIM, SERVO_ARM_PWM_Reg, pulse);
}

static void manipulator_claw_move(ClawServoDirection dir)
{
    uint32_t pulse = LL_TIM_ReadReg(SERVO_TIM, SERVO_CLAW_PWM_Reg);
    pulse += CLAW_PULSE_FADE * dir;
    constrain_pulse(&pulse, CLAW_MAX_PULSE, CLAW_MIN_PULSE);
    LL_TIM_WriteReg(SERVO_TIM, SERVO_CLAW_PWM_Reg, pulse);
}

static void constrain_pulse(uint32_t* pulse, uint32_t maxval, uint32_t minval)
{
    if (*pulse > maxval)
        *pulse = maxval;
    else if (*pulse < minval)
        *pulse = minval;
}
