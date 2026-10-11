/* Cylinder geometry: GameCube names and WWHD layout.
 * Guest references use pointers; SetShape and crossCylinder/crossSphere name overloads
 * separately because autoglue requires unambiguous function addresses.
 * The cross methods omit GameCube const qualification for verifier member-pointer support. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"

struct cM3dGCylS {
    cXyz mCenter;
    be<f32> mRadius;
    be<f32> mHeight;
};
WWHD_SIZE(cM3dGCylS, 0x14);
class cM3dGSph;
class cM3dGCyl {
public:
    void SetC(const cXyz* center);
    void SetH(f32 height);
    void SetR(f32 radius);
    void SetShape(const cM3dGCylS* src);
    void Set(const cXyz* center, f32 radius, f32 height);
    s32 crossCylinder(const cM3dGCyl* other, be<f32>* distance);
    s32 crossSphere(const cM3dGSph* sphere, cXyz* position);

    /* 0x00 */ cXyz mCenter;
    /* 0x0C */ be<f32> mRadius;
    /* 0x10 */ be<f32> mHeight;
    /* 0x14 */ be<u32> __vtbl;
};
WWHD_OFFSET(cM3dGCyl, mCenter, 0x00);
WWHD_OFFSET(cM3dGCyl, mRadius, 0x0C);
WWHD_OFFSET(cM3dGCyl, mHeight, 0x10);
WWHD_OFFSET(cM3dGCyl, __vtbl, 0x14);
WWHD_SIZE(cM3dGCyl, 0x18);
s32 cM3d_Cross_CylCyl(const cM3dGCyl*, const cM3dGCyl*, be<f32>*);
s32 cM3d_Cross_CylSph(const cM3dGCyl*, const cM3dGSph*, cXyz*, be<f32>*);

// Existing bindings are declared here without importing d/d_cc_d.h's caller-only cylinder.
void* operator_new(u32 size);
void JUT_ASSERT_fail(const char* file, s32 line, const char* msg);
void registerGlobalObject(void* descriptor);
void initializeHeaderByte(void* object);
void initializeHeaderFlag(void* object);
