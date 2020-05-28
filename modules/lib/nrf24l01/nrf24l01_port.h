/**
 * @file
 * @brief   Платформозависимая часть драйвера трансиверов NRF24L01/+.
 *
 * Путём реализации данного интерфейса можно перенести библиотеку
 * на любую платформу.
 *
 * @note
 * В данном файле можно подключать любые заголовочные файлы,
 * необходимые для реализации объявленного интерфейса.
 */

#ifndef NRF24L01_PORT_H_INCLUDED
#define NRF24L01_PORT_H_INCLUDED

#include <libport.h>
#include "main.h"

/**
 * @name    Интерфейс, используемый драйвером.
 * @{
 */
#define nrf24l01_csn_1() LL_GPIO_SetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin)
#define nrf24l01_csn_0() LL_GPIO_ResetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin)
#define nrf24l01_ce_1()  LL_GPIO_SetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin)
#define nrf24l01_ce_0()  LL_GPIO_ResetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin)

static inline uint8_t nrf24l01_spi_transfer_byte(uint8_t byte)
{
    return spi_transfer_byte(byte);
}
static inline void nrf24l01_spi_transfer_bytes(uint8_t *in, const uint8_t *out,
                                               size_t len)
{
    spi_transfer_bytes(in, out, len);
}
/** @} */

#endif /* NRF24L01_PORT_H_INCLUDED */
