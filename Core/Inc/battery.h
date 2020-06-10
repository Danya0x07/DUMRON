/**
 * Функции получения примерного уровня заряда аккумуляторов робота.
 */

#ifndef _BATTERY_H
#define _BATTERY_H

#include <stdint.h>

void Battery_StartMeasurement(void);
uint8_t Battery_GetBrainsCharge(void);
uint8_t Battery_GetMotorsCharge(void);

#endif /* _BATTERY_H */
