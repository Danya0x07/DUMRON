/**
 * @file
 * @brief   Простая кроссплатформенная библиотека для ультразвуковых
 *          дальномеров HC-SR04.
 *
 * @author  Danya0x07
 */
#ifndef HCSR04_H
#define HCSR04_H

#include <stdint.h>

int hcsr04_measure(unsigned int sensor_id);

#endif /* HCST04_H */
