/**
 * Функции управления различного рода излучателями, установленными на роботе.
 *
 * Кроме как "излучателями", обобщить светодиод, фары и пьезобуззер
 * никак не получилось.
 */

#ifndef EMMITERS_H
#define EMMITERS_H

#include <stdbool.h>

void Led_SetState(bool state);
void Lights_SetState(bool state);
void Buzzer_SetState(bool state);

#endif /* EMMITERS_H */
