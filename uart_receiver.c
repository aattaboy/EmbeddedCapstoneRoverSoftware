#include "debug.h"
#include "debug_codes.h"
#include "uart_receiver.h"
#include "uart_transmitter_public.h"

UART_RECEIVER_DATA uart_receiverData;

// Byte sequence that indicates the start of a frame

static const char frameSequence[4] = { 0x80, 0x08, 0x13, 0x55 };

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

BaseType_t sendToUartReceiverQueue(uint8_t *info) {
  BaseType_t retval = xQueueSendToBack(uart_receiverData.uartReceiverQueue,
                                       info, portMAX_DELAY);
  return retval;
}

BaseType_t sendToUartReceiverQueueFromISR(uint8_t *info,
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
      UART_RECEIVER_QUEUE_SIZE, sizeof(uint8_t));
  // If we fail to create the queue, die.
  if (uart_receiverData.uartReceiverQueue == 0) {
    errorCheck(__FILE__, __LINE__);
  }
  vQueueAddToRegistry(uart_receiverData.uartReceiverQueue, "Uart Receiver Qeueue");
}

// Consume a message from the receive queue and unpack it as a char.
// Blocks if there is no message available.
static char receive_and_unpack() {
  uint8_t received;
  if (xQueueReceive(uart_receiverData.uartReceiverQueue, &(received),
                    portMAX_DELAY)) {
    return received;
  } else {
    return 0xff;
  }
}

void UART_RECEIVER_Tasks(void) {
  switch (uart_receiverData.state) {
  case UART_RECEIVER_STATE_INIT: {
    uart_receiverData.state = UART_RECEIVER_FRAME_START_1;
    break;
  }

  // Receive the first byte of the frame delimiter word. If the received byte
  // does not match, stay in this state.
  case UART_RECEIVER_FRAME_START_1: {
    uint8_t test_char = receive_and_unpack();
    struct UART_TRANSMITTER_VARIANT var;
    var.type = TEST_CHAR;
    var.data.test_char = test_char;
    sendToUartQueue(&var);
#if 0
    if (receive_and_unpack() == frameSequence[0]) {
      uart_receiverData.state = UART_RECEIVER_FRAME_START_2;
    }
#endif
  } break;

  // Receive the second byte of the frame delimiter word. If the received byte
  // does not match, fall back to state 1
  case UART_RECEIVER_FRAME_START_2: {
    if (receive_and_unpack() == frameSequence[1]) {
      uart_receiverData.state = UART_RECEIVER_FRAME_START_3;
    } else {
      uart_receiverData.state = UART_RECEIVER_FRAME_START_1;
    }
  } break;

  // Receive the third byte of the frame delimiter word. If the received byte
  // does not match, fall back to state 1
  case UART_RECEIVER_FRAME_START_3: {
    if (receive_and_unpack() == frameSequence[2]) {
      uart_receiverData.state = UART_RECEIVER_FRAME_START_4;
    } else {
      uart_receiverData.state = UART_RECEIVER_FRAME_START_1;
    }
  } break;

  // Receive the fourth byte of the frame delimiter word. If the received byte
  // does not match, fall back to state 1
  case UART_RECEIVER_FRAME_START_4: {
    if (receive_and_unpack() == frameSequence[3]) {
      // Set our receive index to 0 so we receive a brand new message.
      uart_receiverData.receive_buf_idx = 0;
      uart_receiverData.state = UART_RECEIVER_STATE_RECEIVE;
    } else {
      uart_receiverData.state = UART_RECEIVER_FRAME_START_1;
    }
  } break;

  // We've made it through the frame delimiter sequence. Receive a full message.
  case UART_RECEIVER_STATE_RECEIVE: {
    // Populate the current byte in our receive buffer.
    uart_receiverData.receive_buf[uart_receiverData.receive_buf_idx++] =
        receive_and_unpack();

    // If we've received the correct number of bytes for a message, parse the
    // message.
    if (uart_receiverData.receive_buf_idx == sizeof(WiFlyStringWireFormat)) {
      // Proto object we're parsing into
      WiFlyStringWireFormat received_obj;

      // Sequence number
      uint32_t seq;
      static uint32_t seq_expected;

      // Parse from bytes. Also does a hash integrity check
      if (!WiFlyStringWireFormat_from_bytes(
              &received_obj, uart_receiverData.receive_buf, &seq)) {
        // If we fail our integrity check, fail.
        // TODO: notify
        break;
      }

      if (seq != seq_expected) {
        // TODO: notify
      }
      seq_expected++;

      // Switch back to receiving the first byte of the frame delimiter word.
      uart_receiverData.state = UART_RECEIVER_FRAME_START_1;

      // Write out first char to LEDs (for testing)
      PORTD = received_obj.msg[0];

      struct UART_RECEIVER_VARIANT var;
      var.type = STRING;
      var.data.string = received_obj;
      sendMessageToCallbacks(&var);
    }
  } break;

  default: { break; }
  }
}