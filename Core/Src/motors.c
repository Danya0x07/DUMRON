#include <main.h>

#include "motors.h"
#include "debug.h"

#define NUM_OF_DIRECTIONS   5

static const uint32_t resetPinTable[NUM_OF_DIRECTIONS] = {
    MOTOR_L1_Pin | MOTOR_L2_Pin | MOTOR_R1_Pin | MOTOR_R2_Pin,
    MOTOR_L2_Pin | MOTOR_R2_Pin,
    MOTOR_L1_Pin | MOTOR_R1_Pin,
    MOTOR_L1_Pin | MOTOR_R2_Pin,
    MOTOR_L2_Pin | MOTOR_R1_Pin
};

static const uint32_t setPinTable[NUM_OF_DIRECTIONS] = {
    0,
    MOTOR_L1_Pin | MOTOR_R1_Pin,
    MOTOR_L2_Pin | MOTOR_R2_Pin,
    MOTOR_L2_Pin | MOTOR_R1_Pin,
    MOTOR_L1_Pin | MOTOR_R2_Pin
};

static MoveDirection_e currentDirection = MOVEDIR_NONE;

static inline void Motors_PinSet(uint32_t mask)
{
    LL_GPIO_SetOutputPin(MOTOR_GPIO_Port, mask);
}

static inline void Motors_PinReset(uint32_t mask)
{
    LL_GPIO_ResetOutputPin(MOTOR_GPIO_Port, mask);
}

void Motors_SetDirection(MoveDirection_e direction)
{
    if (direction == currentDirection)
        return;

    Motors_SetSpeed(0, 0);
    Motors_PinReset(resetPinTable[direction]);
    Motors_PinSet(setPinTable[direction]);
    currentDirection = direction;
}

void Motors_SetSpeed(uint8_t speedL, uint8_t speedR)
{
    LL_TIM_WriteReg(MOTOR_TIM, MOTOR_L_PWM_Reg, speedL);
    LL_TIM_WriteReg(MOTOR_TIM, MOTOR_R_PWM_Reg, speedR);
}

MoveDirection_e Motors_GetDirection(void)
{
    return currentDirection;
}
