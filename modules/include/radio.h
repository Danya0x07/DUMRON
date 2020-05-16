#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include <protocol.h>

void Radio_Init(void);
void Radio_TakeIncoming(DataToRobot_s *);
void Radio_PutOutcoming(DataFromRobot_s *);

#endif
