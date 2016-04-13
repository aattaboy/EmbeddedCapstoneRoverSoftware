#ifndef _POSE_H_
#define _POSE_H_

#include "pose_public.h"

#include <FreeRTOS.h>
#include <queue.h>

typedef enum { POSE_STATE_INIT = 0, POSE_STATE_RECEIVE } POSE_STATES;

#define POSE_CALLBACKS_VECTOR_SIZE (10)

typedef struct {
  POSE_STATES state;
  QueueHandle_t poseQueue;

  double x, y, yaw;
  struct EncoderCounts prev_counts;

  size_t pose_callbacks_idx;
  pose_callback_t callbacks[POSE_CALLBACKS_VECTOR_SIZE];
} POSE_DATA;

void POSE_Initialize(void);

void POSE_Tasks(void);

#endif /* _POSE_H_ */
