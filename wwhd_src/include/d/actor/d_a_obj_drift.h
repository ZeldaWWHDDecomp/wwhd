/* WWHD floating tree platform reconstruction. */
#pragma once
#include "bindings.h"
namespace daObjDrift {
struct Act_c : dBgS_MoveBgActor {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel;
  u8 mCollisionStatus[0x3C], mCylinder[0x130];
  be<s32> mType;
  Mtx34 mMtx;
  be<u32> mFlowerPid;
  be<s32> mMode;
  be<f32> mRideYOff, mTiltTargetZ, mTiltTargetX, mTiltVelZ, mTiltVelX, mTiltZ,
      mTiltX;
  be<s16> mWavePhaseX, mWavePhaseY, mWavePhaseZ;
  be<u8> mRideFlag, mUnusedFlag;
  be<s16> mTargetAngleY, mAngleChaseY;
  cXyz mHitDir, mScratch, mPrevPos;
  be<f32> mHeight, mPrevHeight, mEffect, mUnusedEffect, mRotTimer;
  s32 Mthd_Create();
  BOOL Mthd_Delete();
  BOOL CreateHeap();
  BOOL Create();
  void set_mtx();
  void init_mtx();
  void calc_flower_param(cXyz *, csXyz *);
  void make_flower();
  void set_flower_current();
  void mode_wait_init();
  void mode_wait();
  void mode_rot_init();
  void mode_rot();
  void set_current();
  BOOL Execute(gptr<Mtx34> *);
  BOOL Draw();
};
WWHD_OFFSET(Act_c, mCylinder, 0x428);
WWHD_OFFSET(Act_c, mType, 0x558);
WWHD_OFFSET(Act_c, mMtx, 0x55C);
WWHD_OFFSET(Act_c, mTiltZ, 0x5A8);
WWHD_OFFSET(Act_c, mRotTimer, 0x5F0);
WWHD_SIZE(Act_c, 0x5F4);
} // namespace daObjDrift
