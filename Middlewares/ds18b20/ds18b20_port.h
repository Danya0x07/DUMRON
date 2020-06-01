/**
 * @file
 * @brief   Платформозависимая часть библиотеки датчиков температуры DS18B20.
 *
 * Путём реализации данного интерфейса можно перенести библиотеку
 * на любую платформу.
 *
 * @note
 * В данном файле можно подключать любые заголовочные файлы,
 * необходимые для реализации объявленного интерфейса.
 *
 * @author  Danya0x07
 */

#ifndef DS18B20_PORT_H
#define DS18B20_PORT_H

#include <main.h>
#include <tim.h>

/**
 * @name    Интерфейс, используемый библиотекой.
 * @{
 */
#define onewire_pin_1() LL_GPIO_SetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define onewire_pin_0() LL_GPIO_ResetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define onewire_pin_is_1() LL_GPIO_IsInputPinSet(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define onewire_delay_us(us)    delay_us((us))
/** @} */

#endif /* DS18B20_PORT_H */
