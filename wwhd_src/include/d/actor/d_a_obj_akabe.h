#pragma once
#include "bindings.h"
namespace daObjAkabe {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<dBgW> mBgW;
  Mtx34 mMatrix;
  be<s32> mType;
  be<u8> mAppear;
  u8 padding[3];
  BOOL chk_appear();
  void init_scale();
  void init_mtx();
  BOOL create_heap();
  s32 Create();
  BOOL Delete();
  BOOL Execute();
};
WWHD_OFFSET(Act_c, mPhase, 0x3AC);
WWHD_OFFSET(Act_c, mBgW, 0x3B4);
WWHD_OFFSET(Act_c, mMatrix, 0x3B8);
WWHD_OFFSET(Act_c, mType, 0x3E8);
WWHD_OFFSET(Act_c, mAppear, 0x3EC);
WWHD_SIZE(Act_c, 0x3F0);
} // namespace daObjAkabe
