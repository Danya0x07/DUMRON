#include "temperature.h"
#include "ds18b20/ds18b20.h"

static const uint8_t internalSensorAddress[8] = {
        0x8E, 0x01, 0x14, 0x48, 0xB1, 0xCE, 0xAA, 0x28};
static const uint8_t ambientSensorAddress[8] = {
        0xEF, 0x03, 0x14, 0x97, 0x79, 0xA7, 0x0A, 0x28};

static int8_t Temperature_Get(const uint8_t sensorAddress[8])
{
    DsOutputData data;
    ds_select_single(sensorAddress);
    ds_read_data(&data);
    return (int8_t)((data.temp_msb << 4) | (data.temp_lsb >> 4));
}

void Temperature_Init(void)
{
    DsConfig config = {
        .temp_lim_h = 80,
        .temp_lim_l = 10,
        .resolution = DS_RESOLUTION_9BIT
    };
    ds_select_all();
    ds_write_config(&config);
}

void Temperature_StartMeasurement(void)
{
    ds_select_all();
    ds_start_measuring();
}

int8_t Temperature_GetAmbient(void)
{
    return Temperature_Get(ambientSensorAddress);
}

int8_t Temperature_GetInternal(void)
{
    return Temperature_Get(internalSensorAddress);
}
