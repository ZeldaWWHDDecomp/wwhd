#pragma once
#include "f_op/f_op_actor.h"
struct daObjIce_c : fopAc_ac_c {
  request_of_phase_process_class phase; // 3AC
  be<u32> iceModel;                     // 3B4
  u8 collisionStatus[0x3C];             // 3B8
  u8 cylinder[0x130];                   // 3F4
  be<u32> background;                   // 524
  Mtx34 backgroundMatrix;               // 528
  be<s16> actionDelta, actionIndex;     // 558
  be<u32> actionTarget;                 // 55C
  be<u8> appears;                       // 560
  u8 reserved[3];
  be<f32> width, height, alpha;   // 564
  be<s32> fadeTimer, hitState;    // 570
  Mtx34 effectSmall, effectLarge; // 578,5A8 HD
};
WWHD_SIZE(daObjIce_c, 0x5D8);
WWHD_OFFSET(daObjIce_c, cylinder, 0x3F4);
WWHD_OFFSET(daObjIce_c, effectSmall, 0x578);
