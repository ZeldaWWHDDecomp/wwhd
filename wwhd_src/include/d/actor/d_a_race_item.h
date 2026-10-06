/* Barrel items, WWHD layout. */
#pragma once
#include "bindings.h"
struct daRaceItem_c : fopAc_ac_c {
  request_of_phase_process_class mPhs; // 0x3AC
  gptr<J3DModel> mpModel;              // 0x3B4
  u8 _3B8[0x3D4 - 0x3B8];
  dBgS_ObjAcch mAcch;
  dBgS_AcchCir mAcchCir;
  dCcD_Stts mStts;
  dCcD_Cyl mCyl;
  be<u32> mItemBitNo; // 0x744
  be<u32> m_timer;
  u8 _74C[2];
  be<u8> m_itemNo;
  u8 _74F;
  be<s32> mGetTimer; // GC field_0x63C
  be<u32> mState;    // GC field_0x640
  be<u8> mGetType;
  be<u8> mFlags;
  u8 _75A[2];
  void set_mtx();
  void set_mtx(cXyz *);
  BOOL Delete();
  cPhs_State create();
  BOOL CreateInit();
  void checkGet();
  void normalItemGet();
  void raceItemGet();
  void raceItemForceGet();
  BOOL startOffsetPos();
  BOOL endOffsetPos(f32, cXyz *, f32, f32, csXyz *);
  BOOL checkOffsetPos();
};
WWHD_OFFSET(daRaceItem_c, mAcch, 0x3D4);
WWHD_OFFSET(daRaceItem_c, mStts, 0x5D8);
WWHD_OFFSET(daRaceItem_c, mCyl, 0x614);
WWHD_OFFSET(daRaceItem_c, mItemBitNo, 0x744);
WWHD_OFFSET(daRaceItem_c, mState, 0x754);
WWHD_SIZE(daRaceItem_c, 0x75C);
