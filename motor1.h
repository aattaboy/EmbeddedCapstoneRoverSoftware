
#ifndef _MOTOR1_H
#define _MOTOR1_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "uart_receiver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOTOR1_QUEUE_SIZE (10)

typedef enum {
  MOTOR1_STATE_INIT = 0,
  MOTOR1_STATE_RECEIVE,
} MOTOR1_STATES;

typedef struct {
  MOTOR1_STATES state;
  QueueHandle_t motor1Queue;

} MOTOR1_DATA;

enum {
  MOTOR_TESTING,
  MOTOR_COMMAND_SET,
  MOTOR_PID_SET,
  MOTOR_FORWARD,
  MOTOR_BACKWARD,
  MOTOR_LEFT,
  MOTOR_RIGHT,
  MOTOR_STOP
};

void moveRover(uint8_t direction, uint8_t leftDuty, uint8_t rightDuty);

int Motor_Command_Callback(struct UART_RECEIVER_VARIANT *data);

void MOTOR1_Initialize(void);

void MOTOR1_Tasks(void);

#endif

#ifdef __cplusplus
}
#endif