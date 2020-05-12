#ifndef TEMPERATURE_H_INCLUDED
#define TEMPERATURE_H_INCLUDED

#include <stdint.h>

#define TEMPERATURE_CONVERSION_TIME     1000

void temperature_init(void);
void temperature_start_conversion(void);
int8_t temperature_get_ambient(void);
int8_t temperature_get_radiators(void);

#ifdef DEBUG
void temperature_print_address(void);
#endif

#endif
