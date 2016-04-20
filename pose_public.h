#ifndef _POSE_PUBLIC_H_
#define _POSE_PUBLIC_H_

#include "encoders_public.h"
#include "generated/RoverPose.pbo.h"

#define POSE_QUEUE_TYPE struct EncoderCounts

void sendToPoseQueue(POSE_QUEUE_TYPE *counts);

typedef int (*pose_callback_t)(RoverPose *);

int registerPoseCallback(pose_callback_t callback);

#endif /* _POSE_CALC_PUBLIC_H_ */