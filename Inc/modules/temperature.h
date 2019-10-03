#ifndef TEMPERATURE_H_INCLUDED
#define TEMPERATURE_H_INCLUDED

#include "main.h"

#define TEMPERATURE_CONVERSION_TIME     800

void temperature_init(void);
void temperature_start_conversion(void);
int8_t temperature_get_environment(void);
int8_t temperature_get_radiators(void);
void temperature_print_addr(void);

#endif
