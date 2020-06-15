/*
 * Пример работы с приёмником с использованием нескольких входящих соединений.
 * В этом примере используются соединения 0, 1 и 2.
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

struct p0_payload {  // формат полезной нагрузки для 0-го соединения
    uint8_t data0;
} pld0;

struct p1_payload {  // формат полезной нагрузки для 1-го соединения
    uint8_t data0;
    uint8_t data1;
} pld1;

struct p2_payload {  // формат полезной нагрузки для 2-го соединения
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
} pld2;

void print_payload_content(uint8_t *pld, uint8_t size)
{
    for (uint8_t i = 0; i < size; i++) {
        uart_logs("data");
        uart_logi(i);
        uart_logs(": ");
        uart_logi(pld[i]);
        uart_logs("\n");
    }
}

int main(void)
{
    setup_gpio();
    setup_spi();
    uart_init();

    // сочиняем конфигурацию приёмника
    struct nrf24l01_rx_config config = {
        // пусть будет 4х-байтовый адрес
        .addr_size = NRF24L01_ADDRS_4BYTE,

        // непринципиальные для данного примера настройки,
        // однако CRC не может быть OFF, т.к. используются автоподтверждения
        .crc_mode = NRF24L01_CRC_1BYTE,
        .datarate = NRF24L01_DATARATE_1MBPS,
        .rf_channel = 42,  // такой же как у передатчика

        // выбираем самый простой режим, с постоянным размером полезной нагрузки
        .mode = NRF24L01_RX_MODE_NODPL,

        // включим прерывание по приёму пакета
        .en_irq = NRF24L01_IRQ_RX_DR
    };

    // сочиняем конфигурацию 0-го входящего соединения.
    struct nrf24l01_pipe_config pipe0_conf = {
        // т.к. выбрано соединение 0, адрес задаётся массивом, размер которого
        // указан выше в конфигурации приёмника
        .address.array = (uint8_t [4]){0x24, 0x24, 0x24, 0x24},

        // выбираем режим с отправкой автоподтверждений
        .mode = NRF24L01_PIPE_MODE_ACK,

        // устанавливаем размер принимаемой полезной нагрузки
        .pld_size = sizeof(struct p0_payload)
    };

    // аналогично для других соединений

    struct nrf24l01_pipe_config pipe1_conf = {
        .address.array = (uint8_t [4]){0x33, 0x33, 0x33, 0x33},

        // почему бы для разнообразия не отказаться от автоподтверждений?
        .mode = NRF24L01_PIPE_MODE_NOACK,
        .pld_size = sizeof(struct p1_payload)
    };

    struct nrf24l01_pipe_config pipe2_conf = {
        .address.lsb = 0x99,  // для соединений 2-5 адрес задаётся 1 байтом
        .mode = NRF24L01_PIPE_MODE_ACK,
        .pld_size = sizeof(struct p2_payload)
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

    // настраиваем входящие соединения
    nrf24l01_rx_setup_pipe(NRF24L01_PIPE0, &pipe0_conf);
    nrf24l01_rx_setup_pipe(NRF24L01_PIPE1, &pipe1_conf);
    nrf24l01_rx_setup_pipe(NRF24L01_PIPE2, &pipe2_conf);

    // начинаем слушать эфир
    nrf24l01_rx_start_listening();

    for (;;) {
        // ждём прерывание (спад напряжения на пине IRQ трансивера)
        // правильнее было бы использовать внешнее прерывание МК
        while (transceiver_irq_pin_is_high())
            ;

        // если прилетели данные с передатчика
        if (nrf24l01_get_interrupts() & NRF24L01_IRQ_RX_DR) {
            int pipe_no;
            void *pld = &pld2;  // инициализируем чем-нибудь на всякий случай
            uint8_t size = sizeof(struct p2_payload);

            do {
                // получаем номер соединения, для которого прилетел пакет
                pipe_no = nrf24l01_rx_get_pld_pipe_no();

                // проверяем на валидность, в этом примере это не обязательно
                if (pipe_no < 0) {
                    uart_logs("wrong pipe number\n");
                    break;
                }

                switch (pipe_no)
                {
                case NRF24L01_PIPE0:
                    uart_logs("recieved payload for pipe 0\n");
                    pld = &pld0;
                    size = sizeof(struct p0_payload);
                    break;

                case NRF24L01_PIPE1:
                    uart_logs("recieved payload for pipe 1\n");
                    pld = &pld1;
                    size = sizeof(struct p1_payload);
                    break;

                case NRF24L01_PIPE2:
                    uart_logs("recieved payload for pipe 2\n");
                    pld = &pld2;
                    size = sizeof(struct p2_payload);
                    break;

                // остальные соединения у нас закрыты
                case NRF24L01_PIPE3:
                case NRF24L01_PIPE4:
                case NRF24L01_PIPE5:
                default:
                    uart_logs("impossible thing occured\n");
                    break;
                }

                // считываем полезную нагрузку и сбрасываем прерывание
                nrf24l01_read_pld(pld, size);
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);

                // печатаем принятые данные
                print_payload_content(pld, size);
                uart_logs("\n\n");
            } while (nrf24l01_data_in_rx_fifo());  // пока есть данные в очереди
        }
    }
}
