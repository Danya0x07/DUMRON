#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "main.h"

static inline void debug_led_set(_Bool active)
{
	LL_GPIO_SetOutputPin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
}

static inline void logi(uint8_t number)
{

}

#endif
