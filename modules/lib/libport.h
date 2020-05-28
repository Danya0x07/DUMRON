/**
 * Интерфейс, который прямо или косвенно нужен библиотекам периферийных
 * устройств.
 */

#ifndef LIBPORT_H_INCLUDED
#define LIBPORT_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


void delay_ms(unsigned int ms);
void delay_us(unsigned int us);

uint8_t spi_transfer_byte(uint8_t byte);

/**
 * @brief   Обменивается массивом байт по SPI.
 *
 * @param[in] out   Буфер исходящих данных. NULL, если не исопльзуется.
 * @param[out] in   Буфер входящих данных. NULL, если не исопльзуется.
 * @param len   Количество байт для обмена.
 *
 * @warning @ref in и @ref out не должны быть NULL одновременно.
 */
void spi_transfer_bytes(uint8_t *in, const uint8_t *out, size_t len);

#endif /* LIBPORT_H_INCLUDED */
