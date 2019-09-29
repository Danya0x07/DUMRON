#include "modules/ds18b20.h"

#define ds_pin_1()      LL_GPIO_SetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define ds_pin_0()      LL_GPIO_ResetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define ds_pin_is_1()   LL_GPIO_IsInputPinSet(ONEWIRE_GPIO_Port, ONEWIRE_Pin)

static ErrorStatus ds_reset_pulse(void);
static void ds_write_bit(_Bool bit);
static _Bool ds_read_bit(void);

void ds_select_single(uint64_t addr)
{
    ds_reset_pulse();
    ds_write_byte(DS_MATCH_ROM);
    for (uint8_t i = 0; i < 64; i++)
        ds_write_bit(addr & (1 << i));
}

void ds_select_all(void)
{
    ds_reset_pulse();
    ds_write_byte(DS_SKIP_ROM);
}

uint64_t ds_get_addr_of_single(void)
{
    ds_reset_pulse();
    ds_write_byte(DS_READ_ROM);
    uint64_t addr;
    for (uint8_t i = 0; i < 64; i++)
        addr |= ds_read_bit() << i;
    return addr;
}

void ds_write_config(DsConfig* ds_eeprom)
{
    ds_write_byte(DS_W_SCRATCHPAD);
    ds_write_byte(ds_eeprom->temp_lim_h);
    ds_write_byte(ds_eeprom->temp_lim_l);
    ds_write_byte(ds_eeprom->resolution);
}

void ds_read_data(DsOutputData* p_data)
{
    ds_write_byte(DS_R_SCRATCHPAD);
    p_data->temp_lsb = ds_read_byte();
    p_data->temp_msb = ds_read_byte();
    ds_reset_pulse();
}

void ds_write_byte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++) {
        ds_write_bit(byte & (1 << i));
        delay_us(5);
    }
}

uint8_t ds_read_byte(void)
{
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++)
        byte |= ds_read_bit() << i;
    return byte;
}

static void ds_write_bit(_Bool bit)
{
    ds_pin_0();
    delay_us(bit ? 3 : 65);
    ds_pin_1();
    delay_us(bit ? 65 : 3);
}

static _Bool ds_read_bit(void)
{
    ds_pin_0();
    delay_us(2);
    ds_pin_1();
    delay_us(15);
    _Bool bit = ds_pin_is_1();
    delay_us(50);
    return bit;
}

static ErrorStatus ds_reset_pulse(void)
{
    if (ds_pin_is_1())
        return ERROR;
    ds_pin_0();
    delay_us(485);
    ds_pin_1();
    delay_us(70);
    ErrorStatus ds_status = ds_pin_is_1() ? SUCCESS : ERROR;
    delay_us(500);
    return ds_status;
}
