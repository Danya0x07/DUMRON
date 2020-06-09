/**
 * Функции радиообмена с пультом.
 */

#ifndef _RADIO_H
#define _RADIO_H

#include "protocol.h"

void Radio_Init(void);
void Radio_TakeIncoming(DataToRobot_s *);
void Radio_PutOutcoming(DataFromRobot_s *);

#endif /* _RADIO_H */
