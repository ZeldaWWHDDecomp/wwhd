/* Sphere geometry: GameCube names and WWHD layout.
 * Guest references use pointers. Overloads have distinct names and cross methods omit
 * const qualification for verifier registration. */
#pragma once
#include "SSystem/SComponent/c_m3d_g_cyl.h"

struct cM3dGSphS {
    cXyz mCenter;
    be<f32> mRadius;
};
WWHD_SIZE(cM3dGSphS, 0x10);
class cM3dGSph {
public:
    void SetC(const cXyz* center);
    void SetR(f32 radius);
    void Set(const cXyz* center, f32 radius);
    void SetShape(const cM3dGSphS* source);
    void SetCxyz(f32 x, f32 y, f32 z);
    u32 crossCylinder(const cM3dGCyl* other, cXyz* out);
    u32 crossSphere(const cM3dGSph* other, cXyz* out);

    /* 0x00 */ cXyz mCenter;
    /* 0x0C */ be<f32> mRadius;
    /* 0x10 */ be<u32> __vtbl;
};
WWHD_OFFSET(cM3dGSph, mCenter, 0x00);
WWHD_OFFSET(cM3dGSph, mRadius, 0x0C);
WWHD_OFFSET(cM3dGSph, __vtbl, 0x10);
WWHD_SIZE(cM3dGSph, 0x14);
u32 cM3d_Cross_SphSph(const cM3dGSph* a, const cM3dGSph* b, cXyz* out);
