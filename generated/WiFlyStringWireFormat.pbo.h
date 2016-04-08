
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/
#ifndef _WIFLYSTRINGWIREFORMAT_H_
#define _WIFLYSTRINGWIREFORMAT_H_
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"
#include "debuginfo.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define WiFlyStringWireFormat_FIELD_MAX (1)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[WiFlyStringWireFormat_FIELD_MAX / CHAR_BIT + 1];
  uint32_t msg_size;
  char msg[100];
} WiFlyStringWireFormat;
void WiFlyStringWireFormat_init(WiFlyStringWireFormat *msg);
/*
 * string msg
 */
bool WiFlyStringWireFormat_has_msg(const WiFlyStringWireFormat *msg);
size_t WiFlyStringWireFormat_msg(const WiFlyStringWireFormat *msg, char *dest,
                                 size_t lim);
void WiFlyStringWireFormat_set_msg(WiFlyStringWireFormat *msg, const char *src,
                                   size_t src_len);
void WiFlyStringWireFormat_clear_msg(WiFlyStringWireFormat *msg);
void WiFlyStringWireFormat_to_bytes(WiFlyStringWireFormat *msg, char *buf,
                                    uint32_t seq);
bool WiFlyStringWireFormat_from_bytes(WiFlyStringWireFormat *msg,
                                      const char *buf, uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
