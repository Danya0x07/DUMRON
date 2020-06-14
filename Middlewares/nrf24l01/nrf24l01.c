/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

#include "nrf24l01.h"
#include "nrf24l01_port.h"

#if (NRF24L01_DELAY_US_AVAILABLE == 0)
#   define _delay_tx()      _delay_ms(1)
#   define _delay_rpd()     _delay_ms(1)
#else
#   define _delay_tx()      _delay_us(11)
#   define _delay_rpd()     _delay_us(200)
#endif

enum nrf24l01_cmd {
    CMD_R_REGISTER = 0x00,
    CMD_W_REGISTER = 0x20,
    CMD_R_RX_PAYLOAD = 0x61,
    CMD_W_TX_PAYLOAD = 0xA0,
    CMD_FLUSH_TX = 0xE1,
    CMD_FLUSH_RX = 0xE2,
    CMD_REUSE_TX_PL = 0xE3,
    CMD_R_RX_PL_WID = 0x60,
    CMD_W_ACK_PAYLOAD = 0xA8,
    CMD_W_TX_PAYLOAD_NOACK = 0xB0,
    CMD_NOP = 0xFF
};

enum nrf24l01_reg {
    REG_CONFIG = 0x00,
    REG_EN_AA = 0x01,
    REG_EN_RXADDR = 0x02,
    REG_SETUP_AW = 0x03,
    REG_SETUP_RETR = 0x04,
    REG_RF_CH = 0x05,
    REG_RF_SETUP = 0x06,
    REG_STATUS = 0x07,
    REG_OBSERVE_TX = 0x08,
    REG_RPD_CD = 0x09,
    REG_RX_ADDR_P0 = 0x0A,
    REG_RX_ADDR_P1 = 0x0B,
    REG_RX_ADDR_P2 = 0x0C,
    REG_RX_ADDR_P3 = 0x0D,
    REG_RX_ADDR_P4 = 0x0E,
    REG_RX_ADDR_P5 = 0x0F,
    REG_TX_ADDR = 0x10,
    REG_RX_PW_P0 = 0x11,
    REG_RX_PW_P1 = 0x12,
    REG_RX_PW_P2 = 0x13,
    REG_RX_PW_P3 = 0x14,
    REG_RX_PW_P4 = 0x15,
    REG_RX_PW_P5 = 0x16,
    REG_FIFO_STATUS = 0x17,
    REG_DYNPD = 0x1C,
    REG_FEATURE = 0x1D
};

#define CONFIG_EN_CRC   (1 << 3)
#define CONFIG_PWR_UP   (1 << 1)
#define CONFIG_PRIM_RX  (1 << 0)

#define STATUS_RESET_VALUE    0x0E  /* для выявления аппаратных проблем */

#define FIFO_STATUS_TX_REUSE    (1 << 6)
#define FIFO_STATUS_TX_FULL     (1 << 5)
#define FIFO_STATUS_TX_EMPTY    (1 << 4)
#define FIFO_STATUS_RX_FULL     (1 << 1)
#define FIFO_STATUS_RX_EMPTY    (1 << 0)

#define RF_SETUP_PLL_LOCK   (1 << 4)
#if (NRF24L01_PLUS == 1)
#   define RF_SETUP_CONT_WAVE   (1 << 7)
#else
#   define RF_SETUP_LNA_HCURR   (1 << 0)
#endif

#define FEATURE_EN_DPL  (1 << 2)
#define FEATURE_EN_ACK_PAY  (1 << 1)
#define FEATURE_EN_DYN_ACK  (1 << 0)

static uint8_t nrf24l01_exec(enum nrf24l01_cmd cmd)
{
    uint8_t status;
    _csn_low();
    status = _spi_transfer_byte(cmd);
    _csn_high();
    return status;
}

static inline uint8_t nrf24l01_get_status(void)
{
    return nrf24l01_exec(CMD_NOP);
}

