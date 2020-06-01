#include <main.h>
#include <adc.h>

#include "battery.h"

static uint8_t Battery_GetPercentage(ADC_ChannelConfTypeDef *config)
{
    return (uint8_t)(ADC1_Measure(config) * 100 / 4095);
}

uint8_t Battery_GetPercentageBrains(void)
{
    static ADC_ChannelConfTypeDef config = {
        .Channel = BATTERY_BRAIN_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_7CYCLES_5
    };
    return Battery_GetPercentage(&config);
}

uint8_t Battery_GetPercentageMotors(void)
{
    static ADC_ChannelConfTypeDef config = {
        .Channel = BATTERY_RADIATORS_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_7CYCLES_5
    };
    return Battery_GetPercentage(&config);
}
