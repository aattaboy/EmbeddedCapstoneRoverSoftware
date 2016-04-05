
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/
#ifndef _PIDSETPOINT_H_
#define _PIDSETPOINT_H_
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define PIDSetPoint_FIELD_MAX (2)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[PIDSetPoint_FIELD_MAX / CHAR_BIT + 1];
  uint32_t targetX;
  uint32_t targetY;
} PIDSetPoint;
void PIDSetPoint_init(PIDSetPoint *msg);
/*
 * int32 targetX
 */
bool PIDSetPoint_has_targetX(const PIDSetPoint *msg);
int32_t PIDSetPoint_targetX(const PIDSetPoint *msg);
void PIDSetPoint_set_targetX(PIDSetPoint *msg, int32_t value);
void PIDSetPoint_clear_targetX(PIDSetPoint *msg);
/*
 * int32 targetY
 */
bool PIDSetPoint_has_targetY(const PIDSetPoint *msg);
int32_t PIDSetPoint_targetY(const PIDSetPoint *msg);
void PIDSetPoint_set_targetY(PIDSetPoint *msg, int32_t value);
void PIDSetPoint_clear_targetY(PIDSetPoint *msg);
void PIDSetPoint_to_bytes(PIDSetPoint *msg, char *buf, uint32_t seq);
bool PIDSetPoint_from_bytes(PIDSetPoint *msg, const char *buf,
                            uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