static uint8_t nrf24l01_read_reg(enum nrf24l01_reg reg)
{
    uint8_t value;
    _csn_low();
    _spi_transfer_byte(CMD_R_REGISTER | reg);
    value = _spi_transfer_byte(CMD_NOP);
    _csn_high();
    return value;
}

static void nrf24l01_write_reg(enum nrf24l01_reg reg, uint8_t value)
{
    _csn_low();
    _spi_transfer_byte(CMD_W_REGISTER | reg);
    _spi_transfer_byte(value);
    _csn_high();
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
    _csn_low();
    _spi_transfer_byte(cmd);
    _spi_transfer_bytes(buff, NULL, size);
    _csn_high();
}

static void nrf24l01_write_buffer(uint8_t cmd, const uint8_t *buff,
                                  uint8_t size)
{
    _csn_low();
    _spi_transfer_byte(cmd);
    _spi_transfer_bytes(NULL, buff, size);
    _csn_high();
}

int nrf24l01_tx_configure(struct nrf24l01_tx_config *config)
{
    uint8_t features = 0;

    config->en_irq &= NRF24L01_IRQ_ALL;

    /*
     * Запись 1 на место бита прерывания в регистре CONFIG выключает данное
     * прерывание, поэтому пользовательскую маску нужно инвертировать.
     */
    nrf24l01_write_reg(REG_CONFIG, config->crc_mode |
                       (config->en_irq ^ NRF24L01_IRQ_ALL));

    nrf24l01_write_reg(REG_RF_SETUP, config->datarate | config->power);
    nrf24l01_write_reg(REG_SETUP_RETR, config->retr_delay | config->retr_count);

    nrf24l01_write_reg(REG_SETUP_AW, config->addr_size);
    /*
     * Битовое представление ширины адреса определяется от 1 до 3,
     * а фактический размер от 3 до 5 байт.
     */
    nrf24l01_write_buffer(CMD_W_REGISTER | REG_TX_ADDR, config->address,
                          config->addr_size + 2);

    if (config->mode >= NRF24L01_TX_MODE_ACK) {
        /*
         * Включаем и настраиваем входящее соединение 0 передатчика
         * для приёма автоподтверждений.
         */
        nrf24l01_write_reg(REG_EN_RXADDR, 1 << NRF24L01_PIPE0);
        nrf24l01_write_reg(REG_EN_AA, 1 << NRF24L01_PIPE0);
        nrf24l01_write_buffer(CMD_W_REGISTER | REG_RX_ADDR_P0, config->address,
                              config->addr_size + 2);

        if (config->mode == NRF24L01_TX_MODE_ACK_PAYLOAD) {
            features |=
                    (FEATURE_EN_DPL | FEATURE_EN_ACK_PAY | FEATURE_EN_DYN_ACK);
        }
    } else {
        nrf24l01_write_reg(REG_EN_AA, 0);
        nrf24l01_write_reg(REG_EN_RXADDR, 0);
    }
    nrf24l01_write_reg(REG_FEATURE, features);
    nrf24l01_write_reg(REG_DYNPD, !!features << NRF24L01_PIPE0);

    nrf24l01_power_up();
    nrf24l01_set_rf_channel(config->rf_channel);
    nrf24l01_flush_tx_fifo();
    nrf24l01_flush_rx_fifo();
    nrf24l01_clear_interrupts(NRF24L01_IRQ_ALL);

    return nrf24l01_get_status() == STATUS_RESET_VALUE ? 0 : -1;
}

int nrf24l01_tx_configure_default(void)
{
    nrf24l01_power_up();

    return nrf24l01_get_status() == STATUS_RESET_VALUE ? 0 : -1;
}

void nrf24l01_tx_write_pld(const void *pld, uint8_t size)
{
    if (size > 32)
        size = 32;
    nrf24l01_write_buffer(CMD_W_TX_PAYLOAD, (uint8_t *)pld, size);
}

