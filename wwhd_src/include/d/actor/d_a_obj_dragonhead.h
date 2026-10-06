/* WWHD source reconstruction. */
#pragma once
#include "bindings.h"
namespace daObjDragonhead {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhs;
  gptr<J3DModel> mModel;
  u8 mCollisionStatus[0x3C];
  u8 mSphere[0x12C];
  be<u8> mAlpha, mSwitchOn;
  u8 _522[2];
  gptr<dBgW> mBgW;
  be<u8> mRegistered;
  u8 _529[3];
  cXyz mSphereCenter;
  Mtx34 mMatrix;
  BOOL CreateHeap();
  void set_mtx();
  void CreateInit();
};
WWHD_OFFSET(Act_c, mCollisionStatus, 0x3B8);
WWHD_OFFSET(Act_c, mSphere, 0x3F4);
WWHD_OFFSET(Act_c, mAlpha, 0x520);
WWHD_OFFSET(Act_c, mMatrix, 0x538);
WWHD_SIZE(Act_c, 0x568);
} // namespace daObjDragonhead
