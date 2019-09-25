#include "modules/battery.h"
#include "adc.h"

static uint8_t battery_get_percentage(ADC_ChannelConfTypeDef* channel_conf)
{
    HAL_ADC_ConfigChannel(&hadc1, channel_conf);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    uint16_t result = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return result * 100 / 4095;
}

uint8_t battery_get_voltage_brains(void)
{
    static ADC_ChannelConfTypeDef brain_channel_conf = {
        .Channel = BATTERY_BRAIN_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_1CYCLE_5
    };
    return battery_get_percentage(&brain_channel_conf);
}

uint8_t battery_get_voltage_radiators(void)
{
    static ADC_ChannelConfTypeDef radiators_channel_conf = {
        .Channel = BATTERY_RADIATORS_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_1CYCLE_5
    };
    return battery_get_percentage(&radiators_channel_conf);
}
