
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

#include "UartRxData.pbo.h"
void UartRxData_charData_test() {
  UartRxData my_message;
  UartRxData_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, UartRxData_has_charData(&my_message));
  EXPECT_EQ(0x00, my_message.charData);
  UartRxData_set_charData(&my_message, 0x12);
  EXPECT_EQ(true, UartRxData_has_charData(&my_message));
  EXPECT_EQ((0x12), my_message.charData);
  EXPECT_EQ(0x12, UartRxData_charData(&my_message));
  UartRxData_clear_charData(&my_message);
  EXPECT_EQ(false, UartRxData_has_charData(&my_message));
  EXPECT_EQ(0x00, my_message.charData);
}
int main() {
  DRV_USART0_Initialize();
  UartRxData_charData_test();
  PRINT("Tests completed")
}
