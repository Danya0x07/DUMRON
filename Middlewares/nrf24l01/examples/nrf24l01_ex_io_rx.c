/*
 * Этот пример работы с приёмником показывает, как можно организовать
 * двухсторонний обмен на NRF24L01/+. Данные с приёмника отправляются
 * передатчику вместе с пакетами автоподтверждения.
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

struct tx_payload {  // структура полезной нагрузки, принимаемой от передатчика
    uint8_t data;
    uint8_t other_data;
} in_buffer;

struct ack_payload {  // структура полезной нагрузки, отправляемой передатчику
    uint8_t data;
    uint8_t other_data;
} out_buffer;

void print_tx_pld(struct tx_payload *pld)
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

    // сочиняем конфигурацию приёмника
    struct nrf24l01_rx_config config = {
        // пусть будет 3х-байтовый адрес, (как и у передатчика)
        .addr_size = NRF24L01_ADDRS_3BYTE,

        // непринципиальные для данного примера настройки
        .crc_mode = NRF24L01_CRC_1BYTE,
        .datarate = NRF24L01_DATARATE_1MBPS,
        .rf_channel = 42,  // такой же как у передатчика

        // выбираем режим для отправки полезной нагрузки с автоподтверждениями
        .mode = NRF24L01_RX_MODE_DPL_ACK_PAYLOAD,

        // включим прерывание по приёму пакета
        .en_irq = NRF24L01_IRQ_RX_DR
    };

    // сочиняем конфигурацию входящего соединения, например 0-го.
    struct nrf24l01_pipe_config pipe0_conf = {
        // т.к. выбрано соединение 0, адрес задаётся массивом, размер которого
        // указан выше в конфигурации приёмника
        .address.array = (uint8_t [3]){0x42, 0x42, 0x42},

        // выбираем режим для отправки полезной нагрузки с автоподтверждениями
        .mode = NRF24L01_PIPE_MODE_ACK_DPL
        /*
         * Поскольку используется динамическая длина полезной нагрузки,
         * поле pld_size инициализировать не нужно.
         */
    };

    // на всякий случай ждём стабилизации трансивера после подачи питания
    _delay_ms(NRF24L01_PWR_ON_DELAY_MS);

    if (nrf24l01_rx_configure(&config) < 0) {
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

    // настраиваем входящее соединение
    nrf24l01_rx_setup_pipe(NRF24L01_PIPE0, &pipe0_conf);

    uint8_t irq;

    // пусть начальные отправляемые данные будут такими
    out_buffer.data = 0;
    out_buffer.other_data = 255;

    // начинаем слушать эфир
    nrf24l01_rx_start_listening();

    for (;;) {
        // если заполнили очередь отправки, чистим
        if (nrf24l01_full_tx_fifo())
            nrf24l01_flush_tx_fifo();

        // записываем в очередь полезную нагрузку для отправки вместе с
        // автоподтверждением для 0-го соединения.
        nrf24l01_rx_write_ack_pld(NRF24L01_PIPE0, &out_buffer,
                                  sizeof(struct ack_payload));

        // ждём прерывание (спад напряжения на пине IRQ трансивера)
        // правильнее было бы использовать внешнее прерывание МК
        while (transceiver_irq_pin_is_high())
            ;

        // если прилетели данные с передатчика
        if (nrf24l01_get_interrupts() & NRF24L01_IRQ_RX_DR) {
            do {
                /*
                 * Поскольку используется динамический размер полезной нагрузки,
                 * его нужно считываь и проверять на валидность.
                 */
                if (nrf24l01_read_pld_size() != sizeof(struct tx_payload)) {
                    // чистим входную очередь
                    nrf24l01_flush_rx_fifo();
                    nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
                    uart_logs("incorrect ack pld size\n");
                    break;
                }

                // считываем полезную нагрузку и сбрасываем прерывание
                nrf24l01_read_pld(&in_buffer, sizeof(struct tx_payload));
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);

                // печатаем принятые данные
                uart_logs("payload recieved: ");
                print_tx_pld(&in_buffer);
            } while (nrf24l01_data_in_rx_fifo());  // пока есть данные в очереди
        }

        // изменяем отправляемые данные для наглядности
        out_buffer.data++;
        out_buffer.other_data--;
    }
}
