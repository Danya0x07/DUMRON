#ifndef TEMPERATURE_H_INCLUDED
#define TEMPERATURE_H_INCLUDED

#include "main.h"

void temperature_init(void);
int8_t temperature_get(uint64_t sensor_addr);

#define temperature_get_environment()   temperature_get(DS_ADDR_ENVIRONMENT)
#define temperatire_get_radiators()     temperatute_get(DS_ADDR_RADIATORS)

void temperature_print_addr(void);

#endif
