#include "main.h"
#include "adc.h"

#include "battery.h"

static volatile uint16_t conversionResults[2];  /* буфер для DMA */

static uint8_t getChargePercentage(uint16_t convResult)
{
    return (uint8_t)(convResult * 100 / 4095);
}

void Battery_StartMeasurement(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)conversionResults, 2);
}

uint8_t Battery_GetBrainsCharge(void)
{
    return getChargePercentage(conversionResults[0]);
}

uint8_t Battery_GetMotorsCharge(void)
{
    return getChargePercentage(conversionResults[1]);
}
