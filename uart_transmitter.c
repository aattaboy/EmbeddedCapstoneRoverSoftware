/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    uart_transmitter.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

#include "debug.h"
#include "system_config/default/framework/driver/usart/drv_usart_static.h"
#include "uart_transmitter.h"
#include "debug_codes.h"

#define QSIZE 10
UART_TRANSMITTER_DATA uart_transmitterData;

bool sendToUartQueue(struct UART_TRANSMITTER_VARIANT *var) {
  if (errQUEUE_FULL == xQueueSendToBack(uart_transmitterData.xQueue1, var,
                                        50 / portTICK_PERIOD_MS)) {
    return false;
  }
  return true;
}

bool sendToUartQueueFromISR(struct UART_TRANSMITTER_VARIANT *var,
                            BaseType_t *higherPriorityTaskWoken) {
  if (errQUEUE_FULL == xQueueSendToBackFromISR(uart_transmitterData.xQueue1,
                                               var, higherPriorityTaskWoken)) {
    return false;
  }
  return true;
}

bool sendToFullBufferQueueFromISR(char var,
                                  BaseType_t *higherPriorityTaskWoken) {
  if (errQUEUE_FULL == xQueueSendToBackFromISR(uart_transmitterData.xQueue2,
                                               &var, higherPriorityTaskWoken)) {
    return false;
  }
  return true;
}

/*******************************************************************************
  Function:
    void UART_TRANSMITTER_Initialize ( void )

  Remarks:
    See prototype in uart_transmitter.h.
 */

void UART_TRANSMITTER_Initialize(void) {
  /* Place the App state machine in its initial state. */
  uart_transmitterData.state = UART_TRANSMITTER_STATE_INIT;

  uart_transmitterData.xQueue1 =
      xQueueCreate(QSIZE, sizeof(struct UART_TRANSMITTER_VARIANT));
  uart_transmitterData.xQueue2 = xQueueCreate(QSIZE, sizeof(char));

  if (uart_transmitterData.xQueue1 == 0) {
    errorCheck(UART_TX_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(uart_transmitterData.xQueue1,
                      "UART Transmitter RX Queue");
  if (uart_transmitterData.xQueue2 == 0) {
    errorCheck(UART_TX_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(uart_transmitterData.xQueue2,
                      "UART Transmitter Full Queue");
}

/******************************************************************************
  Function:
    void UART_TRANSMITTER_Tasks ( void )

  Remarks:
    See prototype in uart_transmitter.h.
 */

void UART_TRANSMITTER_Tasks(void) {

  /* Check the application's current state. */
  switch (uart_transmitterData.state) {
  /* Application's initial state. */
  case UART_TRANSMITTER_STATE_INIT: {
    uart_transmitterData.state = UART_TRANSMITTER_STATE_RECEIVE;
    break;
  }
  case UART_TRANSMITTER_STATE_RECEIVE: {
    struct UART_TRANSMITTER_VARIANT receivedMessage;
    if (uart_transmitterData.xQueue1 != 0) {
      // Receive a message on the created queue.
      if (xQueueReceive(uart_transmitterData.xQueue1, &(receivedMessage),
                        portMAX_DELAY)) {
        uart_transmitterData.transmit_idx = 0;
        uart_transmitterData.transmit_str[0] = 0x13;
        uart_transmitterData.transmit_str[1] = 0x37;
        uart_transmitterData.transmit_str[2] = 0x80;
        uart_transmitterData.transmit_str[3] = 0x08;
        if (receivedMessage.type == RSSI_PAIR) {
          uart_transmitterData.transmit_str[4] = 0x00;
          memcpy(uart_transmitterData.transmit_str + 5, &receivedMessage,
                 sizeof(receivedMessage));
          uart_transmitterData.transmit_size =
              sizeof(struct UART_TRANSMITTER_VARIANT) + 5;
        } else {
          struct UART_TRANSMITTER_VARIANT_WIRE var_wire;
          var_wire.type = receivedMessage.type;
          switch (var_wire.type) {
          case DEBUG_INFO:
            memcpy(&var_wire.data.debug_info, &receivedMessage.data.debug_info,
                   sizeof(DebugInfo));
            break;
          case TEST_CHAR:
            memcpy(&var_wire.data.test_char, &receivedMessage.data.test_char,
                   sizeof(uint8_t));
            break;
          default:
            break;
          }
          uart_transmitterData.transmit_str[4] = 0x01;
          memcpy(uart_transmitterData.transmit_str + 5, &var_wire,
                 sizeof(struct UART_TRANSMITTER_VARIANT_WIRE));
          uart_transmitterData.transmit_size =
              sizeof(struct UART_TRANSMITTER_VARIANT_WIRE) + 5;
        }
        uart_transmitterData.state = UART_TRANSMITTER_STATE_SEND;
      }
    }
  } break;
  case UART_TRANSMITTER_STATE_SEND: {
    // If buffer is full -> go to buffer full state
    if (PLIB_USART_TransmitterBufferIsFull(USART_ID_1)) {
      PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
      uart_transmitterData.state = UART_TRANSMITTER_STATE_BUFFERFULL;
    } else {
      PLIB_USART_TransmitterByteSend(
          USART_ID_1,
          uart_transmitterData.transmit_str[uart_transmitterData.transmit_idx]);
      uart_transmitterData.transmit_idx++;
      writeToDebug(TRANSMITTER_WRITE_TO_UART_BYTE);
    }
    // If done sending message -> go to receive state
    if (uart_transmitterData.transmit_idx ==
        uart_transmitterData.transmit_size) {
      uart_transmitterData.state = UART_TRANSMITTER_STATE_RECEIVE;
    }
  } break;

  case UART_TRANSMITTER_STATE_BUFFERFULL: {
    // Create another queue, create API function to put stuff in queue, and
    // block on queue receive here
    char received;
    if (xQueueReceive(uart_transmitterData.xQueue2, &(received),
                      portMAX_DELAY)) {
      xQueueReset(uart_transmitterData.xQueue2);
      uart_transmitterData.state = UART_TRANSMITTER_STATE_SEND;
    }
  } break;
  default: {
    errorCheck(UART_TX_IDENTIFIER, __LINE__);
    break;
  }
  }
}