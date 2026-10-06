/* WWHD flame pillar. */
#pragma once
#include "bindings.h"
namespace daObjFlame {
struct Act_c : fopAc_ac_c {
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel; be<u32> mpBtk,mpBrk;
    dCcD_Stts mStts; dCcD_Cps mCps;
    cXyz mCpsP0,mCpsP1; be<f32> mCpsRad; be<u8> mCollision; u8 _551[3];
    be<s32> mType,mModeProc; be<f32> mTimer,mHeight;
    be<u32> mpEmitter0,mpEmitter1,mpEmitter2;
    be<u8> mEm0State,mEm1State,mEm2State,m457; be<s8> mReverb; be<u8> m459,mKiActive; u8 _577;
    be<s32> mKiDelay,mKiTimer,mKiCount; be<s16> mRotY; u8 _586[2];
    be<f32> m46C,mExtraScaleY; cXyz mOrigScale; u8 _59C[0x80];
    s32 prm(s32 width,s32 shift) const { return gabi::call<s32>(0x02345AC0,this,width,shift); }
    void set_switch(); void ki_init(); void mode_wait(); u32 se_fireblast_omen(); void mode_wait2();
    u32 mode_l_before(); void mode_l_u(); void mode_u(); void mode_u_l(); void mode_l_after();
};
WWHD_OFFSET(Act_c,mPhs,0x3AC);
WWHD_OFFSET(Act_c,mCps,0x3FC);
WWHD_OFFSET(Act_c,mType,0x554);
WWHD_OFFSET(Act_c,mOrigScale,0x590);
WWHD_SIZE(Act_c,0x61C);
}
