#ifndef BATTERY_H_INCLUDED
#define BATTERY_H_INCLUDED

#include <stdint.h>

uint8_t battery_get_percentage_brains(void);
uint8_t battery_get_percentage_motors(void);

#endif
