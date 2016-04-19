#ifndef POSE_POSITION_PAIR_H
#define POSE_POSITION_PAIR_H

#include "generated/RoverPose.pbo.h"
#include "generated/RSSIData.pbo.h"

struct __attribute__((packed)) PosePositionPair {
  RoverPose pose;
  RSSIData rssi;
};

#endif