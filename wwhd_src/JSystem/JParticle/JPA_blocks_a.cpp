// Local reconstruction from WWHD, with the CC0 TWW JPA1 getters as template.
// Dynamics names confirmed from vtable 0x1017B5E8.
#include "gabi.h"

namespace jpa_blocks_a {
using namespace gabi;

u8 JPAKeyBlockArc_getID(u32 self) {
    WWHD_FUNC(0x028484B4, u8, self);
    return load<u8>(load<u32>(self + 4));
}
VERIFY(0x028484B4, JPAKeyBlockArc_getID);

bool JPAKeyBlockArc_isLoopEnable(u32 self) {
    WWHD_FUNC(0x028484C0, bool, self);
    return load<u8>(load<u32>(self + 4) + 6) != 0;
}
VERIFY(0x028484C0, JPAKeyBlockArc_isLoopEnable);

u8 JPAKeyBlockArc_getNumber(u32 self) {
    WWHD_FUNC(0x028484D4, u8, self);
    return load<u8>(load<u32>(self + 4) + 4);
}
VERIFY(0x028484D4, JPAKeyBlockArc_getNumber);

u32 JPAKeyBlockArc_getKeyDataPtr(u32 self) {
    WWHD_FUNC(0x028484E0, u32, self);
    return load<u32>(self + 8);
}
VERIFY(0x028484E0, JPAKeyBlockArc_getKeyDataPtr);

u32 JPADynamicsBlockArc_getDataFlag(u32 self) {
    WWHD_FUNC(0x0284B4D8, u32, self);
    return load<u32>(load<u32>(self + 4));
}
VERIFY(0x0284B4D8, JPADynamicsBlockArc_getDataFlag);

u32 JPADynamicsBlockArc_getUseKeyFlag(u32 self) {
    WWHD_FUNC(0x0284B4E4, u32, self);
    return 0;
}
VERIFY(0x0284B4E4, JPADynamicsBlockArc_getUseKeyFlag);

void JPADynamicsBlockArc_getEmitterScl(u32 self, u32 out) {
    WWHD_FUNC(0x0284B4EC, void, self, out);
    u32 data = load<u32>(self + 4);
    // Pure lfs/stfs copies preserve all bits, including signalling NaNs.
    u32 x = load<u32>(data + 0x54);
    u32 z = load<u32>(data + 0x5C);
    u32 y = load<u32>(data + 0x58);
    store<u32>(out + 8, z);
    store<u32>(out, x);
    store<u32>(out + 4, y);
}
VERIFY(0x0284B4EC, JPADynamicsBlockArc_getEmitterScl);

void JPADynamicsBlockArc_getEmitterRot(u32 self, u32 out) {
    WWHD_FUNC(0x0284B50C, void, self, out);
    u32 data = load<u32>(self + 4);
    s16 z = load<s16>(data + 0x7C);
    s16 y = load<s16>(data + 0x7A);
    s16 x = load<s16>(data + 0x78);
    store<s16>(out + 4, z);
    store<s16>(out, x);
    store<s16>(out + 2, y);
}
VERIFY(0x0284B50C, JPADynamicsBlockArc_getEmitterRot);

void JPADynamicsBlockArc_getEmitterTrs(u32 self, u32 out) {
    WWHD_FUNC(0x0284B52C, void, self, out);
    u32 data = load<u32>(self + 4);
    u32 x = load<u32>(data + 0x60);
    u32 z = load<u32>(data + 0x68);
    u32 y = load<u32>(data + 0x64);
    store<u32>(out + 8, z);
    store<u32>(out, x);
    store<u32>(out + 4, y);
}
VERIFY(0x0284B52C, JPADynamicsBlockArc_getEmitterTrs);

void JPADynamicsBlockArc_getEmitterDir(u32 self, u32 out) {
    WWHD_FUNC(0x0284B54C, void, self, out);
    u32 data = load<u32>(self + 4);
    u32 x = load<u32>(data + 0x6C);
    u32 z = load<u32>(data + 0x74);
    u32 y = load<u32>(data + 0x70);
    store<u32>(out + 8, z);
    store<u32>(out, x);
    store<u32>(out + 4, y);
}
VERIFY(0x0284B54C, JPADynamicsBlockArc_getEmitterDir);

f32 JPADynamicsBlockArc_getInitVelOmni(u32 self) {
    WWHD_FUNC(0x0284B600, f32, self);
    return load<f32>(load<u32>(self + 4) + 0x24);
}
VERIFY(0x0284B600, JPADynamicsBlockArc_getInitVelOmni);

f32 JPADynamicsBlockArc_getInitVelAxis(u32 self) {
    WWHD_FUNC(0x0284B60C, f32, self);
    return load<f32>(load<u32>(self + 4) + 0x28);
}
VERIFY(0x0284B60C, JPADynamicsBlockArc_getInitVelAxis);

f32 JPADynamicsBlockArc_getInitVelRndm(u32 self) {
    WWHD_FUNC(0x0284B618, f32, self);
    return load<f32>(load<u32>(self + 4) + 0x2C);
}
VERIFY(0x0284B618, JPADynamicsBlockArc_getInitVelRndm);

f32 JPADynamicsBlockArc_getInitVelDir(u32 self) {
    WWHD_FUNC(0x0284B624, f32, self);
    return load<f32>(load<u32>(self + 4) + 0x30);
}
VERIFY(0x0284B624, JPADynamicsBlockArc_getInitVelDir);

f32 JPADynamicsBlockArc_getSpread(u32 self) {
    WWHD_FUNC(0x0284B630, f32, self);
    return load<f32>(load<u32>(self + 4) + 0x38);
}
VERIFY(0x0284B630, JPADynamicsBlockArc_getSpread);

f32 JPADynamicsBlockArc_getInitVelRatio(u32 self) {
    WWHD_FUNC(0x0284B63C, f32, self);
    return load<f32>(load<u32>(self + 4) + 0x34);
}
VERIFY(0x0284B63C, JPADynamicsBlockArc_getInitVelRatio);

u32 JPADynamicsBlockArc_getVolumeType(u32 self) {
    WWHD_FUNC(0x0284B56C, u32, self);
    return (load<u32>(load<u32>(self + 4)) >> 8) & 7;
}
VERIFY(0x0284B56C, JPADynamicsBlockArc_getVolumeType);

u32 JPADynamicsBlockArc_getVolumeSize(u32 self) {
    WWHD_FUNC(0x0284B57C, u32, self);
    return load<u16>(load<u32>(self + 4) + 0x0C);
}
VERIFY(0x0284B57C, JPADynamicsBlockArc_getVolumeSize);
} // namespace jpa_blocks_a
