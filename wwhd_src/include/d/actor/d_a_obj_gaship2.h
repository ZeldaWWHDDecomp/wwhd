#pragma once
#include "bindings.h"
namespace daObjGaship2 {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel;
  BOOL CreateHeap();
  void set_mtx();
  gptr<dBgW> mBgW;
  Mtx34 mMatrix;
  s32 Create();
  BOOL Delete();
  BOOL Draw();
  BOOL Execute();
};
} // namespace daObjGaship2
WWHD_OFFSET(daObjGaship2::Act_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjGaship2::Act_c, mModel, 0x3B4);
WWHD_OFFSET(daObjGaship2::Act_c, mMatrix, 0x3BC);
WWHD_OFFSET(daObjGaship2::Act_c, mBgW, 0x3B8);
WWHD_SIZE(daObjGaship2::Act_c, 0x3EC);
