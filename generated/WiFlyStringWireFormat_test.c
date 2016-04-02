
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

#include "WiFlyStringWireFormat.pbo.h"
void WiFlyStringWireFormat_msg_test() {
  WiFlyStringWireFormat my_message;
  WiFlyStringWireFormat_init(&my_message);
  EXPECT_EQ(0xDEADBEEF, my_message.magic);
  EXPECT_EQ(false, WiFlyStringWireFormat_has_msg(&my_message));
  EXPECT_EQ(0, my_message.msg_size);

  const char *test_str = "The quick brown fox jumps over the lazy dog";
  WiFlyStringWireFormat_set_msg(&my_message, test_str, strlen(test_str));
  EXPECT_EQ(true, WiFlyStringWireFormat_has_msg(&my_message));

  char buf[150];
  WiFlyStringWireFormat_msg(&my_message, buf, sizeof(buf));

  EXPECT_EQ(0, strcmp(buf, test_str));
  WiFlyStringWireFormat_clear_msg(&my_message);
  EXPECT_EQ(false, WiFlyStringWireFormat_has_msg(&my_message));
  EXPECT_EQ(0, my_message.msg_size);

  // Test string truncation
  memset(buf, 3, sizeof(buf));
  buf[sizeof(buf) - 1] = 0;
  WiFlyStringWireFormat_set_msg(&my_message, buf, strlen(buf));

  char buf2[150];
  WiFlyStringWireFormat_msg(&my_message, buf2, sizeof(buf2));

  buf[100] = 0;
  EXPECT_EQ(100, strlen(buf2));
  EXPECT_EQ(0, strcmp(buf, buf2));
}
int main() {
  DRV_USART0_Initialize();
  WiFlyStringWireFormat_msg_test();
  PRINT("Tests completed")
}
