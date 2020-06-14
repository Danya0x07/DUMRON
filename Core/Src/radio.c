#include "main.h"

#include "radio.h"
#include "errors.h"
#include "debug.h"

#include <nrf24l01/nrf24l01.h>

/*static char expected_msg[] = "Hello NRF24L01/+!";
static char msg_buffer[sizeof(expected_msg)];*/

#define uart_logs(s)    debug_logs(s)
#define uart_logi(n)    debug_logi(n)
#define uart_logx(n)    debug_logx(n)
#define transceiver_irq_pin_is_high()    LL_GPIO_IsInputPinSet(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin)

struct tx_payload {
    uint8_t data;
    uint8_t other_data;
} in_buffer;

struct ack_payload {
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

void Radio_Init(void)
{

    if (nrf24l01_tx_configure_default() < 0) {
        /*
         * Обрабатываем ошибку инициализации.
         * Возможно она вызвана плохим контактом модуля с макетной платой,
         * если таковая используется.
         */
         uart_logs("nrf24l01 init error\n");
    }

    /*
     * Печатаем номера каналов, номер каждого канала печатается в виде
     * шестнадцатеричного числа, расположенного "вертикально": сначала старший
     * байт, потом младший.
     */
    uart_logs("\n\n\tnoise snapshot, channels:\n");
    for (uint8_t i = 0; i < NRF24L01_CHANNELS; i++) {
        uart_logx(i >> 4 & 0x0F);
    }
    uart_logs("\n");
    for (uint8_t i = 0; i < NRF24L01_CHANNELS; i++) {
        uart_logx(i & 0x0F);
    }
    uart_logs("\n\n");

    uint8_t noise_buff[NRF24L01_CHANNELS];

    for (;;) {
        nrf24l01_measure_noise(noise_buff, 0, sizeof(noise_buff) - 1);
        for (int i = 0; i < NRF24L01_CHANNELS; i++) {
            /*
             * Зашумлённости каналов представлены числами от 0 до F, поэтому
             * помещаются в 1 разряд на канал.
             */
            uart_logx(noise_buff[i]);
        }
        uart_logs("\n");
    }
}

void Radio_TakeIncoming(DataToRobot_s *incoming)
{
    if (nrf24l01_get_interrupts() & NRF24L01_IRQ_RX_DR) {
        do {
            if (nrf24l01_read_pld_size() != sizeof(DataToRobot_s)) {
                nrf24l01_flush_rx_fifo();
                nrf24l01_clear_interrupts(NRF24L01_IRQ_RX_DR);
                debug_logs("incorrect pld size\n");
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
        debug_logs("full tx fifo\n");
    }
    nrf24l01_rx_write_ack_pld(NRF24L01_PIPE0, outcoming,
                              sizeof(DataFromRobot_s));
}
