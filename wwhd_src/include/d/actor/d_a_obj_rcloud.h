#pragma once
#include "f_op/f_op_actor.h"
struct daObjRcloud_c : fopAc_ac_c {
  u8 mPhase[8];
  gptr<u8> mpModel;
  u8 mBtk[0x74];
  be<u32> mActDescriptor[2];
  be<s32> mAction, mDemoName;
  be<u8> mLoaded;
  u8 _43D[3];
  be<f32> mProgress;
  BOOL create_heap();
  void init_mtx();
  void setup_action(s32);
  s32 _create();
  BOOL _delete();
  BOOL _execute();
  void setTexMtx();
  BOOL _draw();
  void wait_act_proc();
  void clouds_lift_start_wait_act_proc();
  void clouds_lift_act_proc();
};
WWHD_OFFSET(daObjRcloud_c, mActDescriptor, 0x42C);
WWHD_SIZE(daObjRcloud_c, 0x444);
