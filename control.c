#include "debug.h"
#include "debuginfo.h"
#include "control.h"
#include "motor1_public.h"
#include "system_config/default/framework/driver/tmr/drv_tmr_static.h"

static CONTROL_DATA controlData;
static RoverPose setPoint;
static RoverPose roverPosition;
static MotorCommand motorCommand;

static int16_t yawDiff;
static int16_t baseYaw;
static int32_t xDiff;
static int32_t yDiff;

#define CONTROL_QUEUE_SIZE (10)

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

void CONTROL_Initialize(void) {
  controlData.state = CONTROL_STATE_INIT;

  // Create the queues
  controlData.controlQueueSetPoint =
      xQueueCreate(CONTROL_QUEUE_SIZE, sizeof(setPoint));
  if (controlData.controlQueueSetPoint == 0) {
    errorCheck(CONTROL_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(controlData.controlQueueSetPoint,
                      "Control Queue SetPoint");

  controlData.controlQueueRoverPosition =
      xQueueCreate(CONTROL_QUEUE_SIZE, sizeof(roverPosition));
  if (controlData.controlQueueRoverPosition == 0) {
    errorCheck(CONTROL_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(controlData.controlQueueRoverPosition,
                      "Control Queue RoverPosition");

  controlData.control_callbacks_idx = 0;

  // Initialize the motorCommand
  MotorCommand_init(&motorCommand);

  // Initialize the differences
  yawDiff = 0;
  baseYaw = 0;
  xDiff = 0;
  yDiff = 0;

  // Initializing each position command
  RoverPose_init(&setPoint);
  RoverPose_set_xPosition(&setPoint, 0);
  RoverPose_set_yPosition(&setPoint, 0);
  RoverPose_set_yaw(&setPoint, 0);

  RoverPose_init(&roverPosition);
  RoverPose_set_xPosition(&roverPosition, 0);
  RoverPose_set_yPosition(&roverPosition, 0);
  RoverPose_set_yaw(&roverPosition, 0);

  registerPoseCallback(sendToControlRoverPositionQueue);
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

void CONTROL_Tasks(void) {
  switch (controlData.state) {
  case CONTROL_STATE_INIT: {
    controlData.state = CONTROL_STATE_RECEIVE;
  } break;

  case CONTROL_STATE_RECEIVE: {
    RoverPose received;
    if (xQueueReceive(controlData.controlQueueSetPoint, &received,
                      portMAX_DELAY)) {
      // Update value of setPoint
      RoverPose_set_xPosition(&setPoint, RoverPose_xPosition(&received));
      RoverPose_set_yPosition(&setPoint, RoverPose_yPosition(&received));
      RoverPose_set_yaw(&setPoint, RoverPose_yaw(&received));

      // Calculate direction
      baseYaw = RoverPose_yaw(&roverPosition);
      xDiff =
          RoverPose_xPosition(&setPoint) - RoverPose_xPosition(&roverPosition);
      yDiff =
          RoverPose_yPosition(&setPoint) - RoverPose_yPosition(&roverPosition);

      if (baseYaw == 0) {
        if ((xDiff == 0) && (yDiff > 0)) {
          controlData.state = CONTROL_STATE_FORWARD;
        } else if ((xDiff > 0) && (yDiff == 0)) {
          controlData.state = CONTROL_STATE_RIGHT90;
        } else if ((xDiff < 0) && (yDiff == 0)) {
          controlData.state = CONTROL_STATE_LEFT90;
        } else if ((xDiff == 0) && (yDiff < 0)) {
          controlData.state = CONTROL_STATE_RIGHT180;
        } else {
          errorCheck(CONTROL_IDENTIFIER, __LINE__);
        }
      } else if (baseYaw == 90) {
        if ((xDiff > 0) && (yDiff == 0)) {
          controlData.state = CONTROL_STATE_FORWARD;
        } else if ((xDiff == 0) && (yDiff < 0)) {
          controlData.state = CONTROL_STATE_RIGHT90;
        } else if ((xDiff == 0) && (yDiff > 0)) {
          controlData.state = CONTROL_STATE_LEFT90;
        } else if ((xDiff < 0) && (yDiff == 0)) {
          controlData.state = CONTROL_STATE_RIGHT180;
        } else {
          errorCheck(CONTROL_IDENTIFIER, __LINE__);
        }
      } else if (baseYaw == 180) {
        if ((xDiff == 0) && (yDiff < 0)) {
          controlData.state = CONTROL_STATE_FORWARD;
        } else if ((xDiff < 0) && (yDiff == 0)) {
          controlData.state = CONTROL_STATE_RIGHT90;
        } else if ((xDiff > 0) && (yDiff == 0)) {
          controlData.state = CONTROL_STATE_LEFT90;
        } else if ((xDiff == 0) && (yDiff > 0)) {
          controlData.state = CONTROL_STATE_RIGHT180;
        } else {
          errorCheck(CONTROL_IDENTIFIER, __LINE__);
        }
      } else if (baseYaw == 270) {
        if ((xDiff < 0) && (yDiff == 0)) {
          controlData.state = CONTROL_STATE_FORWARD;
        } else if ((xDiff == 0) && (yDiff > 0)) {
          controlData.state = CONTROL_STATE_RIGHT90;
        } else if ((xDiff == 0) && (yDiff < 0)) {
          controlData.state = CONTROL_STATE_LEFT90;
        } else if ((xDiff > 0) && (yDiff == 0)) {
          controlData.state = CONTROL_STATE_RIGHT180;
        } else {
          errorCheck(CONTROL_IDENTIFIER, __LINE__);
        }
      } else {
        errorCheck(CONTROL_IDENTIFIER, __LINE__);
      }
    }
    xQueueReset(controlData.controlQueueRoverPosition);
  } break;

  case CONTROL_STATE_FORWARD: {
    RoverPose received;
    if (xQueueReceive(controlData.controlQueueRoverPosition, &received,
                      portMAX_DELAY)) {
      // Update value of roverPosition
      RoverPose_set_xPosition(&roverPosition, RoverPose_xPosition(&received));
      RoverPose_set_yPosition(&roverPosition, RoverPose_yPosition(&received));
      RoverPose_set_yaw(&roverPosition, RoverPose_yaw(&received));

      xDiff =
          RoverPose_xPosition(&setPoint) - RoverPose_xPosition(&roverPosition);
      yDiff =
          RoverPose_yPosition(&setPoint) - RoverPose_yPosition(&roverPosition);

      if (baseYaw == 0) {
        if (yDiff == 0) {
          controlData.state = CONTROL_STATE_STOP;
        } else if (yDiff > 0) {
          MotorCommand_set_direction(&motorCommand, MOTOR_FORWARD);
          MotorCommand_set_dutyCycle(&motorCommand,
                                     constrain(abs(yDiff) * 20, 100, 30));
          MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
          sendToControlCallbacks(&motorCommand);
        } else {
          MotorCommand_set_direction(&motorCommand, MOTOR_BACKWARD);
          MotorCommand_set_dutyCycle(&motorCommand,
                                     constrain(abs(yDiff) * 20, 100, 30));
          MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
          sendToControlCallbacks(&motorCommand);
        }
      } else if (baseYaw == 90) {
        if (xDiff == 0) {
          controlData.state = CONTROL_STATE_STOP;
        } else if (xDiff > 0) {
          MotorCommand_set_direction(&motorCommand, MOTOR_FORWARD);
          MotorCommand_set_dutyCycle(&motorCommand,
                                     constrain(abs(xDiff) * 20, 100, 30));
          MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
          sendToControlCallbacks(&motorCommand);
        } else {
          MotorCommand_set_direction(&motorCommand, MOTOR_BACKWARD);
          MotorCommand_set_dutyCycle(&motorCommand,
                                     constrain(abs(yDiff) * 20, 100, 30));
          MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
          sendToControlCallbacks(&motorCommand);
        }
      } else if (baseYaw == 180) {
        if (yDiff == 0) {
          controlData.state = CONTROL_STATE_STOP;
        } else if (yDiff > 0) {
          MotorCommand_set_direction(&motorCommand, MOTOR_BACKWARD);
          MotorCommand_set_dutyCycle(&motorCommand,
                                     constrain(abs(yDiff) * 20, 100, 30));
          MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
          sendToControlCallbacks(&motorCommand);
        } else {
          MotorCommand_set_direction(&motorCommand, MOTOR_FORWARD);
          MotorCommand_set_dutyCycle(&motorCommand,
                                     constrain(abs(yDiff) * 20, 100, 30));
          MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
          sendToControlCallbacks(&motorCommand);
        }
      } else if (baseYaw == 270) {
        if (xDiff == 0) {
          controlData.state = CONTROL_STATE_STOP;
        } else if (xDiff > 0) {
          MotorCommand_set_direction(&motorCommand, MOTOR_BACKWARD);
          MotorCommand_set_dutyCycle(&motorCommand,
                                     constrain(abs(xDiff) * 20, 100, 30));
          MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
          sendToControlCallbacks(&motorCommand);
        } else {
          MotorCommand_set_direction(&motorCommand, MOTOR_FORWARD);
          MotorCommand_set_dutyCycle(&motorCommand,
                                     constrain(abs(xDiff) * 20, 100, 30));
          MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
          sendToControlCallbacks(&motorCommand);
        }
      } else {
        errorCheck(CONTROL_IDENTIFIER, __LINE__);
      }
    }
  } break;

  case CONTROL_STATE_LEFT90: {
    RoverPose received;
    if (xQueueReceive(controlData.controlQueueRoverPosition, &received,
                      portMAX_DELAY)) {
      // Update value of roverPosition
      RoverPose_set_xPosition(&roverPosition, RoverPose_xPosition(&received));
      RoverPose_set_yPosition(&roverPosition, RoverPose_yPosition(&received));
      RoverPose_set_yaw(&roverPosition, RoverPose_yaw(&received));

      int16_t currentYaw = RoverPose_yaw(&roverPosition);

      if ((currentYaw > 180) && (baseYaw == 0 || baseYaw == 90)) {
        currentYaw = currentYaw - 360;
      }

      yawDiff = baseYaw - 90;

      if (yawDiff - currentYaw == 0) {
        baseYaw = currentYaw;
        controlData.state = CONTROL_STATE_FORWARD;
      } else if (yawDiff - currentYaw < 0) {
        MotorCommand_set_direction(&motorCommand, MOTOR_LEFT);
        MotorCommand_set_dutyCycle(&motorCommand,
                                   constrain(abs(yawDiff) * 10, 100, 30));
        MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
        sendToControlCallbacks(&motorCommand);
      } else {
        MotorCommand_set_direction(&motorCommand, MOTOR_RIGHT);
        MotorCommand_set_dutyCycle(&motorCommand,
                                   constrain(abs(yawDiff) * 10, 100, 30));
        MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
        sendToControlCallbacks(&motorCommand);
      }
    }
  } break;

  case CONTROL_STATE_RIGHT90: {
    RoverPose received;
    if (xQueueReceive(controlData.controlQueueRoverPosition, &received,
                      portMAX_DELAY)) {
      // Update value of roverPosition
      RoverPose_set_xPosition(&roverPosition, RoverPose_xPosition(&received));
      RoverPose_set_yPosition(&roverPosition, RoverPose_yPosition(&received));
      RoverPose_set_yaw(&roverPosition, RoverPose_yaw(&received));

      int16_t currentYaw = RoverPose_yaw(&roverPosition);

      if ((currentYaw >= 0) && (currentYaw < 90) && (baseYaw == 270)) {
        currentYaw = currentYaw + 360;
      }

      yawDiff = baseYaw + 90;

      if (yawDiff - currentYaw == 0) {
        baseYaw = currentYaw;
        controlData.state = CONTROL_STATE_FORWARD;
      } else if (yawDiff - currentYaw < 0) {
        MotorCommand_set_direction(&motorCommand, MOTOR_LEFT);
        MotorCommand_set_dutyCycle(&motorCommand,
                                   constrain(abs(yawDiff) * 10, 100, 30));
        MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
        sendToControlCallbacks(&motorCommand);
      } else {
        MotorCommand_set_direction(&motorCommand, MOTOR_RIGHT);
        MotorCommand_set_dutyCycle(&motorCommand,
                                   constrain(abs(yawDiff) * 10, 100, 30));
        MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
        sendToControlCallbacks(&motorCommand);
      }
    }
  } break;

  case CONTROL_STATE_RIGHT180: {
    RoverPose received;
    if (xQueueReceive(controlData.controlQueueRoverPosition, &received,
                      portMAX_DELAY)) {
      // Update value of roverPosition
      RoverPose_set_xPosition(&roverPosition, RoverPose_xPosition(&received));
      RoverPose_set_yPosition(&roverPosition, RoverPose_yPosition(&received));
      RoverPose_set_yaw(&roverPosition, RoverPose_yaw(&received));

      int16_t currentYaw = RoverPose_yaw(&roverPosition);

      if ((currentYaw >= 0) && (currentYaw < 90) && (baseYaw == 180)) {
        currentYaw = currentYaw + 360;
      }
      if ((currentYaw >= 270) && (baseYaw == 270)) {
        currentYaw = currentYaw - 360;
      }

      if (baseYaw == 270) {
        yawDiff = 90;
      } else {
        yawDiff = baseYaw + 180;
      }

      if (yawDiff - currentYaw == 0) {
        baseYaw = currentYaw;
        controlData.state = CONTROL_STATE_FORWARD;
      } else if (yawDiff - currentYaw < 0) {
        MotorCommand_set_direction(&motorCommand, MOTOR_LEFT);
        MotorCommand_set_dutyCycle(&motorCommand,
                                   constrain(abs(yawDiff) * 10, 100, 30));
        MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
        sendToControlCallbacks(&motorCommand);
      } else {
        MotorCommand_set_direction(&motorCommand, MOTOR_RIGHT);
        MotorCommand_set_dutyCycle(&motorCommand,
                                   constrain(abs(yawDiff) * 10, 100, 30));
        MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
        sendToControlCallbacks(&motorCommand);
      }
    }
  } break;

  case CONTROL_STATE_STOP: {
    MotorCommand_set_direction(&motorCommand, MOTOR_STOP);
    MotorCommand_set_dutyCycle(&motorCommand, 0);
    MotorCommand_set_mode(&motorCommand, MOTOR_COMMAND_SET);
    sendToControlCallbacks(&motorCommand);
    controlData.state = CONTROL_STATE_RECEIVE;
  } break;

  default: { break; }
  }
}
