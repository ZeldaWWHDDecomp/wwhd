#pragma once
#include "bindings.h"
struct daObjCafelmp_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel;
  BOOL CreateHeap();
  void set_mtx();
  void CreateInit();
  s32 Create();
  BOOL Delete();
  BOOL Draw();
  BOOL Execute();
};
WWHD_OFFSET(daObjCafelmp_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjCafelmp_c, mModel, 0x3B4);
WWHD_SIZE(daObjCafelmp_c, 0x3B8);
