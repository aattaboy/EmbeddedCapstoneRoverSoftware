
#ifndef _ENCODERS_H
#define _ENCODERS_H

#include <FreeRTOS.h>
#include <queue.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "encoders_public.h"
#include "system_config.h"
#include "system_definitions.h"

typedef enum {
  ENCODERS_STATE_INIT = 0,
  ENCODERS_STATE_RECEIVE,
} ENCODERS_STATES;

#define ENCODERS_CALLBACKS_VECTOR_SIZE (10)

typedef struct {
  ENCODERS_STATES state;
  QueueHandle_t encodersQueue;

  uint64_t leftCount;
  uint64_t rightCount;
  
  uint32_t left_cycles;
  uint32_t right_cycles;

  size_t encoders_callbacks_idx;
  encoders_callback_t callbacks[ENCODERS_CALLBACKS_VECTOR_SIZE];

} ENCODERS_DATA;

void ENCODERS_Initialize(void);

void ENCODERS_Tasks(void);

#endif