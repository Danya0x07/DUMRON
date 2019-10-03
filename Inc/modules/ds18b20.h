#ifndef DS18B20_H_INCLUDED
#define DS18B20_H_INCLUDED

#include "main.h"

typedef enum {
    DS_SEARCH_ROM = 0xF0,
    DS_READ_ROM   = 0x33,
    DS_MATCH_ROM  = 0x55,
    DS_SKIP_ROM   = 0xCC,
    DS_CONVERT_TEMP  = 0x44,
    DS_W_SCRATCHPAD  = 0x4E,
    DS_R_SCRATCHPAD  = 0XBE,
    DS_CP_SCRATCHPAD = 0x48,
    DS_ALARM_SEARCH  = 0xEC,
    DS_R_PWR_SUPPLY  = 0xB4,
    DS_RECALL_E2 = 0xB8,
} DsCommands;

typedef enum {
    DS_RESOLUTION_9BIT  = 0x1F,
    DS_RESOLUTION_10BIT = 0x3F,
    DS_RESOLUTION_11BIT = 0x5F,
    DS_RESOLUTION_12BIT = 0x7F
} DsResulution;

/**
 * @brief  Структура, описывающая настройки датчика,
 *         которые могут быть установлены пользователем.
 */
typedef struct {
    uint8_t temp_lim_h;
    uint8_t temp_lim_l;
    DsResulution resolution;
} DsConfig;

/**
 * @brief  Структура, описывающая выходные данные с датчика.
 *         Помимо двух байтов с температурой, из того, что
 *         может интересоввать пользователя, датчик ещё
 *         выдаёт девятым байтом контрольную сумму для
 *         предыдущих восьми, но функционал её проверки
 *         ещё не реализован.
 */
typedef struct {
    uint8_t temp_lsb;
    uint8_t temp_msb;
    uint8_t th;
} DsOutputData;

void ds_select_all(void);
void ds_select_single(const uint8_t address[8]);
void ds_get_addr_of_single(uint8_t address_buff[8]);

void ds_write_config(const DsConfig*);
void ds_read_data(DsOutputData*);

ErrorStatus ds_reset_pulse(void);
void ds_write_byte(const uint8_t);
uint8_t ds_read_byte(void);

#define ds_start_measuring()  ds_write_byte(DS_CONVERT_TEMP)
#define ds_measure_delay()    delay_us(800)

#endif
