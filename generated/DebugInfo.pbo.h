
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/
#ifndef _DEBUGINFO_H_
#define _DEBUGINFO_H_
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define DebugInfo_FIELD_MAX (3)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[DebugInfo_FIELD_MAX / CHAR_BIT + 1];
  uint32_t identifier;
  uint32_t debugID;
  uint32_t data;
} DebugInfo;
void DebugInfo_init(DebugInfo *msg);
/*
 * int32 identifier
 */
bool DebugInfo_has_identifier(const DebugInfo *msg);
int32_t DebugInfo_identifier(const DebugInfo *msg);
void DebugInfo_set_identifier(DebugInfo *msg, int32_t value);
void DebugInfo_clear_identifier(DebugInfo *msg);
/*
 * int32 debugID
 */
bool DebugInfo_has_debugID(const DebugInfo *msg);
int32_t DebugInfo_debugID(const DebugInfo *msg);
void DebugInfo_set_debugID(DebugInfo *msg, int32_t value);
void DebugInfo_clear_debugID(DebugInfo *msg);
/*
 * int32 data
 */
bool DebugInfo_has_data(const DebugInfo *msg);
int32_t DebugInfo_data(const DebugInfo *msg);
void DebugInfo_set_data(DebugInfo *msg, int32_t value);
void DebugInfo_clear_data(DebugInfo *msg);
void DebugInfo_to_bytes(DebugInfo *msg, char *buf, uint32_t seq);
bool DebugInfo_from_bytes(DebugInfo *msg, const char *buf, uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
