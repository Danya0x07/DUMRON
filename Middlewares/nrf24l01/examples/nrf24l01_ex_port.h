/*
 * Пример содержимого nrf24l01_port.h для портирования на микроконтроллер AVR
 * ATmega328P.
 */

#ifndef _NRF24L01_EX_PORT_H
#define _NRF24L01_EX_PORT_H

#include <avr/io.h>
#include <util/delay.h>

#define _csn_high() (PORTB |= _BV(1))
#define _csn_low()  (PORTB &= ~_BV(1))
#define _ce_high()  (PORTD |= _BV(7))
#define _ce_low()   (PORTD &= ~_BV(7))

static inline uint8_t _spi_transfer_byte(uint8_t byte)
{
    SPDR = data;
    loop_until_bit_is_set(SPSR, SPIF);
    return SPDR;
}

static inline void _spi_transfer_bytes(uint8_t *in, const uint8_t *out,
                                       size_t len)
{
    if (in == NULL) {
        while (len--)
            spi_transfer_byte(*out++);
    } else if (out == NULL) {
        while (len--)
            *in++ = spi_transfer_byte(0);
    } else {
        while (len--)
            *in++ = spi_transfer_byte(*out++);
    }
}

#define _delay_ms(ms)   _delay_ms((ms))
#define _delay_us(us)   _delay_us((us))

#endif /* _NRF24L01_EX_PORT_H */
