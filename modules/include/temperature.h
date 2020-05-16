/**
 * Модуль измерения температуры.
 *
 * Принцип использования такой: вызываем Temperature_StartMeasurement()
 * и после этого, не ранее чем через TEMPERATURE_MEASURE_TIME_MS
 * можно вызывать Temperature_GetAmbient() и Temperature_GetRadiators().
 *
 * Честно говоря, измерение температуры роботу совершенно не нужно,
 * просто исторически сложилось так, что у него есть датчики температуры.
 */
#ifndef TEMPERATURE_H_INCLUDED
#define TEMPERATURE_H_INCLUDED

#include <stdint.h>

#define TEMPERATURE_MEASURE_TIME_MS     1000

void Temperature_Init(void);
void Temperature_StartMeasurement(void);
int8_t Temperature_GetAmbient(void);
int8_t Temperature_GetInternal(void);

#endif
