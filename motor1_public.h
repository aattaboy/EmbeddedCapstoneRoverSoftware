#ifndef MOTOR1_PUBLIC_H
#define MOTOR1_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "generated/MotorCommand.pbo.h"

BaseType_t sendToMotor1Queue(MotorCommand *info);
BaseType_t sendToMotor1QueueFromISR(MotorCommand *info,
                                    BaseType_t *higherPriorityTaskWoken);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR1_PUBLIC_H */
