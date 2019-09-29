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

typedef struct {
    uint8_t temp_lim_h;
    uint8_t temp_lim_l;
    DsResulution resolution;
} DsConfig;

typedef struct {
    uint8_t temp_lsb;
    uint8_t temp_msb;
    //uint8_t crc;
} DsOutputData;

void ds_select_single(uint64_t addr);
void ds_select_all(void);
uint64_t ds_get_addr_of_single(void);

void ds_write_config(DsConfig*);
void ds_read_data(DsOutputData*);

void ds_write_byte(uint8_t);
uint8_t ds_read_byte(void);

#define ds_start_measuring()  ds_write_byte(DS_CONVERT_TEMP)
#define ds_measure_delay()    delay_us(480)

#endif
