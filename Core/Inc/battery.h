/**
 * Функции получения примерного уровня заряда аккумуляторов робота.
 */

#ifndef _BATTERY_H
#define _BATTERY_H

#include <stdint.h>

uint8_t Battery_GetChargeBrains(void);
uint8_t Battery_GetChargeMotors(void);

#endif /* _BATTERY_H */
