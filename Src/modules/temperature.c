#include "modules/temperature.h"
#include "modules/ds18b20.h"
#include "modules/debug.h"

static const uint8_t sensor_environment_address[8] = {
        0x8E, 0x01, 0x14, 0x48, 0xB1, 0xCE, 0xAA, 0x28};
static const uint8_t sensor_radiators_address[8] = {
        0x39, 0x02, 0x15, 0x92, 0x46, 0xFA, 0x77, 0x28};

static int8_t temperature_get(const uint8_t sensor_address[8]);

void temperature_init(void)
{
    DsConfig ds_config = {
        .temp_lim_h = 80,
        .temp_lim_l = 10,
        .resolution = DS_RESOLUTION_9BIT
    };
    ds_select_all();
    ds_write_config(&ds_config);
}

void temperature_start_conversion(void)
{
    ds_select_all();
    ds_start_measuring();
}

int8_t temperature_get_environment(void)
{
    return temperature_get(sensor_environment_address);
}

int8_t temperature_get_radiators(void)
{
    return temperature_get(sensor_radiators_address);
}

void temperature_print_addr(void)
{
    if (ds_reset_pulse() == ERROR) {
        debug_logs("ds not responding\n");
        return;
    }
    uint8_t address_buff[8];
    ds_get_addr_of_single(address_buff);
    for (uint8_t i = 0; i < 8; i++) {
        debug_logx(address_buff[i]);
        debug_logs(" ");
    }
    debug_logs("\n");
}

static int8_t temperature_get(const uint8_t sensor_address[8])
{
    DsOutputData data;
    ds_select_single(sensor_address);
    ds_read_data(&data);
    int8_t temperature = (data.temp_msb << 4) | (data.temp_lsb >> 4);
    return temperature;
}