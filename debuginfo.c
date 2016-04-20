#include "debug.h"
#include "debuginfo.h"
#include "generated/DebugInfo.pbo.h"
#include "util.h"

void packAndSendDebugInfo(int32_t task_id, int32_t event_id, int32_t value) {
  DebugInfo info;
  DebugInfo_init(&info);
  DebugInfo_set_identifier(&info, task_id);
  DebugInfo_set_debugID(&info, event_id);
  DebugInfo_set_data(&info, value);
  DebugInfo_set_cpuTicks(&info, getCpuCycles());
  static uint32_t seq;
  DebugInfo_to_bytes(&info, (char *)&info, __sync_fetch_and_add(&seq, 1));
  sendDebugInfo(&info);
}

void packAndSendDebugInfoFromISR(int32_t task_id, int32_t event_id,
                                 int32_t value,
                                 BaseType_t *higherPriorityTaskWoken) {
  DebugInfo info;
  DebugInfo_init(&info);
  DebugInfo_set_identifier(&info, task_id);
  DebugInfo_set_debugID(&info, event_id);
  DebugInfo_set_data(&info, value);
  DebugInfo_set_cpuTicks(&info, getCpuCycles());
  static uint32_t seq;
  DebugInfo_to_bytes(&info, (char *)&info, __sync_fetch_and_add(&seq, 1));
  sendDebugInfoFromISR(&info, higherPriorityTaskWoken);
}