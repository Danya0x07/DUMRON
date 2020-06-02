/**
 * Функции обнаружения опасного перепада высоты позади робота.
 */

#ifndef DISTANCE_H
#define DISTANCE_H

#include <stdbool.h>

bool Distance_DetectCliff(void);
bool Distance_DetectObstacle(void);

#endif /* DISTANCE_H */
