
/*****************************************************************************
 *                   AUTOMATICALLY GENERATED FILE. DO NOT EDIT.              *
 ****************************************************************************/
#ifndef _MOTORCOMMAND_H_
#define _MOTORCOMMAND_H_
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define MotorCommand_FIELD_MAX (3)
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint64_t siphash;
  uint32_t seq;
  unsigned char has_field[MotorCommand_FIELD_MAX / CHAR_BIT + 1];
  uint8_t direction;
  uint8_t dutyCycle;
  uint8_t mode;
} MotorCommand;
void MotorCommand_init(MotorCommand *msg);
/*
 * int8 direction
 */
bool MotorCommand_has_direction(const MotorCommand *msg);
uint8_t MotorCommand_direction(const MotorCommand *msg);
void MotorCommand_set_direction(MotorCommand *msg, uint8_t value);
void MotorCommand_clear_direction(MotorCommand *msg);
/*
 * int8 dutyCycle
 */
bool MotorCommand_has_dutyCycle(const MotorCommand *msg);
uint8_t MotorCommand_dutyCycle(const MotorCommand *msg);
void MotorCommand_set_dutyCycle(MotorCommand *msg, uint8_t value);
void MotorCommand_clear_dutyCycle(MotorCommand *msg);
/*
 * int8 mode
 */
bool MotorCommand_has_mode(const MotorCommand *msg);
uint8_t MotorCommand_mode(const MotorCommand *msg);
void MotorCommand_set_mode(MotorCommand *msg, uint8_t value);
void MotorCommand_clear_mode(MotorCommand *msg);
void MotorCommand_to_bytes(MotorCommand *msg, char *buf, uint32_t seq);
bool MotorCommand_from_bytes(MotorCommand *msg, const char *buf,
                             uint32_t *seq_out);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
