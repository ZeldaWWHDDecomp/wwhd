#pragma once
#include "f_op/f_op_actor.h"
namespace daObjCorrect {
struct Act_c : fopAc_ac_c {
  be<s16> mSearchValid;
  u8 _3ae[2];
  be<s32> mMode, mDemo, mType;
  be<s16> mEventId;
  u8 _3be[2];
  be<f32> mRadiusSq;
};
WWHD_OFFSET(Act_c, mSearchValid, 0x3AC);
WWHD_OFFSET(Act_c, mType, 0x3B8);
WWHD_OFFSET(Act_c, mRadiusSq, 0x3C0);
WWHD_SIZE(Act_c, 0x3C4);
} // namespace daObjCorrect
