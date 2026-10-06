#pragma once
#include "f_op/f_op_actor.h"
namespace daStone2 {
struct Act_c : fopAc_ac_c {
 be<u32> mpBgW; // 3AC
 u8 _3B0[0x30];
 request_of_phase_process_class mPhase; // 3E0
 gptr<J3DModel> mpModel; // 3E8
 u8 mAcch[0x1C4]; // 3EC
 u8 mAcchCir[0x40]; // 5B0
 u8 mStts[0x3C]; // 5F0
 be<u32> mAtFlags; // 62C
 u8 _630[0x14];
 be<u32> mTgFlags; // 644
 u8 _648[0x10];
 be<u32> mCoFlags; // 658
 u8 _65C[0x100];
 be<s32> mType,mMode,mDemo; // 75C
 be<s16> mEvent;
 be<u16> mPrmZ;
 be<s16> mFineTimer;
 be<u8> mPrmInitialized,mForceUpdate,mLifted,mAppears,mBgLockDelay;
 u8 _773;
 u8 mBreakSmoke[0x20]; // 774
 cXyz mSmokePosition; // 794
 u8 mLiftSmoke[0x20]; // 7A0
 be<u8> mSmokeStarted,mSmokeActive;
 u8 _7C2[2];
};
WWHD_OFFSET(Act_c,mpModel,0x3E8);
WWHD_OFFSET(Act_c,mType,0x75C);
WWHD_OFFSET(Act_c,mBreakSmoke,0x774);
WWHD_OFFSET(Act_c,mSmokeActive,0x7C1);
WWHD_SIZE(Act_c,0x7C4);
}
