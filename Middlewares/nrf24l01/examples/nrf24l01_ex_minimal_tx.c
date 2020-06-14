/*
 * Пример работы с передатчиком, использующим настройки по умолчанию.
 * Минимальная конфигурация передатчика и приёмника позволяет протестировать
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
extern void uart_logi(int);
extern void setup_gpio(void);
extern void setup_spi(void);
extern uint8_t transiever_irq_pin_is_high(void);

/*
 * В дефолтной конфигурации трансивера размер полезной нагрузки постоянен,
 * передатчик должен соблюдать это правило, несмотря на то что при его настройке
 * этот размер нигду не указывается.
 */
char simple_msg[] = "Hello NRF24L01/+!";

int main(void)
{
    setup_gpio();
    setup_spi();
    uart_init();

    // на всякий случай ждём стабилизации трансивера после подачи питания
    _delay_ms(NRF24L01_PWR_ON_DELAY_MS);

    if (nrf24l01_tx_configure_minimal() < 0) {
        /*
         * Обрабатываем ошибку инициализации.
         * Возможно она вызвана плохим контактом модуля с макетной платой,
         * если таковая используется.
         */
         uart_logs("nrf24l01 init error\n");
    }

    uint8_t irq;
    uint8_t lost, retr;

    for (;;) {
        // записываем полезную нагрузку
        nrf24l01_tx_write_pld(simple_msg, sizeof(simple_msg));

        // отправляем
        nrf24l01_tx_transmit();

        // ждём прерывание (спад напряжения на пине IRQ трансивера)
        // правильнее было бы использовать внешнее прерывание МК
        while (transiever_irq_pin_is_high())
            ;

        // получаем маску произошедших прерываний
        irq = nrf24l01_get_interrupts();

        if (irq & NRF24L01_IRQ_TX_DS) {  // если получено автоподтверждение
            uart_logs("sent successfully\n");
            nrf24l01_clear_interrupts(NRF24L01_IRQ_TX_DS);
        }
        if (irq & NRF24L01_IRQ_MAX_RT) {  // если кончились попытки отправки
            uart_logs("failed to send\n");
            if (nrf24l01_full_tx_fifo())
                nrf24l01_flush_tx_fifo();
            nrf24l01_clear_interrupts(NRF24L01_IRQ_MAX_RT);
        }

        // читаем и печатаем статистику отправки
        nrf24l01_tx_get_statistics(&lost, &retr);

        uart_logs("lost: ");
        uart_logi(lost);
        uart_logs(" retransmits: ");
        uart_logi(retr);
        uart_logs("\n");

        _delay_ms(200);
    }
}
