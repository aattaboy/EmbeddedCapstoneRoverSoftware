#ifndef _UART_TRANSMITTER_H
#define _UART_TRANSMITTER_H

#include <FreeRTOS.h>
#include <queue.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <timers.h>
#include "system_config.h"
#include "system_definitions.h"
#include "uart_transmitter_public.h"
#ifdef __cplusplus

extern "C" {

#endif

// Application states
typedef enum {
  /* Application's state machine's initial state. */
  UART_TRANSMITTER_STATE_INIT = 0,
  UART_TRANSMITTER_STATE_SEND = 1,
  UART_TRANSMITTER_STATE_RECEIVE = 2,
  UART_TRANSMITTER_STATE_BUFFERFULL = 3,

  /* TODO: Define states used by the application state machine. */

} UART_TRANSMITTER_STATES;

// Application Data
typedef struct {
  /* The application's current state */
  UART_TRANSMITTER_STATES state;
  QueueHandle_t xQueue1;
  QueueHandle_t xQueue2;

  char transmit_str[sizeof(struct UART_TRANSMITTER_VARIANT) + 4];
  size_t transmit_idx;
  size_t transmit_size;

  TimerHandle_t profile_timer;

} UART_TRANSMITTER_DATA;

void UART_TRANSMITTER_Initialize(void);

void UART_TRANSMITTER_Tasks(void);

#ifdef __cplusplus
}
#endif

#endif