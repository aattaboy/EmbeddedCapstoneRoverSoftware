#ifndef _RSSI_COLLECTOR_H
#define _RSSI_COLLECTOR_H

#include "rssi_collector_public.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

#ifdef __cplusplus

extern "C" {

#endif

typedef enum {
  RSSI_COLLECTOR_STATE_INIT = 0,
  RSSI_COLLECTOR_FRAME_START_1,
  RSSI_COLLECTOR_FRAME_START_2,
  RSSI_COLLECTOR_FRAME_START_3,
  RSSI_COLLECTOR_FRAME_START_4,
  RSSI_COLLECTOR_STATE_RECEIVE,
} RSSI_COLLECTOR_STATES;

#define RSSI_COLLECTOR_CALLBACKS_VECTOR_SIZE (10)
#define RSSI_COLLECTOR_QUEUE_SIZE (50)

typedef struct {
  RSSI_COLLECTOR_STATES state;
  QueueHandle_t rssiCollectorQueue;

  size_t rssi_collector_callbacks_idx;
  rssi_collector_callback_t callbacks[RSSI_COLLECTOR_CALLBACKS_VECTOR_SIZE];

  // Receive buffer members
  size_t receive_buf_idx;
  char receive_buf[sizeof(RSSIData)];
} RSSI_COLLECTOR_DATA;

void RSSI_COLLECTOR_Initialize(void);

void RSSI_COLLECTOR_Tasks(void);

#endif

#ifdef __cplusplus
}
#endif
