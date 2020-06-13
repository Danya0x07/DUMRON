/*
 * Пример кода для приёмника.
 * В этом примере приёмник принимает данные от передатчика.
 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include <nrf24l01.h>

extern void uart_init(void);
extern void uart_logs(const char *);
extern void setup_gpio(void);
extern void setup_spi(void);

/*
 * В конфигурации по умолчанию размер полезной нагрузки постоянен, поэтому его
 * нужно указать при настройке трансивера. Поэтому нам нужен массив
 * _expected_msg - чтобы создать входной буфер нужного размера.
 */
const char _expected_msg[] = "Hello NRF24L01/+!";
char msg_buffer[sizeof(_expected_msg)];

int main(void)
{
    setup_gpio();
    setup_spi();
    uart_init();

    if (nrf24l01_rx_configure_minimal(sizeof(msg_buffer)) < 0) {
        /*
         * Обрабатываем ошибку инициализации.
         * Возможно она вызвана плохим контактом модуля с макетной платой,
         * если таковая используется.
         */
         uart_logs("nrf24l01 init error\n");
    }

    uint8_t irq;

    for (;;) {
        // ждём прерывание (спад напряжения на пине IRQ трансивера)
        while (transiever_irq_pin_is_high())
            ;

        // получаем маску произошедших прерываний
        irq = nrf24l01_get_interrupts();

        if (irq & NRF24L01_IRQ_RX_DR) {  // если был получен пакет
            do {
                uart_logs("received: ");

                // считываем полезную нагрузку
                nrf24l01_read_pld(msg_buffer, sizeof(msg_buffer));

                // очищаем прерывание
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);

                // и печатаем сообщение
                uart_logs(msg_buffer);
                uart_logs("\n");
            } while (nrf24l01_data_in_rx_fifo());  // пока есть пакеты в очереди
        }
    }
}
