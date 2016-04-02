
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/
#ifndef _UARTRXDATA_H_
#define _UARTRXDATA_H_
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define UartRxData_FIELD_MAX (1)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[UartRxData_FIELD_MAX / CHAR_BIT + 1];
  uint8_t charData;
} UartRxData;
void UartRxData_init(UartRxData *msg);
/*
 * int8 charData
 */
bool UartRxData_has_charData(const UartRxData *msg);
uint8_t UartRxData_charData(const UartRxData *msg);
void UartRxData_set_charData(UartRxData *msg, uint8_t value);
void UartRxData_clear_charData(UartRxData *msg);
void UartRxData_to_bytes(UartRxData *msg, char *buf, uint32_t seq);
bool UartRxData_from_bytes(UartRxData *msg, const char *buf, uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
