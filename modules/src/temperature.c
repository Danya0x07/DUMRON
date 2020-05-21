#include "temperature.h"
#include "ds18b20/ds18b20.h"

static const uint8_t internalSensorAddress[8] =
        {0x28, 0xAA, 0xCE, 0xB1, 0x48, 0x14, 0x01, 0x8E};
static const uint8_t ambientSensorAddress[8] =
        {0x28, 0x0A, 0xA7, 0x79, 0x97, 0x14, 0x03, 0xEF};

static int8_t Temperature_Get(const uint8_t *sensorAddress)
{
    int32_t result;
    int8_t res_integer;
    ds18b20_get_result(sensorAddress, &result);
    ds18b20_parse_result(result, &res_integer, NULL);
    return res_integer;
}

void Temperature_Init(void)
{
    struct ds18b20_config config = {
        .temp_lim_h = 80,
        .temp_lim_l = 10,
        .resolution = DS18B20_RESOLUTION_9BIT
    };

    ds18b20_configure(NULL, &config);
}

void Temperature_StartMeasurement(void)
{
    ds18b20_start_measurement(NULL);
}

int8_t Temperature_GetAmbient(void)
{
    return Temperature_Get(ambientSensorAddress);
}

int8_t Temperature_GetInternal(void)
{
    return Temperature_Get(internalSensorAddress);
}
