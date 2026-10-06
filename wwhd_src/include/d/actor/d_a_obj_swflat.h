#pragma once
#include "d/d_bg_s.h"
namespace daObjSwflat {
struct Act_c : dBgS_MoveBgActor {
 request_of_phase_process_class mPhase; // 0x3E0
 gptr<J3DModel> mpModel; // 0x3E8
 be<u32> mpBrk; // 0x3EC
 u8 mStts[0x3C]; // 0x3F0
 u8 mCyl[0x130]; // 0x42C
 be<u32> mpInactiveEmitter; // 0x55C
 be<u32> mpActiveEmitter; // 0x560
 cXyz mEffectPos; // 0x564
 be<s16> mFrame; // 0x570
 be<s16> mPressEndFrame; // 0x572
 be<s16> mEventTimer; // 0x574
 be<s16> mLockTimer; // 0x576
 be<u8> mType; // 0x578
 be<u8> mPressed; // 0x579
 be<u8> mWasPressed; // 0x57A
 be<u8> mSwitch; // 0x57B
 be<u8> mSwitch2; // 0x57C
 be<u8> mInactiveAlpha; // 0x57D
 be<u8> mActiveAlpha; // 0x57E
 be<u8> mLocked; // 0x57F
 be<u8> mEventState; // 0x580
 u8 _581[3];
};
WWHD_OFFSET(Act_c,mCyl,0x42C);
WWHD_OFFSET(Act_c,mFrame,0x570);
WWHD_SIZE(Act_c,0x584);
}
