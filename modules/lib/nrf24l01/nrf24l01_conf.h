/**
 * @file
 * @brief   Файл с настройками для конкретного варианта использования
 *          драйвера NRF24L01/+.
 * @note
 * Заголовочные файлы, предоставляющие стандартные типы, тип bool и NULL,
 * могут быть заменены на локальные в проекте.
 */

#ifndef NRF24L01_CONF_H_INCLUDED
#define NRF24L01_CONF_H_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Должно быть закомментировано если используется
 * модификация трансивера без +.
 */
#define NRF24L01_PLUS

/** Раскомментировать, если микросекундная задержка не доступна. */
//#define NRF24L01_DELAY_ONLY_MS

#endif /* NRF24L01_CONF_H_INCLUDED */
