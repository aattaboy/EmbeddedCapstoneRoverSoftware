#include "debug.h"
#include "debuginfo.h"
#include "rssi_collector_public.h"
#include "rssi_pairs.h"
#include "rssi_pairs_public.h"
#include "uart_transmitter_public.h"

RSSI_PAIRS_DATA rssi_pairsData;

// Public Functions
BaseType_t sendToRssiPairsQueue(struct RSSI_PAIRS_VARIANT *info) {
  BaseType_t retval =
      xQueueSendToBack(rssi_pairsData.rssi_pairsQueue, info, portMAX_DELAY);
  return retval;
}

BaseType_t sendToRssiPairsQueueFromISR(struct RSSI_PAIRS_VARIANT *info,
                                        BaseType_t *higherPriorityTaskWoken) {
  BaseType_t retval = xQueueSendToBackFromISR(rssi_pairsData.rssi_pairsQueue,
                                              info, higherPriorityTaskWoken);
  return retval;
}

// API Functions

int registerRSSIPairsCallback(rssi_pairs_callback_t cb) {
  if (rssi_pairsData.rssi_pairs_callbacks_idx !=
      RSSI_PAIRS_CALLBACKS_VECTOR_SIZE) {
    rssi_pairsData.callbacks[rssi_pairsData.rssi_pairs_callbacks_idx++] = cb;
    return 1;
  } else {
    return 0;
  }
}

RSSI_PAIRS_STATES getRSSIPairsCurrentState() { return rssi_pairsData.state; }

bool getRSSIPairsReceivedRoverPose() { return rssi_pairsData.receieved_pose; }

// Internal functions

static void sendPairToCallbacks(struct PosePositionPair *out) {
  size_t i;
  for (i = 0; i < rssi_pairsData.rssi_pairs_callbacks_idx; i++) {
    rssi_pairsData.callbacks[i](out);
  }
}

static bool queueReceiveWrapperProd(struct RSSI_PAIRS_VARIANT *var) {
  return xQueueReceive(rssi_pairsData.rssi_pairsQueue, var, portMAX_DELAY);
}

static void common_init(void) {
  rssi_pairsData.receieved_pose = false;
  RoverPose_init(&rssi_pairsData.last_reported_pose);
}

static int pairs_rssi_collector_cb(RSSIData *data) {
  struct RSSI_PAIRS_VARIANT var;
  var.type = RSSI_DATA;
  memcpy(&var.data.rssi_data, data, sizeof(*data));
  sendToRssiPairsQueue(&var);
  return 0;
}

static int pairs_pose_callback(RoverPose *pose) {
  struct RSSI_PAIRS_VARIANT var;
  var.type = ROVER_POSE;
  memcpy(&var.data.rover_pose, pose, sizeof(*pose));
  sendToRssiPairsQueue(&var);
  return 0;
}

void RSSI_PAIRS_Initialize(void) {
  rssi_pairsData.state = RSSI_PAIRS_STATE_INIT;
  rssi_pairsData.rssi_pairsQueue =
      xQueueCreate(RSSI_PAIRS_QUEUE_SIZE, sizeof(struct RSSI_PAIRS_VARIANT));
  if (rssi_pairsData.rssi_pairsQueue == 0) {
    errorCheck(RSSI_PAIRS_IDENTIFIER, __LINE__);
  }
  rssi_pairsData.queueReceive = queueReceiveWrapperProd;
  common_init();
  registerRSSICollectorCallback(pairs_rssi_collector_cb);
  registerPoseCallback(pairs_pose_callback);
}

void RSSI_PAIRS_Initialize_Testing(rssi_pairs_queue_receive_cb cb) {
  rssi_pairsData.state = RSSI_PAIRS_STATE_INIT;
  rssi_pairsData.queueReceive = cb;
  common_init();
}

void RSSI_PAIRS_Tasks(void) {
  switch (rssi_pairsData.state) {
  case RSSI_PAIRS_STATE_INIT: {
    rssi_pairsData.state = RSSI_PAIRS_STATE_RECEIVE;
    break;
  }
  case RSSI_PAIRS_STATE_RECEIVE: {
    struct RSSI_PAIRS_VARIANT received;
    if (rssi_pairsData.queueReceive(&received)) {
      switch (received.type) {
      case RSSI_DATA: {
        RSSIData rssi;
        uint32_t seq;
        static uint32_t seq_expected;
        if (!RSSIData_from_bytes(&rssi, (void *)&received.data.rssi_data,
                                 &seq)) {
          // TODO: handle less catastrophically
          errorCheck(RSSI_PAIRS_IDENTIFIER, __LINE__);
        }
        if (seq != seq_expected) {
          // TODO: notify
        }
        seq_expected++;

        if (rssi_pairsData.receieved_pose) {
          struct PosePositionPair outgoing_pair;
          outgoing_pair.pose = rssi_pairsData.last_reported_pose;
          outgoing_pair.rssi = rssi;

          struct UART_TRANSMITTER_VARIANT var;
          var.type = RSSI_PAIR;
          memcpy(&var.data.rover_pair, &outgoing_pair, sizeof(outgoing_pair));
          sendToUartQueue(&var);
        }

      } break;
      case ROVER_POSE: {
        RoverPose pose;
        uint32_t seq;
        static uint32_t seq_expected;
        if (!RoverPose_from_bytes(&pose, (void *)&received.data.rover_pose,
                                  &seq)) {
          // TODO: Handle less catastrophically
          errorCheck(RSSI_PAIRS_IDENTIFIER, __LINE__);
        }
        if (seq != seq_expected) {
          // TODO: notify
        }
        seq_expected++;

        rssi_pairsData.receieved_pose = true;
        rssi_pairsData.last_reported_pose = pose;

      } break;
      default: { errorCheck(RSSI_PAIRS_IDENTIFIER, __LINE__); }
      }
    }
    break;
  }
  default: { break; }
  }
}
