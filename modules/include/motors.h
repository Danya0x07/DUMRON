/**
 * Модуль управления моторами робота.
 *
 * Моторов 2: на левую и правую гусеницы соответственно.
 */
#ifndef MOTORS_H_INCLUDED
#define MOTORS_H_INCLUDED

#include <protocol.h>
#include <stdint.h>

void Motors_SetDirection(RobotDirection_e);
void Motors_SetSpeed(uint8_t speedL, uint8_t speedR);
RobotDirection_e Motors_GetDirection(void);

#endif
