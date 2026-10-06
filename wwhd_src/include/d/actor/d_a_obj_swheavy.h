#pragma once
#include "f_op/f_op_actor.h"
namespace daObjSwheavy {
struct Act_c : fopAc_ac_c {
 request_of_phase_process_class mPhase; // 0x3AC
 gptr<J3DModel> mpModel1,mpModel2; // 0x3B4
 be<u32> mpBgW1,mpBgW2; // 0x3BC
 Mtx34 mMtx1,mMtx2; // 0x3C4
 be<u32> mType,mMode; // 0x424
 be<u8> mRiding,mPrevRiding; // 0x42C
 be<s16> mMiniPushTimer; // 0x42E
 be<u8> mMiniPushFlg,mHeavyRiding,mPrevHeavyRiding; // 0x430
 u8 _433;
 be<s16> mPushTimer; // 0x434
 be<u8> mPushFlg,mChangingState; // 0x436
 be<f32> mTargetHFrac,mCurHFrac,mVSpeed,mModelHeight,mBgAim; // 0x438
 be<s16> mBgAimTimer; // 0x44C
 u8 _44E[2];
 be<f32> mBgAimTarget,mBgHFrac,mTopPos; // 0x450
};
WWHD_OFFSET(Act_c,mMtx1,0x3C4);
WWHD_OFFSET(Act_c,mType,0x424);
WWHD_OFFSET(Act_c,mTopPos,0x458);
WWHD_SIZE(Act_c,0x45C);
}
