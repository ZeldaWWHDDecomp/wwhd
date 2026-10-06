#pragma once
#include "bindings.h"
struct daObjQuake_c : fopAc_ac_c {
  be<f32> mDuration, mTimer, mStart, mEnd;
  be<u8> mType, mActive;
  u8 padding[2];
  u8 getPrmType();
  s32 getPrmPower();
  u8 getPrmSch();
  s32 Create();
  BOOL Execute();
  BOOL IsDelete();
};
WWHD_OFFSET(daObjQuake_c, mDuration, 0x3AC);
WWHD_OFFSET(daObjQuake_c, mType, 0x3BC);
WWHD_OFFSET(daObjQuake_c, mActive, 0x3BD);
WWHD_SIZE(daObjQuake_c, 0x3C0);
struct QuakeHIO {
  be<s8> mNo;
  u8 padding[3];
  be<u32> mCount;
  be<u8> mCameraOverride, mMotorOverride, mCameraPower, mMotorPower;
  be<u8> mCamera, mSound, mMotor, mExtra;
  be<u32> mVtable;
};
WWHD_SIZE(QuakeHIO, 0x14);
