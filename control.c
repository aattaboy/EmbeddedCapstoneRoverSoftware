#include "debug.h"
#include "debuginfo.h"
#include "control.h"
#include "motor1_public.h"
#include "system_config/default/framework/driver/tmr/drv_tmr_static.h"
#include "uart_receiver_public.h"

#include <math.h>

#define CONTROL_QUEUE_SIZE (10)

static CONTROL_DATA controlData;

int sendToControlSetPointQueue(RoverPose *pose) {
  if (pdTRUE ==
      xQueueSendToBack(controlData.controlQueueSetPoint, pose, portMAX_DELAY)) {
    return 1;
  } else {
    return 0;
  }
}

int sendToControlRoverPositionQueue(RoverPose *pose) {
  if (xQueueSendToBack(controlData.controlQueueSetPoint, pose, 0)) {
    return 1;
  } else {
    return 0;
  }
}

static int control_uart_receiver_wrapper(struct UART_RECEIVER_VARIANT *var) {
  if (var->type == CONTROL_CMD) {
    sendToControlSetPointQueue(&var->data.controlCmd);
    return 1;
  }
  return 0;
}

void CONTROL_Initialize(void) {
  controlData.state = CONTROL_STATE_INIT;

  // Create the queues
  controlData.controlQueueSetPoint =
      xQueueCreate(CONTROL_QUEUE_SIZE, sizeof(RoverPose));
  if (controlData.controlQueueSetPoint == 0) {
    errorCheck(CONTROL_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(controlData.controlQueueSetPoint,
                      "Control Queue SetPoint");

  controlData.controlQueueRoverPosition =
      xQueueCreate(CONTROL_QUEUE_SIZE, sizeof(RoverPose));
  if (controlData.controlQueueRoverPosition == 0) {
    errorCheck(CONTROL_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(controlData.controlQueueRoverPosition,
                      "Control Queue RoverPosition");

  registerPoseCallback(sendToControlRoverPositionQueue);
  registerUartReceiverCallback(control_uart_receiver_wrapper);
}

int registerControlCallback(control_callback_t callback) {
  if (controlData.control_callbacks_idx < CONTROL_CALLBACKS_VECTOR_SIZE) {
    controlData.callbacks[controlData.control_callbacks_idx++] = callback;
    return 1;
  } else {
    return 0;
  }
}

static void sendToControlCallbacks(MotorCommand *motorCommand) {
  size_t i;
  for (i = 0; i < controlData.control_callbacks_idx; i++) {
    controlData.callbacks[i](motorCommand);
  }
}

static int32_t constrain(int val, int max, int min) {
  if (val > max) {
    val = max;
  } else if (val < min) {
    val = min;
  }
  return val;
}

typedef enum {
  NONE,
  MOVE,
  ROTATE_AND_MOVE,
} NECESSARY_STATES;

static NECESSARY_STATES figure_necessary_states(RoverPose *setpoint,
                                                RoverPose *current) {
  int32_t delta_x =
      RoverPose_xPosition(setpoint) - RoverPose_xPosition(current);
  int32_t delta_y =
      RoverPose_yPosition(setpoint) - RoverPose_yPosition(current);

  if (delta_x == 0 && delta_y == 0) {
    return NONE;
  } else {
    int32_t target_angle = atan2(delta_x, delta_y);
    if (target_angle < 0)
      target_angle += 360;

    if (target_angle != RoverPose_yaw(current)) {
      return ROTATE_AND_MOVE;
    } else {
      return MOVE;
    }
  }
}

void CONTROL_Tasks(void) {
  switch (controlData.state) {
  case CONTROL_STATE_INIT: {
    controlData.state = CONTROL_STATE_RECEIVE;
  } break;

  case CONTROL_STATE_RECEIVE: {
    if (xQueueReceive(controlData.controlQueueSetPoint, &controlData.setPoint,
                      portMAX_DELAY)) {
      uint32_t seq_out;
      if (!RoverPose_from_bytes(&controlData.setPoint,
                                (char *)&controlData.setPoint, &seq_out)) {
        break;
      }
    }
    xQueueReset(controlData.controlQueueRoverPosition);
  } break;

  case CONTROL_STATE_ROTATE: {
    // TODO: issue rotate cmd

  } break;

  case CONTROL_STATE_MOVE: {
    // TODO: issue move cmd

  } break;

  case CONTROL_STATE_STOP: {
    // TODO: issue stop cmd
    controlData.state = CONTROL_STATE_RECEIVE;
  } break;

  default: { errorCheck(CONTROL_IDENTIFIER, __LINE__); } break;
  }
}
