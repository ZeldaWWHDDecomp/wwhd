#include "gabi.h"

using namespace gabi;

// CC0 tww JPADynamicsBlockArc template; HD identities confirmed from
// vtable 0x1017B5E8 and the binary loads. The archive data pointer is at +4.
// Start frame and lifetime use signed halfword loads in HD.

f32 JPADynamicsBlockArc_getVolumeSweep(u32 block) {
    WWHD_FUNC(0x0284B588, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x4);
}
VERIFY(0x0284B588, JPADynamicsBlockArc_getVolumeSweep);

f32 JPADynamicsBlockArc_getVolumeMinRad(u32 block) {
    WWHD_FUNC(0x0284B594, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x8);
}
VERIFY(0x0284B594, JPADynamicsBlockArc_getVolumeMinRad);

u32 JPADynamicsBlockArc_getDivNumber(u32 block) {
    WWHD_FUNC(0x0284B5A0, u32, block);
    u32 data = load<u32>(block + 4);
    return load<u16>(data + 0xE);
}
VERIFY(0x0284B5A0, JPADynamicsBlockArc_getDivNumber);

f32 JPADynamicsBlockArc_getRate(u32 block) {
    WWHD_FUNC(0x0284B5AC, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x10);
}
VERIFY(0x0284B5AC, JPADynamicsBlockArc_getRate);

f32 JPADynamicsBlockArc_getRateRndm(u32 block) {
    WWHD_FUNC(0x0284B5B8, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x14);
}
VERIFY(0x0284B5B8, JPADynamicsBlockArc_getRateRndm);

u32 JPADynamicsBlockArc_getRateStep(u32 block) {
    WWHD_FUNC(0x0284B5C4, u32, block);
    u32 data = load<u32>(block + 4);
    return load<u8>(data + 0x18);
}
VERIFY(0x0284B5C4, JPADynamicsBlockArc_getRateStep);

s32 JPADynamicsBlockArc_getMaxFrame(u32 block) {
    WWHD_FUNC(0x0284B5D0, s32, block);
    u32 data = load<u32>(block + 4);
    return load<s16>(data + 0x1A);
}
VERIFY(0x0284B5D0, JPADynamicsBlockArc_getMaxFrame);

s32 JPADynamicsBlockArc_getStartFrame(u32 block) {
    WWHD_FUNC(0x0284B5DC, s32, block);
    u32 data = load<u32>(block + 4);
    return load<s16>(data + 0x1C);
}
VERIFY(0x0284B5DC, JPADynamicsBlockArc_getStartFrame);

s32 JPADynamicsBlockArc_getLifeTime(u32 block) {
    WWHD_FUNC(0x0284B5E8, s32, block);
    u32 data = load<u32>(block + 4);
    return load<s16>(data + 0x1E);
}
VERIFY(0x0284B5E8, JPADynamicsBlockArc_getLifeTime);

f32 JPADynamicsBlockArc_getLifeTimeRndm(u32 block) {
    WWHD_FUNC(0x0284B5F4, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x20);
}
VERIFY(0x0284B5F4, JPADynamicsBlockArc_getLifeTimeRndm);

f32 JPADynamicsBlockArc_getAccel(u32 block) {
    WWHD_FUNC(0x0284B648, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x4C);
}
VERIFY(0x0284B648, JPADynamicsBlockArc_getAccel);

f32 JPADynamicsBlockArc_getAccelRndm(u32 block) {
    WWHD_FUNC(0x0284B654, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x50);
}
VERIFY(0x0284B654, JPADynamicsBlockArc_getAccelRndm);

f32 JPADynamicsBlockArc_getAirResist(u32 block) {
    WWHD_FUNC(0x0284B660, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x3C);
}
VERIFY(0x0284B660, JPADynamicsBlockArc_getAirResist);

f32 JPADynamicsBlockArc_getAirResistRndm(u32 block) {
    WWHD_FUNC(0x0284B66C, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x40);
}
VERIFY(0x0284B66C, JPADynamicsBlockArc_getAirResistRndm);

f32 JPADynamicsBlockArc_getMoment(u32 block) {
    WWHD_FUNC(0x0284B678, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x44);
}
VERIFY(0x0284B678, JPADynamicsBlockArc_getMoment);

f32 JPADynamicsBlockArc_getMomentRndm(u32 block) {
    WWHD_FUNC(0x0284B684, f32, block);
    u32 data = load<u32>(block + 4);
    return load<f32>(data + 0x48);
}
VERIFY(0x0284B684, JPADynamicsBlockArc_getMomentRndm);

