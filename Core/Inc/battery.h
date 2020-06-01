/**
 * Функции получения примерного уровня заряда аккумуляторов робота.
 */

#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

uint8_t Battery_GetPercentageBrains(void);
uint8_t Battery_GetPercentageMotors(void);

#endif /* BATTERY_H */
