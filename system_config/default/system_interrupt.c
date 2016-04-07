#include <xc.h>
#include <sys/attribs.h>
#include "debug.h"
#include "debug_codes.h"
#include "encoders_public.h"
#include "uart_receiver_public.h"
#include "sensor1_public.h"
#include "system_definitions.h"

void IntHandlerDrvTmrInstance0(void) {
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  sendToEncodersQueueFromISR(ENCODERS_LEFT, &higherPriorityTaskWoken);
  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_TIMER_3);

  portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void IntHandlerDrvTmrInstance1(void) {
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  sendToEncodersQueueFromISR(ENCODERS_RIGHT, &higherPriorityTaskWoken);
  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_TIMER_4);

  portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

  static size_t rx_idx __attribute((unused));
  static uint8_t rx_buf[sizeof(struct UART_RECEIVER_VARIANT)] __attribute((unused));

  static enum {
    RX_FRAME_START_1,
    RX_FRAME_START_2,
    RX_FRAME_START_3,
    RX_FRAME_START_4,
    RX_BYTES_RECEIVE
  } rx_state  = RX_FRAME_START_1;

void IntHandlerDrvUsartInstance0(void) {
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT)) {
    // Send message to transmitter to start transmitting again
    writeToDebug(RECEIVER_ISR_BYTE);
    if (PLIB_USART_TransmitterIsEmpty(USART_ID_1)) {
      sendToFullBufferQueueFromISR(' ', &higherPriorityTaskWoken);
      PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
      PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
  }
  
  if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE)) {
    while (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)) {
      uint8_t rxedChar __attribute((unused)) = PLIB_USART_ReceiverByteReceive(USART_ID_1);

      switch (rx_state) {
      case RX_FRAME_START_1: {
        if (rxedChar == 0x80u) {
          rx_state = RX_FRAME_START_2;
        }
      } break;
      case RX_FRAME_START_2: {
        if (rxedChar == 0x08u) {
          rx_state = RX_FRAME_START_3;
        } else {
          rx_state = RX_FRAME_START_1;
        }
      } break;
      case RX_FRAME_START_3: {
        if (rxedChar == 0x13u) {
          rx_state = RX_FRAME_START_4;
        } else {
          rx_state = RX_FRAME_START_1;
        }
      } break;
      case RX_FRAME_START_4: {
        if (rxedChar == 0x55u) {
          rx_state = RX_BYTES_RECEIVE;
        } else {
          rx_state = RX_FRAME_START_1;
        }
      } break;
      case RX_BYTES_RECEIVE: {
        rx_buf[rx_idx++] = rxedChar;

        if (rx_idx == sizeof(struct UART_RECEIVER_VARIANT)) {
          rx_idx = 0;
          rx_state = RX_FRAME_START_1;
          sendToUartReceiverQueueFromISR((struct UART_RECEIVER_VARIANT *)rx_buf,
                                         &higherPriorityTaskWoken);
        }
      } break;
      } // switch (rx_state)
    }
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
  }
  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);

  portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void IntHandlerDrvTmrInstance2(void) {
  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_TIMER_2);
}

void IntHandlerDrvAdc(void) {
  BaseType_t higherPriorityTaskWoken = pdFALSE;
  struct sensorInterrupt isr_data;

  isr_data.center = DRV_ADC_SamplesRead(0);
  isr_data.left = DRV_ADC_SamplesRead(1);
  isr_data.right = DRV_ADC_SamplesRead(2);

  sendToSensor1QueueFromISR(&isr_data, &higherPriorityTaskWoken);

  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_ADC_1);

  portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}
