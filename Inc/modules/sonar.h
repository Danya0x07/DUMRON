#ifndef SONAR_H_INCLUDED
#define SONAR_H_INCLUDED

#include "main.h"

/**
 * @brief  Опрашивает дальномер.
 * @retval Расстояние до объекта в см.
 */
uint16_t sonar_scan(void);

#endif
