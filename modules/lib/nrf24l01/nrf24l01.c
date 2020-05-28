/**
 * Функции для низкоуровневого взаимодействия
 * с модулем NRF24L01+ по интерфейсной шине SPI.
 */
#include "nrf24l01.h"
#include "nrf24l01_port.h"

/* Команды радиомодуля */
enum nrf24l01_command {
    R_REGISTER = 0x00,   /* + n Прочитать регистр n */
    W_REGISTER = 0x20,   /* + n Записать регистр n */
    R_RX_PAYLOAD = 0x61, /* Принять данные данные из верхнего слота очереди приёмника. */
    W_TX_PAYLOAD = 0xA0, /* Записать в очередь передатчика данные для отправки */
    FLUSH_TX = 0xE1,     /* Сбросить очередь передатчика */
    FLUSH_RX = 0xE2,     /* Сбросить очередь приёмника */
    REUSE_TX_PL = 0xE3,  /* Использовать повторно последний переданный пакет */
    R_RX_PL_WID = 0x60,  /* Прочитать размер данных принятого пакета в начале очереди приёмника. */
    W_ACK_PAYLOAD = 0xA8,  /* + p Записать данные для отправки с пакетом подтверждения по каналу p. */
    W_TX_PAYLOAD_NOACK = 0xB0,  /* Записать в очередь передатчика данные, для отправки без подтверждения */
    NOP = 0xFF  /* Нет операции. Может быть использовано для чтения регистра статуса */
};

/* Регистры радуомодуля */
enum nrf24l01_reg {
    CONFIG = 0x00,  /* Регистр настроек */
    EN_AA = 0x01,   /* Выбор автоподтверждения */
    EN_RXADDR = 0x02,  /* Выбор каналов приёмника */
    SETUP_AW = 0x03,   /* Настройка размера адреса */
    SETUP_RETR = 0x04, /* Настройка повторной отправки */
    RF_CH = 0x05,  /* Номер радиоканала, на котором осуществляется работа. От 0 до 125. */
    RF_SETUP = 0x06,  /* Настройка радиоканала */
    STATUS = 0x07,    /* Регистр статуса. */
    OBSERVE_TX = 0x08,  /* Количество повторов передачи и потерянных пакетов */
    RPD_CD = 0x09,  /* Мощность принимаемого сигнала. Если младший бит = 1, то уровень более -64dBm */
    RX_ADDR_P0 = 0x0A,  /* 3-5 байт (начиная с младшего байта). Адрес канала 0 приёмника. */
    RX_ADDR_P1 = 0x0B,  /* 3-5 байт (начиная с младшего байта). Адрес канала 1 приёмника. */
    RX_ADDR_P2 = 0x0C,  /* Младший байт адреса канала 2 приёмника. Старшие байты из RX_ADDR_P1 */
    RX_ADDR_P3 = 0x0D,  /* Младший байт адреса канала 3 приёмника. Старшие байты из RX_ADDR_P1 */
    RX_ADDR_P4 = 0x0E,  /* Младший байт адреса канала 4 приёмника. Старшие байты из RX_ADDR_P1 */
    RX_ADDR_P5 = 0x0F,  /* Младший байт адреса канала 5 приёмника. Старшие байты из RX_ADDR_P1 */
    TX_ADDR = 0x10,  /* 3-5 байт (начиная с младшего байта). Адрес удалённого устройства для передачи */
    RX_PW_P0 = 0x11,  /* Размер данных при приёме по каналу 0: от 1 до 32. 0 - канал не используется. */
    RX_PW_P1 = 0x12,  /* Размер данных при приёме по каналу 1: от 1 до 32. 0 - канал не используется. */
    RX_PW_P2 = 0x13,  /* Размер данных при приёме по каналу 2: от 1 до 32. 0 - канал не используется. */
    RX_PW_P3 = 0x14,  /* Размер данных при приёме по каналу 3: от 1 до 32. 0 - канал не используется. */
    RX_PW_P4 = 0x15,  /* Размер данных при приёме по каналу 4: от 1 до 32. 0 - канал не используется. */
    RX_PW_P5 = 0x16,  /* Размер данных при приёме по каналу 5: от 1 до 32. 0 - канал не используется. */
    FIFO_STATUS = 0x17, /* Состояние очередей FIFO приёмника и передатчика */
    DYNPD = 0x1C,   /* Выбор каналов приёмника для которых используется произвольная длина пакетов. */
    FEATURE = 0x1D  /* Регистр опций */
};

