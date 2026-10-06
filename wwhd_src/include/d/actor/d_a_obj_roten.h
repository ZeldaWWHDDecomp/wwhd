#pragma once
#include "f_op/f_op_actor.h"
// WWHD Zunari shop curtain/post actor.
struct daObj_Roten_c : fopAc_ac_c {
  u8 mPhs[8];
  gptr<u8> mpModel;
  u8 _3B8[0x30];
  gptr<u8> mpBgW;
  be<u8> mType, mRejected;
  u8 _3EE[2];
  BOOL CreateHeap();
  void set_mtx();
  s32 CreateInit();
};
WWHD_OFFSET(daObj_Roten_c, mpModel, 0x3B4);
WWHD_OFFSET(daObj_Roten_c, mpBgW, 0x3E8);
WWHD_OFFSET(daObj_Roten_c, mType, 0x3EC);
WWHD_SIZE(daObj_Roten_c, 0x3F0);
struct daObj_Roten_HIO_HD {
  be<s8> mNo;
  u8 _1[3];
  be<f32> mOffset;
  be<s16> mFlags;
  u8 _A[2];
  be<u32> mVtable;
};
WWHD_SIZE(daObj_Roten_HIO_HD, 0x10);
