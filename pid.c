#include "debug.h"
#include "debug_codes.h"
#include "debuginfo.h"
#include "motor1_public.h"
#include "pid.h"
#include "pid_public.h"

PID_DATA pidData;

// Define parameters
#define EPSILON (0.01)
#define DT (1.0)
#define MAX (100)
#define MIN (0)
#define KP (10.)
#define KI (0.5)
#define KD (1.)
#define BASE_DUTY_CYCLE (70)

// Public Functions

int registerPIDCallback(pid_callback_t callback) {
  if (pidData.pid_callbacks_idx != PID_CALLBACKS_VECTOR_SIZE) {
    pidData.callbacks[pidData.pid_callbacks_idx++] = callback;
    return 1;
  } else {
    return 0;
  }
}

BaseType_t sendToPIDQueue(struct PID_VARIANT *info) {
  BaseType_t retval = xQueueSendToBack(pidData.pidQueue, info, portMAX_DELAY);
  return retval;
}

BaseType_t sendToPIDQueueFromISR(struct PID_VARIANT *info,
                                 BaseType_t *higherPriorityTaskWoken) {
  BaseType_t retval =
      xQueueSendToBackFromISR(pidData.pidQueue, info, higherPriorityTaskWoken);
  return retval;
}

// Internal function
static void sendMessageToCallbacks(MotorCommand *info) {
  size_t i;
  for (i = 0; i < pidData.pid_callbacks_idx; i++) {
    pidData.callbacks[i](info);
  }
}

static int PID_encoder_count_cb(struct EncoderCounts *counts) {
  struct PID_VARIANT var;
  var.type = ENCODER_COUNTS;
  var.data.encoder_counts = *counts;
  if (pdTRUE == xQueueSendToBack(pidData.pidQueue, &var, portMAX_DELAY)) {
    return 1;
  }
  return 0;
}

void PID_Initialize(void) {
  pidData.state = PID_STATE_INIT;
  pidData.pidQueue = xQueueCreate(PID_QUEUE_SIZE, sizeof(struct PID_VARIANT));
  if (pidData.pidQueue == 0) {
    errorCheck(__FILE__, __LINE__);
  }
  registerEncodersCallback(PID_encoder_count_cb);
}

static int32_t constrain(int val, int max, int min) {
  if (val > max) {
    val = max;
    PORTE = 0x1;
  } else if (val < min) {
    val = min;
    PORTE = 0x2;
  }
  return val;
}

void PID_Tasks(void) {
  switch (pidData.state) {
  case PID_STATE_INIT: {
    pidData.state = PID_STATE_RECEIVE;
    break;
  }
  case PID_STATE_RECEIVE: {
    struct PID_VARIANT received;
    if (xQueueReceive(pidData.pidQueue, &received, portMAX_DELAY)) {
      switch (received.type) {
      case ENCODER_COUNTS: {
        // PID setup
        static int32_t pre_error = 0;
        static int32_t integral = 0;
        int32_t error;
        int32_t derivative;
        int32_t pid;

        error = received.data.encoder_counts.left -
                received.data.encoder_counts.right;

        // Check to see if error is too small
        if (abs(error) > EPSILON) {
          integral = integral + error * DT;
        }
        derivative = (error - pre_error) / DT;
        pid = KP * error + KI * integral + KD * derivative;

        // Update error
        pre_error = error;

        // send to Motor1Queue
        MotorCommand pid_set;
        MotorCommand_init(&pid_set);
        MotorCommand_set_direction(&pid_set, MOTOR_FORWARD);
        MotorCommand_set_mode(&pid_set, MOTOR_PID_SET);
        MotorCommand_set_dutyCycle(&pid_set,
                                   constrain(BASE_DUTY_CYCLE + pid, MIN, MAX));
        MotorCommand_to_bytes(&pid_set, (char *)&pid_set, 0);
        sendToMotor1Queue(&pid_set);

        MotorCommand command;
        MotorCommand_init(&command);
        MotorCommand_set_mode(&command, MOTOR_PID_SET);
        MotorCommand_set_direction(&command, MOTOR_FORWARD);
        MotorCommand_set_dutyCycle(&command, 75);
        static uint32_t seq3;
        MotorCommand_to_bytes(&command, (char *)&command, seq3++);

        writeToDebug(PID_RECEIVE_BYTE);

        sendMessageToCallbacks(&command);
      } break;
      default: { errorCheck(__FILE__, __LINE__); }
      } // switch (received.type)
    } else {
      errorCheck(__FILE__, __LINE__);
    }
    break;
  }
  default: { break; }
  }
}