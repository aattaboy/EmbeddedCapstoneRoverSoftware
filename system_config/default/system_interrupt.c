#include <xc.h>
#include <sys/attribs.h>
#include "debug.h"
#include "debug_codes.h"
#include "uart_receiver_public.h"
#include "system_definitions.h"

void IntHandlerDrvUsartInstance0(void)
{
  BaseType_t higherPriorityTaskWoken = pdFALSE;
  
  if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT)) {
    // Send message to transmitter to start transmitting again
      writeToDebug(RECEIVER_ISR_BYTE);
    if (PLIB_USART_TransmitterIsEmpty(USART_ID_1)) {
      //sendToFullBufferQueueFromISR(' ', &higherPriorityTaskWoken);
      PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
      PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
  }
  
  if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE)) {
    while (PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)) {
      uint8_t testChar = PLIB_USART_ReceiverByteReceive(USART_ID_1);

      sendToUartReceiverQueueFromISR(&testChar, &higherPriorityTaskWoken);

      writeToDebug(testChar);
      //writeToDebug(RECEIVER_ISR_BYTE);
    }
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
  }
  PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);

  portEND_SWITCHING_ISR(higherPriorityTaskWoken);
}