#pragma once
#include "bindings.h"
namespace daObjJump {
struct Act_c : dBgS_MoveBgActor {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel;
  be<s32> mType;
  be<f32> mGroundY;
  be<s32> mMode;
  be<s16> mTimer;
  be<u8> mIsSetup;
  u8 pad;
  be<f32> mTargetScale, mSpringScale, mVelocity;
  be<u8> mIsRide, mRideStarted, mRideEnded, mRideCount, mUnriddenCount;
  be<u8> mIsPlayerRide, mPlayerPush, mPlayerCount;
  be<u8> mIsHeavyRide, mHeavyStarted, mHeavyEnded, mHeavyCount, mLightCount;
  u8 pad2;
  be<s16> mVibrationTimer;
  be<u8> mWobble;
  u8 pad3[3];
  s32 Mthd_Create();
  s32 Mthd_Delete();
  s32 CreateHeap();
  s32 Create();
  s32 Delete();
  void set_mtx();
  void init_mtx();
  void set_push_flag();
  void clear_push_flag();
  void calc_vib_pos();
  void mode_wait_init();
  void mode_wait();
  void mode_w_l_init();
  void mode_w_l();
  void mode_lower_init();
  void mode_lower();
  void mode_l_u_init();
  void mode_l_u();
  void mode_upper_init();
  void mode_upper();
  void mode_u_w_init();
  void mode_u_w();
  s32 Execute(Mtx34 **);
  s32 Draw();
};
WWHD_OFFSET(Act_c, mPhase, 0x3E0);
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_OFFSET(Act_c, mType, 0x3EC);
WWHD_OFFSET(Act_c, mMode, 0x3F4);
WWHD_OFFSET(Act_c, mTargetScale, 0x3FC);
WWHD_OFFSET(Act_c, mIsRide, 0x408);
WWHD_OFFSET(Act_c, mVibrationTimer, 0x416);
WWHD_SIZE(Act_c, 0x41C);
} // namespace daObjJump
