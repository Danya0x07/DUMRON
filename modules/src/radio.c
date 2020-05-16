#include "radio.h"
#include "nrf24l01p/nrf24l01p.h"
#include "debug.h"

#define delay_ms(ms)    HAL_Delay((ms))  // временное решение

void Radio_Init(void)
{
    uint8_t address[4] = {0xC7, 0x68, 0xAC, 0x35};

    /*
     * Включаем проверку контрольной суммы
     * и переводимся в режим приёмника.
     */
    const uint8_t config = PWR_UP | PRIM_RX | EN_CRC | MASK_TX_DS | MASK_MAX_RT;

    /* Выбираем частотный канал. */
    const uint8_t rf_ch = 112;

    /* Выбираем скорость работы 1 Мбит/с. */
    const uint8_t rf_setup = 0;

    /* Выбираем длину адреса в 4 байта. */
    const uint8_t setup_aw = SETUP_AW_4BYTES_ADDRESS;

    /* Включаем канал 0. */
    const uint8_t en_rxaddr = ERX_P0;

    /* Включаем автоподтверждение на канале 0. */
    const uint8_t en_aa = ENAA_P0;

    /*
     * Включаем возможность слать данные вместе с пакетами подтверждения
     * и не указывать размер полезной нагрузки.
     */
    const uint8_t feature = EN_DPL | EN_ACK_PAY;

    /* Включаем динамическую длину полезной нагрузки на канале 0. */
    const uint8_t dynpd = DPL_P0;

    /* Подали питание. */
    nrf_csn_1();
    nrf_ce_0();
    delay_ms(100);

    /* Power down --> Standby_1 */
    nrf_overwrite_byte(CONFIG, PWR_UP);
    delay_ms(5);

    /* Записываем настройки в модуль. */
    nrf_apply_mask(CONFIG, config, SET);
    nrf_overwrite_byte(RF_CH, rf_ch);
    nrf_overwrite_byte(RF_SETUP, rf_setup);
    nrf_overwrite_byte(SETUP_AW, setup_aw);
    nrf_overwrite_byte(EN_RXADDR, en_rxaddr);
    nrf_overwrite_byte(EN_AA, en_aa);
    nrf_overwrite_byte(FEATURE, feature);
    nrf_overwrite_byte(DYNPD, dynpd);

    /* Записываем адрес канала. */
    nrf_rw_buff(W_REGISTER | RX_ADDR_P0, address, 4, NRF_OPERATION_WRITE);

    /* Проверяем, что модуль всё понял. */
    if (nrf_read_byte(CONFIG) != config) {
        debug_logs("nrf not responding\n");
    }

    /* Чистим буферы на всякий случай. */
    nrf_cmd(FLUSH_TX);
    nrf_cmd(FLUSH_RX);

    /*
     * Начинаем слушать эфир.
     * Standby_1 --> Rx mode.
     */
    nrf_ce_1();
    delay_ms(1);
}

void Radio_TakeIncoming(DataToRobot_s *incoming)
{
    uint8_t status = nrf_get_status();
    if (status & RX_DR) {
        uint8_t fifo_status, data_size;
        do {
            nrf_rw_buff(R_RX_PL_WID, &data_size, 1, NRF_OPERATION_READ);
            if (data_size != sizeof(DataToRobot_s)) {
                nrf_cmd(FLUSH_RX);
                nrf_clear_interrupts();
                break;
            }
            nrf_rw_buff(R_RX_PAYLOAD, (uint8_t *)incoming,
                        sizeof(DataToRobot_s), NRF_OPERATION_READ);
            nrf_clear_interrupts();
            fifo_status = nrf_read_byte(FIFO_STATUS);
        } while (!(fifo_status & RX_EMPTY));
    }
}

void Radio_PutOutcoming(DataFromRobot_s *outcoming)
{
    uint8_t status = nrf_get_status();
    if (status & TX_FULL_STATUS) {
        nrf_cmd(FLUSH_TX);
    }
    nrf_rw_buff(W_ACK_PAYLOAD | 0, (uint8_t *)outcoming,
                sizeof(DataFromRobot_s), NRF_OPERATION_WRITE);
}
