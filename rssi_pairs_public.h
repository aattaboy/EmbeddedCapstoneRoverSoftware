#ifndef RSSI_PAIRS_PUBLIC_H
#define RSSI_PAIRS_PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "generated/RSSIData.pbo.h"
#include "generated/RoverPose.pbo.h"
#include "pose_position_pair.h"

typedef enum { RSSI_DATA, ROVER_POSE } RSSI_PAIRS_VARIANT_TYPE;

struct RSSI_PAIRS_VARIANT {
  union {
    RSSIData rssi_data;
    RoverPose rover_pose;
  } data;
  RSSI_PAIRS_VARIANT_TYPE type;
};

typedef enum {
  RSSI_PAIRS_STATE_INIT = 0,
  RSSI_PAIRS_STATE_RECEIVE,
} RSSI_PAIRS_STATES;

typedef int (*rssi_pairs_callback_t)(struct PosePositionPair *pair);

int registerRSSIPairsCallback(rssi_pairs_callback_t cb);
BaseType_t sendToRssiPairsQueue(struct RSSI_PAIRS_VARIANT *info);
BaseType_t sendToRssiPairsQueueFromISR(struct RSSI_PAIRS_VARIANT *info,
                                       BaseType_t *higherPriorityTaskWoken);

void RSSI_PAIRS_Initialize(void);

void RSSI_PAIRS_Tasks(void);

// Debug APIs

typedef bool (*rssi_pairs_queue_receive_cb)(struct RSSI_PAIRS_VARIANT *);

void RSSI_PAIRS_Initialize_Testing(rssi_pairs_queue_receive_cb cb);

RSSI_PAIRS_STATES getRSSIPairsCurrentState();

bool getRSSIPairsReceivedRoverPose();

#ifdef __cplusplus
}
#endif

#endif /* RSSI_PAIRS_PUBLIC_H */
