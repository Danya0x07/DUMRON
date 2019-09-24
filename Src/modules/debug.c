#include "modules/debug.h"
#include <stdlib.h>

static inline void debug_uart_send_byte(uint8_t byte)
{
    while (!LL_USART_IsActiveFlag_TXE(USART1))
        ;
    LL_USART_TransmitData8(USART1, byte);
}

void debug_led_set(_Bool state)
{
    if (state)
        LL_GPIO_SetOutputPin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
    else
        LL_GPIO_ResetOutputPin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin);
}

void debug_logs(char* str)
{
    while (*str)
        debug_uart_send_byte(*str++);
}

void debug_logi(int n)
{
    char buffer[17];
    debug_logs(itoa(n, buffer, 10));
}
