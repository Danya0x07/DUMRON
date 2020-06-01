#include "hcsr04.h"
#include "hcsr04_port.h"

#define TRIGGER_PULSE_WIDTH_US    11
#define ECHO_RESPONSE_TIMEOUT_US  50
#define MAX_ECHO_PULSE_WIDTH_US   38000

int hcsr04_measure(unsigned int sensor_id)
{
    struct hcsr04_gpio *sensor = &sensors[sensor_id];

    gpio_write_high(sensor->gport, sensor->gpin_trig);
    delay_us(TRIGGER_PULSE_WIDTH_US);
    gpio_write_low(sensor->gport, sensor->gpin_trig);
    tim_us_reset_counter();
    while (!gpio_read(sensor->gport, sensor->gpin_echo) &&
            tim_us_get_counter() < ECHO_RESPONSE_TIMEOUT_US)
        ;
    tim_us_reset_counter();
    while (gpio_read(sensor->gport, sensor->gpin_echo) &&
            tim_us_get_counter() < MAX_ECHO_PULSE_WIDTH_US)
        ;
    return tim_us_get_counter() / 58;
}
