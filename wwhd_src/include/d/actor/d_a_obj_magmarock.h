#pragma once
#include "f_op/f_op_actor.h"
// HD layout: three quaternions separate rider tilt from the lava-ground normal.
struct daObjMagmarock_c : fopAc_ac_c {
  u8 _3AC[4];
  be<s16> mTiltAngle, mUnusedAngle, mRiderFrames;
  be<u8> mHasRider, mStartStay;
  gptr<u8> mpAppearEffect, mpAppearEffect2, mpBeforeLiftEffect, mpLiftEffect;
  be<f32> mCurrentQuat[4], mRiderQuat[4], mGroundQuat[4];
  be<s16> mProcessAdjustment, mProcessVtableIndex;
  be<u32> mProcess;
  u8 mPhase[8];
  gptr<u8> mpModel, mpBrkData;
  u8 mBrkController[0x78];
  gptr<u8> mpBckData;
  u8 mBckController[0x8C];
  be<f32> mBackgroundMatrix[12];
  gptr<u8> mpBackground;
  u8 mLighting[0x1C8];
  be<f32> mGroundTriangle[9];
  be<f32> mWobbleAmplitude, mBckFrame, mBrkFrame, mLiftTarget[3];
  be<s32> mTimer, mElapsedFrames;
  be<u32> mUnused;
  be<s16> mWobblePhase, mWobbleStep, mUnusedShort, mDemoState, mLifted,
      mBeforeLift;
};
WWHD_OFFSET(daObjMagmarock_c, mTiltAngle, 0x3B0);
WWHD_OFFSET(daObjMagmarock_c, mProcess, 0x3FC);
WWHD_OFFSET(daObjMagmarock_c, mpModel, 0x408);
WWHD_OFFSET(daObjMagmarock_c, mpBckData, 0x488);
WWHD_OFFSET(daObjMagmarock_c, mBackgroundMatrix, 0x518);
WWHD_OFFSET(daObjMagmarock_c, mGroundTriangle, 0x714);
WWHD_OFFSET(daObjMagmarock_c, mBeforeLift, 0x766);
WWHD_SIZE(daObjMagmarock_c, 0x768);
