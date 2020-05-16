/**
 * Функции управления различного рода излучателями, установленными на роботе.
 *
 * Кроме как "излучателями", обобщить светодиод, фары и пьезобуззер
 * никак не получилось.
 */
#ifndef EMMITERS_H_INCLUDED
#define EMMITERS_H_INCLUDED

#include <stdbool.h>

void Led_SetState(bool state);
void Lights_SetState(bool state);
void Buzzer_SetState(bool state);

#endif
