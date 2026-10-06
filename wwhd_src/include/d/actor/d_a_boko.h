#pragma once
#include "f_op/f_op_actor.h"
struct daBoko_c : fopAc_ac_c {
  u8 phase[8];
  gptr<void> model;
  u8 brkAnimation[0x78];
  be<u8> mode, hidden, thrown, moveCounter, floorFlag;
  u8 pad435[3];
  be<s16> angleSpeed, rotationSpeed, flameTimer;
  u8 pad43e[2];
  be<s16> timer, throwAngle;
  u8 pad444[8];
  cXyz top, root;
  u8 pad464[12];
  cXyz flamePosition;
  u8 light[0x24], lightTail[12], particleCallback[0x14];
  u8 acch[0x1C4], acchCircle[0x40], status[0x3C];
  u8 sphere[0x12C], capsule[0x138];
  be<f32> waterHeight;
  Mtx34 alphaMatrices[4];
  gptr<void> line;
  be<s16> procedureDelta, procedureSlot;
  be<u32> procedure;
};
WWHD_OFFSET(daBoko_c, model, 0x3B4);
WWHD_OFFSET(daBoko_c, mode, 0x430);
WWHD_OFFSET(daBoko_c, top, 0x44C);
WWHD_OFFSET(daBoko_c, light, 0x47C);
WWHD_OFFSET(daBoko_c, particleCallback, 0x4AC);
WWHD_OFFSET(daBoko_c, acch, 0x4C0);
WWHD_OFFSET(daBoko_c, status, 0x6C4);
WWHD_OFFSET(daBoko_c, line, 0xA28);
WWHD_OFFSET(daBoko_c, procedure, 0xA30);
static_assert(sizeof(daBoko_c) == 0xA34);
