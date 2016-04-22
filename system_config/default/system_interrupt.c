#include <xc.h>
#include <sys/attribs.h>
#include "debug.h"
#include "debuginfo.h"
#include "debug_codes.h"
#include "encoders_public.h"
#include "rssi_collector_public.h"
#include "sensor1_public.h"
#include "system_definitions.h"
#include "uart_receiver_public.h"
#include "util.h"

void IntHandlerDrvTmrInstance0(void) {
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  static uint32_t prev_cycles;
  uint32_t current_ticks = getCpuCycles();

  static uint32_t cycles_lpf[3] = {};
  static size_t lpf_idx = 0;
  size_t i;

  struct EncodersISRData data;
  data.encoder_id = ENCODERS_LEFT;
  cycles_lpf[lpf_idx++] =
      positive_modulo_u(current_ticks - prev_cycles, 0xffffffffu);
  if (lpf_idx == 3) {
    lpf_idx = 0;
  }
  uint32_t avg = 0;
  for (i = 0; i < 3; i++) {
    avg += (cycles_lpf[i] / 3);
  }
  data.cycles = avg;
  sendToEncodersQueueFromISR(&data, &higherPriorityTaskWoken);

  prev_cycles = current_ticks;

  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_TIMER_3);

  portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void IntHandlerDrvTmrInstance1(void) {
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  static uint32_t prev_cycles;
  uint32_t current_ticks = getCpuCycles();

  static uint32_t cycles_lpf[3] = {};
  static size_t lpf_idx = 0;
  size_t i;

  struct EncodersISRData data;
  data.encoder_id = ENCODERS_RIGHT;
  cycles_lpf[lpf_idx++] =
      positive_modulo_u(current_ticks - prev_cycles, 0xffffffffu);
  if (lpf_idx == 3) {
    lpf_idx = 0;
  }
  uint32_t avg = 0;
  for (i = 0; i < 3; i++) {
    avg += (cycles_lpf[i] / 3);
  }
  data.cycles = avg;
  sendToEncodersQueueFromISR(&data, &higherPriorityTaskWoken);

  prev_cycles = current_ticks;

  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_TIMER_4);

  portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}

void IntHandlerDrvUsartInstance0(void) {
  static size_t rx_idx;
  static uint8_t rx_buf[sizeof(struct UART_RECEIVER_VARIANT)];

  static enum {
    RX_FRAME_START_1,
    RX_FRAME_START_2,
    RX_FRAME_START_3,
    RX_FRAME_START_4,
    RX_BYTES_RECEIVE
  } rx_state = RX_FRAME_START_1;

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
      uint8_t rxedChar = PLIB_USART_ReceiverByteReceive(USART_ID_1);

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
      default: { errorCheck(UART_RX_IDENTIFIER, __LINE__); }
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

static const uint8_t frame_sequence[] = { 0xab, 0xcd, 0xef, 0x12 };

void IntHandlerDrvUsartInstance1(void) {
  BaseType_t higherPriorityTaskWoken = pdFALSE;

  static char rx_buf[sizeof(RSSIData)];
  static size_t rx_idx;

  static enum {
    FRAME_START_1,
    FRAME_START_2,
    FRAME_START_3,
    FRAME_START_4,
    RX,
    TRANSMIT_ACK
  } state;

  static uint32_t numBufferOverruns;
  if (U4STA & 1 << 1) {
    numBufferOverruns++;
    packAndSendDebugInfoFromISR(RSSI_COLLECTOR_IDENTIFIER,
                                RSSICollectorBufferOverrun, numBufferOverruns,
                                &higherPriorityTaskWoken);
    PLIB_USART_TransmitterByteSend(USART_ID_4, 0x15);
    state = FRAME_START_1;
  }

  if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_4_RECEIVE)) {
    while (PLIB_USART_ReceiverDataIsAvailable(USART_ID_4)) {
      uint8_t rxed_char = PLIB_USART_ReceiverByteReceive(USART_ID_4);

      switch (state) {
      case FRAME_START_1: {
        if (rxed_char == frame_sequence[0]) {
          state = FRAME_START_2;
        }
      } break;
      case FRAME_START_2: {
        if (rxed_char == frame_sequence[1]) {
          state = FRAME_START_3;
        } else {
          state = FRAME_START_1;
        }
      } break;
      case FRAME_START_3: {
        if (rxed_char == frame_sequence[2]) {
          state = FRAME_START_4;
        } else {
          state = FRAME_START_1;
        }
      } break;
      case FRAME_START_4: {
        if (rxed_char == frame_sequence[3]) {
          rx_idx = 0;
          state = RX;
        } else {
          state = FRAME_START_1;
        }
      } break;
      case RX: {
        rx_buf[rx_idx++] = rxed_char;
        if (rx_idx == sizeof(RSSIData)) {
          state = TRANSMIT_ACK;
          sendToRSSICollectorQueueFromISR((RSSIData *)rx_buf,
                                          &higherPriorityTaskWoken);
          //static uint32_t total_msgs_rxed;
          //packAndSendDebugInfoFromISR(
          //    RSSI_COLLECTOR_IDENTIFIER, RSSICollectorTotalMsgsRxed,
          //    total_msgs_rxed++, &higherPriorityTaskWoken);
        }
      } break;
      case TRANSMIT_ACK: {
        PLIB_USART_TransmitterByteSend(USART_ID_4, 0x06);
        state = FRAME_START_1;
      } break;
      default: { errorCheck(RSSI_COLLECTOR_IDENTIFIER, __LINE__); } break;
      }
    }
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_4_RECEIVE);
  }

  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_4_TRANSMIT);
  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_4_ERROR);
  U4STACLR = 1 << 1;

  portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}
