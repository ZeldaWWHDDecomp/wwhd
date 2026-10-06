#pragma once
#include "bindings.h"
namespace daObjSwpush {
struct Attr_c {
  be<u32> mHeapSize, mFlags;
  be<f32> mScale;
  be<u32> mKbotaResName, mHhbotResName, mBgArcName, mModelArcName, mBtpArcName;
  be<s16> mBgResIndex, mModelResIndices[2], mBtpResIndex;
  be<f32> mSpring, mSpeedDecay, mPushSpeed0, m34;
  be<s16> m38, mMiniPushDelay1, mPushDelay, mMiniPushDelay2;
  be<f32> m40, m44;
  be<s16> mPauseDuration;
  u8 _4a[2];
};
WWHD_SIZE(Attr_c, 0x4c);
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mKbotaPhs, mHhbotPhs;
  be<u32> mpBgW;
  Mtx34 mMtx;
  be<f32> m2D4;
  gptr<J3DModel> mpModel;
  u8 mBtpAnm[0x74];
  be<s32> mType, mMode, mDemoMode;
  be<s16> mPauseTimer, mEventID;
  be<u16> mPrmZ;
  be<u8> mPrmZInit, mVibTimer, mRidingMode, mPrevRiding;
  be<s16> mMiniPushTimer;
  be<u8> mMiniPushFlg, mHeavyRiding, mPrevHeavyRiding;
  u8 _487;
  be<s16> mPushTimer;
  be<u8> mPushFlg, mChangingState;
  be<f32> mTargetHFrac, mCurHFrac, mSpeed, m31C, m320;
  be<s16> m324;
  u8 _4a2[2];
  be<f32> m328, m32C, mTopPos;
  be<s16> mDebounceTimer;
  u8 _4b2[2];
  void prmZ_init();
  bool is_switch2();
  s32 create_res_load();
  bool create_heap();
  void set_mtx();
  void init_mtx();
  void set_btp_frame();
  void calc_top_pos();
  void set_push_flag();
  void top_bg_aim_req(f32, s16);
  void mode_upper_init();
  void mode_lower_init();
  void mode_u_l_init();
  void mode_l_u_init();
  void mode_upper();
  void mode_lower();
  void mode_u_l();
  void mode_l_u();
  void demo_non_init();
  void demo_non();
  void demo_reqPause_init();
  void demo_reqPause();
  void demo_runPause_init();
  void demo_runPause();
  void demo_stop_puase();
  void demo_reqSw_init();
  void demo_reqSw();
  void demo_runSw_init();
  void demo_runSw();
  s32 Mthd_Create();
  s32 Mthd_Delete();
  s32 Mthd_Execute();
  s32 Mthd_Draw();
  Attr_c &attr(u32 file, u32 msg) {
    if ((u32)mType >= 4)
      JUT_ASSERT_fail(STR(file), 0x1ba, STR(msg));
    return *gabi::at<Attr_c>(0x10030400 + (u32)mType * 0x4c);
  }
};
WWHD_OFFSET(Act_c, mpModel, 0x3f4);
WWHD_OFFSET(Act_c, mType, 0x46c);
WWHD_OFFSET(Act_c, mDebounceTimer, 0x4b0);
WWHD_SIZE(Act_c, 0x4b4);
} // namespace daObjSwpush
