
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

uint64_t siphash24(const void *src, unsigned long src_sz, const char key[16]);

#define BIG_ENDIAN 0

#if (!BIG_ENDIAN)
static uint32_t swap_32(uint32_t op) {
  return ((op >> 24) & 0xff) | ((op << 8) & 0xff0000) | ((op >> 8) & 0xff00) |
         ((op << 24) & 0xff000000);
}
#endif

static uint32_t ntohl(uint32_t netlong) {
#ifdef NETWORK_BIG_ENDIAN
#if BIG_ENDIAN
  return netlong;
#else
  return swap_32(netlong);
#endif
#else
  return netlong;
#endif
}

static uint32_t htonl(uint32_t hostlong) {
#ifdef NETWORK_BIG_ENDIAN
#if BIG_ENDIAN
  return hostlong;
#else
  return swap_32(hostlong);
#endif
#else
  return hostlong;
#endif
}

#if (!BIG_ENDIAN)
static uint16_t swap_16(uint16_t op) { return (op >> 8) | (op << 8); }
#endif

static uint16_t ntohs(uint16_t netshort) {
#ifdef NETWORK_BIG_ENDIAN
#if BIG_ENDIAN
  return netshort;
#else
  return swap_16(netshort);
#endif
#else
  return netshort;
#endif
}

static uint16_t htons(uint16_t hostshort) {
#ifdef NETWORK_BIG_ENDIAN
#if BIG_ENDIAN
  return hostshort;
#else
  return swap_16(hostshort);
#endif
#else
  return hostshort;
#endif
}

#include "WiFlyStringWireFormat.pbo.h"

static bool has_field(const WiFlyStringWireFormat *msg, size_t i) {
  size_t byte_index = i / CHAR_BIT;
  return ((unsigned)msg->has_field[byte_index] & (1u << (i % CHAR_BIT - 1)));
}

static void set_field_present(WiFlyStringWireFormat *msg, size_t i) {
  size_t byte_index = i / CHAR_BIT;
  msg->has_field[byte_index] |= (1u << (i % CHAR_BIT - 1));
}

static void set_field_absent(WiFlyStringWireFormat *msg, size_t i) {
  size_t byte_index = i / CHAR_BIT;
  msg->has_field[byte_index] &= ~(1u << (i % CHAR_BIT - 1));
}

static void check_magic(const WiFlyStringWireFormat *msg) {
  if (msg->magic != htonl(0xDEADBEEFu)) {
    fprintf(stderr,
            "Invalid magic number for message WiFlyStringWireFormat at %p\n",
            (void *)msg);
    errorCheck(__FILE__, __LINE__);
  }
}

void WiFlyStringWireFormat_init(WiFlyStringWireFormat *msg) {
  memset(msg, 0, sizeof(*msg));
  // TODO: generate this from a hash at compile time
  msg->magic = htonl(0xDEADBEEF);
}
/*
 * string msg
 */
bool WiFlyStringWireFormat_has_msg(const WiFlyStringWireFormat *msg) {
  check_magic(msg);
  return has_field(msg, 1);
}
size_t WiFlyStringWireFormat_msg(const WiFlyStringWireFormat *msg, char *dest,
                                 size_t lim) {
  if (has_field(msg, 1)) {
    size_t char_count = (lim - 1 < msg->msg_size ? lim - 1 : msg->msg_size);
    memmove(dest, msg->msg, char_count);
    dest[char_count] = '\0';
    return char_count;
  } else {
    fprintf(stderr, "Requested field msg from WiFlyStringWireFormat at address "
                    "%p, but message does not have the field\n",
            (void *)msg);
    return -1;
  }
}
void WiFlyStringWireFormat_set_msg(WiFlyStringWireFormat *msg, const char *src,
                                   size_t src_len) {
  size_t char_count = (100 > src_len ? src_len : 100);
  memmove((void *)msg->msg, src, char_count);
  msg->msg_size = htonl(char_count);
  set_field_present(msg, 1);
}
void WiFlyStringWireFormat_clear_msg(WiFlyStringWireFormat *msg) {
  if (has_field(msg, 1)) {
    set_field_absent(msg, 1);
    msg->msg_size = 0;
  }
}
void WiFlyStringWireFormat_to_bytes(WiFlyStringWireFormat *msg, char *buf,
                                    uint32_t seq) {
  size_t offset = sizeof(msg->magic) + sizeof(msg->siphash);
  msg->seq = seq;
  msg->siphash = siphash24((char *)msg + offset, sizeof(*msg) - offset,
                           "scary spooky skeletons");
  memmove((void *)buf, (void *)msg, sizeof(*msg));
}

bool WiFlyStringWireFormat_from_bytes(WiFlyStringWireFormat *msg,
                                      const char *buf, uint32_t *seq_out) {
  size_t offset = sizeof(msg->magic) + sizeof(msg->siphash);
  memmove((void *)msg, (void *)buf, sizeof(*msg));
  *seq_out = msg->seq;
  return (siphash24(((char *)msg) + offset, sizeof(*msg) - offset,
                    "scary spooky skeletons") == msg->siphash);
}
