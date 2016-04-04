
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

#include "IRSensorData.pbo.h"
void IRSensorData_front_test() {
  IRSensorData my_message;
  IRSensorData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, IRSensorData_has_front(&my_message));
  EXPECT_EQ(0x0000, my_message.front);
  IRSensorData_set_front(&my_message, 0x1234);
  EXPECT_EQ(true, IRSensorData_has_front(&my_message));
  EXPECT_EQ(htons(0x1234), my_message.front);
  EXPECT_EQ(0x1234, IRSensorData_front(&my_message));
  IRSensorData_clear_front(&my_message);
  EXPECT_EQ(false, IRSensorData_has_front(&my_message));
  EXPECT_EQ(0x0000, my_message.front);
}
void IRSensorData_left_test() {
  IRSensorData my_message;
  IRSensorData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, IRSensorData_has_left(&my_message));
  EXPECT_EQ(0x0000, my_message.left);
  IRSensorData_set_left(&my_message, 0x1234);
  EXPECT_EQ(true, IRSensorData_has_left(&my_message));
  EXPECT_EQ(htons(0x1234), my_message.left);
  EXPECT_EQ(0x1234, IRSensorData_left(&my_message));
  IRSensorData_clear_left(&my_message);
  EXPECT_EQ(false, IRSensorData_has_left(&my_message));
  EXPECT_EQ(0x0000, my_message.left);
}
void IRSensorData_right_test() {
  IRSensorData my_message;
  IRSensorData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, IRSensorData_has_right(&my_message));
  EXPECT_EQ(0x0000, my_message.right);
  IRSensorData_set_right(&my_message, 0x1234);
  EXPECT_EQ(true, IRSensorData_has_right(&my_message));
  EXPECT_EQ(htons(0x1234), my_message.right);
  EXPECT_EQ(0x1234, IRSensorData_right(&my_message));
  IRSensorData_clear_right(&my_message);
  EXPECT_EQ(false, IRSensorData_has_right(&my_message));
  EXPECT_EQ(0x0000, my_message.right);
}
int main() {
  DRV_USART0_Initialize();
  IRSensorData_front_test();
  IRSensorData_left_test();
  IRSensorData_right_test();
  PRINT("Tests completed")
}
