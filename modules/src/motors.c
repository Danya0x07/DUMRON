#include "main.h"
#include "motors.h"

static RobotDirection_e currentDirection = ROBOT_DIRECTION_NONE;

static inline void Motors_PinSet(uint32_t mask)
{
    LL_GPIO_SetOutputPin(MOTOR_GPIO_Port, mask);
}

static inline void Motors_PinReset(uint32_t mask)
{
    LL_GPIO_ResetOutputPin(MOTOR_GPIO_Port, mask);
}

void Motors_SetDirection(RobotDirection_e direction)
{
    Motors_SetSpeed(0, 0);
    switch (direction)
    {
    case ROBOT_DIRECTION_NONE:
        Motors_PinReset(MOTOR_L1_Pin | MOTOR_L2_Pin | MOTOR_R1_Pin | MOTOR_R2_Pin);
        break;
    case ROBOT_DIRECTION_FORWARD:
        Motors_PinReset(MOTOR_L2_Pin | MOTOR_R2_Pin);
        Motors_PinSet  (MOTOR_L1_Pin | MOTOR_R1_Pin);
        break;
    case ROBOT_DIRECTION_BACKWARD:
        Motors_PinReset(MOTOR_L1_Pin | MOTOR_R1_Pin);
        Motors_PinSet  (MOTOR_L2_Pin | MOTOR_R2_Pin);
        break;
    case ROBOT_DIRECTION_LEFTWARD:
        Motors_PinReset(MOTOR_L1_Pin | MOTOR_R2_Pin);
        Motors_PinSet  (MOTOR_L2_Pin | MOTOR_R1_Pin);
        break;
    case ROBOT_DIRECTION_RIGHTWARD:
        Motors_PinReset(MOTOR_L2_Pin | MOTOR_R1_Pin);
        Motors_PinSet  (MOTOR_L1_Pin | MOTOR_R2_Pin);
        break;
    }
    currentDirection = direction;
}

void Motors_SetSpeed(uint8_t speedL, uint8_t speedR)
{
    LL_TIM_WriteReg(MOTOR_TIM, MOTOR_L_PWM_Reg, speedL);
    LL_TIM_WriteReg(MOTOR_TIM, MOTOR_R_PWM_Reg, speedR);
}

RobotDirection_e Motors_GetDirection(void)
{
    return currentDirection;
}
