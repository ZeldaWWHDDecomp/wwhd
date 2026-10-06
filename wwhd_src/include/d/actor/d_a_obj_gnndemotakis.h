#pragma once
#include "bindings.h"
struct GnntakisBtk {
  be<f32> speed, frame;
  be<s16> startFrame, endFrame;
  u8 animationState[0x68];
};
WWHD_SIZE(GnntakisBtk, 0x74);
struct daObjGnntakis_c : fopAc_ac_c {
  gptr<J3DModel> mpModel;
  request_of_phase_process_class mPhase;
  GnntakisBtk mBtk;
  be<s32> mPlaying;
  BOOL create_heap();
  s32 _create();
  BOOL _delete();
  void init_mtx();
  BOOL _execute();
  BOOL _draw();
};
WWHD_OFFSET(daObjGnntakis_c, mpModel, 0x3AC);
WWHD_OFFSET(daObjGnntakis_c, mPhase, 0x3B0);
WWHD_OFFSET(daObjGnntakis_c, mBtk, 0x3B8);
WWHD_OFFSET(daObjGnntakis_c, mPlaying, 0x42C);
WWHD_SIZE(daObjGnntakis_c, 0x430);
