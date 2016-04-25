#include "debug.h"
#include "debug_codes.h"
#include "debuginfo.h"
#include "motor1_public.h"
#include "pid.h"
#include "pid_public.h"

PID_DATA pidData;

#define PID_PERIOD_MS (16)

// Define parameters
#define EPSILON (0.01)
#define DT (PID_PERIOD_MS/1000.0)
#define MAX (100)
#define MIN (0)

#ifdef VEL
#define KP (0.000128)
#define KI (0.00001285)
#define KD (0.0)
#else 
#define KP (5.0)
#define KI (0.0)
#define KD (0.0)
#endif

int32_t pidBaseDutyCycle;

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

// Internal functions
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

static void pidTimerCallback(TimerHandle_t timer) {
  struct PID_VARIANT var;
  var.type = PID_TRIGGER;
  sendToPIDQueue(&var);
}

void PID_Initialize(void) {
  pidData.state = PID_STATE_INIT;
  pidData.pidQueue = xQueueCreate(PID_QUEUE_SIZE, sizeof(struct PID_VARIANT));
  if (pidData.pidQueue == 0) {
    errorCheck(PID_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(pidData.pidQueue, "PID Queue");

  registerEncodersCallback(PID_encoder_count_cb);

  pidData.timer = xTimerCreate("PID Timer", PID_PERIOD_MS / portTICK_PERIOD_MS, pdTRUE,
                               NULL, pidTimerCallback);
  if (pidData.timer == NULL) {
    errorCheck(PID_IDENTIFIER, __LINE__);
  }

  pidData.velocity_left = 0;
  pidData.velocity_right = 0;
}

void PID_Tasks(void) {
  switch (pidData.state) {
  case PID_STATE_INIT: {
    pidData.state = PID_STATE_RECEIVE;
    if (xTimerStart(pidData.timer, 0) != pdPASS) {
      errorCheck(PID_IDENTIFIER, __LINE__);
    }
    break;
  }
  case PID_STATE_RECEIVE: {
    struct PID_VARIANT received;
    if (xQueueReceive(pidData.pidQueue, &received, portMAX_DELAY)) {
      switch (received.type) {
      case PID_TRIGGER: {
        // PID setup
        static int32_t pre_error = 0;
        static int32_t integral = 0;
        double error;
        double derivative;
        double pid;
        
#ifdef VEL
        error = pidData.velocity_left - pidData.velocity_right + 12500;
#else
        error = pidData.displacement_right - pidData.displacement_left;
#endif
        
        // Check to see if error is too small
        if (abs(error) > EPSILON) {
          integral = integral + error * DT;
        }
        derivative = (error - pre_error) / DT;
        pid = KP * error + KI * integral + KD * derivative;
        static uint32_t mod;
        if (mod++ == 20) {
          packAndSendDebugInfo(PID_IDENTIFIER, PIDValueRecalculated, pid);
          mod = 0;
        }
        // Update error
        pre_error = error;

        int32_t atomicBaseDutyCycle =
            __sync_fetch_and_add(&pidBaseDutyCycle, 0);
        // send to Motor1Queue
        MotorCommand pid_set;
        MotorCommand_init(&pid_set);
        MotorCommand_set_direction(&pid_set, MOTOR_FORWARD);
        MotorCommand_set_mode(&pid_set, MOTOR_PID_SET);
        MotorCommand_set_dutyCycle(
            &pid_set, constrain(atomicBaseDutyCycle + pid, MAX, MIN));
        MotorCommand_to_bytes(&pid_set, (char *)&pid_set, 0);
        sendToMotor1Queue(&pid_set);

        writeToDebug(PID_RECEIVE_BYTE);

      } break;

      case ENCODER_COUNTS: {
        pidData.velocity_left = received.data.encoder_counts.velocity_left;
        pidData.velocity_right = received.data.encoder_counts.velocity_right;
        pidData.displacement_left = received.data.encoder_counts.left;
        pidData.displacement_right = received.data.encoder_counts.right;
      } break;
      default: { errorCheck(PID_IDENTIFIER, __LINE__); }
      } // switch (received.type)
    } else {
      errorCheck(PID_IDENTIFIER, __LINE__);
    }
    break;
  }
  default: { break; }
  }
}
