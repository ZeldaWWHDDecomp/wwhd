#pragma once
#include "f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"
// HD animation containers are substantially larger than their GameCube
// versions.
struct BarrierBrkAnm : mDoExt_baseAnm {
  u8 state[0x78 - 0x10];
};
struct BarrierAnimation {
  be<u32> model;
  mDoExt_btkAnm btk;
  BarrierBrkAnm brk;
  be<f32> brkFrame;
};
struct BarrierEffect {
  be<u32> activeFlags;
  be<u32> model[4];
  mDoExt_btkAnm btk[4];
  mDoExt_bckAnm bck[4];
  BarrierBrkAnm brk[4];
  cXyz position[4];
  be<s16> angle[4];
  be<u32> hitActor[4];
};
struct daObjBarrier_c : fopAc_ac_c {
  BarrierAnimation animation;                      // 3AC
  request_of_phase_process_class phase;            // 4A0
  be<u32> background;                              // 4A8
  Mtx34 backgroundMatrix;                          // 4AC
  u8 attackStatus[0x3C], targetStatus[0x3C];       // 4DC,518
  u8 attackCylinder[0x130], targetCylinder[0x130]; // 554,684
  BarrierEffect effect;                            // 7B4
  be<u8> active;
  u8 reserved[3]; // DF0
  be<s32> moya;   // DF4
  be<s16> eventID;
  u8 reservedEvent[2]; // DF8
  be<s32> procedure;   // DFC
};
WWHD_SIZE(BarrierBrkAnm, 0x78);
WWHD_SIZE(BarrierAnimation, 0xF4);
WWHD_SIZE(BarrierEffect, 0x63C);
WWHD_SIZE(daObjBarrier_c, 0xE00);
WWHD_OFFSET(daObjBarrier_c, effect, 0x7B4);
WWHD_OFFSET(BarrierEffect, bck, 0x1E4);
WWHD_OFFSET(BarrierEffect, hitActor, 0x62C);

WWHD_OFFSET(daObjBarrier_c, animation, 0x3AC);
WWHD_OFFSET(daObjBarrier_c, phase, 0x4A0);
WWHD_OFFSET(daObjBarrier_c, backgroundMatrix, 0x4AC);
WWHD_OFFSET(daObjBarrier_c, attackCylinder, 0x554);
WWHD_OFFSET(daObjBarrier_c, targetCylinder, 0x684);
WWHD_OFFSET(daObjBarrier_c, active, 0xDF0);
WWHD_OFFSET(daObjBarrier_c, procedure, 0xDFC);
WWHD_OFFSET(BarrierEffect, brk, 0x414);
WWHD_OFFSET(BarrierEffect, position, 0x5F4);
WWHD_OFFSET(BarrierEffect, angle, 0x624);