/* Настройка CRC. */

#define PWR_UP      (1 << 1)
#define PRIM_RX     (1 << 0)

#ifdef NRF24L01_PLUS
    #define CONT_WAVE  (1 << 7)  /* (Только для nRF24L01+) Непрерывная передача несущей (для тестов) */
#else
    #define LNA_HCURR   (1 << 0)
#endif

#define PLL_LOCK    (1 << 4)  /* Для тестов */

/* STATUS */
#define RX_DR   (1 << 6)  /* Флаг получения новых данных в FIFO приёмника. Для сброса флага нужно записать 1 */
#define TX_DS   (1 << 5)  /* Флаг завершения передачи. Для сброса флага нужно записать 1 */
#define MAX_RT  (1 << 4)  /* Флаг превышения установленного числа повторов. Без сброса (записать 1) обмен невозможен */
#define RX_P_NO (1 << 1)  /* 3 бита. Номер канала, данные для которого доступны в FIFO приёмника. 7 -  FIFO пусто. */
/* Признак заполнения FIFO передатчика: 1 - заполнено; 0 - есть доступные слоты
  (переименовано из TX_FULL во избежание путаницы с одноимённым битом из регистра FIFO_STATUS) */
#define TX_FULL_STATUS (1 << 0)
#define INITIAL_STATUS_VALUE    0x0E

/* OBSERVE_TX */
#define PLOS_CNT 4  /* 4 бита. Общее количество пакетов без подтверждения. Сбрасывается записью RF_CH */
#define ARC_CNT  0  /* 4 бита. Количество предпринятых повторов при последней отправке. */

/* FIFO_STATUS */
#define TX_REUSE (1 << 6)  /* Признак готовности последнего пакета для повторной отправки. */
/* Флаг переполнения FIFO очереди передатчика.
  (переименовано из TX_FULL во избежание путаницы с одноимённым битом из регистра STATUS) */
#define TX_FULL_FIFO (1 << 5)
#define TX_EMPTY (1 << 4)  /* Флаг освобождения FIFO очереди передатчика. */
#define RX_FULL  (1 << 1)  /* Флаг переполнения FIFO очереди приёмника. */
#define RX_EMPTY (1 << 0)  /* Флаг освобождения FIFO очереди приёмника. */

/* FEATURE */
#define EN_DPL     (1 << 2)  /* Включает поддержку приёма и передачи пакетов произвольной длины */
#define EN_ACK_PAY (1 << 1)  /* Разрешает передачу данных с пакетами подтверждения приёма */
#define EN_DYN_ACK (1 << 0)  /* Разрешает использование W_TX_PAYLOAD_NOACK */

#define NRF24L01_FEATURES_MASK  0x07

static uint8_t nrf24l01_cmd(enum nrf24l01_command cmd)
{
    uint8_t status;
    nrf24l01_csn_0();
    status = nrf24l01_spi_transfer_byte(cmd);
    nrf24l01_csn_1();
    return status;
}

static inline uint8_t nrf24l01_get_status(void)
{
    return nrf24l01_cmd(NOP);
}

static uint8_t nrf24l01_read_reg(enum nrf24l01_reg reg)
{
    uint8_t value;
    nrf24l01_csn_0();
    nrf24l01_spi_transfer_byte(R_REGISTER | reg);
    value = nrf24l01_spi_transfer_byte(NOP);
    nrf24l01_csn_1();
    return value;
}

static void nrf24l01_write_reg(enum nrf24l01_reg reg, uint8_t value)
{
    nrf24l01_csn_0();
    nrf24l01_spi_transfer_byte(W_REGISTER | reg);
    nrf24l01_spi_transfer_byte(value);
    nrf24l01_csn_1();
}

