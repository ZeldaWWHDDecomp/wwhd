#pragma once
#include "bindings.h"
namespace daObjXfuta {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel;
  Mtx34 mMatrix;
  BOOL create_heap();
  void set_mtx();
  s32 Create();
  BOOL Delete();
  BOOL Execute();
  BOOL Draw();
};
WWHD_OFFSET(Act_c, mPhase, 0x3AC);
WWHD_OFFSET(Act_c, mModel, 0x3B4);
WWHD_OFFSET(Act_c, mMatrix, 0x3B8);
WWHD_SIZE(Act_c, 0x3E8);
} // namespace daObjXfuta
