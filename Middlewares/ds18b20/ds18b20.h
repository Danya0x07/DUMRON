/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

/**
 * @mainpage Simple cross-platform DS18B20 library
 *
 * Простая кроссплатформенная библиотека для датчиков температуры DS18B20.
 * Код библиотеки совместим со стандартом С99.
 *
 * Для расчёта контрольной суммы CRC реализован только побитовый  алгоритм,
 * поскольку выигрыш по скорости от табличного метода не будет иметь большого
 * по сравнению с проигрышем по памяти значения вследствие невысокой скорости
 * самого протокола 1-Wire.
 *
 * @note Поддерживается работа только с одной линией 1-Wire.
 *
 * @author  Danya0x07 <dlef0xf8@gmail.com>
 */

/**
 * @file    ds18b20.h
 * @brief   Главный заголовочный файл библиотеки.
 */

#ifndef _DS18B20_H
#define _DS18B20_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Возможные разрешения значения температуры.
 */
enum ds18b20_resolution {
    DS18B20_RESOLUTION_9BIT  = 0x1F,
    DS18B20_RESOLUTION_10BIT = 0x3F,
    DS18B20_RESOLUTION_11BIT = 0x5F,
    DS18B20_RESOLUTION_12BIT = 0x7F
};

/**
 * @brief   Структура настроек датчика.
 *
 * Разрешение значения температуры влияет на длительность измерения.
 */
struct ds18b20_config {
    enum ds18b20_resolution resolution;  /**< разрешение результата измерений */
};

/**
 * @name    Длительности процесса измерения для различных разрешений
 * @details в миллисекундах и с небольшим запасом.
 * @{
 */
#define DS18B20_MEASURE_TIME_9BIT_MS    94
#define DS18B20_MEASURE_TIME_10BIT_MS   188
#define DS18B20_MEASURE_TIME_11BIT_MS   375
#define DS18B20_MEASURE_TIME_12BIT_MS   750
/** @} */

/**
 * @name    Статусы успешности операций с датчиками.
 * @{
 */
#define DS18B20_OK      (0)
#define DS18B20_EABSENT (-1)
#define DS18B20_ECRC    (-2)
#define DS18B20_EBUSY   (-3)
/** @} */

/**
 * @brief   Отправляет импульс перезагрузки по линии 1-Wire
 *
 * Предполагается, что данная функция будет использоваться в качестве проверки
 * до вызова остальных API-функций обращения с датчиками.
 *
 * @return  `DS18B20_OK`, если был получен импульс присутствия от датчиков,
 * @return  `DS18B20_EABSENT`, если не был получен импульс присутствия.
 */
int ds18b20_check_presense(void);

/**
 * @brief   Считывает адрес датчика на линии.
 *
 * @attention
 * Работает только когда на линии один единственный датчик.
 *
 * @param[out] address  8-байтовый массив для принимаемого адреса.
 *
 * @return  `DS18B20_OK`, если всё прошло успешно,
 * @return  `DS18B20_EABSENT`, если если не был получен импульс присутствия,
 * @return  `DS18B20_ECRC`, если не совпали контрольные суммы.
 */
int ds18b20_read_address(uint8_t address[8]);

/**
 * @brief   Записывает настройки в DS18B20.
 *
 * @param address   8-байтовый массив с адресом целевого датчика
 *                  или NULL, если действие относится ко всем датчикам на линии.
 *
 * @param config    Указатель на структуру с настройками.
 *
 * @return  `DS18B20_OK`, если всё прошло успешно,
 * @return  `DS18B20_EABSENT`, если если не был получен импульс присутствия.
 */
int ds18b20_configure(const uint8_t *address,
                      const struct ds18b20_config *config);

/**
 * @brief   Запускает измерение температуры.
 *
 * @note
 * Если используется паразитное питание (DS18B20_PARASITE_POWER == 1),
 * эта функция блокирует управление на время DS18B20_MEASURE_TIME_12BIT_MS.
 *
 * @param address   8-байтовый массив с адресом целевого датчика
 *                  или NULL, если действие относится ко всем датчикам на линии.
 *
 * @return  `DS18B20_OK`, если всё прошло успешно,
 * @return  `DS18B20_EABSENT`, если если не был получен импульс присутствия,
 */
int ds18b20_start_measurement(const uint8_t *address);

/**
 * @brief   Считывает результат измерения температуры.
 *
 * @attention
 * Эту функцию можно вызывать не ранее чем через соответствующее выбранному
 * разрешению температуры время после вызова ds18b20_start_measurement().
 * Если же используется паразитное питание (DS18B20_PARASITE_POWER == 1), эту
 * функцию вызывать можно сразу после ds18b20_start_measurement().
 *
 * @param[in] address   8-байтовый массив с адресом опрашиваемого датчика
 *                      или NULL, если на линии один единственный датчик.
 *
 * @param[out] result   Указатель на переменную, в которую будет записан
 *                      результат измерения.
 *
 * @note
 * Результат записывается в градусах Цельсия в "сыром" виде,
 * т.е. 4 младших десятичных разряда отведены под дробную часть.
 * Например, для 10.0625 результат будет 100625,
 * а для -52.5 результат будет -525000.
 *
 * @return  `DS18B20_OK`, если всё прошло успешно,
 * @return  `DS18B20_EABSENT`, если если не был получен импульс присутствия,
 * @return  `DS18B20_ECRC`, если не совпали контрольные суммы.
 */
int ds18b20_get_result(const uint8_t *address, int32_t *result);

/**
 * @brief   Производит измерение температуры.
 *
 * @attention
 * Эта функция блокирует поток управления на время измерений.
 *
 * @param[in] address   8-байтовый массив с адресом опрашиваемого датчика
 *                      или NULL, если на линии один единственный датчик.
 *
 * @param[out] result   Указатель на переменную, в которую будет записан
 *                      результат измерения.
 *
 * @note
 * Формат результата измерения такой же, как у ds18b20_get_result().
 *
 * @return  `DS18B20_OK`, если всё прошло успешно,
 * @return  `DS18B20_EABSENT`, если если не был получен импульс присутствия,
 * @return  `DS18B20_ECRC`, если не совпали контрольные суммы.
 * @return  `DS18B20_EBUSY`, если истёк срок ожидания (скорее всего означает
 *           короткое замыкание линии 1-Wire на землю).
 */
int ds18b20_measure(const uint8_t *address, int32_t *result);

/**
 * @brief   Парсит результат измерения на целую и дробную части.
 *
 * Вообще, распарсить результат измерения температуры может понадобиться
 * в том случае, если его нужно отобразить на каком-нибудь дисплее,
 * либо если нужна именно целая его часть.
 * Если нужно сравнить результаты измерений, то делать это гораздо удобнее
 * в "сыром" виде.
 *
 * @param[in] result    Результат измерения, полученный вызовом функции
 *                      ds18b20_get_result() или ds18b20_measure().
 *
 * @param[out] integer      Указатель на переменную, в которую будет записана
 *                          целая часть результата (*с учётом знака*), или NULL,
 *                          если она не нужна.
 *
 * @param[out] fractional   Указатель на переменную, в которую будет записана
 *                          дробная часть результата (*без учёта знака*),
 *                          или NULL,  если она не нужна.
 */
void ds18b20_parse_result(int32_t result, int8_t *integer,
                          uint16_t *fractional);

#ifdef __cplusplus
}
#endif

#endif  /* _DS18B20_H */
