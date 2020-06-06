/**
 * Модуль управления моторами робота.
 *
 * Моторов 2: на левую и правую гусеницы соответственно.
 */

#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>

#include "protocol.h"

void Motors_SetDirection(MoveDirection_e);
void Motors_SetSpeed(uint8_t speedL, uint8_t speedR);
MoveDirection_e Motors_GetDirection(void);

#endif /* MOTORS_H */
