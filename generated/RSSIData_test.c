
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "system_config/default/framework/driver/usart/drv_usart_static.h"

#define PRINT(buf)                                                             \
  do {                                                                         \
    size_t i;                                                                  \
    for (i = 0; i < strlen((buf)); i++) {                                      \
      DRV_USART0_WriteByte((buf)[i]);                                          \
    }                                                                          \
  } while (0);

#define EXPECT_EQ(a, b)                                                        \
  if (a != b) {                                                                \
    char buf[150];                                                             \
    sprintf(buf, "Test Failed: %s %i", __FILE__, __LINE__);                    \
    PRINT(buf);                                                                \
  }

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

#include "RSSIData.pbo.h"
void RSSIData_bssid_test() {
  RSSIData my_message;
  RSSIData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, RSSIData_has_bssid(&my_message));
  EXPECT_EQ(0, my_message.bssid_size);

  const char *test_str = "The quick brown fox jumps over the lazy dog";
  RSSIData_set_bssid(&my_message, test_str, strlen(test_str));
  EXPECT_EQ(true, RSSIData_has_bssid(&my_message));

  char buf[150];
  RSSIData_bssid(&my_message, buf, sizeof(buf));

  EXPECT_EQ(0, strcmp(buf, test_str));
  RSSIData_clear_bssid(&my_message);
  EXPECT_EQ(false, RSSIData_has_bssid(&my_message));
  EXPECT_EQ(0, my_message.bssid_size);

  // Test string truncation
  memset(buf, 3, sizeof(buf));
  buf[sizeof(buf) - 1] = 0;
  RSSIData_set_bssid(&my_message, buf, strlen(buf));

  char buf2[150];
  RSSIData_bssid(&my_message, buf2, sizeof(buf2));

  buf[100] = 0;
  EXPECT_EQ(100, strlen(buf2));
  EXPECT_EQ(0, strcmp(buf, buf2));
}
void RSSIData_rssi_test() {
  RSSIData my_message;
  RSSIData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, RSSIData_has_rssi(&my_message));
  EXPECT_EQ(0x0000, my_message.rssi);
  RSSIData_set_rssi(&my_message, 0x1234);
  EXPECT_EQ(true, RSSIData_has_rssi(&my_message));
  EXPECT_EQ(htons(0x1234), my_message.rssi);
  EXPECT_EQ(0x1234, RSSIData_rssi(&my_message));
  RSSIData_clear_rssi(&my_message);
  EXPECT_EQ(false, RSSIData_has_rssi(&my_message));
  EXPECT_EQ(0x0000, my_message.rssi);
}
void RSSIData_pairSeq_test() {
  RSSIData my_message;
  RSSIData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, RSSIData_has_pairSeq(&my_message));
  EXPECT_EQ(0x00000000, my_message.pairSeq);
  RSSIData_set_pairSeq(&my_message, 0x12345678);
  EXPECT_EQ(true, RSSIData_has_pairSeq(&my_message));
  EXPECT_EQ(htonl(0x12345678), my_message.pairSeq);
  EXPECT_EQ(0x12345678, RSSIData_pairSeq(&my_message));
  RSSIData_clear_pairSeq(&my_message);
  EXPECT_EQ(false, RSSIData_has_pairSeq(&my_message));
  EXPECT_EQ(0x00000000, my_message.pairSeq);
}
void RSSIData_frameSize_test() {
  RSSIData my_message;
  RSSIData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, RSSIData_has_frameSize(&my_message));
  EXPECT_EQ(0x00000000, my_message.frameSize);
  RSSIData_set_frameSize(&my_message, 0x12345678);
  EXPECT_EQ(true, RSSIData_has_frameSize(&my_message));
  EXPECT_EQ(htonl(0x12345678), my_message.frameSize);
  EXPECT_EQ(0x12345678, RSSIData_frameSize(&my_message));
  RSSIData_clear_frameSize(&my_message);
  EXPECT_EQ(false, RSSIData_has_frameSize(&my_message));
  EXPECT_EQ(0x00000000, my_message.frameSize);
}
void RSSIData_frameNum_test() {
  RSSIData my_message;
  RSSIData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, RSSIData_has_frameNum(&my_message));
  EXPECT_EQ(0x00000000, my_message.frameNum);
  RSSIData_set_frameNum(&my_message, 0x12345678);
  EXPECT_EQ(true, RSSIData_has_frameNum(&my_message));
  EXPECT_EQ(htonl(0x12345678), my_message.frameNum);
  EXPECT_EQ(0x12345678, RSSIData_frameNum(&my_message));
  RSSIData_clear_frameNum(&my_message);
  EXPECT_EQ(false, RSSIData_has_frameNum(&my_message));
  EXPECT_EQ(0x00000000, my_message.frameNum);
}
int main() {
  DRV_USART0_Initialize();
  RSSIData_bssid_test();
  RSSIData_rssi_test();
  RSSIData_pairSeq_test();
  RSSIData_frameSize_test();
  RSSIData_frameNum_test();
  PRINT("Tests completed")
}
