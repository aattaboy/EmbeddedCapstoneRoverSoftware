
#ifndef _RSSI_PAIRS_H
#define _RSSI_PAIRS_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

#include "rssi_pairs_public.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RSSI_PAIRS_QUEUE_SIZE (10)
#define RSSI_PAIRS_CALLBACKS_VECTOR_SIZE (10)

typedef struct {
  RSSI_PAIRS_STATES state;
  QueueHandle_t rssi_pairsQueue;

  rssi_pairs_queue_receive_cb queueReceive;

  bool receieved_pose;
  RoverPose last_reported_pose;

  size_t rssi_pairs_callbacks_idx;
  rssi_pairs_callback_t callbacks[RSSI_PAIRS_CALLBACKS_VECTOR_SIZE];

} RSSI_PAIRS_DATA;

#endif

#ifdef __cplusplus
}
#endif
