#include "libport.h"

void delay_us(unsigned int us)
{
    LL_TIM_SetCounter(TIM3, 0);
    while (LL_TIM_GetCounter(TIM3) < us)
        ;
}

uint8_t spi_transfer(uint8_t byte)
{
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
        ;
    LL_SPI_TransmitData8(SPI1, byte);
    while (!LL_SPI_IsActiveFlag_RXNE(SPI1))
        ;
    return LL_SPI_ReceiveData8(SPI1);
}
