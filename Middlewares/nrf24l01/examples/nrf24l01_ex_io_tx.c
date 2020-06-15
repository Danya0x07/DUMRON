/*
 * Этот пример работы с передатчиком показывает, как можно организовать
 * двухсторонний обмен на NRF24L01/+. Данные от приёмника принимаются
 * передатчиком вместе с пакетами автоподтверждения.
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
extern uint8_t transceiver_irq_pin_is_high(void);

struct tx_payload {  // структура полезной нагрузки, отправляемой передатчиком
    uint8_t data;
    uint8_t other_data;
} out_buffer;

struct ack_payload {  // структура полезной нагрузки, принимаемой передатчиком
    uint8_t data;
    uint8_t other_data;
} in_buffer;

void print_ack_pld(struct ack_payload *pld)
{
    uart_logs("data: ");
    uart_logi(pld->data);
    uart_logs("\tother_data: ");
    uart_logi(pld->other_data);
    uart_logs("\n");
}

int main(void)
{
    setup_gpio();
    setup_spi();
    uart_init();

    // сочиняем конфигурацию передатчика
    struct nrf24l01_tx_config config = {
        // пусть будет такой 3х-байтовый адрес
        .address = (uint8_t [3]){0x42, 0x42, 0x42},
        .addr_size = NRF24L01_ADDRS_3BYTE,

        // непринципиальные для данного примера настройки
        .crc_mode = NRF24L01_CRC_1BYTE,
        .datarate = NRF24L01_DATARATE_1MBPS,
        .power = NRF24L01_POWER_N6DBM,
        .retr_delay = NRF24L01_RETR_DELAY_1000US,
        .retr_count = NRF24L01_RETR_COUNT_5,
        .rf_channel = 42,  // такой же как у приёмника

        // выбираем режим для приёма автоподтверждений с полезной нагрузкой
        .mode = NRF24L01_TX_MODE_ACK_PAYLOAD,

        // включим все прерывания
        .en_irq = NRF24L01_IRQ_ALL
    };

    // на всякий случай ждём стабилизации трансивера после подачи питания
    _delay_ms(NRF24L01_PWR_ON_DELAY_MS);

    if (nrf24l01_tx_configure(&config) < 0) {
        /*
         * Обрабатываем ошибку инициализации.
         * Возможно она вызвана плохим контактом модуля с макетной платой,
         * если таковая используется.
         */
         uart_logs("nrf24l01 init error\n");
    }

#if (NRF24L01_PLUS == 0)  // только для модификации без +
    nrf24l01_activate();
#endif

    uint8_t irq;

    // пусть начальные отправляемые данные будут такими
    out_buffer.data = 0;
    out_buffer.other_data = 255;

    for (;;) {
        // записываем полезную нагрузку
        nrf24l01_tx_write_pld(&out_buffer, sizeof(struct tx_payload));

        // отправляем
        nrf24l01_tx_transmit();

        // ждём прерывание (спад напряжения на пине IRQ трансивера)
        // правильнее было бы использовать внешнее прерывание МК
        while (transceiver_irq_pin_is_high())
            ;

        // получаем маску произошедших прерываний
        irq = nrf24l01_get_interrupts();

        if (irq & NRF24L01_IRQ_TX_DS) {  // если получено автоподтверждение
            uart_logs("sent successfully\n");
            nrf24l01_clear_interrupts(NRF24L01_IRQ_TX_DS);
        }
        if (irq & NRF24L01_IRQ_MAX_RT) {  // если кончились попытки отправки
            uart_logs("failed to send\n");

            // если заполнили очередь отправки, чистим
            if (nrf24l01_full_tx_fifo())
                nrf24l01_flush_tx_fifo();

            nrf24l01_clear_interrupts(NRF24L01_IRQ_MAX_RT);
        }
        if (irq & NRF24L01_IRQ_RX_DR) {  // если прилетели данные с приёмника
            do {
                /*
                 * Поскольку используется динамический размер полезной нагрузки,
                 * его нужно считывать и проверять на валидность.
                 */
                if (nrf24l01_read_pld_size() != sizeof(struct ack_payload)) {
                    // чистим входную очередь
                    nrf24l01_flush_rx_fifo();
                    nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
                    uart_logs("incorrect ack pld size\n");
                    break;
                }

                // считываем полезную нагрузку и сбрасываем прерывание
                nrf24l01_read_pld(&in_buffer, sizeof(struct ack_payload));
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);

                // печатаем принятые данные
                uart_logs("ack payload recieved: ");
                print_ack_pld(&in_buffer);
            } while (nrf24l01_data_in_rx_fifo());  // пока есть данные в очереди
        }

        // изменяем отправляемые данные для наглядности
        out_buffer.data++;
        out_buffer.other_data--;

        _delay_ms(200);
    }
}
