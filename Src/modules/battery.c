#include "modules/battery.h"
#include "adc.h"

static uint8_t battery_get_percentage(ADC_ChannelConfTypeDef* p_ch_conf)
{
    uint8_t percentage = ADC1_Measure(p_ch_conf);
    return percentage * 100 / 4095;
}

uint8_t battery_get_voltage_brains(void)
{
    static ADC_ChannelConfTypeDef brain_ch_config = {
        .Channel = BATTERY_BRAIN_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_1CYCLE_5
    };
    return battery_get_percentage(&brain_ch_config);
}

uint8_t battery_get_voltage_radiators(void)
{
    static ADC_ChannelConfTypeDef radiators_ch_conf = {
        .Channel = BATTERY_RADIATORS_Channel,
        .Rank = ADC_REGULAR_RANK_1,
        .SamplingTime = ADC_SAMPLETIME_1CYCLE_5
    };
    return battery_get_percentage(&radiators_ch_conf);
}