void nrf24l01_tx_write_noack_pld(const void *pld, uint8_t size)
{
    if (size > 32)
        size = 32;
    nrf24l01_write_buffer(CMD_W_TX_PAYLOAD_NOACK, (uint8_t *)pld, size);
}

void nrf24l01_tx_reuse_pld(void)
{
    nrf24l01_exec(CMD_REUSE_TX_PL);
}

bool nrf24l01_tx_reusing_pld(void)
{
    return (nrf24l01_read_reg(REG_FIFO_STATUS) & FIFO_STATUS_TX_REUSE) != 0;
}

void nrf24l01_tx_transmit(void)
{
    _ce_high();
    _delay_tx();
    _ce_low();
}

void nrf24l01_tx_start_cont_transmission(void)
{
    _ce_high();
}

void nrf24l01_tx_stop_cont_transmission(void)
{
    _ce_low();
}

void nrf24l01_tx_get_statistics(uint8_t *lost, uint8_t *retr)
{
    uint8_t observe_tx = nrf24l01_read_reg(REG_OBSERVE_TX);
    *lost = observe_tx >> 4;
    *retr = observe_tx & 0x0F;
}

int nrf24l01_rx_configure(struct nrf24l01_rx_config *config)
{
    uint8_t features = 0;

    config->en_irq &= NRF24L01_IRQ_ALL;

    nrf24l01_write_reg(REG_CONFIG, config->crc_mode | CONFIG_PRIM_RX |
                       (config->en_irq ^ NRF24L01_IRQ_ALL));

    nrf24l01_write_reg(REG_RF_SETUP, config->datarate);
    nrf24l01_write_reg(REG_SETUP_AW, config->addr_size);

    if (config->mode >= NRF24L01_RX_MODE_DPL) {
        features |= FEATURE_EN_DPL;
        if (config->mode == NRF24L01_RX_MODE_DPL_ACK_PAYLOAD)
            features |= FEATURE_EN_ACK_PAY | FEATURE_EN_DYN_ACK;
    }
    nrf24l01_write_reg(REG_FEATURE, features);

    nrf24l01_power_up();
    nrf24l01_set_rf_channel(config->rf_channel);
    nrf24l01_flush_tx_fifo();
    nrf24l01_flush_rx_fifo();
    nrf24l01_clear_interrupts(NRF24L01_IRQ_ALL);

    return nrf24l01_get_status() == STATUS_RESET_VALUE ? 0 : -1;
}

int nrf24l01_rx_configure_default(uint8_t pld_size)
{
    if (pld_size > 32)
        pld_size = 32;

    nrf24l01_power_up();
    nrf24l01_write_bits(REG_CONFIG, CONFIG_PRIM_RX, 1);
    nrf24l01_write_reg(REG_RX_PW_P0, pld_size);

    return nrf24l01_get_status() == STATUS_RESET_VALUE ? 0 : -1;
}

void nrf24l01_rx_setup_pipe(enum nrf24l01_pipe_number pipe_no,
                            struct nrf24l01_pipe_config *config)
{
    if (config->pld_size > 32)
        config->pld_size = 32;

    nrf24l01_write_bits(REG_EN_RXADDR, 1 << pipe_no, 1);
    nrf24l01_write_reg(REG_RX_PW_P0 + pipe_no, config->pld_size);

    if (pipe_no == NRF24L01_PIPE0 || pipe_no == NRF24L01_PIPE1) {
        uint8_t addr_size = nrf24l01_read_reg(REG_SETUP_AW) + 2;
        nrf24l01_write_buffer(CMD_W_REGISTER | (REG_RX_ADDR_P0 + pipe_no),
                              config->address.array, addr_size);
    } else {
        nrf24l01_write_buffer(CMD_W_REGISTER | (REG_RX_ADDR_P0 + pipe_no),
                              &config->address.lsb, 1);
    }
    nrf24l01_write_bits(REG_EN_AA, 1 << pipe_no,
                        config->mode >= NRF24L01_PIPE_MODE_ACK);
    nrf24l01_write_bits(REG_DYNPD, 1 << pipe_no,
                        config->mode == NRF24L01_PIPE_MODE_ACK_DPL);
}

