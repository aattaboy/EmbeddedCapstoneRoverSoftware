#include "debug.h"
#include "debug_codes.h"
#include "uart_receiver.h"
#include "uart_transmitter_public.h"

UART_RECEIVER_DATA uart_receiverData;

// Public functions

int registerUartReceiverCallback(uart_receiver_callback_t callback) {
  if (uart_receiverData.uart_receiver_callbacks_idx !=
      UART_RECEIVER_CALLBACKS_VECTOR_SIZE) {
    uart_receiverData
        .callbacks[uart_receiverData.uart_receiver_callbacks_idx++] = callback;
    return 1;
  } else {
    return 0;
  }
}

BaseType_t sendToUartReceiverQueue(struct UART_RECEIVER_VARIANT *info) {
  BaseType_t retval = xQueueSendToBack(uart_receiverData.uartReceiverQueue,
                                       info, portMAX_DELAY);
  return retval;
}

BaseType_t sendToUartReceiverQueueFromISR(struct UART_RECEIVER_VARIANT *info,
                                          BaseType_t *higherPriorityTaskWoken) {
  BaseType_t retval = xQueueSendToBackFromISR(
      uart_receiverData.uartReceiverQueue, info, higherPriorityTaskWoken);
  return retval;
}

// Internal functions

static void sendMessageToCallbacks(struct UART_RECEIVER_VARIANT *info) {
  size_t i;
  for (i = 0; i < uart_receiverData.uart_receiver_callbacks_idx; i++) {
    uart_receiverData.callbacks[i](info);
  }
}

void UART_RECEIVER_Initialize(void) {
  uart_receiverData.state = UART_RECEIVER_STATE_INIT;
  // Initialize callback vector
  uart_receiverData.uart_receiver_callbacks_idx = 0;
  // Initialize input queue
  uart_receiverData.uartReceiverQueue = xQueueCreate(
      UART_RECEIVER_QUEUE_SIZE, sizeof(struct UART_RECEIVER_VARIANT));
  // If we fail to create the queue, die.
  if (uart_receiverData.uartReceiverQueue == 0) {
    errorCheck(__FILE__, __LINE__);
  }
  vQueueAddToRegistry(uart_receiverData.uartReceiverQueue,
                      "Uart Receiver Qeueue");
}

void UART_RECEIVER_Tasks(void) {
  switch (uart_receiverData.state) {
  case UART_RECEIVER_STATE_INIT: {
    uart_receiverData.state = UART_RECEIVER_STATE_RX;
  } break;

  case UART_RECEIVER_STATE_RX: {
    struct UART_RECEIVER_VARIANT var;
    if (xQueueReceive(uart_receiverData.uartReceiverQueue, &var,
                      portMAX_DELAY)) {
      switch (var.type) {
      case MOTOR_MESSAGE: {
        MotorCommand cmd;
        uint32_t seq_out;
        static uint32_t seq_expected;
        if (!MotorCommand_from_bytes(&cmd, (char *)&var.data.motorMessage,
                                     &seq_out)) {
          errorCheck(__FILE__, __LINE__);
        }

        if (seq_out != seq_expected) {
          // TODO: notify
        }

        sendMessageToCallbacks(&var);
      } break;
      default:
        break;
      }
    }
  } break;

  default: { break; }
  }
}
