#pragma once
#include "bindings.h"
#include "gabi.h"
// WWHD Blade Trap. The GC source is stubbed; offsets are audited from HD.
struct daObjTrap_c : fopAc_ac_c {
  char pad3ac[0x3ac - sizeof(fopAc_ac_c)];
  gptr<void> model;
  be<f32> anmSpeed, anmFrame;
  char pad3b8[0x598 - 0x3b8];
  gptr<void> path;
  cXyz target, origin, direction, displacement, searchVelocity, savedPosition,
      bounceOffset;
  be<f32> pathLength;
  be<s16> waitTimer;
  be<u16> bounceTimer;
  be<f32> bounceAmplitude;
  be<u16> vibrationTimer;
  be<s16> savedAngle;
  be<u8> pathPoint, pathId, directionValid, mode, shining, speedType;
  char pad606[2];
  be<f32> moveSpeed;
  be<s16> waitDuration;
  char pad60e[2];
  gptr<void> background;
};
WWHD_OFFSET(daObjTrap_c, model, 0x3ac);
WWHD_OFFSET(daObjTrap_c, path, 0x598);
WWHD_OFFSET(daObjTrap_c, target, 0x59c);
WWHD_OFFSET(daObjTrap_c, mode, 0x603);
WWHD_OFFSET(daObjTrap_c, background, 0x610);
