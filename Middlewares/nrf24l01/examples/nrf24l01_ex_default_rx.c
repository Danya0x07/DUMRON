/*
 * Пример работы с приёмником с использованием настроек по умолчанию.
 * Дефолтная конфигурация передатчика и приёмника позволяет протестировать
 * их работоспособность. Она подразумевает односторонний обмен данными с
 * подтверждениями и фиксированным размером полезной нагрузки.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include <nrf24l01.h>

// вспомогательные функции для демонстрации
extern void uart_init(void);
extern void uart_logs(const char *);
extern void setup_gpio(void);
extern void setup_spi(void);
extern uint8_t transceiver_irq_pin_is_high(void);

/*
 * Чтобы настроить приёмник и создать буфер нужного размера, нам нужно знать
 * размер принимаемой полезной нагрузки. В дефолтной конфигурации он постоянен.
 */
char msg_buffer[sizeof("Hello NRF24L01/+!")];

int main(void)
{
    setup_gpio();
    setup_spi();
    uart_init();

    if (nrf24l01_rx_configure_default(sizeof(msg_buffer)) < 0) {
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
        while (transceiver_irq_pin_is_high())
            ;

        // получаем маску произошедших прерываний
        irq = nrf24l01_get_interrupts();

        if (irq & NRF24L01_IRQ_RX_DR) {  // если был получен пакет
            do {
                uart_logs("received: ");

                // // считываем полезную нагрузку и сбрасываем прерывание
                nrf24l01_read_pld(msg_buffer, sizeof(msg_buffer));
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);

                // и печатаем сообщение
                uart_logs(msg_buffer);
                uart_logs("\n");
            } while (nrf24l01_data_in_rx_fifo());  // пока есть пакеты в очереди
        }
    }
}
