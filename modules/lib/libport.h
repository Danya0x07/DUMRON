/**
 * Интерфейс, который прямо или косвенно нужен библиотекам периферийных
 * устройств.
 */

#ifndef LIBPORT_H_INCLUDED
#define LIBPORT_H_INCLUDED

#include "main.h"

#define delay_ms(ms)    HAL_Delay((ms))

void delay_us(unsigned int us);
uint8_t spi_transfer(uint8_t byte);

#endif /* LIBPORT_H_INCLUDED */
