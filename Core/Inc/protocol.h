/**
 * Протокол общения пульта с роботом.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

/** Возможные направления вращения моторов робота. */
typedef enum {
    MOVEDIR_NONE = 0,  // в никуда
    MOVEDIR_FORWARD,   // оба вперёд
    MOVEDIR_BACKWARD,  // оба назад
    MOVEDIR_LEFTWARD,  // левый назад, правый вперёд
    MOVEDIR_RIGHTWARD  // левый вперёд, правый назад
} MoveDirection_e;

/** Возможные состояния плеча манипулятора. */
typedef enum {
    ARM_STOP = 0,
    ARM_UP,
    ARM_DOWN
} ArmControl_e;

/** Возможные состояние клешни манипулятора. */
typedef enum {
    CLAW_STOP = 0,
    CLAW_SQUEESE,
    CLAW_RELEASE
} ClawControl_e;

/**
 * Структура пакетов, идущих от пульта к роботу.
 */
typedef struct {
    union {
        struct {
            uint8_t moveDir :3;
            uint8_t armCtrl  :2;
            uint8_t clawCtrl :2;
            uint8_t lightsEn :1;

            uint8_t _extra   :7;
            uint8_t buzzerEn :1;
        } bf;
        uint16_t reg;
    } ctrl;
    uint8_t speedL;
    uint8_t speedR;
} DataToRobot_s;

/** Возможные реалии расстояния от кормы робота до поверхности. */
typedef enum {
    DIST_NOTHING = 0,
    DIST_CLIFF,
    DIST_OBSTACLE,
    DIST_ERROR  // если сонар сорвало
} Distance_e;

/**
 * Структура пакетов, идущих от робота к пульту.
 */
typedef struct {
    union {
        struct {
            uint8_t _extra :6;
            uint8_t backDistance :2;
        } bf;
        uint8_t reg;
    } status;
    uint8_t brainsCharge;
    uint8_t motorsCharge;
    int8_t  ambientTemperature;
    int8_t  internalTemperature;
} DataFromRobot_s;

_Static_assert(sizeof(DataToRobot_s) == 4, "ss");
_Static_assert(sizeof(DataFromRobot_s) == 5, "ss");

#endif /* PROTOCOL_H */
