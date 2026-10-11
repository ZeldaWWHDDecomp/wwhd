/**
 * c_m3d_g_aab.cpp
 * Component - axis-aligned bounding boxes
 *
 * Written in the style of the GameCube decompilation and verified function by function
 * against the recompiled WWHD original (tools/verify, unit c_m3d_g_aab).
 */

#include "SSystem/SComponent/c_m3d_g_aab.h"

static const f32 G_CM3D_F_INF = 1000000000.0f;
static const f32 G_CM3D_F_NEG_INF = -1000000000.0f;
static const f32 centerScale = 0.5f;

/* 02017E74 */
void cM3dGAab::SetMinMax(const cXyz* pMinMax) {
    SetMin(pMinMax);
    // HD: the tail call forwards the argument registers left by SetMin.
    gabi::at<cM3dGAab>(gabi::cpu->r[3])->SetMax(gabi::at<cXyz>(gabi::cpu->r[4]));
}

/* 02017DF4 */
void cM3dGAab::SetMin(const cXyz* pMin) {
    f32 x = pMin->x;
    f32 oldX = mMin.x;
    f32 oldY = mMin.y;
    if (oldX > x) {
        mMin.x = x;
    }
    f32 y = pMin->y;
    f32 oldZ = mMin.z;
    if (oldY > y) {
        mMin.y = y;
    }
    f32 z = pMin->z;
    if (oldZ > z) {
        mMin.z = z;
    }
}

/* 02017E34 */
void cM3dGAab::SetMax(const cXyz* pMax) {
    f32 x = pMax->x;
    f32 oldX = mMax.x;
    f32 oldY = mMax.y;
    if (oldX < x) {
        mMax.x = x;
    }
    f32 y = pMax->y;
    f32 oldZ = mMax.z;
    if (oldY < y) {
        mMax.y = y;
    }
    f32 z = pMax->z;
    if (oldZ < z) {
        mMax.z = z;
    }
}

/* 02017E98 */
void cM3dGAab::SetMinMaxAab(const cM3dGAab* aab) {
    SetMinMax(&aab->mMin);
    // HD: the second call forwards the first call's argument registers, advancing to mMax.
    cM3dGAab* box = gabi::at<cM3dGAab>(gabi::cpu->r[3]);
    cM3dGAab* bounds = gabi::at<cM3dGAab>(gabi::cpu->r[4]);
    box->SetMinMax(&bounds->mMax);
}

/* 02017D08 */
void cM3dGAab::Set(const cXyz* min, const cXyz* max) {
    mMin = *min;
    mMax = *max;
}

/* 02017F0C */
void cM3dGAab::CalcCenter(cXyz* pOut) {
    PSVECAdd(&mMin, &mMax, pOut);
    PSVECScale(pOut, pOut, centerScale);
}

/* 02017DAC */
void cM3dGAab::ClearForMinMax() {
    f32 lo = G_CM3D_F_INF;
    f32 hi = G_CM3D_F_NEG_INF;
    mMin.z = lo;
    mMax.x = hi;
    mMin.y = lo;
    mMax.y = hi;
    mMin.x = lo;
    mMax.z = hi;
}

/* 02017DD8 */
void cM3dGAab::ClearForMinMaxY() {
    f32 lo = G_CM3D_F_INF;
    f32 hi = G_CM3D_F_NEG_INF;
    mMin.y = lo;
    mMax.y = hi;
}

/* 02017EE8 */
void cM3dGAab::SetMinMaxY(f32 y) {
    f32 lo = mMin.y;
    f32 hi = mMax.y;
    if (lo > y) {
        mMin.y = y;
    }
    if (hi < y) {
        mMax.y = y;
    }
}

/* 02017EC0 */
void cM3dGAab::SetMinY(f32 y) {
    if (mMin.y > y) {
        mMin.y = y;
    }
}

/* 02017ED4 */
void cM3dGAab::SetMaxY(f32 y) {
    if (mMax.y < y) {
        mMax.y = y;
    }
}

/* 02017F54 */
void cM3dGAab::PlusR(f32 r) {
    // GHS load/store order and PowerPC NaN operand order are kept explicit.
    f32 x = mMin.x;
    f32 y = mMin.y;
    f32 z = mMin.z;
    f32 maxX = mMax.x;
    f32 maxZ = mMax.z;
    y = gabi::fsubs_ppc(y, r);
    z = gabi::fsubs_ppc(z, r);
    maxX = gabi::fadds_ppc(maxX, r);
    mMin.y = y;
    x = gabi::fsubs_ppc(x, r);
    f32 maxY = mMax.y;
    mMin.z = z;
    maxY = gabi::fadds_ppc(maxY, r);
    mMin.x = x;
    maxZ = gabi::fadds_ppc(maxZ, r);
    mMax.x = maxX;
    mMax.y = maxY;
    mMax.z = maxZ;
}

/* 02017D3C */
BOOL cM3dGAab::CrossY(const cXyz* v) {
    f32 x = v->x;
    if (mMin.x > x || mMax.x < x) {
        return FALSE;
    }
    f32 z = v->z;
    return !(mMin.z > z || mMax.z < z);
}

/* 02017D84 */
BOOL cM3dGAab::UnderPlaneYUnder(f32 y) {
    return mMin.y < y;
}

/* 02017D98 */
BOOL cM3dGAab::TopPlaneYUnder(f32 y) {
    return mMax.y < y;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02017FA0 */
static void __sinit_c_m3d_g_aab_cpp() {
    sinit_header_statics_rodata(0x101FF8BC, 0x1018C8D4, 0x101FF8C8, 0x100030E0);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data G_CM3D_F_INF 100030D0
data G_CM3D_F_NEG_INF 100030D4
data centerScale 100030D8
*/
