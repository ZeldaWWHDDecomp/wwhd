/* HD Hyrule Castle triangle blocks; actor-local layout. */
#pragma once
#include "bindings.h"
namespace daObjTribox {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mBlockModel, mLightModel;
  u8 mBlockBrk[0x78], mLightBrk[0x78];
  gptr<u8> mBackground;
  u8 mBackgroundMatrix[0x30];
  be<s32> mState, mMode;
  be<s16> mModeTimer;
  u8 _4EA[2];
  be<u8> mPushInput;
  u8 _4ED[3];
  be<s32> mPushDirection, mPushSide, mPull;
  be<u8> mPreviousPushInput;
  u8 _4FD[3];
  be<s32> mPreviousDirection, mPreviousSide, mPreviousPull;
  be<u8> mWalkInput;
  u8 _50D[3];
  be<s32> mWalkDirection, mWalkSide, mWalkPull;
  be<s16> mHoldTimer;
  u8 _51E[2];
  cXyz mWalkStart;
  be<s32> mRotationStep, mPivot;
  be<s16> mRotationSign;
  u8 _536[2];
  u8 mSmokeCallback[0x30];
  be<s16> mSmokeAngleZ;
  be<u8> mLeader;
  u8 _56B;
  be<f32> mSinkVelocity;
  be<u8> mFlash, mDemoActive;
  be<s16> mEvent;
  u8 mSinkCallbacks[3][0x20];
  be<u8> mSinkSmokeActive, mSinkVibrationActive;
  be<s16> mSoundTimer;
};
WWHD_SIZE(Act_c, 0x5D8);
WWHD_OFFSET(Act_c, mPhase, 0x3AC);
WWHD_OFFSET(Act_c, mBlockModel, 0x3B4);
WWHD_OFFSET(Act_c, mLightModel, 0x3B8);
WWHD_OFFSET(Act_c, mBlockBrk, 0x3BC);
WWHD_OFFSET(Act_c, mLightBrk, 0x434);
WWHD_OFFSET(Act_c, mBackground, 0x4AC);
WWHD_OFFSET(Act_c, mBackgroundMatrix, 0x4B0);
WWHD_OFFSET(Act_c, mState, 0x4E0);
WWHD_OFFSET(Act_c, mMode, 0x4E4);
WWHD_OFFSET(Act_c, mModeTimer, 0x4E8);
WWHD_OFFSET(Act_c, mPushInput, 0x4EC);
WWHD_OFFSET(Act_c, mWalkStart, 0x520);
WWHD_OFFSET(Act_c, mRotationStep, 0x52C);
WWHD_OFFSET(Act_c, mPivot, 0x530);
WWHD_OFFSET(Act_c, mRotationSign, 0x534);
WWHD_OFFSET(Act_c, mSmokeCallback, 0x538);
WWHD_OFFSET(Act_c, mLeader, 0x56A);
WWHD_OFFSET(Act_c, mSinkVelocity, 0x56C);
WWHD_OFFSET(Act_c, mFlash, 0x570);
WWHD_OFFSET(Act_c, mEvent, 0x572);
WWHD_OFFSET(Act_c, mSinkSmokeActive, 0x5D4);
WWHD_OFFSET(Act_c, mSoundTimer, 0x5D6);
WWHD_OFFSET(Act_c, mSinkCallbacks, 0x574);
} // namespace daObjTribox
