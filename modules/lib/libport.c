#include "libport.h"
#include "main.h"

void delay_ms(unsigned int ms)
{
    HAL_Delay(ms);
}

void delay_us(uint us)
{
    LL_TIM_SetCounter(TIM3, 0);
    while (LL_TIM_GetCounter(TIM3) < us)
        ;
}

uint8_t spi_transfer_byte(uint8_t byte)
{
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
        ;
    LL_SPI_TransmitData8(SPI1, byte);
    while (!LL_SPI_IsActiveFlag_RXNE(SPI1))
        ;
    return LL_SPI_ReceiveData8(SPI1);
}

void spi_transfer_bytes(uint8_t *in, const uint8_t *out, size_t len)
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
