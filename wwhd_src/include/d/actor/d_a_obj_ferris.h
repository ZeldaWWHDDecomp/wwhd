#pragma once
#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"
namespace daObjFerris {
struct Act_c : fopAc_ac_c {
  gptr<u8> mpBgW[6];
  be<s32> mRideState[6];
  cXyz mRidePos;
  u8 mPhs[8];
  dCcD_Stts mCylStatus[5];
  u8 mCylinders[5][0x130];
  dCcD_Stts mBackStatus[5];
  u8 mBackCylinders[5][0x130];
  dCcD_Stts mSphereStatus[5];
  u8 mSpheres[5][0x12C];
  gptr<u8> mpModel[6];
  be<f32> mMtx[6][12];
  be<s16> mRotTimer, mRotState, mRotAngle, mRotSpeed;
  be<s16> mEventIdxVive, mEventIdxStart, mFrameTimer, mGondolaWaveAngle,
      mGondolaWaveTimer;
  be<s16> mRideWaveAngle[5], mRideWaveTarget[5];
  be<s16> mEventIdx, mEventState, mUnused;
  void set_mtx(s32);
  void init_mtx();
  BOOL create_heap();
  BOOL create();
  BOOL remove();
  BOOL set_event(s16);
  bool now_event(s16);
  void angle_mng();
  void rot_mng();
  void exe_event();
  void make_lean();
  void set_collision();
  BOOL execute();
  BOOL draw();
};
WWHD_OFFSET(Act_c, mpBgW, 0x3AC);
WWHD_OFFSET(Act_c, mpModel, 0x1930);
WWHD_OFFSET(Act_c, mRotTimer, 0x1A68);
WWHD_OFFSET(Act_c, mEventState, 0x1A90);
WWHD_SIZE(Act_c, 0x1A94);
} // namespace daObjFerris
