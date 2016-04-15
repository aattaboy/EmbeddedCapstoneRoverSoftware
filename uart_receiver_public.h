#ifndef UART_RECEIVER_PUBLIC_H
#define UART_RECEIVER_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <FreeRTOS.h>
#include "generated/MotorCommand.pbo.h"
#include "generated/RoverPose.pbo.h"
#include "generated/UartRxData.pbo.h"
#include <queue.h>

typedef enum { UART_RX_DATA, STRING, MOTOR_MESSAGE, CONTROL_CMD } UART_RECEIVER_VARIANT_TYPE;

struct __attribute__((packed)) UART_RECEIVER_VARIANT {
  union {
    UartRxData rx_data;
    WiFlyStringWireFormat string;
    MotorCommand motorMessage;
    RoverPose controlCmd;
  } data;
  UART_RECEIVER_VARIANT_TYPE type;
};

typedef int (*uart_receiver_callback_t)(struct UART_RECEIVER_VARIANT *);

int registerUartReceiverCallback(uart_receiver_callback_t callback);

BaseType_t sendToUartReceiverQueue(struct UART_RECEIVER_VARIANT *info);
BaseType_t sendToUartReceiverQueueFromISR(struct UART_RECEIVER_VARIANT *info,
                                          BaseType_t *higherPriorityTaskWoken);

#ifdef __cplusplus
}
#endif

#endif /* UART_RECEIVER_PUBLIC_H */
