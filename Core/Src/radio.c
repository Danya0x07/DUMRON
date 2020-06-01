#include <main.h>

#include "radio.h"
#include "debug.h"

#include <nrf24l01/nrf24l01.h>

void Radio_Init(void)
{
    uint8_t rx_address[4] = {0xC7, 0x68, 0xAC, 0x35};

    struct nrf24l01_rx_config config = {
        .addr_size = NRF24L01_ADDRS_4BYTE,
        .crc_mode  = NRF24L01_CRC_1BYTE,
        .datarate  = NRF24L01_DATARATE_1MBPS,
        .mode = NRF24L01_RX_MODE_DPL_ACK_PAYLOAD,
        .en_irq = NRF24L01_IRQ_RX_DR,
        .rf_channel = 112,
    };

    struct nrf24l01_pipe_config p0 = {
        .address.array = rx_address,
        .number = NRF24L01_PIPE0,
        .mode = NRF24L01_PIPE_MODE_ACK_DPL,
    };

    HAL_Delay(NRF24L01_PWR_ON_DELAY_MS);

    nrf24l01_rx_configure(&config);
    nrf24l01_rx_setup_pipe(&p0);

    nrf24l01_rx_start_listening();
}

void Radio_TakeIncoming(DataToRobot_s *incoming)
{
    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_RX_DR) {
        do {
            if (nrf24l01_read_pld_size() != sizeof(DataToRobot_s)) {
                nrf24l01_flush_rx_fifo();
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
                break;
            }
            nrf24l01_read_pld(incoming, sizeof(DataToRobot_s));
            nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
        } while (nrf24l01_data_in_rx_fifo());
    }
}

void Radio_PutOutcoming(DataFromRobot_s *outcoming)
{
    if (nrf24l01_full_tx_fifo()) {
        nrf24l01_flush_tx_fifo();
    }
    nrf24l01_rx_write_ack_pld(NRF24L01_PIPE0, outcoming,
                              sizeof(DataFromRobot_s));
}
