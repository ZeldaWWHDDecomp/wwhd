#pragma once
#include "bindings.h"
namespace daObjSwlight {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel;
  u8 mBtk[0x74];
  u8 mBck[0x8C];
  gptr<dBgW> mBg;
  dCcD_Tri mTri[8];
  dCcD_Stts mStts[8];
  be<s32> mType, mMode;
  be<s16> mTimer;
  u8 _1126[2];
  be<f32> mPower;
  u8 _112c[4];
  Mtx34 mBgMtx;
};
} // namespace daObjSwlight
WWHD_OFFSET(daObjSwlight::Act_c, mModel, 0x3B4);
WWHD_OFFSET(daObjSwlight::Act_c, mBg, 0x4B8);
WWHD_OFFSET(daObjSwlight::Act_c, mTri, 0x4BC);
WWHD_OFFSET(daObjSwlight::Act_c, mMode, 0x1120);
WWHD_OFFSET(daObjSwlight::Act_c, mBgMtx, 0x1130);
WWHD_SIZE(daObjSwlight::Act_c, 0x1160);
