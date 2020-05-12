#ifndef MOTORS_H_INCLUDED
#define MOTORS_H_INCLUDED

#include <protocol.h>
#include <stdint.h>


void motors_set_direction(RobotDirection);
void motors_set_speed(uint8_t, uint8_t);
RobotDirection motors_get_direction(void);

#endif
