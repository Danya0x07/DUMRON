/*
 * Пример кода для измерения зашумлённости частотного диапазона и печати снимка
 * в UART.
 * Функции измерения зашумлённости могут вызываться как в режиме приёмника, так
 * и в режиме передатчика.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include <nrf24l01.h>

// вспомогательные функции для демонстрации
extern void uart_init(void);
extern void uart_logs(const char *);
extern void uart_logx(int);
extern void setup_gpio(void);
extern void setup_spi(void);

int main(void)
{
    setup_gpio();
    setup_spi();
    uart_init();

    if (nrf24l01_tx_configure_default() < 0) {
        /*
         * Обрабатываем ошибку инициализации.
         * Возможно она вызвана плохим контактом модуля с макетной платой,
         * если таковая используется.
         */
         uart_logs("nrf24l01 init error\n");
    }

    /*
     * Печатаем номера каналов, номер каждого канала печатается в виде
     * шестнадцатеричного числа, расположенного "вертикально": сначала старший
     * байт, потом младший.
     */
    uart_logs("\n\n\tnoise snapshot, channels:\n");
    for (uint8_t i = 0; i < NRF24L01_CHANNELS; i++) {
        uart_logx(i >> 4 & 0x0F);
    }
    uart_logs("\n");
    for (uint8_t i = 0; i < NRF24L01_CHANNELS; i++) {
        uart_logx(i & 0x0F);
    }
    uart_logs("\n\n");

    uint8_t noise_buff[NRF24L01_CHANNELS];

    for (;;) {
        nrf24l01_measure_noise(noise_buff, 0, NRF24L01_CHANNELS - 1);
        for (int i = 0; i < NRF24L01_CHANNELS; i++) {
            /*
             * Зашумлённости каналов представлены числами от 0 до F, поэтому
             * помещаются в 1 разряд. Таким образом каждому каналу будет
             * соответствовать одна колонка чисел в терминале.
             */
            uart_logx(noise_buff[i]);
        }
        uart_logs("\n");
    }
}
