
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/
#ifndef _ROVERPOSE_H_
#define _ROVERPOSE_H_
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define RoverPose_FIELD_MAX (3)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[RoverPose_FIELD_MAX / CHAR_BIT + 1];
  uint32_t xPosition;
  uint32_t yPosition;
  uint16_t yaw;
} RoverPose;
void RoverPose_init(RoverPose *msg);
/*
 * int32 xPosition
 */
bool RoverPose_has_xPosition(const RoverPose *msg);
int32_t RoverPose_xPosition(const RoverPose *msg);
void RoverPose_set_xPosition(RoverPose *msg, int32_t value);
void RoverPose_clear_xPosition(RoverPose *msg);
/*
 * int32 yPosition
 */
bool RoverPose_has_yPosition(const RoverPose *msg);
int32_t RoverPose_yPosition(const RoverPose *msg);
void RoverPose_set_yPosition(RoverPose *msg, int32_t value);
void RoverPose_clear_yPosition(RoverPose *msg);
/*
 * int16 yaw
 */
bool RoverPose_has_yaw(const RoverPose *msg);
int16_t RoverPose_yaw(const RoverPose *msg);
void RoverPose_set_yaw(RoverPose *msg, int16_t value);
void RoverPose_clear_yaw(RoverPose *msg);
void RoverPose_to_bytes(RoverPose *msg, char *buf, uint32_t seq);
bool RoverPose_from_bytes(RoverPose *msg, const char *buf, uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
