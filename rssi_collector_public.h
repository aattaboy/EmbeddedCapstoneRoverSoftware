#ifndef RSSI_COLLECTOR_PUBLIC_H
#define RSSI_COLLECTOR_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "generated/RSSIData.pbo.h"
#include "queue.h"

typedef int (*rssi_collector_callback_t)(RSSIData *data);

int registerRSSICollectorCallback(rssi_collector_callback_t callback);
BaseType_t sendToRSSICollectorQueue(RSSIData *data);
BaseType_t sendToRSSICollectorQueueFromISR(RSSIData *info,
                                           BaseType_t *higherPriorityTaskWoken);

#ifdef __cplusplus
}
#endif

#endif /* RSSI_COLLECTOR_PUBLIC_H */
