#ifndef _SENSOR1_H
#define _SENSOR1_H

#include <FreeRTOS.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <timers.h>

#include "sensor1_public.h"
#include "system_config.h"
#include "system_definitions.h"

#ifdef __cplusplus

extern "C" {

#endif

#define SENSOR1_CALLBACKS_VECTOR_SIZE (10)
#define SENSOR1_QUEUE_SIZE (10)

typedef enum {
  SENSOR1_STATE_INIT = 0,
  SENSOR1_STATE_RECEIVE,
} SENSOR1_STATES;

typedef enum {

  SENSOR_CALIBRATION = 0,
  SENSOR_TEST_1,
  SENSOR_TEST_2,
  SENSOR_TEST_3,
  SENSOR_TESTS_CONCLUDED
} UNIT_TESTING;

typedef struct {
  SENSOR1_STATES state;
  QueueHandle_t sensor1Queue;
  UNIT_TESTING unitTesting;
  size_t sensor1_callbacks_idx;
  sensor1_callback_t callbacks[SENSOR1_CALLBACKS_VECTOR_SIZE];

  TimerHandle_t timerHandle;
} SENSOR1_DATA;

void SENSOR1_Initialize(void);

void SENSOR1_Tasks(void);

volatile extern uint32_t forward_sensor_val;
volatile extern uint32_t right_sensor_val;

#endif

#ifdef __cplusplus
}
#endif