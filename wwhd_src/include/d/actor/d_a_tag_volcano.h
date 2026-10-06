#pragma once
#include "bindings.h"
namespace daTagvolcano {
struct Act_c : fopAc_ac_c {
  u8 unused[8];
  be<u8> mTimerCreated, mTimerStarted;
  u8 padding[2];
  be<s32> mType, mCountdown;
  be<u8> mEventPending;
  u8 tail[3];
  s32 Create();
  BOOL check_timer_clear();
  BOOL Delete();
  BOOL Execute();
};
WWHD_OFFSET(Act_c, mTimerCreated, 0x3B4);
WWHD_OFFSET(Act_c, mType, 0x3B8);
WWHD_OFFSET(Act_c, mCountdown, 0x3BC);
WWHD_OFFSET(Act_c, mEventPending, 0x3C0);
WWHD_SIZE(Act_c, 0x3C4);
} // namespace daTagvolcano
