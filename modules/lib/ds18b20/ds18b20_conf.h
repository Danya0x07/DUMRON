/**
 * @file
 * @brief   Файл с настройками для конкретного варианта использования
 *          драйвера DS18B20.
 */
#ifndef DS18B20_CONF_H_INCLUDED
#define DS18B20_CONF_H_INCLUDED

#include "main.h"

/**
 * @brief   Платформозависимые макросы и функции.
 *
 * Путём реализации данного интерфейса можно перенести библиотеку
 * на любую платформу.
 */
#define onewire_pin_1() LL_GPIO_SetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define onewire_pin_0() LL_GPIO_ResetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define onewire_pin_is_1() LL_GPIO_IsInputPinSet(ONEWIRE_GPIO_Port, ONEWIRE_Pin)

void delay_us(unsigned int);

#endif /* DS18B20_CONF_H_INCLUDED */