static void nrf24l01_write_bits(enum nrf24l01_reg reg, uint8_t bits, bool state)
{
    uint8_t value = nrf24l01_read_reg(reg);
    if (state)
        value |= bits;
    else
        value &= ~bits;
    nrf24l01_write_reg(reg, value);
}

static void nrf24l01_read_buffer(uint8_t cmd, uint8_t *buff, uint8_t size)
{
    nrf24l01_csn_0();
    nrf24l01_spi_transfer_byte(cmd);
    nrf24l01_spi_transfer_bytes(buff, NULL, size);
    nrf24l01_csn_1();
}

static void nrf24l01_write_buffer(uint8_t cmd, const uint8_t *buff,
                                  uint8_t size)
{
    nrf24l01_csn_0();
    nrf24l01_spi_transfer_byte(cmd);
    nrf24l01_spi_transfer_bytes(NULL, buff, size);
    nrf24l01_csn_1();
}

#ifndef NRF24L01_PLUS

void nrf24l01_activate(void)
{
    nrf24l01_csn_0();
    nrf24l01_spi_transfer_byte(0x50);
    nrf24l01_spi_transfer_byte(0x73);
    nrf24l01_csn_1();
}
#endif

int nrf24l01_tx_configure(struct nrf24l01_tx_config *config)
{
    config->en_irq &= NRF24L01_IRQ_ALL;
    config->features &= NRF24L01_FEATURES_MASK;

    /*
     * Запись 1 на место бита прерывания в регистре CONFIG выключает данное
     * прерывание, поэтому пользовательскую маску нужно инвертировать.
     */
    nrf24l01_write_reg(CONFIG, config->crc_mode |
                       (config->en_irq ^ NRF24L01_IRQ_ALL));

    nrf24l01_write_reg(RF_SETUP, config->datarate | config->power);
    nrf24l01_write_reg(SETUP_RETR, config->retr_delay | config->retr_count);

    nrf24l01_write_reg(SETUP_AW, config->addr_size);
    /*
     * Битовое представление ширины адреса определяется от 1 до 3,
     * а фактический размер от 3 до 5 байт.
     */
    nrf24l01_write_buffer(W_REGISTER | TX_ADDR, config->address,
                          config->addr_size + 2);

    nrf24l01_write_reg(FEATURE, config->features);
    if (config->features & NRF24L01_FEATURE_ACK) {
        nrf24l01_write_reg(EN_RXADDR, 1 << NRF24L01_PIPE0);
        nrf24l01_write_reg(EN_AA, 1 << NRF24L01_PIPE0);
        nrf24l01_write_buffer(W_REGISTER | RX_ADDR_P0, config->address,
                              config->addr_size + 2);
    } else {
        nrf24l01_write_reg(EN_RXADDR, 0);
    }

    if (config->features & NRF24L01_FEATURE_DYNPL) {
        /*
         * Для DYNPD почему-то нужно устанавливать ещё и EN_AA
         * в соответствующем соединении.
         */
        nrf24l01_write_reg(EN_AA, 1 << NRF24L01_PIPE0);
        nrf24l01_write_reg(DYNPD, 1 << NRF24L01_PIPE0);
    } else {
        nrf24l01_write_reg(EN_AA, 0);
        nrf24l01_write_reg(DYNPD, 0);
    }

    nrf24l01_power_up();
    nrf24l01_set_rf_channel(config->rf_channel);
    nrf24l01_flush_tx_fifo();
    nrf24l01_flush_rx_fifo();
    nrf24l01_clear_interrupts(NRF24L01_IRQ_ALL);

    return nrf24l01_get_status() == INITIAL_STATUS_VALUE ?
            NRF24L01_OK : NRF24L01_ERR_BUS;
}

int nrf24l01_tx_configure_minimal(void)
{
    nrf24l01_power_up();

    return nrf24l01_get_status() == INITIAL_STATUS_VALUE ?
            NRF24L01_OK : NRF24L01_ERR_BUS;
}

void nrf24l01_tx_write_pld(const void *pld, uint8_t size)
{
    nrf24l01_write_buffer(W_TX_PAYLOAD, (uint8_t *)pld, size);
}

