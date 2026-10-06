#pragma once
#include "f_op/f_op_actor.h"
struct daObj_Pbco_c : fopAc_ac_c {
  u8 mPhase[8];
  gptr<u8> mpModel, mpBgW;
  be<f32> mMatrix[12];
  BOOL CreateHeap();
  void set_mtx();
  s32 CreateInit();
};
struct daObj_Pbco_HIO_c {
  be<s8> mNo;
  u8 _1[3];
  be<u32> mVtable;
};
WWHD_OFFSET(daObj_Pbco_c, mpBgW, 0x3B8);
WWHD_SIZE(daObj_Pbco_c, 0x3EC);
WWHD_SIZE(daObj_Pbco_HIO_c, 8);
