
#include "debug.h"
#include "pose.h"

#include <math.h>

POSE_DATA poseData;

#define POSE_QUEUE_SIZE (10)

static int pose_encoder_counts_callback(struct EncoderCounts *counts) {
  return xQueueSendToBack(poseData.poseQueue, counts, portMAX_DELAY);
}

void POSE_Initialize(void) {
  poseData.state = POSE_STATE_INIT;
  poseData.poseQueue =
      xQueueCreate(POSE_QUEUE_SIZE, sizeof(struct EncoderCounts));
  if (poseData.poseQueue == 0) {
    errorCheck(POSE_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(poseData.poseQueue, "Pose Queue");

  poseData.x = 0;
  poseData.y = 0;
  poseData.yaw = 0;
  memset(&poseData.prev_counts, 0, sizeof(poseData.prev_counts));

  registerEncodersCallback(pose_encoder_counts_callback);
}

static double degToRad(double deg) { return deg * 180.0 / 3.14; }

void POSE_Tasks(void) {
  switch (poseData.state) {
  case POSE_STATE_INIT: {
    poseData.state = POSE_STATE_RECEIVE;
  } break;
  case POSE_STATE_RECEIVE: {
    struct EncoderCounts counts;
    if (xQueueReceive(poseData.poseQueue, &counts, portMAX_DELAY)) {
      uint32_t diff = counts.left - poseData.prev_counts.left;

      uint8_t diff_sw = (counts.left_dir << 1) | (counts.right_dir);
      switch (diff_sw) {
      case 0x00: { // FORWARD
        double r = 3. * 0.142 * (diff);
        double x_disp = r * sin(degToRad(poseData.yaw));
        double y_disp = r * cos(degToRad(poseData.yaw));
        poseData.x += x_disp;
        poseData.y += y_disp;
      } break;
      case 0x01: { // RIGHT
        double disp_theta = 3. * 1.54 * (diff);
        poseData.yaw += disp_theta;
      } break;
      case 0x02: { // LEFT
        double disp_theta = 3. * -1.54 * (diff);
        poseData.yaw += disp_theta;
      } break;
      case 0x03: { // BACKWARD
        double r = 3. * -0.142 * (diff);
        double x_disp = r * sin(degToRad(poseData.yaw));
        double y_disp = r * cos(degToRad(poseData.yaw));
        poseData.x += x_disp;
        poseData.y += y_disp;
      } break;
      default: { errorCheck(POSE_IDENTIFIER, __LINE__); }
      }

      poseData.prev_counts = counts;
      packAndSendDebugInfo(POSE_IDENTIFIER, XUpdated, poseData.x);
      packAndSendDebugInfo(POSE_IDENTIFIER, YUpdated, poseData.y);
      packAndSendDebugInfo(POSE_IDENTIFIER, YawUpdated, poseData.yaw);
    }
  } break;
  default: { errorCheck(POSE_IDENTIFIER, __LINE__); }
  }
}
