#pragma once
#include "f_op/f_op_actor.h"
struct VdsLight {
  cXyz position;
  be<s16> red, green, blue, alpha;
  be<f32> radius, fluctuation;
  u8 pad1c[4];
  be<f32> scale;
};
struct daObjVds_c : fopAc_ac_c {
  u8 phase[8];
  be<f32> matrix[12];
  be<u32> morf0, joint0;
  u8 brk0[0x78];
  be<u32> color0, morf1, joint1;
  u8 brk1[0x78];
  be<u32> color1, background;
  be<s32> loopAnimation, process, lightStage;
  be<s32> switchActor[2];
  be<f32> strength[2];
  be<s16> event, eventState, eventIndex;
  u8 pad512[2];
  VdsLight lights[2];
  cXyz lightPosition[2];
};
WWHD_OFFSET(daObjVds_c, morf0, 0x3E4);
WWHD_OFFSET(daObjVds_c, process, 0x4F4);
WWHD_OFFSET(daObjVds_c, lights, 0x514);
WWHD_OFFSET(daObjVds_c, lightPosition, 0x55C);
static_assert(sizeof(VdsLight) == 0x24);
static_assert(sizeof(daObjVds_c) == 0x574); // RPX profile size at 101CD85C.
