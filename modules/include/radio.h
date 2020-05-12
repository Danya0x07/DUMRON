#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include <protocol.h>

void radio_init(void);
void radio_take_incoming(DataToRobot*);
void radio_put_outcoming(DataFromRobot*);

#endif
