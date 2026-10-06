#pragma once
#include "f_op/f_op_actor.h"
struct daObjKanoke_c : fopAc_ac_c {
  be<u32> phase;
  u8 unknown3b0[4];
  gptr<void> bodyModel, lidModel, bodyBackground, lidBackground;
  be<f32> bodyMatrix[12], lidMatrix[12];
  u8 collision[0x51c];
  be<u32> effectHandles[2];
  u8 smoke[0x34];
  cXyz lidOffset, pivot;
  be<f32> smokeAlpha;
  be<s16> lidX, lidY, lidZ, angularSpeed, timer, shakeSpeed, lightTimer;
  be<u8> type, state, searchRange, switchNo, switchNo2, hidden;
};
WWHD_OFFSET(daObjKanoke_c, bodyModel, 0x3b4);
WWHD_OFFSET(daObjKanoke_c, bodyMatrix, 0x3c4);
WWHD_OFFSET(daObjKanoke_c, effectHandles, 0x940);
WWHD_OFFSET(daObjKanoke_c, smoke, 0x948);
WWHD_OFFSET(daObjKanoke_c, lidOffset, 0x97c);
WWHD_OFFSET(daObjKanoke_c, pivot, 0x988);
WWHD_OFFSET(daObjKanoke_c, lidX, 0x998);
WWHD_OFFSET(daObjKanoke_c, type, 0x9a6);
static_assert(sizeof(daObjKanoke_c) == 0x9ac);
