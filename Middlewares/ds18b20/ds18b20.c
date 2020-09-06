/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

#include "ds18b20.h"
#include "ds18b20_port.h"

#define DS18B20_CRC_POLYNOM 0x18

enum ds18b20_commands {
    SEARCH_ROM   = 0xF0,
    ALARM_SEARCH = 0xEC,
    READ_ROM   = 0x33,
    MATCH_ROM  = 0x55,
    SKIP_ROM   = 0xCC,
    W_SCRATCHPAD  = 0x4E,
    R_SCRATCHPAD  = 0XBE,
    CP_SCRATCHPAD = 0x48,
    RECALL_E2  = 0xB8,
    CONVERT_T  = 0x44,
    R_PWR_SUPPLY  = 0xB4,
};

struct ds18b20_scratchpad {
    uint8_t temp_lsb;
    uint8_t temp_msb;
    uint8_t temp_lim_h;
    uint8_t temp_lim_l;
    uint8_t resolution;
    uint8_t _reserved1;
    uint8_t _reserved2;
    uint8_t _reserved3;
    uint8_t crc;
};

static void onewire_write_bit(uint_fast8_t bit)
{
    _wire_low();
    if (bit)
        _delay_us(3);
    else
        _delay_us(65);

    _wire_high();
    if (bit)
        _delay_us(65);
    else
        _delay_us(3);
}

static uint_fast8_t onewire_read_bit(void)
{
    _wire_low();
    _delay_us(2);
    _wire_high();
    _delay_us(10);
    uint_fast8_t bit = _wire_is_high();
    _delay_us(55);
    return bit;
}

static void onewire_write_byte(uint8_t byte)
{
    for (uint_fast8_t i = 0; i < 8; i++)
        onewire_write_bit(byte & (1 << i));
}

static uint8_t onewire_read_byte(void)
{
    uint8_t byte = 0;
    for (uint_fast8_t i = 0; i < 8; i++)
        byte |= onewire_read_bit() << i;
    return byte;
}

static uint_fast8_t onewire_perform_reset(void)
{
    _wire_low();
    _delay_us(490);
    _wire_high();
    _delay_us(70);
    uint_fast8_t wire_status = _wire_is_high();
    _delay_us(250);
    return wire_status;
}

#ifdef USE_ALTWIRE_FOR_ADDRESS_READING

static void altwire_write_bit(uint_fast8_t bit)
{
    _alt_wire_low();
    if (bit)
        _delay_us(3);
    else
        _delay_us(65);

    _alt_wire_high();
    if (bit)
        _delay_us(65);
    else
        _delay_us(3);
}

static uint_fast8_t altwire_read_bit(void)
{
    _alt_wire_low();
    _delay_us(2);
    _alt_wire_high();
    _delay_us(10);
    uint_fast8_t bit = _alt_wire_is_high();
    _delay_us(55);
    return bit;
}

static uint_fast8_t altwire_perform_reset(void)
{
    _alt_wire_low();
    _delay_us(490);
    _alt_wire_high();
    _delay_us(70);
    uint_fast8_t status = _alt_wire_is_high();
    _delay_us(250);
    return status;
}
#endif  /* USE_ALTWIRE_FOR_ADDRESS_READING */

int ds18b20_check_presense(void)
{
    uint_fast8_t presense = onewire_perform_reset() == 0;
    return presense ? DS18B20_OK : DS18B20_EABSENT;
}

static uint8_t ds18b20_calc_crc8(const uint8_t *data, uint8_t len)
{
    uint_fast8_t i;
    uint8_t crc = 0;
    uint8_t inbyte;

    while (len--) {
        inbyte = *data++;
        for (i = 0; i < 8; i++) {
            uint8_t bit0 = (crc ^ inbyte) & 0x01;
            /*
             * Следующий код является слегка укороченным вариантом этого:
             *
             * if (bit0) {
             *     crc ^= DS18B20_CRC_POLYNOM;
             *     crc >>= 1;
             *     crc |= 0x80
             * }
             * else {
             *     crc >>= 1;
             * }
             *
             * т.к. мы заранее сдвигаем crc на 1 вправо, то и
             * DS18B20_CRC_POLYNOM тоже нужно сдвинуть на 1 вправо и тогда
             * операцию XOR с ним можно объединить с установкой старшего бита
             * в crc.
             */
            crc >>= 1;
            if (bit0)
                crc ^= 0x80 | DS18B20_CRC_POLYNOM >> 1;
            inbyte >>= 1;
        }
    }

    return crc;
}


