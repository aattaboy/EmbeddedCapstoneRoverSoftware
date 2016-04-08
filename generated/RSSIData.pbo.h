
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

#define RSSIData_FIELD_MAX (2)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[RSSIData_FIELD_MAX / CHAR_BIT + 1];
  uint32_t bssid_size;
  char bssid[100];
  uint16_t rssi;
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
void RSSIData_to_bytes(RSSIData *msg, char *buf, uint32_t seq);
bool RSSIData_from_bytes(RSSIData *msg, const char *buf, uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
