// HD barrel actor layout, established from its original functions.
#pragma once
#include "bindings.h"
namespace daObjBarrel {
struct Act_c : fopAc_ac_c {
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    be<s32> mMode;
    be<s16> mVibrationAngle, mRotationAngle;
    be<s32> mTimer;
    be<f32> mLastGroundY;
    be<s16> mCarryAngle;
    u8 _73A[2];
    be<u8> mOnGround;
    be<s8> mInitTimer;
    be<u8> mForceExec, mSunk;
    cXyz mMove;
    be<s16> mWalkAngle;
    u8 _74E[2];
};
} // namespace daObjBarrel
WWHD_OFFSET(daObjBarrel::Act_c, mMode, 0x728);
WWHD_OFFSET(daObjBarrel::Act_c, mMove, 0x740);
WWHD_SIZE(daObjBarrel::Act_c, 0x750);
