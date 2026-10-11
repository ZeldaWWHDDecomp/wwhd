/* Short-coordinate vectors. */
#pragma once
#include "bindings.h"

WWHD_OFFSET(csXyz, x, 0);
WWHD_OFFSET(csXyz, y, 2);
WWHD_OFFSET(csXyz, z, 4);
WWHD_SIZE(csXyz, 6);

// HD: preserve GHS's six-byte value return in r3 and the high half of r4.
struct SxyzResult {
    u32 xy;
    u16 z;
};
namespace gabi {
template<> constexpr RetKind ret_kind<SxyzResult>() { return RET_AGG6; }
template<> inline void store_ret<SxyzResult>(Cpu* cpu, SxyzResult value) {
    cpu->r[3] = value.xy;
    cpu->r[4] = u32(value.z) << 16;
}
template<> inline SxyzResult result<SxyzResult>(Cpu* cpu) {
    return {cpu->r[3], u16(cpu->r[4] >> 16)};
}
}

// HD: explicit GHS entry points for the constructor and value operators.
// The canonical shared csXyz layout has fields only; autoglue cannot express these operators.
csXyz* csXyz__ct(csXyz*, s16, s16, s16);
SxyzResult csXyz_Add(csXyz*, csXyz*);
void csXyz_AddAssign(csXyz*, csXyz*);
SxyzResult csXyz_Multiply(csXyz*, f32);
void* csXyz_allocate(u32);
