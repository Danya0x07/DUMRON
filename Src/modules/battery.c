#include "modules/battery.h"
#include "adc.h"

static uint8_t battery_get_percentage(ADC_ChannelConfTypeDef* pChannelConfig)
{
    uint8_t percentage = ADC1_Measure(pChannelConfig);
    return percentage * 100 / 4095;
}

uint8_t battery_get_voltage_brains(void)
{
    static ADC_ChannelConfTypeDef brainChannelConfig = {
        .Channel = BATTERY_BRAIN_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_1CYCLE_5
    };
    return battery_get_percentage(&brainChannelConfig);
}

uint8_t battery_get_voltage_radiators(void)
{
    static ADC_ChannelConfTypeDef radiatorsChannelConf = {
        .Channel = BATTERY_RADIATORS_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_1CYCLE_5
    };
    return battery_get_percentage(&radiatorsChannelConf);
}
