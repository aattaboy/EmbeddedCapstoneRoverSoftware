#include "debug.h"
#include "encoders.h"
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
    errorCheck(__FILE__, __LINE__);
  }

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

void ENCODERS_Tasks(void) {
  switch (encodersData.state) {
  case ENCODERS_STATE_INIT: {
    encodersData.state = ENCODERS_STATE_RECEIVE;
  } break;

  case ENCODERS_STATE_RECEIVE: {
    EncoderID id;
    if (xQueueReceive(encodersData.encodersQueue, &id, portMAX_DELAY)) {
      switch (id) {
      case ENCODERS_LEFT: {
        encodersData.leftCount++;
      } break;
      case ENCODERS_RIGHT: {
        encodersData.rightCount++;
      } break;
      default: { errorCheck(__FILE__, __LINE__); }
      }
      struct EncoderCounts counts;
      counts.left = encodersData.leftCount;
      counts.right = encodersData.rightCount;
      sendToEncodersCallbacks(&counts);

      PORTE = encodersData.leftCount;
    }
  } break;

  default: { break; }
  }
}
