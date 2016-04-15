#ifndef CONTROL_PUBLIC_H
#define CONTROL_PUBLIC_H

#include "generated/RoverPose.pbo.h"
#include "generated/MotorCommand.pbo.h"

typedef int (*control_callback_t)(MotorCommand *);

int registerControlCallback(control_callback_t callback);

int sendToControlSetPointQueue(RoverPose *);

int sendToControlRoverPositionQueue(RoverPose *);

#endif /* CONTROL_PUBLIC_H */
