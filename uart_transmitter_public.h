
#ifndef UART_TRANSMITTER_PUBLIC_H
#define UART_TRANSMITTER_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "generated/DebugInfo.pbo.h"
#include "pose_position_pair.h"

typedef enum { DEBUG_INFO, TEST_CHAR, RSSI_PAIR } UART_TRANSMITTER_VARIANT_TYPE;

struct __attribute__((packed)) UART_TRANSMITTER_VARIANT {
  union {
    DebugInfo debug_info;
    uint8_t test_char;
    struct PosePositionPair rover_pair;
  } data;
  UART_TRANSMITTER_VARIANT_TYPE type;
};

struct __attribute((packed)) UART_TRANSMITTER_VARIANT_WIRE {
  union {
    DebugInfo debug_info;
    uint8_t test_char;
  } data;
  UART_TRANSMITTER_VARIANT_TYPE type;
};

bool sendToUartQueue(struct UART_TRANSMITTER_VARIANT *var);
bool sendToUartQueueFromISR(struct UART_TRANSMITTER_VARIANT *var,
                            BaseType_t *higherPriorityTaskWoken);
bool sendToFullBufferQueueFromISR(char var,
                                  BaseType_t *higherPriorityTaskWoken);

#ifdef __cplusplus
}
#endif

#endif /* UART_TRANSMITTER_PUBLIC_H */