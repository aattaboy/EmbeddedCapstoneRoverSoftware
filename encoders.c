#include "debug.h"
#include "debuginfo.h"
#include "encoders.h"
#include "motor1_public.h"
#include "system_config/default/framework/driver/tmr/drv_tmr_static.h"

ENCODERS_DATA encodersData;

#define ENCODERS_QUEUE_SIZE (10)

void sendToEncodersQueueFromISR(EncoderID encoder_id,
                                BaseType_t *higherPriorityTaskWoken) {
  xQueueSendToBackFromISR(encodersData.encodersQueue, &encoder_id,
                          higherPriorityTaskWoken);
}

void ENCODERS_Initialize(void) {
  encodersData.state = ENCODERS_STATE_INIT;
  encodersData.encodersQueue =
      xQueueCreate(ENCODERS_QUEUE_SIZE, sizeof(EncoderID));
  if (encodersData.encodersQueue == 0) {
    errorCheck(ENCODER1_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(encodersData.encodersQueue, "Encoders Queue");

  encodersData.leftCount = 0;
  encodersData.rightCount = 0;

  encodersData.encoders_callbacks_idx = 0;

  DRV_TMR0_Start();
  DRV_TMR1_Start();
}

int registerEncodersCallback(encoders_callback_t callback) {
  if (encodersData.encoders_callbacks_idx < ENCODERS_CALLBACKS_VECTOR_SIZE) {
    encodersData.callbacks[encodersData.encoders_callbacks_idx++] = callback;
    return 1;
  } else {
    return 0;
  }
}

static void sendToEncodersCallbacks(struct EncoderCounts *counts) {
  size_t i;
  for (i = 0; i < encodersData.encoders_callbacks_idx; i++) {
    encodersData.callbacks[i](counts);
  }
}

static int32_t constrain(int val, int max, int min) {
  if (val > max) {
    val = max;
    // PORTE = 0x1;
  } else if (val < min) {
    val = min;
    // PORTE = 0x2;
  }
  return val;
}

// static size_t state_sequence_idx = 0;

const enum MOTOR1DIRECTION state_sequence[] = {
  MOTOR_FORWARD,  MOTOR_LEFT,  MOTOR_FORWARD,  MOTOR_LEFT,
  MOTOR_FORWARD,  MOTOR_LEFT,  MOTOR_FORWARD,  MOTOR_LEFT,
  MOTOR_FORWARD,  MOTOR_RIGHT, MOTOR_FORWARD,  MOTOR_RIGHT,
  MOTOR_FORWARD,  MOTOR_RIGHT, MOTOR_FORWARD,  MOTOR_RIGHT,
  MOTOR_BACKWARD, MOTOR_RIGHT, MOTOR_BACKWARD, MOTOR_RIGHT,
  MOTOR_BACKWARD, MOTOR_RIGHT, MOTOR_BACKWARD, MOTOR_RIGHT,
  MOTOR_BACKWARD, MOTOR_LEFT,  MOTOR_BACKWARD, MOTOR_LEFT,
  MOTOR_BACKWARD, MOTOR_LEFT,  MOTOR_BACKWARD, MOTOR_LEFT
};

void ENCODERS_Tasks(void) {
  switch (encodersData.state) {
  case ENCODERS_STATE_INIT: {
    encodersData.state = ENCODERS_STATE_RECEIVE;
    encodersData.leftCount = 0;
    encodersData.rightCount = 0;
  } break;

  case ENCODERS_STATE_RECEIVE: {
    EncoderID id;
    if (xQueueReceive(encodersData.encodersQueue, &id, portMAX_DELAY)) {
      switch (id) {
      case ENCODERS_LEFT: {
        encodersData.leftCount++;
        packAndSendDebugInfo(ENCODER1_IDENTIFIER, Encoder1LeftCount,
                             encodersData.leftCount);
      } break;
      case ENCODERS_RIGHT: {
        encodersData.rightCount++;
        packAndSendDebugInfo(ENCODER1_IDENTIFIER, Encoder1RightCount,
                             encodersData.rightCount);
      } break;
      default: { errorCheck(ENCODER1_IDENTIFIER, __LINE__); }
      }

      struct EncoderCounts counts;
      counts.left = encodersData.leftCount;
      counts.right = encodersData.rightCount;
      counts.left_dir = (PORTC >> 14) & 0x1;
      counts.right_dir = (PORTG >> 1) & 0x1;
      sendToEncodersCallbacks(&counts);
    }
  } break;

  default: { break; }
  }
}
