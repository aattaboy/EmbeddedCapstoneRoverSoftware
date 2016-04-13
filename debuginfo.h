#ifndef _DEBUGINFO_H
#define _DEBUGINFO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include <queue.h>

// Task identifiers
#define SENSOR1_IDENTIFIER (0)
#define RSSI_COLLECTOR_IDENTIFIER (1)
#define ENCODER1_IDENTIFIER (2)
#define ENCODER2_IDENTIFIER (3)
#define POSITION_CALCULATOR_IDENTIFIER (4)
#define PID_IDENTIFIER (5)
#define MOTOR1_IDENTIFIER (6)
#define MOTOR2_IDENTIFIER (7)
// errorcheck-specific IDs
#define UART_RX_IDENTIFIER (8)
#define UART_TX_IDENTIFIER (9)
#define PROTO_IDENTIFIER (10)
#define ERRORCHECK_IDENTIFIER (11)
#define WARNING_IDENTIFIER (12)
#define POSE_IDENTIFIER (13)

// Sensor1 event identifiers
#define Sensor1Receive_debugid (0)
#define Sensor1Send_debugid (1)
#define Sensor1LeftSensorValue (2)
#define Sensor1RightSensorValue (3)
#define Sensor1CenterSensorValue (4)

// RSSI Collector Event Identifiers
#define RSSICollectorReceivedMessage (0)
#define RSSICollectorBufferOverrun (1)
#define RSSICollectorTotalMsgsRxed (2)

// Encoder 1 events
#define Encoder1ReceivedMessage (0)
#define Encoder1LeftCount (1)
#define Encoder1RightCount (2)

// Encoder 2 events
#define Encoder2ReceivedMessage (0)

// Position Calculator Events
#define PositionCalculatorReceivedMessage (0)

// PID Events
#define PIDReceivedMessage (0)
#define PIDValueRecalculated (1)

// Motor1 events
#define Motor1ReceivedMessage (0)
#define Motor1CommandDutyCycleSet (1)
#define Motor1PIDDutyCycleSet (2)

// Motor2 events
#define Motor2ReceivedMessage (0)

// Pose events
#define XUpdated (0)
#define YUpdated (1)
#define YawUpdated (2)

void packAndSendDebugInfo(int32_t task_id, int32_t event_id, int32_t value);
void packAndSendDebugInfoFromISR(int32_t task_id, int32_t event_id,
                                 int32_t value,
                                 BaseType_t *higherPriorityTaskWoken);

#endif
