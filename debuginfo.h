
#ifndef _DEBUGINFO_H
#define _DEBUGINFO_H

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

// Task identifiers
#define SENSOR1_IDENTIFIER (0)
#define RSSI_COLLECTOR_IDENTIFIER (1)
#define ENCODER1_IDENTIFIER (2)
#define ENCODER2_IDENTIFIER (3)
#define POSITION_CALCULATOR_IDENTIFIER (4)
#define PID_IDENTIFIER (5)
#define MOTOR1_IDENTIFIER (6)
#define MOTOR2_IDENTIFIER (7)

// Sensor1 event identifiers
#define Sensor1Receive_debugid (0)
#define Sensor1Send_debugid (1)

// RSSI Collector Event Identifiers
#define RSSICollectorReceivedMessage (0)

// Encoder 1 events
#define Encoder1ReceivedMessage (0)

// Encoder 2 events
#define Encoder2ReceivedMessage (0)

// Position Calculator Events
#define PositionCalculatorReceivedMessage (0)

// PID Events
#define PIDReceivedMessage (0)

// Motor1 events
#define Motor1ReceivedMessage (0)

// Motor2 events
#define Motor2ReceivedMessage (0)

#endif

#ifdef __cplusplus
}
#endif