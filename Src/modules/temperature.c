#include "modules/temperature.h"
#include "modules/ds18b20.h"
#include "modules/debug.h"

void temperature_init(void)
{
    DsConfig ds_config = {
        .temp_lim_h = 100,
        .temp_lim_l = 10,
        .resolution = DS_RESOLUTION_9BIT
    };
    ds_select_all();
    ds_write_config(&ds_config);
}

int8_t temperature_get(uint64_t sensor_addr)
{
    DsOutputData data;
    ds_select_single(sensor_addr);
    ds_read_data(&data);
    int8_t temp = ((data.temp_msb & 0x0F) << 4) | (data.temp_lsb >> 4);
    return temp;
}

void temperature_print_addr(void)
{
    uint64_t addr = ds_get_addr_of_single();
    debug_logi(addr >> 16 * 3);
    debug_logi(addr >> 16 * 2);
    debug_logi(addr >> 16 * 1);
    debug_logi(addr);
}