void nrf24l01_rx_close_pipe(enum nrf24l01_pipe_number pipe_no)
{
    nrf24l01_write_bits(REG_EN_RXADDR, 1 << pipe_no, 0);
}

void nrf24l01_rx_open_pipe(enum nrf24l01_pipe_number pipe_no)
{
    nrf24l01_write_bits(REG_EN_RXADDR, 1 << pipe_no, 1);
}

void nrf24l01_rx_start_listening(void)
{
    _ce_high();
}

void nrf24l01_rx_stop_listening(void)
{
    _ce_low();
}

int nrf24l01_rx_get_pld_pipe_no(void)
{
    uint8_t pipe_no = (nrf24l01_get_status() >> 1) & 0x07;
    return pipe_no < 6 ? pipe_no : -1;
}

void nrf24l01_rx_write_ack_pld(enum nrf24l01_pipe_number pipe_no,
                               const void *pld, uint8_t size)
{
    if (size > 32)
        size = 32;
    nrf24l01_write_buffer(CMD_W_ACK_PAYLOAD | pipe_no, (uint8_t *)pld, size);
}

void nrf24l01_power_down(void)
{
    nrf24l01_write_bits(REG_CONFIG, CONFIG_PWR_UP, 0);
}

void nrf24l01_power_up(void)
{
    nrf24l01_write_bits(REG_CONFIG, CONFIG_PWR_UP, 1);
    _delay_ms(2);
}

uint8_t nrf24l01_get_interrupts(void)
{
    return nrf24l01_get_status() & NRF24L01_IRQ_ALL;
}

void nrf24l01_clear_interrupts(uint8_t irq)
{
    irq &= NRF24L01_IRQ_ALL;
    nrf24l01_write_bits(REG_STATUS, irq, 1);
}

bool nrf24l01_data_in_tx_fifo(void)
{
    return (nrf24l01_read_reg(REG_FIFO_STATUS) & FIFO_STATUS_TX_EMPTY) == 0;
}

bool nrf24l01_data_in_rx_fifo(void)
{
    return (nrf24l01_read_reg(REG_FIFO_STATUS) & FIFO_STATUS_RX_EMPTY) == 0;
}

bool nrf24l01_full_tx_fifo(void)
{
    return (nrf24l01_read_reg(REG_FIFO_STATUS) & FIFO_STATUS_TX_FULL) != 0;
}

bool nrf24l01_full_rx_fifo(void)
{
    return (nrf24l01_read_reg(REG_FIFO_STATUS) & FIFO_STATUS_RX_FULL) != 0;
}

int nrf24l01_read_pld_size(void)
{
    uint8_t pld_size;

    nrf24l01_read_buffer(CMD_R_RX_PL_WID, &pld_size, 1);
    return pld_size;
}

void nrf24l01_read_pld(void *pld, uint8_t size)
{
    nrf24l01_read_buffer(CMD_R_RX_PAYLOAD, (uint8_t *)pld, size);
}

void nrf24l01_flush_tx_fifo(void)
{
    nrf24l01_exec(CMD_FLUSH_TX);
}

void nrf24l01_flush_rx_fifo(void)
{
    nrf24l01_exec(CMD_FLUSH_RX);
}

void nrf24l01_set_rf_channel(uint8_t channel)
{
    if (channel > NRF24L01_CHANNELS - 1)
        channel = NRF24L01_CHANNELS - 1;
    nrf24l01_write_reg(REG_RF_CH, channel);
}

