#include "debug.h"
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

static int32_t constrain(int val, int max, int min) {
  if (val > max) {
    val = max;
    //PORTE = 0x1;
  } else if (val < min) {
    val = min;
    //PORTE = 0x2;
  }
  return val;
}

//static size_t state_sequence_idx = 0;

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

uint8_t encoders_base_duty_cycle = 0;

void ENCODERS_Tasks(void) {
  switch (encodersData.state) {
  case ENCODERS_STATE_INIT: {
    uint8_t atomic_duty_cycle =
        __sync_fetch_and_add(&encoders_base_duty_cycle, 0);
    MotorCommand command_set;
    MotorCommand_init(&command_set);
    MotorCommand_set_mode(&command_set, MOTOR_COMMAND_SET);
    MotorCommand_set_direction(&command_set, MOTOR_FORWARD);
    MotorCommand_set_dutyCycle(&command_set, atomic_duty_cycle);
    MotorCommand_to_bytes(&command_set, (char *)&command_set, 0);
    sendToMotor1Queue(&command_set);
    MotorCommand_set_mode(&command_set, MOTOR_PID_SET);
    MotorCommand_set_direction(&command_set, MOTOR_FORWARD);
    MotorCommand_set_dutyCycle(&command_set, atomic_duty_cycle);
    MotorCommand_to_bytes(&command_set, (char *)&command_set, 0);
    sendToMotor1Queue(&command_set);
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
      } break;
      case ENCODERS_RIGHT: {
        encodersData.rightCount++;
#if 0
        if (encodersData.rightCount % 57 == 0) {
          state_sequence_idx++;
          if (state_sequence_idx ==
              (sizeof(state_sequence) / sizeof(state_sequence[0]))) {
            state_sequence_idx = 0;
            uint8_t atomic_duty_cycle =
                __sync_fetch_and_add(&encoders_base_duty_cycle, 0);
            MotorCommand command_set;
            MotorCommand_set_mode(&command_set, MOTOR_PID_SET);
            MotorCommand_set_direction(&command_set, MOTOR_FORWARD);
            MotorCommand_set_dutyCycle(&command_set, atomic_duty_cycle);
            MotorCommand_to_bytes(&command_set, (char *)&command_set, 0);
            sendToMotor1Queue(&command_set);
          }
        }
#endif
      } break;
      default: { errorCheck(__FILE__, __LINE__); }
      }

      struct EncoderCounts counts;
      counts.left = encodersData.leftCount;
      counts.right = encodersData.rightCount;
      sendToEncodersCallbacks(&counts);

#if 0
      int32_t diff = counts.left - counts.right;
      static int32_t integral;
      integral += diff;
      int32_t pid = 30* diff;
      PORTE = diff;

      uint8_t atomic_duty_cycle =
          __sync_fetch_and_add(&encoders_base_duty_cycle, 0);
      MotorCommand pid_set;
      MotorCommand_init(&pid_set);
      MotorCommand_set_direction(&pid_set, state_sequence[state_sequence_idx]);
      MotorCommand_set_mode(&pid_set, MOTOR_PID_SET);
      MotorCommand_set_dutyCycle(&pid_set,
                                 constrain(atomic_duty_cycle + pid, 0, 100));
      MotorCommand_to_bytes(&pid_set, (char *)&pid_set, 0);
      sendToMotor1Queue(&pid_set);
#endif
    }
  } break;

  default: { break; }
  }
}
