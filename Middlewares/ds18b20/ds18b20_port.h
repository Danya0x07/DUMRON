/**
 * @file    ds18b20_port.h
 * @brief   Платформозависимая часть библиотеки.
 *
 * Путём реализации интерфейса, объявленного в данном файле можно перенести
 * библиотеку на любую платформу.
 *
 * @note
 * В данном файле можно подключать любые заголовочные файлы, необходимые
 * для реализации объявленного интерфейса.
 */

#ifndef _DS18B20_PORT_H
#define _DS18B20_PORT_H

#include <main.h>
#include <tim.h>

/**
 * @name    Макросы для управления линией 1-Wire.
 * @{
 */
#define _wire_high()    LL_GPIO_SetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define _wire_low()     LL_GPIO_ResetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define _wire_is_high() LL_GPIO_IsInputPinSet(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
/** @} */

/** Функция микросекундной задержки. */
#define _delay_us(us)   delay_us((us))

#endif /* _DS18B20_PORT_H */
