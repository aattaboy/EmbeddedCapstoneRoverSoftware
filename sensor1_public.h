#ifndef SENSOR1_PUBIC_H
#define SENSOR1_PUBIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <FreeRTOS.h>
#include <generated/IRSensorData.pbo.h>
#include <queue.h>

typedef int (*sensor1_callback_t)(IRSensorData *);

struct sensorInterrupt {
  uint32_t left;
  uint32_t right;
  uint32_t center;
};

int registerSensor1Callback(sensor1_callback_t callback);
BaseType_t sendToSensor1Queue(struct sensorInterrupt *info);
BaseType_t sendToSensor1QueueFromISR(struct sensorInterrupt *info,
                                     BaseType_t *higherPriorityTaskWoken);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR1_PUBIC_H */