#pragma once
#include "f_op/f_op_actor.h"
// HD Gale Isle palm layout; derived code.
struct daObjVyasi_c : fopAc_ac_c {
  be<s16> windPhase[14];
  be<s16> windAngle[14];
  u8 quaternion[14][16];
  csXyz jointAngle[14];
  cXyz jointPosition[14];
  cXyz leafScale;
  request_of_phase_process_class phase;
  u8 matrix[48];
  gptr<void> morf;
  gptr<void> animation;
  cXyz sourcePosition;
  be<s16> sourceYaw;
  u8 padding61a[2];
  be<f32> amplitude;
  be<s16> anglePhase[14];
  be<s16> angleVelocity[14];
  u8 padding658[4];
  be<s16> normalCounter;
  u8 padding65e[2];
  u8 collisions[0x1adc - 0x660];
  be<s32> stopped;
  be<s32> state;
  be<f32> restoreAmplitude;
  be<s16> restorePhase;
  u8 padding1aea[2];
  be<f32> windScale;
};
WWHD_OFFSET(daObjVyasi_c, quaternion, 0x3e4);
WWHD_OFFSET(daObjVyasi_c, jointAngle, 0x4c4);
WWHD_OFFSET(daObjVyasi_c, jointPosition, 0x518);
WWHD_OFFSET(daObjVyasi_c, phase, 0x5cc);
WWHD_OFFSET(daObjVyasi_c, morf, 0x604);
WWHD_OFFSET(daObjVyasi_c, normalCounter, 0x65c);
WWHD_OFFSET(daObjVyasi_c, stopped, 0x1adc);
WWHD_SIZE(daObjVyasi_c, 0x1af0);
