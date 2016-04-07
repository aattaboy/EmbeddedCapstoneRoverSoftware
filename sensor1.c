#include <FreeRTOS.h>
#include <math.h>
#include <stdlib.h>
#include <timers.h>

#include "debug.h"
#include "debug_codes.h"
#include "debuginfo.h"
#include "generated/IRSensorData.pbo.h"
#include "sensor1.h"
#include "system_config/default/framework/driver/adc/drv_adc_static.h"

SENSOR1_DATA sensor1Data;

// Public functions
int registerSensor1Callback(sensor1_callback_t callback) {
  if (sensor1Data.sensor1_callbacks_idx != SENSOR1_CALLBACKS_VECTOR_SIZE) {
    sensor1Data.callbacks[sensor1Data.sensor1_callbacks_idx++] = callback;
    return 1;
  } else {
    return 0;
  }
}

BaseType_t sendToSensor1Queue(struct sensorInterrupt *info) {
  BaseType_t retval =
      xQueueSendToBack(sensor1Data.sensor1Queue, info, portMAX_DELAY);
  return retval;
}

BaseType_t sendToSensor1QueueFromISR(struct sensorInterrupt *info,
                                     BaseType_t *higherPriorityTaskWoken) {
  BaseType_t retval = xQueueSendToBackFromISR(sensor1Data.sensor1Queue, info,
                                              higherPriorityTaskWoken);
  return retval;
}

static void sendMessageToCallbacks(IRSensorData *info) {
  size_t i;
  for (i = 0; i < sensor1Data.sensor1_callbacks_idx; i++) {
    sensor1Data.callbacks[i](info);
  }
}

static void sensor1TimerCallback(TimerHandle_t timer __attribute__((unused))) {
  DRV_ADC_Initialize();
  DRV_ADC_Open();
  DRV_ADC_Start();
}

static void initialize_sensor1_timer(void) {
  sensor1Data.timerHandle = xTimerCreate("ADC Timer", 250 / portTICK_PERIOD_MS,
                                         pdTRUE, NULL, sensor1TimerCallback);

  if (sensor1Data.timerHandle == NULL) {
    errorCheck(__FILE__, __LINE__);
  }
}

void SENSOR1_Initialize(void) {
  sensor1Data.state = SENSOR1_STATE_INIT;
  sensor1Data.sensor1Queue =
      xQueueCreate(SENSOR1_QUEUE_SIZE, sizeof(struct sensorInterrupt));
  if (sensor1Data.sensor1Queue == 0) {
    errorCheck(__FILE__, __LINE__);
  }
  vQueueAddToRegistry(sensor1Data.sensor1Queue, "Sensor 1 Queue");

  initialize_sensor1_timer();
}

void SENSOR1_Tasks(void) {
  switch (sensor1Data.state) {
  case SENSOR1_STATE_INIT: {
    if (xTimerStart(sensor1Data.timerHandle, 0) != pdPASS) {
      errorCheck(__FILE__, __LINE__);
    }
    sensor1Data.state = SENSOR1_STATE_RECEIVE;
    sensor1Data.unitTesting = SENSOR_TESTS_CONCLUDED;
    break;
  }
  case SENSOR1_STATE_RECEIVE: {
    struct sensorInterrupt received;

    if (xQueueReceive(sensor1Data.sensor1Queue, &received, portMAX_DELAY)) {

      static uint32_t seq_tx = 0;

      IRSensorData data;
      IRSensorData_init(&data);
      IRSensorData_set_front(&data, received.center);
      IRSensorData_set_left(&data, received.left);
      IRSensorData_set_right(&data, received.right);
      IRSensorData_to_bytes(&data, (char *)&data, seq_tx++);
      sendMessageToCallbacks(&data);

      static uint32_t seq = 0;
      DebugInfo info;
      DebugInfo_init(&info);
      DebugInfo_set_debugID(&info, Sensor1Receive_debugid);
      DebugInfo_set_identifier(&info, SENSOR1_IDENTIFIER);
      DebugInfo_set_data(&info, received.center);
      DebugInfo_to_bytes(&info, (char *)&info, seq++);
      sendDebugInfo(&info);
    }
    break;
  default: { break; }
  }
  }
}