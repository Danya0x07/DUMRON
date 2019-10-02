#include "modules/ds18b20.h"

#define ds_pin_1()      LL_GPIO_SetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define ds_pin_0()      LL_GPIO_ResetOutputPin(ONEWIRE_GPIO_Port, ONEWIRE_Pin)
#define ds_pin_is_1()   LL_GPIO_IsInputPinSet(ONEWIRE_GPIO_Port, ONEWIRE_Pin)

static void ds_write_bit(_Bool bit);
static _Bool ds_read_bit(void);

/**
 * @brief  Начинает общение с одним из датчиков на линии.
 * @param  addr: адрес устройства для начала сеанса.
 */
void ds_select_single(uint8_t address_buff[8])
{
    ds_reset_pulse();
    ds_write_byte(DS_MATCH_ROM);
    for (uint8_t i = 0; i < 8; i++)
        ds_write_byte(address_buff[i]);
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
 * @retval полный ROM-адрес датчика.
 */
void ds_get_addr_of_single(uint8_t addr_buff[8])
{
    ds_reset_pulse();
    ds_write_byte(DS_READ_ROM);
    for (uint8_t i = 0; i < 8; i++)
        addr_buff[7 - i] = ds_read_byte();
}

/**
 * @brief  Записывает настройки в память датчика.
 * @note   Перед этой функцией необходимо вызвать
 *         одну из функций ds_select_*.
 * @param  ds_config: указатель на структуру
 *         с настройками для датчика.
 */
void ds_write_config(DsConfig* ds_config)
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
 * @param  ds_config: указатель на структуру
 *         для принимаемых данных.
 */
void ds_read_data(DsOutputData* p_data)
{
    ds_write_byte(DS_R_SCRATCHPAD);
    p_data->temp_lsb = ds_read_byte();
    p_data->temp_msb = ds_read_byte();
    p_data->th = ds_read_byte();
    ds_reset_pulse();
}

/**
 * @brief  Передаёт байт датчикам по 1-wire.
 * @param  byte: Байт для передачи.
 */
void ds_write_byte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
        ds_write_bit(byte & (1 << i));
}

/**
 * @brief  Читает байт от датчиков по 1-wire.
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
 * @brief  Отпревляет перезагрузочный импульс
 *         для датчиков по 1-wire.
 * @retval Ответил ли кто-нибудь(SUCCESS) из датчиков
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
