/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    debug.h

  Summary:
    This header file provides prototypes and definitions for the debug
functions.

  Description:

*******************************************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

#include "generated/DebugInfo.pbo.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include <queue.h>
#ifdef __cplusplus

extern "C" {

#endif

void writeToDebug(char val);

void sendDebugInfo(DebugInfo *info);
void sendDebugInfoFromISR(DebugInfo *info, BaseType_t *higherPriorityTaskWoken);

void errorCheck(const char *const filename, int lineNum);

#endif

#ifdef __cplusplus
}
#endif
