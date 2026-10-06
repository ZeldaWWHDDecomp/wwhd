#pragma once
#include "bindings.h"
struct daObj_Bscurtain_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  be<u32> unused;
  gptr<J3DModel> mModel;
  BOOL CreateHeap();
  void set_mtx();
  s32 CreateInit();
  s32 Create();
  BOOL Delete();
  BOOL Draw();
  BOOL Execute();
};
WWHD_OFFSET(daObj_Bscurtain_c, mPhase, 0x3AC);
WWHD_OFFSET(daObj_Bscurtain_c, mModel, 0x3B8);
WWHD_SIZE(daObj_Bscurtain_c, 0x3BC);

struct BscurtainHIO {
  be<s8> entry;
  u8 pad[3];
  be<f32> value;
  be<u16> flags;
  u8 pad2[2];
  be<u32> vtable;
};
WWHD_OFFSET(BscurtainHIO, value, 4);
WWHD_OFFSET(BscurtainHIO, flags, 8);
WWHD_OFFSET(BscurtainHIO, vtable, 12);
WWHD_SIZE(BscurtainHIO, 16);
