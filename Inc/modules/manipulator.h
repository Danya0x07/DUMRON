#ifndef MANIPULATOR_H_INCLUDED
#define MANIPULATOR_H_INCLUDED

typedef enum {
    ARM_UP = 1,
    ARM_DOWN = -1
} ArmServoDirection;

typedef enum {
    CLAW_SQUEESE = 1,
    CLAW_RELEASE = -1
} ClawServoDirection;

void manipulator_arm_move(ArmServoDirection dir);
void manipulator_claw_move(ClawServoDirection dir);

#endif
