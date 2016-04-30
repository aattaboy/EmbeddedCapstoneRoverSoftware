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

volatile uint32_t forward_sensor_val = 0xffffffff;
volatile uint32_t right_sensor_val = 0xffffffff;

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
  PLIB_ADC_MuxAInputScanEnable(ADC_ID_1);
}

static void initialize_sensor1_timer(void) {
  sensor1Data.timerHandle = xTimerCreate("ADC Timer", 50 / portTICK_PERIOD_MS,
                                         pdTRUE, NULL, sensor1TimerCallback);

  if (sensor1Data.timerHandle == NULL) {
    errorCheck(SENSOR1_IDENTIFIER, __LINE__);
  }
}

void SENSOR1_Initialize(void) {
  sensor1Data.state = SENSOR1_STATE_INIT;
  sensor1Data.sensor1Queue =
      xQueueCreate(SENSOR1_QUEUE_SIZE, sizeof(struct sensorInterrupt));
  if (sensor1Data.sensor1Queue == 0) {
    errorCheck(SENSOR1_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(sensor1Data.sensor1Queue, "Sensor 1 Queue");

  initialize_sensor1_timer();
}

void SENSOR1_Tasks(void) {
  switch (sensor1Data.state) {
  case SENSOR1_STATE_INIT: {
    if (xTimerStart(sensor1Data.timerHandle, 0) != pdPASS) {
      errorCheck(SENSOR1_IDENTIFIER, __LINE__);
    }
    sensor1Data.state = SENSOR1_STATE_RECEIVE;
    sensor1Data.unitTesting = SENSOR_TESTS_CONCLUDED;
    break;
  }
  case SENSOR1_STATE_RECEIVE: {
    struct sensorInterrupt received;

    if (xQueueReceive(sensor1Data.sensor1Queue, &received, portMAX_DELAY)) {

      static uint32_t seq_tx = 0;
      
      
#define LPF_SIZE (5)
      
      static uint32_t left_lpf[LPF_SIZE], right_lpf[LPF_SIZE], center_lpf[LPF_SIZE];
      static size_t lpf_idx;
      
      left_lpf[lpf_idx] = received.left;
      right_lpf[lpf_idx] = received.right;
      center_lpf[lpf_idx] = received.center;
      lpf_idx++;
      
      if (lpf_idx==LPF_SIZE) lpf_idx = 0;
      
      double left = 0, right = 0, center = 0;
      size_t i;
      for (i=0; i<LPF_SIZE; i++) {
        left += left_lpf[i]/((double)LPF_SIZE);
        right += right_lpf[i]/((double)LPF_SIZE);
        center += center_lpf[i]/((double)LPF_SIZE);
      }

      IRSensorData data;
      IRSensorData_init(&data);
      
      IRSensorData_set_front(&data, center);
      IRSensorData_set_left(&data, left);
      IRSensorData_set_right(&data, right);
      forward_sensor_val = center;
      right_sensor_val = right;
      IRSensorData_to_bytes(&data, (char *)&data, seq_tx++);
      sendMessageToCallbacks(&data);
      
      packAndSendDebugInfo(SENSOR1_IDENTIFIER, Sensor1CenterSensorValue,
                           center);
      packAndSendDebugInfo(SENSOR1_IDENTIFIER, Sensor1LeftSensorValue,
                           left);
      packAndSendDebugInfo(SENSOR1_IDENTIFIER, Sensor1RightSensorValue,
                           right);
    }
    break;
  default: { break; }
  }
  }
}
