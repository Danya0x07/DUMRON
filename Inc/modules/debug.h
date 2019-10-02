#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "main.h"

void debug_led_set(_Bool state);
void debug_uart_send_byte(uint8_t);
void debug_logs(char*);
void debug_logi(int);
void debug_logx(uint64_t);

#endif
