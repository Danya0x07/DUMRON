/**
 * Модуль управления манипулятором робота.
 *
 * Манипулятор состоит из плеча(Arm) и клешни(Claw).
 */
#ifndef MANIPULATOR_H_INCLUDED
#define MANIPULATOR_H_INCLUDED

typedef enum {
    ARM_STOP = 0,
    ARM_UP = 1,
    ARM_DOWN = -1
} ArmDirection_e;

typedef enum {
    CLAW_STOP = 0,
    CLAW_SQUEESE = 1,
    CLAW_RELEASE = -1
} ClawDirection_e;

void Manipulator_SetDirections(ArmDirection_e, ClawDirection_e);
void Manipulator_Move(void);

#endif
