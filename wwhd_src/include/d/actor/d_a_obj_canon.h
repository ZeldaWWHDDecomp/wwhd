#pragma once
#include "f_op/f_op_actor.h"
struct daObj_Canon_c : fopAc_ac_c {
  be<s32> mode;
  be<u8> pathIndex, deathSwitch, appearSwitch, scaleArg;
  u8 pathRun[0x20];
  be<u32> phase[2];
  gptr<void> model;
  be<s16> targetYaw, yaw, targetPitch, pitch;
  cXyz target;
  u8 collision[0x168];
  be<s32> hitTimer, waitTimer;
  u8 padding564[8];
  cXyz muzzle, attention;
  be<s16> shakePhase, recoil;
  be<f32> rise;
  u8 follow[0x14];
  be<s32> smokeTimer, shots;
};
WWHD_OFFSET(daObj_Canon_c, mode, 0x3AC);
WWHD_OFFSET(daObj_Canon_c, model, 0x3DC);
WWHD_OFFSET(daObj_Canon_c, collision, 0x3F4);
WWHD_OFFSET(daObj_Canon_c, hitTimer, 0x55C);
WWHD_OFFSET(daObj_Canon_c, muzzle, 0x56C);
WWHD_OFFSET(daObj_Canon_c, rise, 0x588);
WWHD_OFFSET(daObj_Canon_c, shots, 0x5A4);
