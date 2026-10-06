#pragma once
#include "bindings.h"
struct GnnbtakiBtk {
  be<f32> speed, frame;
  be<s16> startFrame, endFrame;
  u8 animationState[0x68];
};
WWHD_SIZE(GnnbtakiBtk, 0x74);
struct daObjGnnbtaki_c : fopAc_ac_c {
  gptr<J3DModel> mpModel;
  request_of_phase_process_class mPhase;
  GnnbtakiBtk mBtk;
  be<u8> mPlaying, mType;
  u8 padding[2];
  BOOL create_heap();
  s32 _create();
  BOOL _delete();
  void init_mtx();
  BOOL _execute();
  BOOL _draw();
};
WWHD_OFFSET(daObjGnnbtaki_c, mpModel, 0x3AC);
WWHD_OFFSET(daObjGnnbtaki_c, mPhase, 0x3B0);
WWHD_OFFSET(daObjGnnbtaki_c, mBtk, 0x3B8);
WWHD_OFFSET(daObjGnnbtaki_c, mPlaying, 0x42C);
WWHD_OFFSET(daObjGnnbtaki_c, mType, 0x42D);
WWHD_SIZE(daObjGnnbtaki_c, 0x430);
