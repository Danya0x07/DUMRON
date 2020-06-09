/**
 * Модуль управления манипулятором робота.
 *
 * Манипулятор состоит из плеча(Arm) и клешни(Claw).
 */

#ifndef _MANIPULATOR_H
#define _MANIPULATOR_H

#include "protocol.h"

void Manipulator_SetArm(ArmControl_e ctrl);
void Manipulator_SetClaw(ClawControl_e ctrl);
void Manipulator_Move(void);

#endif /* _MANIPULATOR_H */
