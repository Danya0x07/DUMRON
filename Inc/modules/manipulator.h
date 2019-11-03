#ifndef MANIPULATOR_H_INCLUDED
#define MANIPULATOR_H_INCLUDED

typedef enum {
    ARM_STOP = 0,
    ARM_UP = 1,
    ARM_DOWN = -1
} ArmServoDirection;

typedef enum {
    CLAW_STOP = 0,
    CLAW_SQUEESE = 1,
    CLAW_RELEASE = -1
} ClawServoDirection;

void manipulator_set_directions(ArmServoDirection, ClawServoDirection);
void manipulator_move(void);

#endif
