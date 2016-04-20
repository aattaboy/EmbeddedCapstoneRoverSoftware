#ifndef _POSE_PUBLIC_H_
#define _POSE_PUBLIC_H_

#include "encoders_public.h"
#include "generated/RoverPose.pbo.h"

typedef enum { POSE_ENCODER_COUNTS, POSE_OVERRIDE } ROVER_POSE_VARIANT_TYPES;

struct ROVER_POSE_VARIANT {
  union {
    struct EncoderCounts encoderCounts;
    RoverPose poseOverride;
  } data;
  ROVER_POSE_VARIANT_TYPES type;
};

#define POSE_QUEUE_TYPE struct ROVER_POSE_VARIANT

void sendToPoseQueue(POSE_QUEUE_TYPE *counts);

typedef int (*pose_callback_t)(RoverPose *);

int registerPoseCallback(pose_callback_t callback);

#endif /* _POSE_CALC_PUBLIC_H_ */