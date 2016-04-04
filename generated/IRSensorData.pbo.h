
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/
#ifndef _IRSENSORDATA_H_
#define _IRSENSORDATA_H_
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define IRSensorData_FIELD_MAX (3)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[IRSensorData_FIELD_MAX / CHAR_BIT + 1];
  uint16_t front;
  uint16_t left;
  uint16_t right;
} IRSensorData;
void IRSensorData_init(IRSensorData *msg);
/*
 * int16 front
 */
bool IRSensorData_has_front(const IRSensorData *msg);
int16_t IRSensorData_front(const IRSensorData *msg);
void IRSensorData_set_front(IRSensorData *msg, int16_t value);
void IRSensorData_clear_front(IRSensorData *msg);
/*
 * int16 left
 */
bool IRSensorData_has_left(const IRSensorData *msg);
int16_t IRSensorData_left(const IRSensorData *msg);
void IRSensorData_set_left(IRSensorData *msg, int16_t value);
void IRSensorData_clear_left(IRSensorData *msg);
/*
 * int16 right
 */
bool IRSensorData_has_right(const IRSensorData *msg);
int16_t IRSensorData_right(const IRSensorData *msg);
void IRSensorData_set_right(IRSensorData *msg, int16_t value);
void IRSensorData_clear_right(IRSensorData *msg);
void IRSensorData_to_bytes(IRSensorData *msg, char *buf, uint32_t seq);
bool IRSensorData_from_bytes(IRSensorData *msg, const char *buf,
                             uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
