#include "modules/temperature.h"
#include "modules/ds18b20.h"
#include "modules/debug.h"

static uint8_t address_environment[8] = {
        0x08, 0xE1, 0x14, 0x48, 0xB1, 0xCE, 0xAA, 0x28};
static uint8_t address_radiators[8] = {
        0x08, 0xE1, 0x14, 0x48, 0xB1, 0xCE, 0xAA, 0x28};

static int8_t temperature_get(uint8_t [8]);

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
    return temperature_get(address_environment);
}

int8_t temperature_get_radiatots(void)
{
    return temperature_get(address_radiators);
}

void temperature_print_addr(void)
{
    if (ds_reset_pulse() == ERROR) {
        debug_logs("ds not responding\n");
        return;
    }
    uint8_t address[8];
    ds_get_addr_of_single(address);
    for (uint8_t i = 0; i < 8; i++)
        debug_logx(address[i]);
    debug_logs("\n");
}

static int8_t temperature_get(uint8_t sensor_addr[8])
{
    DsOutputData data;
    //ds_select_single(sensor_addr);
    //ds_select_all();
    ds_read_data(&data);
    int8_t temp = ((data.temp_msb & 0x0F) << 4) | ((data.temp_lsb & 0xF0) >> 4);
    debug_logi(data.th);
    debug_logs("\n");
    return temp;
}
