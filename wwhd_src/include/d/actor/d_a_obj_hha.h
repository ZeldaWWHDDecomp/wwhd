#pragma once
#include "f_op/f_op_actor.h"
struct daObjHhaPart_c {
  be<u32> model, background;
  cXyz position, target, direction;
  be<f32> deltaY;
  be<u8> index, middle;
  u8 pad32[2];
  u8 executeMember[8], drawMember[8];
};
static_assert(sizeof(daObjHhaPart_c) == 0x44);
struct daObjHha_c : fopAc_ac_c {
  be<u32> model;
  u8 textureAnimations[0xE8];
  u8 phase[8];
  u8 cylinderStatus[0x3C];
  u8 cylinder[0x134];
  u8 sphereStatus[0x3C];
  u8 sphere[0x12C];
  u8 gush[0x2F8];
  daObjHhaPart_c parts[2];
  u8 splashes[2][0x34];
  cXyz positionOffset;
  be<u32> switchNumber;
  be<u8> middle;
  u8 padB71;
  be<s16> partTimer;
  be<f32> waterScale, minimumWaterScale;
  be<u16> waterTimer;
  be<u8> waterState;
  u8 padB7F;
  be<s16> eventIndex;
  be<u8> state, waterSound;
};
WWHD_OFFSET(daObjHha_c, model, 0x3AC);
WWHD_OFFSET(daObjHha_c, phase, 0x498);
WWHD_OFFSET(daObjHha_c, cylinder, 0x4DC);
WWHD_OFFSET(daObjHha_c, sphere, 0x64C);
WWHD_OFFSET(daObjHha_c, gush, 0x778);
WWHD_OFFSET(daObjHha_c, parts, 0xA70);
WWHD_OFFSET(daObjHha_c, splashes, 0xAF8);
WWHD_OFFSET(daObjHha_c, waterScale, 0xB74);
static_assert(sizeof(daObjHha_c) == 0xB84);