void nrf24l01_tx_write_noack_pld(const void *pld, uint8_t size)
{
    nrf24l01_write_buffer(W_TX_PAYLOAD_NOACK, (uint8_t *)pld, size);
}

void nrf24l01_tx_reuse_pld(void)
{
    nrf24l01_cmd(REUSE_TX_PL);
}

void nrf24l01_tx_transmit(void)
{
    nrf24l01_ce_1();
    delay_ms(1);
    nrf24l01_ce_0();
}

void nrf24l01_tx_start_cont_transmission(void)
{
    nrf24l01_ce_1();
    delay_ms(1);
}

void nrf24l01_tx_stop_cont_transmission(void)
{
    nrf24l01_ce_0();
}

void nrf24l01_tx_get_statistics(uint8_t *lost, uint8_t *retr)
{
    uint8_t observe_tx = nrf24l01_read_reg(OBSERVE_TX);
    *lost = observe_tx >> 4;
    *retr = observe_tx & 0x0F;
}

int nrf24l01_rx_configure(struct nrf24l01_rx_config *config)
{
    config->en_irq &= NRF24L01_IRQ_ALL;
    config->features &= NRF24L01_FEATURES_MASK;

    nrf24l01_write_reg(CONFIG, config->crc_mode | PRIM_RX |
                       (config->en_irq ^ NRF24L01_IRQ_ALL));
    nrf24l01_write_reg(RF_SETUP, config->datarate);
    nrf24l01_write_reg(SETUP_AW, config->addr_size);
    nrf24l01_write_reg(FEATURE, config->features);

    nrf24l01_power_up();
    nrf24l01_set_rf_channel(config->rf_channel);
    nrf24l01_flush_tx_fifo();
    nrf24l01_flush_rx_fifo();
    nrf24l01_clear_interrupts(NRF24L01_IRQ_ALL);

    return nrf24l01_get_status() == INITIAL_STATUS_VALUE ?
            NRF24L01_OK : NRF24L01_ERR_BUS;
}

int nrf24l01_rx_configure_minimal(uint8_t pld_size)
{
    if (pld_size > 32)
        pld_size = 32;
    nrf24l01_power_up();
    nrf24l01_write_reg(RX_PW_P0, pld_size);

    return nrf24l01_get_status() == INITIAL_STATUS_VALUE ?
            NRF24L01_OK : NRF24L01_ERR_BUS;
}

void nrf24l01_rx_setup_pipe(struct nrf24l01_pipe_config *config)
{
    nrf24l01_write_bits(EN_RXADDR, 1 << config->number, 1);
    if (config->number == NRF24L01_PIPE0 || config->number == NRF24L01_PIPE1) {
        enum nrf24l01_addr_size addr_size = nrf24l01_read_reg(SETUP_AW);
        nrf24l01_write_buffer(W_REGISTER | (RX_ADDR_P0 + config->number),
                              config->address.array, addr_size + 2);
    } else {
        nrf24l01_write_buffer(W_REGISTER | (RX_ADDR_P0 + config->number),
                              config->address.lsb, 1);
    }

    if (config->features & NRF24L01_PIPE_FEATURE_ACK) {
        nrf24l01_write_bits(EN_AA, 1 << config->number, 1);
    } else {
        nrf24l01_write_bits(EN_AA, 1 << config->number, 0);
    }

    if (config->features & NRF24L01_PIPE_FEATURE_DYNPL) {
        nrf24l01_write_bits(DYNPD, 1 << config->number, 1);
    } else {
        nrf24l01_write_bits(DYNPD, 1 << config->number, 0);
    }
}

void nrf24l01_rx_start_listening(void)
{
    nrf24l01_ce_1();
}

void nrf24l01_rx_stop_listening(void)
{
    nrf24l01_ce_0();
}

int nrf24l01_rx_get_pld_pipe_no(void)
{
    return (nrf24l01_get_status() >> 1) & 0x07;
}

void nrf24l01_rx_write_ack_pld(enum nrf24l01_pipe_number pipeno,
                               const void *pld, uint8_t size)
{
    nrf24l01_write_buffer(W_ACK_PAYLOAD | pipeno, (uint8_t *)pld, size);
}

