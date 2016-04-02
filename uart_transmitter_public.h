
#ifndef UART_TRANSMITTER_PUBLIC_H
#define UART_TRANSMITTER_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "generated/DebugInfo.pbo.h"

typedef enum { DEBUG_INFO, TEST_CHAR } UART_TRANSMITTER_VARIANT_TYPE;

struct UART_TRANSMITTER_VARIANT {
  union {
    DebugInfo debug_info;
    uint8_t test_char;
  } data;
  UART_TRANSMITTER_VARIANT_TYPE type;
};

bool sendToUartQueue(struct UART_TRANSMITTER_VARIANT *var);
bool sendToFullBufferQueueFromISR(char var,
                                  BaseType_t *higherPriorityTaskWoken);

#ifdef __cplusplus
}
#endif

#endif /* UART_TRANSMITTER_PUBLIC_H */