/**
 * d_a_warphr.h (WWHD)
 * Warphr - warp portal to Hyrule (Ghrwp, daWarphr_c)
 *
 * Written from the WWHD code with the GameCube decompilation (zeldaret/tww src/d/actor/d_a_warphr.cpp) as
 * reference, verified against cking.rpx.
 */
#pragma once
#include "bindings.h"
struct daWarphr_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mpModel1;
  be<u32> mpBtkAnm1;
  gptr<J3DModel> mpModel2;
  be<u32> mpBtkAnm2, mpBrkAnm, mWarpEmitter, mProjectionEmitter;
  Mtx34 mProjectionMatrix;
  be<u8> mMonotoneStarted;
  u8 pad401[11];
  be<s32> mEventState;
  u8 pad410[4];
  be<s16> mEventIdx;
  be<u8> mType;
  u8 pad417[5];
  be<u32> mShapeId;
  be<s32> mStaffId;
  s32 CreateHeap();
  void CreateInit();
  s32 create();
  bool remove();
  bool draw();
  s32 get_return_count();
  s32 check_warp();
  void anim_play(s32);
  bool normal_execute();
  void checkOrder();
  void demo_proc();
  void eventOrder();
  bool demo_execute();
  bool execute();
  void set_end_anim();
  bool actWait(s32);
  void initWarp(s32);
  bool actWarp(s32);
  void initWarpArrive(s32);
  void initWarpArriveEnd(s32);
  bool actWarpArriveEnd(s32);
  void initStartWarp(s32);
  bool actStartWarp(s32);
};
WWHD_OFFSET(daWarphr_c, mPhase, 0x3AC);
WWHD_OFFSET(daWarphr_c, mpModel1, 0x3B4);
WWHD_OFFSET(daWarphr_c, mWarpEmitter, 0x3C8);
WWHD_OFFSET(daWarphr_c, mProjectionMatrix, 0x3D0);
WWHD_OFFSET(daWarphr_c, mMonotoneStarted, 0x400);
WWHD_OFFSET(daWarphr_c, mEventState, 0x40C);
WWHD_OFFSET(daWarphr_c, mEventIdx, 0x414);
WWHD_OFFSET(daWarphr_c, mShapeId, 0x41C);
WWHD_OFFSET(daWarphr_c, mStaffId, 0x420);
WWHD_SIZE(daWarphr_c, 0x424);
