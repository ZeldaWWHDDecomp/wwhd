#pragma once
#include "f_op/f_op_actor.h"
namespace daObjEff {
// HD profile101C8F9C: six transient particle effects share this actor.
struct Act_c : fopAc_ac_c {
  be<s32> type;
  be<u32> callback;
};
struct SmokeCallback {
  be<u32> vtable;
  u8 state[0x1C];
  be<s32> life;
};
} // namespace daObjEff
WWHD_SIZE(daObjEff::Act_c, 0x3B4);
WWHD_OFFSET(daObjEff::Act_c, type, 0x3AC);
WWHD_OFFSET(daObjEff::Act_c, callback, 0x3B0);
WWHD_SIZE(daObjEff::SmokeCallback, 0x24);
WWHD_OFFSET(daObjEff::SmokeCallback, life, 0x20);
