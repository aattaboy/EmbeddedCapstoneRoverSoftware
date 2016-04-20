#ifndef _PID_H
#define _PID_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <timers.h>
#include "system_config.h"
#include "system_definitions.h"
#include "pid_public.h"
#include "motor1.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PID_CALLBACKS_VECTOR_SIZE (10)
#define PID_QUEUE_SIZE (10)

typedef enum {
  PID_STATE_INIT = 0,
  PID_STATE_RECEIVE,
} PID_STATES;

typedef struct {
  PID_STATES state;

  // Queue
  QueueHandle_t pidQueue;

  // Callbacks vector members
  size_t pid_callbacks_idx;
  pid_callback_t callbacks[PID_CALLBACKS_VECTOR_SIZE];

  uint32_t velocity_left;
  uint32_t velocity_right;
  
  TimerHandle_t timer;
  
} PID_DATA;

void PID_Initialize(void);

void PID_Tasks(void);

#endif

#ifdef __cplusplus
}
#endif
