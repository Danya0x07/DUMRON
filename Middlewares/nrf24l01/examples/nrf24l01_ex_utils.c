/*
 * Common functions needed by examples code.
 * Ей-богу, не знаю как это по русски выразить.
 */

#include <avr/io.h>
#include <stdlib.h>

#define NRF_PIN_CSN   _BV(1)
#define NRF_PIN_CE    _BV(7)
#define NRF_PIN_IRQ   _BV(4)

#define SPI_MOSI  _BV(3)
#define SPI_SCK   _BV(5)
#define SPI_SS    _BV(2)

static char strbuffer[20];

/* Настройка UART для ATmega328P. */
void uart_init(void)
{
    // baudrate: 9600
    UBRR0H = (uint8_t)(103 >> 8);
    UBRR0L = (uint8_t)103;
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

/* Отправляет строку в UART. */
void uart_logs(const char *str)
{
    while (*str) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = *str++;
    }
}

/* Отправляет десятичное представление числа в UART. */
void uart_logi(int n)
{
    uart_logs(itoa(n, strbuffer, 10));
}

/* Отправляет шестнадцатеричное представление числа в UART. */
void uart_logx(int n)
{
    uart_logs(itoa(n, strbuffer, 16));
}

/*
 * Настройка портов ввода-вывода данного конкретного микроконтроллера
 * для работы с трансивером NRF24L01/+.
 */
void setup_gpio(void)
{
    DDRD |= NRF_PIN_CE;
    DDRD &= ~NRF_PIN_IRQ;
    PORTD &= ~NRF_PIN_CE;
    PORTD &= ~NRF_PIN_IRQ;
}

/*
 * Настройка интерфейса SPI данного конкретного микроконтроллера
 * для работы с трансивером NRF24L01/+.
 */
void setup_spi(void)
{
    DDRB |= SPI_MOSI | SPI_SCK | SPI_SS | NRF_PIN_CSN;
    SPCR = _BV(SPE) | _BV(MSTR); // режим 0, мастер, частота 1/4 от частоты ЦП
}

uint8_t transceiver_irq_pin_is_high(void)
{
    return (PIND & NRF_PIN_IRQ) != 0;
}
