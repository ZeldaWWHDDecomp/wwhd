/* WWHD Eayogn bridge reconstruction. */
#pragma once
#include "bindings.h"
namespace daObjEayogn {
struct Act_c : fopAc_ac_c {
  gptr<J3DModel> mModel;
  request_of_phase_process_class mPhase;
  gptr<dBgW> mBgW;
  BOOL create_heap();
  void init_mtx();
  s32 Create();
  BOOL Delete();
  BOOL Draw();
};
WWHD_OFFSET(Act_c, mModel, 0x3AC);
WWHD_OFFSET(Act_c, mPhase, 0x3B0);
WWHD_OFFSET(Act_c, mBgW, 0x3B8);
WWHD_SIZE(Act_c, 0x3BC);
} // namespace daObjEayogn
