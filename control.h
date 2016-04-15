
#ifndef _CONTROL_H
#define _CONTROL_H

#include <FreeRTOS.h>
#include <queue.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "control_public.h"
#include "system_config.h"
#include "system_definitions.h"

typedef enum {
  CONTROL_STATE_INIT = 0,
  CONTROL_STATE_RECEIVE,
  CONTROL_STATE_FORWARD,
  CONTROL_STATE_LEFT90,
  CONTROL_STATE_RIGHT90,
  CONTROL_STATE_RIGHT180,
  CONTROL_STATE_STOP
} CONTROL_STATES;

#define CONTROL_CALLBACKS_VECTOR_SIZE (10)

typedef struct {
  CONTROL_STATES state;
  QueueHandle_t controlQueueSetPoint;
  QueueHandle_t controlQueueRoverPosition;

  size_t control_callbacks_idx;
  control_callback_t callbacks[CONTROL_CALLBACKS_VECTOR_SIZE];

} CONTROL_DATA;

void CONTROL_Initialize(void);

void CONTROL_Tasks(void);

#endif
