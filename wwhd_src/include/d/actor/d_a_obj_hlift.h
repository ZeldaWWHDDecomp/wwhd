#pragma once
#include "d/d_bg_s.h"
namespace daObjHlift {
struct Act_c : dBgS_MoveBgActor {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel1, mModel2;
  be<s32> mDistance, mSize, mMode, mNextMode;
  be<s16> mVibTimer, mVibAngle;
  be<f32> mVibOffset, mMoveSpeed;
  be<s16> mLag;
  be<u8> mDemo;
  u8 _40f;
  be<s16> mEventId;
  u8 _412[2];
};
WWHD_OFFSET(Act_c, mPhase, 0x3E0);
WWHD_OFFSET(Act_c, mMode, 0x3F8);
WWHD_OFFSET(Act_c, mEventId, 0x410);
WWHD_SIZE(Act_c, 0x414);
} // namespace daObjHlift
