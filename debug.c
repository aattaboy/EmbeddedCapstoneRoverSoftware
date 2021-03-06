/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    debug.c

  Summary:
    This file contains the source code for the debugging functions.

  Description:
 * This file allows to do the following:
 * 1. Write to port E

 *******************************************************************************/

#include "debug.h"
#include "debuginfo.h"
//#include "uart_transmitter_public.h"
#include "system/debug/sys_debug.h"

#define USE_UART_DEBUG 0

// Write a char to port E using built-in function
void writeToDebug(char val) {
#if USE_UART_DEBUG
  struct UART_TRANSMITTER_VARIANT var;
  var.type = WRITE_CHAR;
  var.writeChar = val;
  sendToUartQueue(&var);
#else
// Write a char out to PORT E
// PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_E, val);
#endif
}

// Sends DebugInfo messages
void sendDebugInfo(DebugInfo *info) {
  struct UART_TRANSMITTER_VARIANT var;
  var.data.debug_info = *info;
  var.type = DEBUG_INFO;
  sendToUartQueue(&var);
}

void sendDebugInfoFromISR(DebugInfo *info,
                          BaseType_t *higherPriorityTaskWoken) {
  struct UART_TRANSMITTER_VARIANT var;
  var.data.debug_info = *info;
  var.type = DEBUG_INFO;
  sendToUartQueueFromISR(&var, higherPriorityTaskWoken);
}

// Error checking for each function
void errorCheck(int taskID, int lineNum) {
  // Set port D to 0xF
  PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_D, 0xF);
  // Send message to monitor
  packAndSendDebugInfo(ERRORCHECK_IDENTIFIER, taskID, lineNum);
  // Ends all tasks
  vTaskSuspendAll();
  // Makes the program not restart
  while (1) {
    SYS_DEBUG_BreakPoint();
  }
}

void warning(int taskID, int lineNum) {
  // packAndSendDebugInfo(WARNING_IDENTIFIER, taskID, lineNum);
}