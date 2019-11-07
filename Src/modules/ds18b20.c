/**
 * Функции для нискоуровневого взаимодействия
 * с датчиками DS18B20 по шине 1-Wire.
 */

#include "modules/ds18b20.h"
#include "modules/debug.h"

#define ds_pin_1()      LL_GPIO_SetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define ds_pin_0()      LL_GPIO_ResetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define ds_pin_is_1()   LL_GPIO_IsInputPinSet(ONEWIRE_GPIO_Port, ONEWIRE_Pin)

static void ds_write_bit(_Bool bit);
static _Bool ds_read_bit(void);

/**
 * @brief  Начинает общение с одним из датчиков на линии.
 * @param  address: адрес устройства для начала сеанса связи.
 */
void ds_select_single(const uint8_t address[8])
{
    ds_reset_pulse();
    ds_write_byte(DS_MATCH_ROM);
    for (uint8_t i = 0; i < 8; i++)
        ds_write_byte(address[7 - i]);
}

/**
 * @brief  Начинает общение со всеми датчиками на линии.
 */
void ds_select_all(void)
{
    ds_reset_pulse();
    ds_write_byte(DS_SKIP_ROM);
}

/**
 * @brief  Считывает адрес единственного датчика на линии.
 * @param  address_buff: Буфер для сохранения адреса датчика.
 */
void ds_get_addr_of_single(uint8_t address_buff[8])
{
    ds_reset_pulse();
    ds_write_byte(DS_READ_ROM);
    for (uint8_t i = 0; i < 8; i++)
        address_buff[7 - i] = ds_read_byte();
}

/**
 * @brief  Записывает настройки в память датчика.
 * @note   Перед этой функцией необходимо вызвать
 *         одну из функций ds_select_*.
 * @param  ds_config: указатель на структуру
 *         с настройками для датчика.
 */
void ds_write_config(const DsConfig* ds_config)
{
    ds_write_byte(DS_W_SCRATCHPAD);
    ds_write_byte(ds_config->temp_lim_h);
    ds_write_byte(ds_config->temp_lim_l);
    ds_write_byte(ds_config->resolution);
}

/**
 * @brief  Считывает данные из датчика.
 * @note   Перед этой функцией необходимо вызвать
 *         одну из функций ds_select_*.
 * @param  p_odata: указатель на структуру
 *         для принимаемых данных.
 */
void ds_read_data(DsOutputData* p_odata)
{
    ds_write_byte(DS_R_SCRATCHPAD);
    p_odata->temp_lsb = ds_read_byte();
    p_odata->temp_msb = ds_read_byte();
    p_odata->th = ds_read_byte();
    ds_reset_pulse();
}

/**
 * @brief  Передаёт байт датчикам по шине 1-Wire.
 * @param  byte: Байт для передачи.
 */
void ds_write_byte(const uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
        ds_write_bit(byte & (1 << i));
}

/**
 * @brief  Читает байт от датчиков по шине 1-Wire.
 * @retval Прочитанный байт.
 */
uint8_t ds_read_byte(void)
{
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++)
        byte |= ds_read_bit() << i;
    return byte;
}

/**
 * @brief  Отпревляет импульс перезагрузки датчикам по 1-Wire.
 * @retval Ответил ли кто-нибудь из датчиков (SUCCESS)
 *         или нет(ERROR).
 */
ErrorStatus ds_reset_pulse(void)
{
    ds_pin_0();
    delay_us(490);
    ds_pin_1();
    delay_us(70);
    ErrorStatus ds_status = ds_pin_is_1() ? ERROR : SUCCESS;
    delay_us(250);
    return ds_status;
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
    delay_us(10);
    _Bool bit = ds_pin_is_1();
    delay_us(55);
    return bit;
}
