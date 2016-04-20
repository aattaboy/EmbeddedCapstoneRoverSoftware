
#include "debug.h"
#include "pose.h"

#include <math.h>

POSE_DATA poseData;

#define POSE_QUEUE_SIZE (10)

static int pose_encoder_counts_callback(struct EncoderCounts *counts) {
  POSE_QUEUE_TYPE var;
  var.type = POSE_ENCODER_COUNTS;
  var.data.encoderCounts = *counts;
  return xQueueSendToBack(poseData.poseQueue, &var, portMAX_DELAY);
}

static int
pose_uart_rx_pose_override_Callback(struct UART_RECEIVER_VARIANT *var) {
  if (var->type == UART_RX_POSE_OVERRIDE) {
    xQueueSendToBack(poseData.poseQueue, &var->data.poseOverride,
                     portMAX_DELAY);
    return 1;
  }
  return 0;
}

int registerPoseCallback(pose_callback_t callback) {
  if (poseData.pose_callbacks_idx < POSE_CALLBACKS_VECTOR_SIZE) {
    poseData.callbacks[poseData.pose_callbacks_idx++] = callback;
    return 1;
  } else {
    return 0;
  }
}

static void sendToCallbacks(RoverPose *pose) {
  size_t i;
  for (i = 0; i < poseData.pose_callbacks_idx; i++) {
    poseData.callbacks[i](pose);
  }
}

void POSE_Initialize(void) {
  poseData.state = POSE_STATE_INIT;
  poseData.poseQueue = xQueueCreate(POSE_QUEUE_SIZE, sizeof(POSE_QUEUE_TYPE));
  if (poseData.poseQueue == 0) {
    errorCheck(POSE_IDENTIFIER, __LINE__);
  }
  vQueueAddToRegistry(poseData.poseQueue, "Pose Queue");

  poseData.x = 0;
  poseData.y = 0;
  poseData.yaw = 0;
  memset(&poseData.prev_counts, 0, sizeof(poseData.prev_counts));

  registerEncodersCallback(pose_encoder_counts_callback);
  registerUartReceiverCallback(pose_uart_rx_pose_override_Callback);
}

static double degToRad(double deg) { return deg * 3.14159 / 180.0; }

void POSE_Tasks(void) {
  switch (poseData.state) {
  case POSE_STATE_INIT: {
    poseData.state = POSE_STATE_RECEIVE;
    RoverPose pose;
    RoverPose_init(&pose);
    RoverPose_set_xPosition(&pose, 0);
    RoverPose_set_yPosition(&pose, 0);
    RoverPose_set_yaw(&pose, 0);
    RoverPose_to_bytes(&pose, (char *)&pose, 0);
    sendToCallbacks(&pose);
  } break;
  case POSE_STATE_RECEIVE: {
    POSE_QUEUE_TYPE var;
    if (xQueueReceive(poseData.poseQueue, &var, portMAX_DELAY)) {
      switch (var.type) {
      case POSE_ENCODER_COUNTS: {
        struct EncoderCounts counts = var.data.encoderCounts;
        uint32_t diff = counts.left - poseData.prev_counts.left;

        uint8_t diff_sw = (counts.left_dir << 1) | (counts.right_dir);
        switch (diff_sw) {
        case 0x03: { // FORWARD
          double r = -0.404 * (diff);
          double x_disp = r * sin(degToRad(poseData.yaw));
          double y_disp = r * cos(degToRad(poseData.yaw));
          poseData.x += x_disp;
          poseData.y += y_disp;
        } break;
        case 0x02: { // RIGHT
          double disp_theta = 4.2 * (diff);
          poseData.yaw += disp_theta;
          if (poseData.yaw < 0.0) {
            poseData.yaw = 360.0 + poseData.yaw;
          }
          poseData.yaw = fmod(poseData.yaw, 360.0);
        } break;
        case 0x01: { // LEFT
          double disp_theta = -4.2 * (diff);
          poseData.yaw += disp_theta;
          if (poseData.yaw < 0.0) {
            poseData.yaw = 360.0 + poseData.yaw;
          }
          poseData.yaw = fmod(poseData.yaw, 360.0);
        } break;
        case 0x00: { // BACKWARD
          double r = 0.404 * (diff);
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

        RoverPose pose;
        RoverPose_init(&pose);
        RoverPose_set_xPosition(&pose, poseData.x);
        RoverPose_set_yPosition(&pose, poseData.y);
        RoverPose_set_yaw(&pose, poseData.yaw);
        RoverPose_to_bytes(&pose, (char *)&pose, 0);

        sendToCallbacks(&pose);
      } break; // case POSE_ENCODER_COUNTS
      case POSE_OVERRIDE: {

      } break; // case POSE_OVERRIDE
      }        // switch (counts.type)
    }
    break;
  default: { errorCheck(POSE_IDENTIFIER, __LINE__); }
  } // case POSE_STATE_RECEIVE:
  } // switch (poseData.state)
}
