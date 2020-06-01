/**
 * Функции радиообмена с пультом.
 */

#ifndef RADIO_H
#define RADIO_H

#include <protocol.h>

void Radio_Init(void);
void Radio_TakeIncoming(DataToRobot_s *);
void Radio_PutOutcoming(DataFromRobot_s *);

#endif /* RADIO_H */
