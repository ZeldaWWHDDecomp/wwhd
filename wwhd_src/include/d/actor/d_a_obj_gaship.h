#pragma once
#include "bindings.h"
namespace daObjGaship {
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel;
  BOOL CreateHeap();
  void set_mtx();
  Mtx34 mMatrix;
  be<u8> birthFlag[2];
  u8 pad[2];
  void birth_flag();
  s32 Create();
  BOOL Delete();
  BOOL Draw();
  BOOL Execute();
};
} // namespace daObjGaship
WWHD_OFFSET(daObjGaship::Act_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjGaship::Act_c, mModel, 0x3B4);
WWHD_OFFSET(daObjGaship::Act_c, mMatrix, 0x3B8);
WWHD_OFFSET(daObjGaship::Act_c, birthFlag, 0x3E8);
WWHD_SIZE(daObjGaship::Act_c, 0x3EC);
