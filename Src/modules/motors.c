#include <modules/motors.h>
/*
 * motors.c
 *
 *  Created on: Sep 20, 2019
 *      Author: danya
 */
#define motors_pins_set(pinmask)    LL_GPIO_SetOutputPin(MOTOR_GPIO_Port, (pinmask))
#define motors_pins_reset(pinmask)  LL_GPIO_ResetOutputPin(MOTOR_GPIO_Port, (pinmask))

void motors_set_direction(RobotDirection direction)
{
    motors_set_speed(0, 0);
    switch (direction)
    {
    case ROBOT_DIRECTION_NONE:
        motors_pins_reset(MOTOR_L1_Pin | MOTOR_L2_Pin | MOTOR_R1_Pin | MOTOR_R2_Pin);
        break;
    case ROBOT_DIRECTION_FORWARD:
        motors_pins_reset(MOTOR_L2_Pin | MOTOR_R2_Pin);
        motors_pins_set  (MOTOR_L1_Pin | MOTOR_R1_Pin);
        break;
    case ROBOT_DIRECTION_BACKWARD:
        motors_pins_reset(MOTOR_L1_Pin | MOTOR_R1_Pin);
        motors_pins_set  (MOTOR_L2_Pin | MOTOR_R2_Pin);
        break;
    case ROBOT_DIRECTION_LEFTWARD:
        motors_pins_reset(MOTOR_L1_Pin | MOTOR_R2_Pin);
        motors_pins_set  (MOTOR_L2_Pin | MOTOR_R1_Pin);
        break;
    case ROBOT_DIRECTION_RIGHTWARD:
        motors_pins_reset(MOTOR_L2_Pin | MOTOR_R1_Pin);
        motors_pins_set  (MOTOR_L1_Pin | MOTOR_R2_Pin);
        break;
    }
}

void motors_set_speed(uint8_t speed_left, uint8_t speed_right)
{
    LL_TIM_WriteReg(MOTOR_TIM, MOTOR_L_PWM_Reg, speed_left);
    LL_TIM_WriteReg(MOTOR_TIM, MOTOR_R_PWM_Reg, speed_right);
}
