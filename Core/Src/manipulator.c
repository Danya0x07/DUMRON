#include "main.h"

#include "manipulator.h"

/*
 * Длительности импульсов в относительных единицах,
 * соответствующие крайним положениям вала для сервомоторов семейства MG.
 */
// #define MG_SERVO_DEGREE_0      63
// #define MG_SERVO_DEGREE_180    248

#define ARM_MIN_PULSE   92
#define ARM_MAX_PULSE   270
#define ARM_PULSE_FADE  2

#define CLAW_MIN_PULSE   195
#define CLAW_MAX_PULSE   270
#define CLAW_PULSE_FADE  3

static const int directions[3] = {0, 1, -1};

static int armDirection = 0;
static int clawDirection = 0;

static void constrainPulse(uint32_t *pulse, uint32_t maxval, uint32_t minval)
{
    if (*pulse > maxval)
        *pulse = maxval;
    else if (*pulse < minval)
        *pulse = minval;
}

static void moveArm(int direction)
{
    uint32_t pulse = LL_TIM_ReadReg(SERVO_TIM, SERVO_ARM_PWM_Reg);
    pulse += ARM_PULSE_FADE * direction;
    constrainPulse(&pulse, ARM_MAX_PULSE, ARM_MIN_PULSE);
    LL_TIM_WriteReg(SERVO_TIM, SERVO_ARM_PWM_Reg, pulse);
}

static void moveClaw(int direction)
{
    uint32_t pulse = LL_TIM_ReadReg(SERVO_TIM, SERVO_CLAW_PWM_Reg);
    pulse += CLAW_PULSE_FADE * direction;
    constrainPulse(&pulse, CLAW_MAX_PULSE, CLAW_MIN_PULSE);
    LL_TIM_WriteReg(SERVO_TIM, SERVO_CLAW_PWM_Reg, pulse);
}

void Manipulator_SetArm(ArmControl_e ctrl)
{
    armDirection = directions[ctrl];
}

void Manipulator_SetClaw(ClawControl_e ctrl)
{
    clawDirection = directions[ctrl];
}

void Manipulator_Move(void)
{
    moveArm(armDirection);
    moveClaw(clawDirection);
}
