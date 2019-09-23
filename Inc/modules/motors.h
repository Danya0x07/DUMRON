#ifndef MOTORS_H_INCLUDED
#define MOTORS_H_INCLUDED

#include "robot_interface.h"
#include "main.h"

void motors_set_direction(RobotDirection);
void motors_set_speed(uint8_t, uint8_t);

#endif
