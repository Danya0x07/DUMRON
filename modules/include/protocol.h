/**
 * Протокол общения пульта с роботом.
 */
#ifndef ROBOT_INTERFACE_H_INCLUDED
#define ROBOT_INTERFACE_H_INCLUDED

#include <stdint.h>

/**
 * Возможные направления вращения моторов робота:
 */
typedef enum {
    ROBOT_DIRECTION_NONE,      // в никуда
    ROBOT_DIRECTION_FORWARD,   // оба вперёд
    ROBOT_DIRECTION_BACKWARD,  // оба назад
    ROBOT_DIRECTION_LEFTWARD,  // левый назад, правый вперёд
    ROBOT_DIRECTION_RIGHTWARD  // левый вперёд, правый назад
} RobotDirection_e;

_Static_assert(sizeof(RobotDirection_e) == 1, "incorrect size of enum");

/**
 * Структура пакетов, идущих от пульта к роботу.
 */
typedef struct {
    RobotDirection_e direction;  // направление
    uint8_t speedL;   // скорость левого мотора [0; 255]
    uint8_t speedR;   // скорость правого мотора [0; 255]
    uint8_t ctrlReg;  // регистр управления
} DataToRobot_s;

/* флаги управления */
#define ROBOT_CFLAG_ARM_DOWN (1 << 0)
#define ROBOT_CFLAG_ARM_UP   (1 << 1)
#define ROBOT_CFLAG_CLAW_SQUEEZE (1 << 2)
#define ROBOT_CFLAG_CLAW_RELEASE (1 << 3)
/* TODO #define ROBOT_CFLAG_ARM_FOLD  (1 << 4) */
#define ROBOT_CFLAG_LIGHTS_EN (1 << 5)
#define ROBOT_CFLAG_KLAXON_EN (1 << 6)

/**
 * Структура пакетов, идущих от робота к пульту.
 */
typedef struct {
    uint8_t brainsCharge;  // заряд аккумулятора "мозговой"(нежной) части
    uint8_t motorsCharge;  // заряд аккумулятора "силовой" части
    uint8_t backDistance;  // расстояние от кормы до поверхности
    int8_t  ambientTemperature;   // температура окружающей среды
    int8_t  internalTemperature;  // температура внутри корпуса
} DataFromRobot_s;

#endif
