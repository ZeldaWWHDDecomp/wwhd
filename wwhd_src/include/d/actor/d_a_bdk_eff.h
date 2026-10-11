#pragma once
#include "d/actor/d_a_bdk.h"

// HD: copies through integer fields preserve every float bit, including signaling NaNs.
static inline void bdkEffect_copyFloat(be<f32>& dst, const be<f32>& src) {
    *reinterpret_cast<be<u32>*>(&dst) = *reinterpret_cast<const be<u32>*>(&src);
}
static inline void bdkEffect_copyVector(cXyz* dst, const cXyz* src) {
    bdkEffect_copyFloat(dst->x, src->x);
    bdkEffect_copyFloat(dst->y, src->y);
    bdkEffect_copyFloat(dst->z, src->z);
}
struct BdkEffectModel { u8 mPrefix[0xC8]; Mtx34 mBaseTRMtx; };
WWHD_OFFSET(BdkEffectModel, mBaseTRMtx, 0xC8);
WWHD_SIZE(BdkEffectModel, 0xF8);
static inline void bdkEffect_setBaseTRMtx(J3DModel* model, const Mtx34* source) {
    Mtx34* destination = &reinterpret_cast<BdkEffectModel*>(model)->mBaseTRMtx;
    f32 elements[3][4];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            elements[row][column] = source->m[row][column];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            destination->m[row][column] = elements[row][column];
}
void BdkEffect_cBgS_LinChk_dt(cBgS_LinChk*, u32);
void BdkEffect_cBgS_Chk_dt(cBgS_Chk*, u32);
BOOL BdkEffect_land_area_check(cXyz*, f32);
enum : u32 { BdkEffect_LIN_PASS_VTABLE = 0x10007F38, BdkEffect_GROUP_VTABLE = 0x10007E88,
    BdkEffect_LIN_POLY_VTABLE = 0x10007E78, BdkEffect_GND_PASS_VTABLE = 0x10007EC8,
    BdkEffect_GND_POLY_VTABLE = 0x10007EA8 };
static inline void bdkEffect_destroyChecks(dBgS_LinChk* lin, dBgS_GndChk* gnd) {
    lin->dBgS_PolyPassChk::__vtbl = BdkEffect_LIN_PASS_VTABLE;
    lin->dBgS_GrpPassChk::__vtbl = BdkEffect_GROUP_VTABLE;
    lin->cBgS_PolyInfo::__vtbl = BdkEffect_LIN_POLY_VTABLE;
    BdkEffect_cBgS_LinChk_dt(lin, 0);
    gnd->dBgS_PolyPassChk::__vtbl = BdkEffect_GND_PASS_VTABLE;
    gnd->dBgS_GrpPassChk::__vtbl = BdkEffect_GROUP_VTABLE;
    gnd->cBgS_PolyInfo::__vtbl = BdkEffect_GND_POLY_VTABLE;
    BdkEffect_cBgS_Chk_dt(gnd, 0);
}