void nrf24l01_power_down(void)
{
    nrf24l01_write_bits(CONFIG, PWR_UP, 0);
}

void nrf24l01_power_up(void)
{
    nrf24l01_write_bits(CONFIG, PWR_UP, 1);
}

uint8_t nrf24l01_get_interrupts(void)
{
    return nrf24l01_get_status() & NRF24L01_IRQ_ALL;
}

void nrf24l01_clear_interrupts(uint8_t irq)
{
    irq &= NRF24L01_IRQ_ALL;
    nrf24l01_write_bits(STATUS, irq, 1);
}

bool nrf24l01_data_in_rx_fifo(void)
{
    return (nrf24l01_read_reg(FIFO_STATUS) & RX_EMPTY) == 0;
}

bool nrf24l01_full_tx_fifo(void)
{
    return (nrf24l01_get_status() & TX_FULL_STATUS) != 0;
}

int nrf24l01_read_pld_size(void)
{
    uint8_t pld_size;

    nrf24l01_read_buffer(R_RX_PL_WID, &pld_size, 1);
    return pld_size;
}

void nrf24l01_read_pld(void *pld, uint8_t size)
{
    nrf24l01_read_buffer(R_RX_PAYLOAD, (uint8_t *)pld, size);
}

void nrf24l01_flush_tx_fifo(void)
{
    nrf24l01_cmd(FLUSH_TX);
}

void nrf24l01_flush_rx_fifo(void)
{
    nrf24l01_cmd(FLUSH_RX);
}

void nrf24l01_set_rf_channel(uint8_t channel)
{
    nrf24l01_write_reg(RF_CH, channel);
}

bool nrf24l01_detect_signal(void)
{
    uint8_t backup = nrf24l01_read_reg(CONFIG);

    nrf24l01_write_bits(CONFIG, PRIM_RX, 1);
    nrf24l01_ce_1();
    delay_ms(1);
    nrf24l01_ce_0();
    return nrf24l01_read_reg(RPD_CD) & 0x01;
}

void nrf24l01_measure_noise(uint8_t *snapshot_buff,
                            uint8_t start_ch, uint8_t end_ch)
{
    uint_fast8_t i, j;

    memset(snapshot_buff, 0, end_ch = start_ch + 1);
    for (i = 0; i < 0xF; i++) {
        for (j = start_ch; j <= end_ch; j++) {
            nrf24l01_set_rf_channel(j);
            if (nrf24l01_detect_signal())
                snapshot_buff[j - start_ch]++;
        }
    }
}

void nrf24l01_start_test_carrier(enum nrf24l01_power power, uint8_t channel)
{
    uint_fast8_t i;

    nrf24l01_write_reg(CONFIG, PWR_UP);
    delay_ms(2);
#ifdef NRF24L01_PLUS
    nrf_write_reg(RF_SETUP, CONT_WAVE | PLL_LOCK | power);
    nrf24l01_set_rf_channel(channel);
    nrf24l01_ce_1();
#else
    nrf24l01_write_reg(EN_AA, 0);
    nrf24l01_write_reg(SETUP_RETR, 0);
    nrf24l01_write_reg(RF_SETUP, PLL_LOCK | power);
    nrf24l01_write_reg(SETUP_AW, NRF24L01_ADDRS_5BYTE);

    nrf24l01_spi_transfer_byte(W_REGISTER | TX_ADDR);
    for (i = 0; i < 5; i++)
        nrf24l01_spi_transfer_byte(0xFF);

    nrf24l01_spi_transfer_byte(W_TX_PAYLOAD);
    for (i = 0; i < 32; i++)
        nrf24l01_spi_transfer_byte(0xFF);

    nrf24l01_write_bits(CONFIG, EN_CRC, 0);
    nrf24l01_ce_1();
    delay_ms(1);
    nrf24l01_ce_0();
    delay_ms(1);
    nrf24l01_ce_1();
    nrf24l01_cmd(REUSE_TX_PL);
#endif
}

void nrf24l01_stop_test_carrier(void)
{
    nrf24l01_ce_0();
}
