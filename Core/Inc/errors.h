/**
 * В случае ошибок на сдадии инициализации робот должен как-то это отобразить.
 * Например попищать буззером или поморгать фарами.
 * В случае ошибок времени выполнения тоже нужно реагировать, но продолжать
 * работу опасно, поэтому просто остаёмся в бесконечном цикле.
 */

#ifndef _ERRORS_H
#define _ERRORS_H

#include <main.h>

void ErrorShow_InitRadio(void);
void ErrorShow_InitTemperature(void);

__NO_RETURN void ErrorCritical_StackOverflow(void);
__NO_RETURN void ErrorCritical_MallocFailed(void);

#endif /* _ERRORS_H */
