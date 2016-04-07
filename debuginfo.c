#include "debug.h"
#include "debuginfo.h"
#include "generated/DebugInfo.pbo.h"

void packAndSendDebugInfo(int32_t task_id, int32_t event_id, int32_t value) {
  DebugInfo info;
  DebugInfo_init(&info);
  DebugInfo_set_identifier(&info, task_id);
  DebugInfo_set_debugID(&info, event_id);
  DebugInfo_set_data(&info, value);
  uint32_t ticks;
  __asm__ __volatile__("mfc0 %0, $9\r\n" : "=r"(ticks));
  DebugInfo_set_cpuTicks(&info, ticks);
  static uint32_t seq;
  DebugInfo_to_bytes(&info, (char *)&info, __sync_fetch_and_add(&seq, 1));
  sendDebugInfo(&info);
}
