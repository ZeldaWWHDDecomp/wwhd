#pragma once
#include "bindings.h"
struct daSTBox_shadowEcallBack_c {
  be<u32> vtable;
  be<s16> ended;
  u8 pad6[2];
  be<f32> waterY, flatWaterY, depth;
  cXyz previous[3], position;
  gptr<csXyz> angle;
  be<f32> scroll, textureScale, direction;
  gptr<void> emitter;
};
WWHD_SIZE(daSTBox_shadowEcallBack_c, 0x58);
struct daSTBox_c : fopAc_ac_c {
  request_of_phase_process_class phase;
  gptr<J3DModel> model;
  gptr<void> emitters[3];
  u8 pad3c4[4], ripple[0x14];
  daSTBox_shadowEcallBack_c shadow;
  cXyz particlePos, cranePos;
  be<u8> itemNo, boxType;
  be<s16> timer;
  be<u8> splashStarted, rippleStarted, bgmStarted;
  u8 pad453;
  be<u32> itemPID;
};
WWHD_OFFSET(daSTBox_c, shadow, 0x3DC);
WWHD_OFFSET(daSTBox_c, boxType, 0x44D);
WWHD_SIZE(daSTBox_c, 0x458);
