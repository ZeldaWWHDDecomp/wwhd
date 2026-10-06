/* WWHD Timer bridge reconstruction. */
#pragma once
#include "bindings.h"
namespace daObjTimer {
struct Act_c : fopAc_ac_c {
  be<s32> mMode;
  be<s32> mTimer;
  be<u8> mIsStop;
  u8 padding[3];
  void mode_count_init();
  void mode_wait_init();
  s32 Create();
  BOOL Execute();
  void mode_wait();
  void mode_count();
};
WWHD_OFFSET(Act_c, mMode, 0x3AC);
WWHD_OFFSET(Act_c, mTimer, 0x3B0);
WWHD_OFFSET(Act_c, mIsStop, 0x3B4);
WWHD_SIZE(Act_c, 0x3B8);
} // namespace daObjTimer
