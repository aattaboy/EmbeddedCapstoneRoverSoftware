#ifndef _UART_RECEIVER_H
#define _UART_RECEIVER_H

#include <FreeRTOS.h>
#include <generated/UartRxData.pbo.h>
#include <generated/WiFlyStringWireFormat.pbo.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "uart_receiver_public.h"

#ifdef __cplusplus

extern "C" {

#endif

typedef enum {
  UART_RECEIVER_STATE_INIT = 0,
  UART_RECEIVER_STATE_RX,
} UART_RECEIVER_STATES;

#define UART_RECEIVER_CALLBACKS_VECTOR_SIZE (10)
#define UART_RECEIVER_QUEUE_SIZE (10)

typedef struct {
  UART_RECEIVER_STATES state;

  // Queue
  QueueHandle_t uartReceiverQueue;

  // Callbacks vector members
  size_t uart_receiver_callbacks_idx;
  uart_receiver_callback_t callbacks[UART_RECEIVER_CALLBACKS_VECTOR_SIZE];
} UART_RECEIVER_DATA;

void UART_RECEIVER_Initialize(void);

void UART_RECEIVER_Tasks(void);

#endif

#ifdef __cplusplus
}
#endif
