#include <main.h>

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

static ArmDirection_e armDirection = ARM_STOP;
static ClawDirection_e clawDirection = CLAW_STOP;

static void ArmMove(ArmDirection_e direction);
static void ClawMove(ClawDirection_e direction);
static void ConstrainPulse(uint32_t *pulse, uint32_t maxval, uint32_t minval);

void Manipulator_SetDirections(ArmDirection_e armDir, ClawDirection_e clawDir)
{
    armDirection = armDir;
    clawDirection = clawDir;
}

void Manipulator_Move(void)
{
    ArmMove(armDirection);
    ClawMove(clawDirection);
}

static void ArmMove(ArmDirection_e direction)
{
    uint32_t pulse = LL_TIM_ReadReg(SERVO_TIM, SERVO_ARM_PWM_Reg);
    pulse += ARM_PULSE_FADE * direction;
    ConstrainPulse(&pulse, ARM_MAX_PULSE, ARM_MIN_PULSE);
    LL_TIM_WriteReg(SERVO_TIM, SERVO_ARM_PWM_Reg, pulse);
}

static void ClawMove(ClawDirection_e direction)
{
    uint32_t pulse = LL_TIM_ReadReg(SERVO_TIM, SERVO_CLAW_PWM_Reg);
    pulse += CLAW_PULSE_FADE * direction;
    ConstrainPulse(&pulse, CLAW_MAX_PULSE, CLAW_MIN_PULSE);
    LL_TIM_WriteReg(SERVO_TIM, SERVO_CLAW_PWM_Reg, pulse);
}

static void ConstrainPulse(uint32_t *pulse, uint32_t maxval, uint32_t minval)
{
    if (*pulse > maxval)
        *pulse = maxval;
    else if (*pulse < minval)
        *pulse = minval;
}
