#include "debug.h"
#include "debug_codes.h"
#include "debuginfo.h"
#include "rssi_collector.h"

RSSI_COLLECTOR_DATA rssi_collectorData;

// Public functions

static const uint8_t frameSequence[4] = { 0xab, 0xcd, 0xef, 0x12 };

int registerRSSICollectorCallback(rssi_collector_callback_t callback) {
  if (rssi_collectorData.rssi_collector_callbacks_idx !=
      RSSI_COLLECTOR_CALLBACKS_VECTOR_SIZE) {
    rssi_collectorData
        .callbacks[rssi_collectorData.rssi_collector_callbacks_idx++] =
        callback;
    return 1;
  } else {
    return 0;
  }
}

BaseType_t sendToRSSICollectorQueue(RSSIData *data) {
  BaseType_t retval = xQueueSendToBack(rssi_collectorData.rssiCollectorQueue,
                                       data, portMAX_DELAY);
  return retval;
}

BaseType_t
sendToRSSICollectorQueueFromISR(RSSIData *info,
                                BaseType_t *higherPriorityTaskWoken) {
  BaseType_t retval = xQueueSendToBackFromISR(
      rssi_collectorData.rssiCollectorQueue, info, higherPriorityTaskWoken);
  return retval;
}

// Internal functions
static void sendMessageToCallbacks(RSSIData *info) {
  size_t i;
  for (i = 0; i < rssi_collectorData.rssi_collector_callbacks_idx; i++) {
    rssi_collectorData.callbacks[i](info);
  }
}

void RSSI_COLLECTOR_Initialize(void) {
  rssi_collectorData.state = RSSI_COLLECTOR_STATE_INIT;
  rssi_collectorData.rssiCollectorQueue =
      xQueueCreate(RSSI_COLLECTOR_QUEUE_SIZE, sizeof(RSSIData));
  if (rssi_collectorData.rssiCollectorQueue == 0) {
    errorCheck(RSSI_COLLECTOR_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(rssi_collectorData.rssiCollectorQueue,
                      "RSSI Collector Queue");
}

// Consume a message from the receive queue and unpack it as a char.
// Blocks if there is no message available.
static uint8_t receive_and_unpack() {
  uint8_t received;
  if (xQueueReceive(rssi_collectorData.rssiCollectorQueue, &(received),
                    portMAX_DELAY)) {
    return received;
  } else {
    return 0xff;
  }
}

static uint64_t message_checksum(RSSIData *data) {
  uint64_t sum = 0;
  size_t offset = __builtin_offsetof(RSSIData, seq);
  size_t i;
  for (i = offset; i < sizeof(*data); i++) {
    sum += ((uint8_t *)data)[i];
  }
  return sum;
}

void RSSI_COLLECTOR_Tasks(void) {
  switch (rssi_collectorData.state) {
  case RSSI_COLLECTOR_STATE_INIT: {
    rssi_collectorData.state = RSSI_COLLECTOR_STATE_RECEIVE;
    break;
  }

  case RSSI_COLLECTOR_STATE_RECEIVE: {
    RSSIData received_obj;

    if (xQueueReceive(rssi_collectorData.rssiCollectorQueue, &received_obj,
                      portMAX_DELAY)) {
      uint32_t seq;
      static uint32_t seq_expected;

      if (!RSSIData_from_bytes(&received_obj, (char *)&received_obj, &seq)) {
        break;
      }

      if (seq != seq_expected) {
        warning(RSSI_COLLECTOR_IDENTIFIER, __LINE__);
      }
      seq_expected++;

      //packAndSendDebugInfo(RSSI_COLLECTOR_IDENTIFIER,
      //                     RSSICollectorReceivedMessage,
      //                     RSSIData_rssi(&received_obj));

      writeToDebug(RSSI_COLLECTOR_RECEIVE_BYTE);
      sendMessageToCallbacks(&received_obj);
    }
  } break;

  default: {
    errorCheck(RSSI_COLLECTOR_IDENTIFIER, __LINE__);
    break;
  }
  }
}
