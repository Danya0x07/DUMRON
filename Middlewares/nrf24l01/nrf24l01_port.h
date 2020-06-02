/**
 * @file
 * @brief   Платформозависимая часть библиотеки трансиверов NRF24L01/+.
 *
 * @author  Danya0x07
 *
 * Путём реализации данного интерфейса можно перенести библиотеку
 * на любую платформу.
 *
 * @note
 * В данном файле можно подключать любые заголовочные файлы,
 * необходимые для реализации объявленного интерфейса.
 */

#ifndef NRF24L01_PORT_H
#define NRF24L01_PORT_H

#include <main.h>
#include <tim.h>
#include <spi.h>

/**
 * @name    Макросы для управления выводами трансивера.
 * @{
 */
#define _csn_1() LL_GPIO_SetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin)
#define _csn_0() LL_GPIO_ResetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin)
#define _ce_1()  LL_GPIO_SetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin)
#define _ce_0()  LL_GPIO_ResetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin)
/** @} */

/**
 * @brief   Обменивается байтом по SPI.
 *
 * @note
 * Если используемый стандарт языка не поддерживает inline, можно использовать
 * макрос.
 */
static inline uint8_t _spi_transfer_byte(uint8_t byte)
{
    return spi_transfer_byte(byte);
}

/**
 * @brief   Обменивается массивом байт по SPI1.
 *
 * @param[in] out   Буфер исходящих данных. NULL, если не исопльзуется.
 * @param[out] in   Буфер входящих данных. NULL, если не исопльзуется.
 * @param len   Количество байт для обмена.
 *
 * @warning
 * in и out не могут быть NULL одновременно.
 *
 * @note
 * Если используемый стандарт языка не поддерживает inline, можно использовать
 * макрос.
 */
static inline void _spi_transfer_bytes(uint8_t *in, const uint8_t *out,
                                       size_t len)
{
    spi_transfer_bytes(in, out, len);
}

/**
 * @brief   Функция миллисекундной задержки.
 */
#define _delay_ms(ms)   delay_ms((ms))

/**
 * @brief   Функция микросекундной задержки.
 *
 * @note
 * Данная функция не является необходимой. Если микросекундная задержка не
 * доступна, необходимо указать это в файле nrf24l01_conf.h
 */
#define _delay_us(us)   delay_us((us))

#endif /* NRF24L01_PORT_H */
