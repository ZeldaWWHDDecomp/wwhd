#pragma once
#include "f_op/f_op_actor.h"

namespace tntrap_local {
struct MemberFunction {
  be<s16> adjustment, slot;
  be<u32> target;
};
static_assert(sizeof(MemberFunction) == 8);
// HD dCcD_Stts + dCcD_GStts constructed at actor+0x3B8/0x3D4.
struct CollisionStatus {
  u8 bytes[0x3C];
};
// The HD triangle stride comes from __construct_array and set_tri, not GC.
struct Triangle {
  u8 collisionData[0x3C];
  be<u32> collisionVtable;
  u8 reserved40[4];
  be<u32> status;
  u8 collisionTail[0xC8];
  be<u32> primitiveVtable;
  be<u32> shapeVtable;
  u8 geometry[0x38];
};
static_assert(sizeof(CollisionStatus) == 0x3C);
static_assert(sizeof(Triangle) == 0x150);
static_assert(offsetof(Triangle, status) == 0x44);
static_assert(offsetof(Triangle, primitiveVtable) == 0x110);
static_assert(offsetof(Triangle, shapeVtable) == 0x114);
static_assert(offsetof(Triangle, geometry) == 0x118);
} // namespace tntrap_local

class daObjTnTrap_c : public fopAc_ac_c {
public:
  request_of_phase_process_class phase;      // 3AC
  be<s32> trapType;                          // 3B4
  tntrap_local::CollisionStatus status;      // 3B8
  tntrap_local::Triangle triangles[8];       // 3F4, two groups of four
  be<u32> background;                        // E74
  Mtx34 matrix;                              // E78
  be<u32> emittersA[2][2];                   // EA8
  be<u32> emittersB[2][3];                   // EB8
  tntrap_local::MemberFunction actionMethod; // ED0
  be<s32> action;                            // ED8
  be<u8> appears;                                // EDC
  u8 reservedEDD[3];
  be<u32> switchSave;  // EE0
  be<u32> switchSave2; // EE4
  be<u32> argument;    // EE8
  be<u32> mapType;     // EEC
  be<s16> eventId;     // EF0
  u8 reservedEF2[6];
  be<u8> particlesActive[2]; // EF8
  u8 reservedEFA[2];
  be<f32> particleOffsetY[2]; // EFC
};
static_assert(sizeof(daObjTnTrap_c) == 0xF04);
#define TNTRAP_OFFSET(field, offset)                                           \
  static_assert(offsetof(daObjTnTrap_c, field) == offset)
TNTRAP_OFFSET(phase, 0x3AC);
TNTRAP_OFFSET(trapType, 0x3B4);
TNTRAP_OFFSET(status, 0x3B8);
TNTRAP_OFFSET(triangles, 0x3F4);
TNTRAP_OFFSET(background, 0xE74);
TNTRAP_OFFSET(matrix, 0xE78);
TNTRAP_OFFSET(emittersA, 0xEA8);
TNTRAP_OFFSET(emittersB, 0xEB8);
TNTRAP_OFFSET(actionMethod, 0xED0);
TNTRAP_OFFSET(action, 0xED8);
TNTRAP_OFFSET(appears, 0xEDC);
TNTRAP_OFFSET(switchSave, 0xEE0);
TNTRAP_OFFSET(switchSave2, 0xEE4);
TNTRAP_OFFSET(argument, 0xEE8);
TNTRAP_OFFSET(mapType, 0xEEC);
TNTRAP_OFFSET(eventId, 0xEF0);
TNTRAP_OFFSET(particlesActive, 0xEF8);
TNTRAP_OFFSET(particleOffsetY, 0xEFC);
#undef TNTRAP_OFFSET
