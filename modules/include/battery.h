/**
 * Функции получения примерного уровня заряда аккумуляторов робота.
 */
#ifndef BATTERY_H_INCLUDED
#define BATTERY_H_INCLUDED

#include <stdint.h>

uint8_t Battery_GetPercentageBrains(void);
uint8_t Battery_GetPercentageMotors(void);

#endif
