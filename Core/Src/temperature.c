#include "temperature.h"
#include "protocol.h"
#include "errors.h"
#include "debug.h"

#include <ds18b20/ds18b20.h>

/* Количество попыток считывания результата измерений при несовпадении CRC. */
#define RESULT_READING_RETRIES    3

static const uint8_t internalSensorAddress[8] = {
    0x28, 0xAA, 0xCE, 0xB1, 0x48, 0x14, 0x01, 0x8E
};

static const uint8_t ambientSensorAddress[8] = {
    0x28, 0x0A, 0xA7, 0x79, 0x97, 0x14, 0x03, 0xEF
};

static int8_t measureTemperature(const uint8_t *sensorAddress)
{
    int32_t result;
    int status;
    int retries = RESULT_READING_RETRIES;

    do {
        status = ds18b20_get_result(sensorAddress, &result);
        --retries;
    } while (status == DS18B20_ECRC && retries > 0);

    if (status == DS18B20_ECRC) {
        debug_logs("ds18b20 crc mismatch\n");
        return TEMPERATURE_ERROR_VALUE;
    }
    else if (status == DS18B20_EABSENT) {
        debug_logs("ds18b20 absent (get)\n");
        return TEMPERATURE_ERROR_VALUE;
    }

    int8_t res_integer;
    ds18b20_parse_result(result, &res_integer, NULL);
    return res_integer;
}

void Temperature_Init(void)
{
    struct ds18b20_config config = {
        .resolution = DS18B20_RESOLUTION_9BIT
    };

    if (ds18b20_configure(NULL, &config) != DS18B20_OK) {
        debug_logs("ds18b20 init error\n");
        ErrorShow_InitTemperature();
    }
}

void Temperature_StartMeasurement(void)
{
    int status = ds18b20_start_measurement(NULL);

    if (status == DS18B20_EABSENT) {
        debug_logs("ds18b20 absent (smsr)\n");
    }
}

int8_t Temperature_GetAmbient(void)
{
    return measureTemperature(ambientSensorAddress);
}

int8_t Temperature_GetInternal(void)
{
    return measureTemperature(internalSensorAddress);
}
