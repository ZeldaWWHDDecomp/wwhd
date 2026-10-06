#pragma once
#include "bindings.h"
struct daObjHomen_c : fopAc_ac_c {
  u8 collision[0x850 - 0x3ac];
  be<u32> phase;
  u8 phaseExtra[4];
  gptr<void> model;
  be<f32> matrix[12];
  gptr<void> background;
  u8 groundCheck[0x54];
  be<f32> groundY;
  be<s32> type;
  be<s16> spinX, spinZ, yaw, followCount, spinTarget, extra;
  be<u32> hookshotID, enemyID, smokeID;
  be<s16> pivoted, itemTimer, enemyTimer, eventIndex, eventOrder, eventState,
      state;
  u8 pad[2];
  cXyz target;
};
static_assert(sizeof(daObjHomen_c) == 0x920);
static_assert(offsetof(daObjHomen_c, phase) == 0x850);
static_assert(offsetof(daObjHomen_c, model) == 0x858);
static_assert(offsetof(daObjHomen_c, type) == 0x8e8);
static_assert(offsetof(daObjHomen_c, state) == 0x910);
static_assert(offsetof(daObjHomen_c, target) == 0x914);
