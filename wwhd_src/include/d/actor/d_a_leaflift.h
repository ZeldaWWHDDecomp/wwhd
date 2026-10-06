#pragma once
#include "f_op/f_op_actor.h"
struct daLlift_c : fopAc_ac_c {
  u8 phase[8];
  gptr<void> model;
  u8 statusAndCylinder[0x524 - 0x3B8];
  gptr<void> background;
  Mtx34 transform;
  be<u8> ridden, risingTilt, hasRider, settleTimer;
  be<s16> tilt;
  u8 pad55e[2];
  be<s16> lifetime;
  u8 pad562[2];
  be<f32> rotation[4], targetRotation[4];
  be<u8> descending, moving;
  u8 pad586[2];
  Mtx34 particleTransform;
  be<s32> emitterTimer;
  gptr<void> emitter1, emitter2, emitter3, emitter4;
  be<f32> waterY;
};
WWHD_OFFSET(daLlift_c, model, 0x3B4);
WWHD_OFFSET(daLlift_c, background, 0x524);
WWHD_OFFSET(daLlift_c, ridden, 0x558);
WWHD_OFFSET(daLlift_c, rotation, 0x564);
WWHD_OFFSET(daLlift_c, particleTransform, 0x588);
WWHD_OFFSET(daLlift_c, waterY, 0x5CC);
static_assert(sizeof(daLlift_c) == 0x5D0);
