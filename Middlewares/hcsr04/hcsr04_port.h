/**
 * @file
 * @brief   Платформозависимая часть библиотеки ультразвуковых дальномеров
 *          HC-SR04.
 *
 * Путём реализации данного интерфейса можно перенести библиотеку
 * на любую платформу.
 *
 * @note
 * В данном файле можно подключать любые заголовочные файлы,
 * необходимые для реализации объявленного интерфейса.
 *
 * @author  Danya0x07
 */

#ifndef HCSR04_PORT_H
#define HCSR04_PORT_H

#include <main.h>
#include <tim.h>

typedef GPIO_TypeDef *gpio_port_t;
typedef uint32_t gpio_pin_t;

struct hcsr04_gpio {
    gpio_port_t gport;
    gpio_pin_t gpin_trig;
    gpio_pin_t gpin_echo;
} sensors[1] = {
    {
        .gport = SONAR_GPIO_Port,
        .gpin_trig = SONAR_TRIG_Pin,
        .gpin_echo = SONAR_ECHO_Pin,
    }
};

static inline void gpio_write_high(gpio_port_t port, gpio_pin_t pin)
{
    LL_GPIO_SetOutputPin(port, pin);
}

static inline void gpio_write_low(gpio_port_t port, gpio_pin_t pin)
{
    LL_GPIO_ResetOutputPin(port, pin);
}

static inline void gpio_write_reverse(gpio_port_t port, gpio_pin_t pin)
{
    LL_GPIO_TogglePin(port, pin);
}

static inline uint32_t gpio_read(gpio_port_t port, gpio_pin_t pin)
{
    return LL_GPIO_IsInputPinSet(port, pin);
}

#endif /* HCSR04_PORT_H */
