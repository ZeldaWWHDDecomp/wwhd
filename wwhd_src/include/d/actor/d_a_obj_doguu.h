/* WWHD Triangle Island goddess statue. */
#pragma once
#include "bindings.h"
namespace daObjDoguu {
struct Act_c : fopAc_ac_c {
  u8 _3AC[0x7DC - 0x3AC];
  request_of_phase_process_class mPhs;
  gptr<J3DModel> mMain, mHead, mBody, mCrystal;
  u8 mBrk[0x78], mHeadBck[0x8C], mBodyBck[0x8C], mCrystalBck[0x8C];
  u8 _A10[0xB80 - 0xA10];
  be<s32> mVariant;
  be<u32> mHeadJoint, mCrystalJoint;
  be<u8> mHasPearl, mUseBody, mEyeFlash, mLightActive;
  be<s16> mDemo1, mDemo2, mDemo3, mMegami;
  be<s32> mState;
  be<s8> mAction;
  u8 _B9D[3];
  cXyz mGoal;
  be<s32> mTimer, mLightTimer;
  be<f32> mColorRatio;
  be<u32> mShape;
  cXyz mLightPos;
  u8 mLightInfluence[0x24];
  be<f32> mLightPower;
  s32 getFinishEventCount();
  void setFinishMyEvent();
  u32 getMsg();
  u16 next_msgStatus(be<u32> *);
  BOOL CreateHeap();
  void setPointLight();
  void set_mtx();
  void setGoal(s32);
  void setPlayerAngle(s32);
  void setQuake(s32);
  void setJDemo(s32);
};
WWHD_OFFSET(Act_c, mVariant, 0xB80);
WWHD_OFFSET(Act_c, mGoal, 0xBA0);
WWHD_OFFSET(Act_c, mLightInfluence, 0xBC8);
WWHD_SIZE(Act_c, 0xBF0);
} // namespace daObjDoguu
