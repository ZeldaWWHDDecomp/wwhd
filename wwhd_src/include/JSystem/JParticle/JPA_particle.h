#pragma once
#include "wwhd.h"
// HD particle prefix; GPU state beyond this prefix remains opaque.
struct JPAParticle_l {
  u8 link[0x10];
  be<f32> offset[3], local[3], global[3], velocity[3];
  be<f32> baseVelocity[3], acceleration[3], fieldVelocity[3],
      fieldAcceleration[3];
  be<f32> airResistance, moment, age, lifetime, normalizedTime, fieldDrag, drag;
  u8 drawParameters[0xcc - 0x8c];
  be<u32> status;
  u8 opaque0d0[0x10c - 0xd0];
  be<u8> deletionDelay;
  u8 opaque10d[0x120 - 0x10d];
  be<u8> hidden;
};
WWHD_OFFSET(JPAParticle_l, velocity, 0x34);
WWHD_OFFSET(JPAParticle_l, age, 0x78);
WWHD_OFFSET(JPAParticle_l, status, 0xcc);
WWHD_OFFSET(JPAParticle_l, deletionDelay, 0x10c);
WWHD_OFFSET(JPAParticle_l, hidden, 0x120);
