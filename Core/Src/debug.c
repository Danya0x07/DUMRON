#include <stdlib.h>

#include "main.h"

static void debug_uart_send_byte(uint8_t byte)
{
    while (!LL_USART_IsActiveFlag_TC(USART1))
        ;
    LL_USART_TransmitData8(USART1, byte);
}

void debug_logs(const char *str)
{
    while (*str)
        debug_uart_send_byte(*str++);
}

void debug_logi(int n)
{
    char buffer[20];
    debug_logs(itoa(n, buffer, 10));
}

void debug_logx(int n)
{
    char buffer[20];
    debug_logs(itoa(n, buffer, 16));
}
