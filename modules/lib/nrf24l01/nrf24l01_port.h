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

/**
 * @name    Интерфейс, используемый драйвером.
 * @{
 */
#define nrf_csn_1() LL_GPIO_SetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin)
#define nrf_csn_0() LL_GPIO_ResetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin)
#define nrf_ce_1()  LL_GPIO_SetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin)
#define nrf_ce_0()  LL_GPIO_ResetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin)
#define nrf_spi_send_recv(byte) spi_transfer((byte))
/** @} */

#endif /* NRF24L01_PORT_H_INCLUDED */
