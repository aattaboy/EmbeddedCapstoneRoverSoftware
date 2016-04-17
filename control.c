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
  if (xQueueSendToBack(controlData.controlQueueRoverPosition, pose, 0)) {
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

static int32_t positive_modulo(int32_t i, int32_t n) { return (i % n + n) % n; }

// Use fancy modulo arithmetic to give a delta between where we are and where we
// want to be
static int32_t calculate_mod_yaw_diff(int32_t target, int32_t current) {
  int mod = positive_modulo(target - current, 360);
  if (mod > 360 / 2) {
    mod = mod - 360;
  }
  return mod;
}

static CONTROL_STATES figure_necessary_states(RoverPose *setpoint,
                                             RoverPose *current) {
  int32_t delta_x =
      RoverPose_xPosition(setpoint) - RoverPose_xPosition(current);
  int32_t delta_y =
      RoverPose_yPosition(setpoint) - RoverPose_yPosition(current);

  if (abs(delta_x) < 5 && abs(delta_y) < 5) {
    return CONTROL_STATE_STOP;
  } else {
    int32_t target_angle = atan2(delta_x, delta_y)*180.0/3.14159;
    if (target_angle < 0)
      target_angle += 360;

    controlData.target_yaw = target_angle; // Recalculating this is expensive

    if (abs(calculate_mod_yaw_diff(target_angle, RoverPose_yaw(current))) > 2) {
      return CONTROL_STATE_ROTATE;
    } else {
      return CONTROL_STATE_MOVE;
    }
  }
}

static void sendMotorCommand(uint8_t direction, uint8_t dutyCycle) {
  MotorCommand motorCommand;
  MotorCommand_init(&motorCommand);
  MotorCommand_set_direction(&motorCommand, direction);
  MotorCommand_set_dutyCycle(&motorCommand, constrain(dutyCycle, 70, 30));
  MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
  MotorCommand_to_bytes(&motorCommand, (char*)&motorCommand, 0);
  sendToControlCallbacks(&motorCommand);
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
      sendMotorCommand(MOTOR_FORWARD, 50);
      controlData.state = CONTROL_STATE_RECEIVE_ROVER_POSITION;
    }
  } break;

  case CONTROL_STATE_RECEIVE_ROVER_POSITION: {
    if (xQueueReceive(controlData.controlQueueRoverPosition,
                      &controlData.currentPosition, portMAX_DELAY)) {
      //uint32_t seq_out;
      //if (!RoverPose_from_bytes(&controlData.currentPosition,
      //                          (char *)&controlData.currentPosition,
      //                          &seq_out)) {
      //  break;
      //}
      controlData.state = figure_necessary_states(&controlData.setPoint,
                                                  &controlData.currentPosition);
    }
  } break;

  case CONTROL_STATE_ROTATE: {
    int32_t yaw_displacement = calculate_mod_yaw_diff(
        controlData.target_yaw, RoverPose_yaw(&controlData.currentPosition));
    if (yaw_displacement > 0) {
      sendMotorCommand(MOTOR_RIGHT, 50 + abs(yaw_displacement));
    } else {
      sendMotorCommand(MOTOR_LEFT, 50+ abs(yaw_displacement));
    }

    controlData.state = CONTROL_STATE_RECEIVE_ROVER_POSITION;
  } break;

  case CONTROL_STATE_MOVE: {
    sendMotorCommand(MOTOR_FORWARD, 50);
    
    controlData.state = CONTROL_STATE_RECEIVE_ROVER_POSITION;
  } break;

  case CONTROL_STATE_STOP: {
    MotorCommand motorCommand;
    MotorCommand_init(&motorCommand);
    MotorCommand_set_direction(&motorCommand, MOTOR_FORWARD);
    MotorCommand_set_dutyCycle(&motorCommand, 0);
    MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
    MotorCommand_to_bytes(&motorCommand, (char*)&motorCommand, 0);
    sendToControlCallbacks(&motorCommand);
    controlData.state = CONTROL_STATE_RECEIVE;
  } break;

  default: { errorCheck(CONTROL_IDENTIFIER, __LINE__); } break;
  }
}
