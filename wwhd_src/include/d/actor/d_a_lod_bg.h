#pragma once
#include "f_op/f_op_actor.h"
namespace lodbg_local {
struct ResourceName {
  be<u32> text, vtable, capacity;
  u8 bytes[0x20];
};
struct MemberFunction {
  be<s16> adjustment, slot;
  be<u32> target;
};
static_assert(sizeof(ResourceName) == 0x2C);
static_assert(sizeof(MemberFunction) == 8);
} // namespace lodbg_local
class daLodbg_c : public fopAc_ac_c {
public:
  lodbg_local::ResourceName resource; // 3AC
  lodbg_local::MemberFunction action; // 3D8
  be<u32> model, model2[2], modelData, modelData2;
  be<u8> alpha, drawModel2, resourceActive;
  u8 reserved3F7;
};
static_assert(sizeof(daLodbg_c) == 0x3F8);
static_assert(offsetof(daLodbg_c, resource) == 0x3AC);
static_assert(offsetof(daLodbg_c, action) == 0x3D8);
static_assert(offsetof(daLodbg_c, model) == 0x3E0);
static_assert(offsetof(daLodbg_c, model2) == 0x3E4);
static_assert(offsetof(daLodbg_c, modelData) == 0x3EC);
static_assert(offsetof(daLodbg_c, modelData2) == 0x3F0);
static_assert(offsetof(daLodbg_c, alpha) == 0x3F4);
static_assert(offsetof(daLodbg_c, resourceActive) == 0x3F6);
