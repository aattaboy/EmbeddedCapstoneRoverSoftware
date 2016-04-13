#ifndef _POSE_PUBLIC_H_
#define _POSE_PUBLIC_H_

#include "encoders_public.h"
#include "generated/RoverPose.pbo.h"

void sendToPoseQueue(struct EncoderCounts *counts);

typedef int (*pose_callback_t)(RoverPose *);

int registerPoseCallback(pose_callback_t callback);

#endif /* _POSE_CALC_PUBLIC_H_ */