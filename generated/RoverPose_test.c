
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

#include "RoverPose.pbo.h"
void RoverPose_xPosition_test() {
  RoverPose my_message;
  RoverPose_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, RoverPose_has_xPosition(&my_message));
  EXPECT_EQ(0x00000000, my_message.xPosition);
  RoverPose_set_xPosition(&my_message, 0x12345678);
  EXPECT_EQ(true, RoverPose_has_xPosition(&my_message));
  EXPECT_EQ(htonl(0x12345678), my_message.xPosition);
  EXPECT_EQ(0x12345678, RoverPose_xPosition(&my_message));
  RoverPose_clear_xPosition(&my_message);
  EXPECT_EQ(false, RoverPose_has_xPosition(&my_message));
  EXPECT_EQ(0x00000000, my_message.xPosition);
}
void RoverPose_yPosition_test() {
  RoverPose my_message;
  RoverPose_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, RoverPose_has_yPosition(&my_message));
  EXPECT_EQ(0x00000000, my_message.yPosition);
  RoverPose_set_yPosition(&my_message, 0x12345678);
  EXPECT_EQ(true, RoverPose_has_yPosition(&my_message));
  EXPECT_EQ(htonl(0x12345678), my_message.yPosition);
  EXPECT_EQ(0x12345678, RoverPose_yPosition(&my_message));
  RoverPose_clear_yPosition(&my_message);
  EXPECT_EQ(false, RoverPose_has_yPosition(&my_message));
  EXPECT_EQ(0x00000000, my_message.yPosition);
}
void RoverPose_yaw_test() {
  RoverPose my_message;
  RoverPose_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, RoverPose_has_yaw(&my_message));
  EXPECT_EQ(0x0000, my_message.yaw);
  RoverPose_set_yaw(&my_message, 0x1234);
  EXPECT_EQ(true, RoverPose_has_yaw(&my_message));
  EXPECT_EQ(htons(0x1234), my_message.yaw);
  EXPECT_EQ(0x1234, RoverPose_yaw(&my_message));
  RoverPose_clear_yaw(&my_message);
  EXPECT_EQ(false, RoverPose_has_yaw(&my_message));
  EXPECT_EQ(0x0000, my_message.yaw);
}
int main() {
  DRV_USART0_Initialize();
  RoverPose_xPosition_test();
  RoverPose_yPosition_test();
  RoverPose_yaw_test();
  PRINT("Tests completed")
}