int ds18b20_read_address(uint8_t address[8])
{
    int status;
#ifdef USE_ALTWIRE_FOR_ADDRESS_READING
    if ((status = -altwire_perform_reset()) == DS18B20_OK) {
        uint_fast8_t i, j;

        for (i = 0; i < 8; i++)
            altwire_write_bit(READ_ROM & (1 << i));

        for (i = 0; i < 8; i++) {
            address[i] = 0;
            for (j = 0; j < 8; j++)
                address[i] |= altwire_read_bit() << j;
        }

        if (ds18b20_calc_crc8(address, 7) != address[7])
            status = DS18B20_ECRC;
    }
#else
    if ((status = ds18b20_check_presense()) == DS18B20_OK) {
        uint_fast8_t i;

        onewire_write_byte(READ_ROM);
        for (i = 0; i < 8; i++)
            address[i] = onewire_read_byte();

        if (ds18b20_calc_crc8(address, 7) != address[7])
            status = DS18B20_ECRC;
    }
#endif
    return status;
}

static int ds18b20_select(const uint8_t *address)
{
    int status;

    if ((status = ds18b20_check_presense()) == DS18B20_OK) {
        if (address) {
            onewire_write_byte(MATCH_ROM);
            for (uint_fast8_t i = 0; i < 8; i++)
                onewire_write_byte(address[i]);
        } else {
            onewire_write_byte(SKIP_ROM);
        }
    }
    return status;
}

int ds18b20_configure(const uint8_t *address,
                      const struct ds18b20_config *config)
{
    int status;

    if ((status = ds18b20_select(address)) == DS18B20_OK) {
        onewire_write_byte(W_SCRATCHPAD);
        /*
         * Допустимые границы температуры удобнее проверять из вызывающего кода.
         */
        onewire_write_byte(127);
        onewire_write_byte(-128);
        onewire_write_byte(config->resolution);
    }
    return status;
}

int ds18b20_start_measurement(const uint8_t *address)
{
    int status;

    if ((status = ds18b20_select(address)) == DS18B20_OK) {
        onewire_write_byte(CONVERT_T);
    }
    return status;
}

int ds18b20_get_result(const uint8_t *address, int32_t *result)
{
    int status;
    struct ds18b20_scratchpad scratchpad;

    if ((status = ds18b20_select(address)) == DS18B20_OK) {
        int res_sign;  /* знак результата */
        int res_int;  /* целая часть результата */
        unsigned int res_fract;  /* дробная часть результата */
        uint8_t *data = (uint8_t *)&scratchpad;
        uint_fast8_t i;

        onewire_write_byte(R_SCRATCHPAD);
        for (i = 0; i < sizeof(scratchpad); i++)
            *data++ = onewire_read_byte();

        if (ds18b20_calc_crc8((uint8_t *)&scratchpad, sizeof(scratchpad) - 1)
                != scratchpad.crc) {
            return DS18B20_ECRC;
        }

        res_sign = (scratchpad.temp_msb & 0x80) ? -1 : 1;
        res_int = 0x7F & ((scratchpad.temp_msb << 4) |
                          (scratchpad.temp_lsb >> 4));
        res_fract = 0;

        if (scratchpad.temp_lsb & 0x08)
            res_fract += 5000;
        if (scratchpad.temp_lsb & 0x04)
            res_fract += 2500;
        if (scratchpad.temp_lsb & 0x02)
            res_fract += 1250;
        if (scratchpad.temp_lsb & 0x01)
            res_fract += 625;

        if (res_sign < 0)
            res_fract = 10000 - res_fract;

        *result = res_sign * ((int32_t)res_int * 10000 + res_fract);
    }

    return status;
}

int ds18b20_measure(const uint8_t *address, int32_t *result)
{
    int status;

    if ((status = ds18b20_start_measurement(address)) == DS18B20_OK) {
        uint16_t wait_time_ms = 0;

        do {
            _delay_us(3 * 1000);
            wait_time_ms += 3;
        } while (onewire_read_bit() == 0 &&
                 wait_time_ms < DS18B20_MEASURE_TIME_12BIT_MS);
        /*
         * Очень маловероятная ситуация, но если не использовать таймаут
         * можно повесить программу, и потом долго искать, где.
         */
        if (wait_time_ms >= DS18B20_MEASURE_TIME_12BIT_MS)
            status = DS18B20_EBUSY;
        else
            status = ds18b20_get_result(address, result);
    }
    return status;
}

void ds18b20_parse_result(int32_t result, int8_t *integer,
                          uint16_t *fractional)
{
    int32_t result_abs, integer_abs;
    int8_t tmp_integer = result / 10000;

    if (result < 0) {
        result_abs = -result;
        integer_abs = -tmp_integer;
    } else {
        result_abs = result;
        integer_abs = tmp_integer;
    }

    if (integer != NULL)
        *integer = tmp_integer;
    if (fractional != NULL)
        *fractional = result_abs - integer_abs * 10000;
}
