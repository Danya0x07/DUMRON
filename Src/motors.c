#include "motors.h"
/*
 * motors.c
 *
 *  Created on: Sep 20, 2019
 *      Author: danya
 */
static inline void motors_pin_set(uint16_t mot_pin);
static inline void motors_pin_reset(uint16_t mot_pin);

void motors_set_direction(RobotDirection direction)
{
	motors_set_speed(0, 0);
	switch (direction)
	{
	case ROBOT_DIRECTION_NONE:
		motors_pin_reset(MOT_L_P1_Pin | MOT_L_P2_Pin | MOT_R_P1_Pin | MOT_R_P2_Pin);
		break;
	case ROBOT_DIRECTION_FORWARD:
		motors_pin_reset(MOT_L_P2_Pin | MOT_R_P2_Pin);
		motors_pin_set  (MOT_L_P1_Pin | MOT_R_P1_Pin);
		break;
	case ROBOT_DIRECTION_BACKWARD:
		motors_pin_reset(MOT_L_P1_Pin | MOT_R_P1_Pin);
		motors_pin_set  (MOT_L_P2_Pin | MOT_R_P2_Pin);
		break;
	case ROBOT_DIRECTION_LEFTWARD:
		motors_pin_reset(MOT_L_P1_Pin | MOT_R_P2_Pin);
		motors_pin_set  (MOT_L_P2_Pin | MOT_R_P1_Pin);
		break;
	case ROBOT_DIRECTION_RIGHTWARD:
		motors_pin_reset(MOT_L_P2_Pin | MOT_R_P1_Pin);
		motors_pin_set  (MOT_L_P1_Pin | MOT_R_P2_Pin);
		break;
	}
}

void motors_set_speed(uint8_t speed_left, uint8_t speed_right)
{
	TIM1->CCR1 = speed_left;
	TIM1->CCR2 = speed_right;
}

static inline void motors_pin_set(uint16_t mot_pin)
{
	HAL_GPIO_WritePin(MOT_GPIO_Port, mot_pin, GPIO_PIN_SET);
}

static inline void motors_pin_reset(uint16_t mot_pin)
{
	HAL_GPIO_WritePin(MOT_GPIO_Port, mot_pin, GPIO_PIN_RESET);
}

