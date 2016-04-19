
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/
#ifndef _RSSIDATA_H_
#define _RSSIDATA_H_
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"
#include "debuginfo.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define RSSIData_FIELD_MAX (5)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[RSSIData_FIELD_MAX / CHAR_BIT + 1];
  uint32_t bssid_size;
  char bssid[100];
  uint16_t rssi;
  uint32_t pairSeq;
  uint32_t frameSize;
  uint32_t frameNum;
} RSSIData;
void RSSIData_init(RSSIData *msg);
/*
 * string bssid
 */
bool RSSIData_has_bssid(const RSSIData *msg);
size_t RSSIData_bssid(const RSSIData *msg, char *dest, size_t lim);
void RSSIData_set_bssid(RSSIData *msg, const char *src, size_t src_len);
void RSSIData_clear_bssid(RSSIData *msg);
/*
 * int16 rssi
 */
bool RSSIData_has_rssi(const RSSIData *msg);
int16_t RSSIData_rssi(const RSSIData *msg);
void RSSIData_set_rssi(RSSIData *msg, int16_t value);
void RSSIData_clear_rssi(RSSIData *msg);
/*
 * int32 pairSeq
 */
bool RSSIData_has_pairSeq(const RSSIData *msg);
int32_t RSSIData_pairSeq(const RSSIData *msg);
void RSSIData_set_pairSeq(RSSIData *msg, int32_t value);
void RSSIData_clear_pairSeq(RSSIData *msg);
/*
 * int32 frameSize
 */
bool RSSIData_has_frameSize(const RSSIData *msg);
int32_t RSSIData_frameSize(const RSSIData *msg);
void RSSIData_set_frameSize(RSSIData *msg, int32_t value);
void RSSIData_clear_frameSize(RSSIData *msg);
/*
 * int32 frameNum
 */
bool RSSIData_has_frameNum(const RSSIData *msg);
int32_t RSSIData_frameNum(const RSSIData *msg);
void RSSIData_set_frameNum(RSSIData *msg, int32_t value);
void RSSIData_clear_frameNum(RSSIData *msg);
void RSSIData_to_bytes(RSSIData *msg, char *buf, uint32_t seq);
bool RSSIData_from_bytes(RSSIData *msg, const char *buf, uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
