/**
 * c_m3d_g_pla.cpp
 * Component - planes with normals
 *
 * Written in the style of the GameCube decompilation and verified function by function
 * against the recompiled WWHD original (tools/verify, unit c_m3d_g_pla).
 */
#include "SSystem/SComponent/c_m3d_g_pla.h"
#include <cmath>

static const f32 initialDistance = 0.0f;
static const f32 G_CM3D_F_ABS_MIN = 0.000003814697265625f;
enum : u32 { planeVtable = 0x1000339C }; // HD: cM3dGPla vtable.

/* 02018BA8 */
s16 cM3dGPla::CalcAngleXz(be<s16>* pAngleX, be<s16>* pAngleY) {
    // HD: forwards the callee's s16 result (GameCube returns void).
    return cM3d_CalcVecAngle(&mNormal, pAngleX, pAngleY);
}

/* 02018A7C */
void cM3dGPla::SetupNP0(const cXyz* pNormal, const cXyz* pPoint) {
    // GHS copies these components through floating-point loads, in this order.
    mNormal.x = pNormal->x;
    mNormal.y = pNormal->y;
    mNormal.z = pNormal->z;
    PSVECNormalize(&mNormal, &mNormal);
    f32 d = PSVECDotProduct(&mNormal, pPoint);
    mD = -d;
}

/* 02018AE4 */
u32 cM3dGPla::getCrossY(const cXyz* i_axis, be<f32>* i_value) {
    f32 y = mNormal.y;
    if (std::fabs(y) < G_CM3D_F_ABS_MIN) {
        return FALSE;
    }
    f32 nx = mNormal.x;
    f32 px = i_axis->x;
    f32 product = nx * px;
    f32 nz = mNormal.z;
    f32 pz = i_axis->z;
    f32 numerator = gabi::fnmsubs(nz, pz, -product);
    f32 d = mD;
    *i_value = (numerator - d) / y;
    return TRUE;
}

/* 02018B38 */
u32 cM3dGPla::getCrossYLessD(const cXyz* i_axis, be<f32>* i_value) {
    f32 y = mNormal.y;
    if (std::fabs(y) < G_CM3D_F_ABS_MIN) {
        return FALSE;
    }
    f32 px = i_axis->x;
    f32 nx = mNormal.x;
    f32 product = nx * px;
    f32 nz = mNormal.z;
    f32 pz = i_axis->z;
    *i_value = gabi::fnmsubs(nz, pz, -product) / y;
    return TRUE;
}

/* 02018B84 */
void cM3dGPla::Set(const cM3dGPla* pla) {
    mNormal = pla->mNormal;
    mD = pla->mD;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 020189A0 */
// HD: GHS constructor allocates when called with NULL, then initializes mD.
static cM3dGPla* cM3dGPla__ct(cM3dGPla* i_this) {
    if (i_this == NULL) {
        i_this = (cM3dGPla*)operator_new(sizeof(cM3dGPla));
    }
    if (i_this != NULL) {
        f32 d = initialDistance;
        i_this->__vtbl = planeVtable;
        i_this->mD = d;
    }
    return i_this;
}

/* 020189EC */
// HD: allocating constructor with a normal and distance; GHS stores mD before the last normal word.
static cM3dGPla* cM3dGPla__ct_normal(cM3dGPla* i_this, const cXyz* normal, f32 d) {
    if (i_this == NULL) {
        i_this = (cM3dGPla*)operator_new(sizeof(cM3dGPla));
    }
    if (i_this != NULL) {
        f32 initial = initialDistance;
        i_this->__vtbl = planeVtable;
        i_this->mD = initial;
        // Integer word copies preserve signalling NaNs and the original aliasing order.
        *reinterpret_cast<be<u32>*>(&i_this->mNormal.x) = *reinterpret_cast<const be<u32>*>(&normal->x);
        *reinterpret_cast<be<u32>*>(&i_this->mNormal.y) = *reinterpret_cast<const be<u32>*>(&normal->y);
        u32 z = *reinterpret_cast<const be<u32>*>(&normal->z);
        i_this->mD = d;
        *reinterpret_cast<be<u32>*>(&i_this->mNormal.z) = z;
    }
    return i_this;
}

/* 02018BAC */
static void __sinit_c_m3d_g_pla_cpp() {
    sinit_header_statics_rodata(0x101FF948, 0x1018C988, 0x101FF954, 0x100033B4);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data initialDistance 100033AC
data G_CM3D_F_ABS_MIN 100030B8
ext 028E8EF0 void PSVECNormalize(const cXyz*, cXyz*)
ext 02017264 s16 cM3d_CalcVecAngle(const cXyz*, be<s16>*, be<s16>*)
*/
