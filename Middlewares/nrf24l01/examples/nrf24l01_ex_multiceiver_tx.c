/*
 * В данном примере передатчик симулирует 3 передатчика, шлющих пакеты на 3
 * открытых соединения приёмника соответственно. Это осуществляется с помощью
 * последовательной смены конфигураций передатчика.
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

struct p0_payload {  // формат полезной нагрузки для 0-го соединения приёмника
    uint8_t data0;
} pld0 = {0};

struct p1_payload {  // формат полезной нагрузки для 1-го соединения приёмника
    uint8_t data0;
    uint8_t data1;
} pld1 = {0};

struct p2_payload {  // формат полезной нагрузки для 2-го соединения приёмника
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
} pld2 = {0};

int main(void)
{
    setup_gpio();
    setup_spi();
    uart_init();

    // адреса 3-х соединений приёмника
    uint8_t address0[4] = {0x24, 0x24, 0x24, 0x24};
    uint8_t address1[4] = {0x33, 0x33, 0x33, 0x33};
    uint8_t address2[4] = {0x99, 0x33, 0x33, 0x33};

    // 3 конфигурации передатчика, каждая настроена на одно соединение приёмника

    struct nrf24l01_tx_config config0 = {
        .address = address0,
        .addr_size = NRF24L01_ADDRS_4BYTE,

        // непринципиальные для данного примера настройки
        .crc_mode = NRF24L01_CRC_1BYTE,
        .datarate = NRF24L01_DATARATE_2MBPS,
        .power = NRF24L01_POWER_N6DBM,
        .retr_delay = NRF24L01_RETR_DELAY_1000US,
        .retr_count = NRF24L01_RETR_COUNT_4,
        .rf_channel = 42,  // такой же как у приёмника

        // выбираем режим для приёма автоподтверждений
        .mode = NRF24L01_TX_MODE_ACK,

        // в этом примере не потребуется отслеживать логический уровень пина IRQ
        .en_irq = 0
    };

    // некоторые настройки вполне могут отличаться, в данном случае, для 1-го
    // соединения приёмник настроен не высылать автоподтверждения
    struct nrf24l01_tx_config config1 = config0;
    config1.address = address1;
    config1.mode = NRF24L01_TX_MODE_NOACK;

    struct nrf24l01_tx_config config2 = config0;
    config2.address = address2;

    /*
     * Механизм смены конфигураций: каждому "виртуальному" передатчику
     * соответствует своя конфигурация, полезная нагрузка и её размер.
     * Текущая конфигурация определяется индексом.
     */
    struct nrf24l01_tx_config *configs[3] = {&config0, &config1, &config2};
    void *payloads[3] = {&pld0, &pld1, &pld2};
    uint8_t pld_sizes[3] = {sizeof(pld0), sizeof(pld1), sizeof(pld2)};
    int index = 0;

    // на всякий случай ждём стабилизации трансивера после подачи питания
    _delay_ms(NRF24L01_PWR_ON_DELAY_MS);

    for (;;) {
        // настраиваем передатчик
        if (nrf24l01_tx_configure(configs[index]) < 0) {
            uart_logs("nrf24l01 config error\n");
        }

        // записываем полезную нагрузку
        nrf24l01_tx_write_pld(payloads[index], pld_sizes[index]);

        // отправляем
        nrf24l01_tx_transmit();

        // если кончились повторные попытки отправки
        if (nrf24l01_get_interrupts() & NRF24L01_IRQ_MAX_RT) {
            uart_logs("failed to send packet for pipe ");
            uart_logi(index);
            uart_logs("\n");

            // если заполнили очередь отправки, чистим
            if (nrf24l01_full_tx_fifo())
                nrf24l01_flush_tx_fifo();

            nrf24l01_clear_interrupts(NRF24L01_IRQ_MAX_RT);
        }

        if (++index > 2)  // индекс работает как циклический счётчик
            index = 0;

        // изменяем отправляемые данные для наглядности
        pld0.data0++;

        pld1.data0++;
        pld1.data1++;

        pld2.data0++;
        pld2.data1++;
        pld2.data2++;

        _delay_ms(333);
    }
}
