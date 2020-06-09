#include "main.h"
#include "adc.h"

#include "battery.h"

static uint8_t getChargePercentage(ADC_ChannelConfTypeDef *config)
{
    return (uint8_t)(ADC1_Measure(config) * 100 / 4095);
}

uint8_t Battery_GetChargeBrains(void)
{
    static ADC_ChannelConfTypeDef config = {
        .Channel = BATTERY_BRAIN_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_7CYCLES_5
    };
    return getChargePercentage(&config);
}

uint8_t Battery_GetChargeMotors(void)
{
    static ADC_ChannelConfTypeDef config = {
        .Channel = BATTERY_RADIATORS_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_7CYCLES_5
    };
    return getChargePercentage(&config);
}