bool nrf24l01_detect_signal(void)
{
    const uint8_t config_backup = nrf24l01_read_reg(REG_CONFIG);

    /*
     * Чтобы эту функцию можно было вызывать в режиме передатчика,
     * сохраняем значение регистра настроек, а потом восстанавливаем его.
     */
    if ((config_backup & CONFIG_PRIM_RX) == 0)
        nrf24l01_write_reg(REG_CONFIG, config_backup | CONFIG_PRIM_RX);

    _ce_high();
    _delay_rpd();
    _ce_low();

    if ((config_backup & CONFIG_PRIM_RX) == 0)
        nrf24l01_write_reg(REG_CONFIG, config_backup);

    return nrf24l01_read_reg(REG_RPD_CD) & 0x01;
}

void nrf24l01_measure_noise(uint8_t *snapshot_buff,
                            uint8_t start_ch, uint8_t end_ch)
{
    uint_fast8_t i, j;
    const uint8_t config_backup = nrf24l01_read_reg(REG_CONFIG);

    if (end_ch > NRF24L01_CHANNELS - 1 || start_ch > end_ch)
        return;

    for (i = 0; i < end_ch - start_ch + 1; i++)
        snapshot_buff[i] = 0;

    /*
     * Чтобы не происходило слишком частой записи по SPI в трансивер,
     * сохраняем регистр настроек до цикла измерения.
     */
    if ((config_backup & CONFIG_PRIM_RX) == 0)
        nrf24l01_write_reg(REG_CONFIG, config_backup | CONFIG_PRIM_RX);

    for (i = 0; i < 0xF; i++) {
        for (j = start_ch; j <= end_ch; j++) {
            nrf24l01_set_rf_channel(j);
            if (nrf24l01_detect_signal())
                snapshot_buff[j - start_ch]++;
        }
    }

    if ((config_backup & CONFIG_PRIM_RX) == 0)
        nrf24l01_write_reg(REG_CONFIG, config_backup);
}

void nrf24l01_start_output_carrier(enum nrf24l01_power power, uint8_t channel)
{
#if (NRF24L01_PLUS == 1)
    nrf24l01_power_up();
    nrf24l01_write_bits(REG_CONFIG, CONFIG_PRIM_RX, 0);
    nrf24l01_write_reg(REG_RF_SETUP, RF_SETUP_CONT_WAVE | RF_SETUP_PLL_LOCK |
                       power);
    nrf24l01_set_rf_channel(channel);
    _ce_high();
#else
    uint_fast8_t i;

    nrf24l01_power_up();
    nrf24l01_write_bits(REG_CONFIG, CONFIG_PRIM_RX, 0);
    nrf24l01_write_reg(REG_EN_AA, 0);
    nrf24l01_write_reg(REG_SETUP_RETR, 0);
    nrf24l01_write_reg(REG_RF_SETUP, RF_SETUP_PLL_LOCK | power);
    nrf24l01_write_reg(REG_SETUP_AW, NRF24L01_ADDRS_5BYTE);

    _spi_transfer_byte(CMD_W_REGISTER | REG_TX_ADDR);
    for (i = 0; i < 5; i++)
        _spi_transfer_byte(0xFF);

    _spi_transfer_byte(CMD_W_TX_PAYLOAD);
    for (i = 0; i < 32; i++)
        _spi_transfer_byte(0xFF);

    nrf24l01_write_bits(REG_CONFIG, CONFIG_EN_CRC, 0);
    nrf24l01_set_rf_channel(channel);
    nrf24l01_tx_transmit();
    _delay_ms(1);
    _ce_high();
    nrf24l01_exec(CMD_REUSE_TX_PL);
#endif
}

#if (NRF24L01_PLUS == 0)

void nrf24l01_activate(void)
{
    _csn_low();
    _spi_transfer_byte(0x50);
    _spi_transfer_byte(0x73);
    _csn_high();
}

void nrf24l01_rx_set_lna(bool state)
{
    nrf24l01_write_bits(REG_RF_SETUP, RF_SETUP_LNA_HCURR, state);
}
#endif
