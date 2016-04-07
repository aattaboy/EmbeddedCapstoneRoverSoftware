#ifndef PID_PUBLIC_H
#define PID_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "encoders_public.h"
#include "generated/PIDSetPoint.pbo.h"
#include "generated/MotorCommand.pbo.h"

typedef enum { ENCODER_COUNTS } PID_VARIANT_TYPE;

struct PID_VARIANT {
  union {
    struct EncoderCounts encoder_counts;
  } data;
  PID_VARIANT_TYPE type;
};

typedef int (*pid_callback_t)(MotorCommand *);

int registerPIDCallback(pid_callback_t callback);

BaseType_t sendToPIDQueue(struct PID_VARIANT *info);
BaseType_t sendToPIDQueueFromISR(struct PID_VARIANT *info,
                                 BaseType_t *higherPriorityTaskWoken);

extern int32_t pidBaseDutyCycle;

#ifdef __cplusplus
}
#endif

#endif /* PID_PUBLIC_H */