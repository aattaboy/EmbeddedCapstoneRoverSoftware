#include "debug.h"
#include "debuginfo.h"
#include "debug_codes.h"
#include "generated/MotorCommand.pbo.h"
#include "debuginfo.h"
#include "motor1.h"
#include "motor1_public.h"
#include "peripheral/oc/plib_oc.h"

#define MOTORPERIOD (500)
MOTOR1_DATA motor1Data;

uint8_t leftDutyCycle = 0;
uint8_t rightDutyCycle = 0;

// Public Functions
BaseType_t sendToMotor1Queue(MotorCommand *info) {
  BaseType_t retval =
      xQueueSendToBack(motor1Data.motor1Queue, info, portMAX_DELAY);
  return retval;
}

BaseType_t sendToMotor1QueueFromISR(MotorCommand *info,
                                    BaseType_t *higherPriorityTaskWoken) {
  BaseType_t retval = xQueueSendToBackFromISR(motor1Data.motor1Queue, info,
                                              higherPriorityTaskWoken);
  return retval;
}

int Motor_Command_Callback(struct UART_RECEIVER_VARIANT *info) {
  if (info->type == MOTOR_MESSAGE)
    sendToMotor1Queue(&info->data.motorMessage);
  return 0;
}

static int control_callback_wrapper(MotorCommand *cmd) {
  sendToMotor1Queue(cmd);
  return 0;
}

// Internal functions
void MOTOR1_Initialize(void) {
  motor1Data.state = MOTOR1_STATE_INIT;
  motor1Data.motor1Queue =
      xQueueCreate(MOTOR1_QUEUE_SIZE, sizeof(MotorCommand));
  if (motor1Data.motor1Queue == 0) {
    errorCheck(MOTOR1_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(motor1Data.motor1Queue, "Motor 1 Queue");

  // Receiver Callback
  registerUartReceiverCallback(Motor_Command_Callback);

  // Motor Control
  DRV_TMR2_Start();
  DRV_OC0_Enable();
  DRV_OC1_Enable();

  PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
  PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);

  PLIB_TMR_Period16BitSet(TMR_ID_2, MOTORPERIOD);

  PLIB_OC_PulseWidth16BitSet(OC_ID_1, 0);
  PLIB_OC_PulseWidth16BitSet(OC_ID_2, 0);

  registerControlCallback(control_callback_wrapper);
}

void moveRover(uint8_t direction, uint8_t leftDuty, uint8_t rightDuty) {
  DRV_OC0_Enable();
  DRV_OC1_Enable();

  if (direction == MOTOR_FORWARD) {
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
  } else if (direction == MOTOR_BACKWARD) {
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
  } else if (direction == MOTOR_LEFT) {
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
  } else if (direction == MOTOR_RIGHT) {
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
  } else if (direction == MOTOR_STOP) {
    leftDutyCycle = 0;
    rightDutyCycle = 0;
  } else {
    // TODO: figure out how to persist direction cmds
    errorCheck(MOTOR1_IDENTIFIER, __LINE__);
  }

  int32_t tempLeft = leftDuty * MOTORPERIOD / 100;
  int32_t tempRight = rightDuty * MOTORPERIOD / 100;

  if (leftDuty >= 15)
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, tempLeft);  // left
  else
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, 0);
  
  if (rightDuty >= 15)
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, tempRight); // right
  else
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, 0);
}

void MOTOR1_Tasks(void) {
  switch (motor1Data.state) {
  case MOTOR1_STATE_INIT: {
    motor1Data.state = MOTOR1_STATE_RECEIVE;
    break;
  }
  case MOTOR1_STATE_RECEIVE: {
    MotorCommand received;
    uint32_t seq;
    static uint32_t seq_expected;
    if (xQueueReceive(motor1Data.motor1Queue, &received, portMAX_DELAY)) {
      if (!MotorCommand_from_bytes(&received, (void *)&received, &seq)) {
        warning(MOTOR1_IDENTIFIER, __LINE__);
        break;
      }
      if (seq != seq_expected) {
        warning(MOTOR1_IDENTIFIER, __LINE__);
      }
      seq_expected++;
      static uint8_t old_direction = MOTOR_FORWARD;

      if (MotorCommand_mode(&received) == MOTOR_TESTING) {
        moveRover(MotorCommand_direction(&received),
                  MotorCommand_dutyCycle(&received),
                  MotorCommand_dutyCycle(&received));
      } else if (MotorCommand_mode(&received) == MOTOR_COMMAND_SET) {
        leftDutyCycle = MotorCommand_dutyCycle(&received);
        rightDutyCycle = MotorCommand_dutyCycle(&received);
        moveRover(MotorCommand_direction(&received), leftDutyCycle,
                  rightDutyCycle);
        pidBaseDutyCycle = leftDutyCycle;
        old_direction = MotorCommand_direction(&received);
        packAndSendDebugInfo(MOTOR1_IDENTIFIER, Motor1CommandDutyCycleSet,
                             MotorCommand_dutyCycle(&received));
      } else if (MotorCommand_mode(&received) == MOTOR_PID_SET) {
        rightDutyCycle = MotorCommand_dutyCycle(&received);
        moveRover(old_direction, leftDutyCycle, rightDutyCycle);
        packAndSendDebugInfo(MOTOR1_IDENTIFIER, Motor1PIDDutyCycleSet,
                             MotorCommand_dutyCycle(&received));
      } else {
        errorCheck(MOTOR1_IDENTIFIER, __LINE__);
      }

      writeToDebug(MOTOR1_RECEIVE_BYTE);

    } else {
      errorCheck(MOTOR1_IDENTIFIER, __LINE__);
    }
    break;
  }
  default: { errorCheck(MOTOR1_IDENTIFIER, __LINE__); }
  }
}
