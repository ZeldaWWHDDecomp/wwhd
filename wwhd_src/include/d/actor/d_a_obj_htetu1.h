#pragma once
#include "f_op/f_op_actor.h"
struct daObjHtetu1Splash_c {
  be<u32> vtable, emitter;
  u8 pad08[12];
  cXyz position;
  csXyz rotation;
  be<s16> timer;
  be<u8> playing;
  u8 pad29[3];
};
struct daObjHtetu1_c : fopAc_ac_c {
  be<u32> model;
  u8 phase[8];
  be<u32> switchBit;
  cXyz movingPosition, shakeOffset;
  be<f32> floorHeight, shakeAmplitude;
  be<u16> unlockTimer;
  be<u8> motionState, eventPadding;
  be<s16> eventIndex;
  be<u8> eventState;
  u8 pad3e3;
  be<s16> quakeTimer;
  u8 pad3e6[2];
  be<u32> background;
  daObjHtetu1Splash_c splash[2];
};
WWHD_OFFSET(daObjHtetu1_c, model, 0x3AC);
WWHD_OFFSET(daObjHtetu1_c, movingPosition, 0x3BC);
WWHD_OFFSET(daObjHtetu1_c, splash, 0x3EC);
static_assert(sizeof(daObjHtetu1Splash_c) == 0x2C);
static_assert(sizeof(daObjHtetu1_c) == 0x444); // HD profile 101CA314.
