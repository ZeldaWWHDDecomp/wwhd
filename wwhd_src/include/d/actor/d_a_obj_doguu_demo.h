/* WWHD Tower of the Gods emergence statue.
 */
#pragma once
#include "bindings.h"
namespace daObjDoguuDemo {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhs;
  gptr<J3DModel> mModel;
  gptr<dBgW> mBgW;
  Mtx34 mMatrix;
  be<u8> mRegistered, mUnusedParam;
  u8 _3EE[2];
  be<u32> mShape;
  BOOL CreateHeap();
  void set_mtx();
  void CreateInit();
};
WWHD_OFFSET(Act_c, mPhs, 0x3AC);
WWHD_OFFSET(Act_c, mMatrix, 0x3BC);
WWHD_OFFSET(Act_c, mRegistered, 0x3EC);
WWHD_SIZE(Act_c, 0x3F4);
} // namespace daObjDoguuDemo
