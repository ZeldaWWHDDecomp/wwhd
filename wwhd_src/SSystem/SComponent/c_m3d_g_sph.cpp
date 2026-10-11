/**
 * c_m3d_g_sph.cpp
 * Component - sphere geometry
 */
#include "SSystem/SComponent/c_m3d_g_sph.h"
#include <cmath>

static const f32 initialRadius = 0.0f;
static const f32 rangeMin = -1.0e32;
static const f32 rangeMax = 1.0e32;
static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
enum : u32 { sphereVtable = 0x100033C4 }; // HD: sphere vtable.

// HD: translation-unit header statics. Use the cylinder header's existing initialization API
// without importing bindings.h, which defines a caller-only cM3dGSph class.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject;
    u8 padding[2];
};
struct HeaderZeroObject { be<u32> words[4]; };
#define headerStatics (*gabi::at<HeaderStatics>(0x101FF964))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x101FF970))

/* 02018C40 */
// HD: allocating constructor initializes radius; GameCube constructor is empty.
static cM3dGSph* cM3dGSph__ct(cM3dGSph* self) {
    if (self == NULL) {
        self = (cM3dGSph*)operator_new(sizeof(cM3dGSph));
    }
    if (self != NULL) {
        f32 radius = initialRadius;
        self->__vtbl = sphereVtable;
        self->mRadius = radius;
    }
    return self;
}

/* 02018C8C */
// HD: assertion strings and line numbers differ from GameCube.
void cM3dGSph::SetR(f32 radius) {
    if (std::isnan(radius)) {
        JUT_ASSERT_fail(STR(0x100033E0), 66, STR(0x1000340C));
    }
    if (!(rangeMin < radius && radius < rangeMax)) {
        JUT_ASSERT_fail(STR(0x100033E0), 67, STR(0x100033F0));
    }
    mRadius = radius;
}

/* 02018D40 */
void cM3dGSph::SetC(const cXyz* center) {
    if (std::isnan((f32)center->x)) {
        JUT_ASSERT_fail(STR(0x10003440), 31, STR(0x10003450));
    }
    if (std::isnan((f32)center->y)) {
        JUT_ASSERT_fail(STR(0x10003440), 32, STR(0x10003488));
    }
    if (std::isnan((f32)center->z)) {
        JUT_ASSERT_fail(STR(0x10003440), 33, STR(0x100034C0));
    }
    f32 x = (f32)center->x;
    f32 lower = rangeMin;
    f32 y, z;
    bool valid = false;
    if (lower < x) {
        f32 upper = rangeMax;
        if (x < upper) {
            y = (f32)center->y;
            if (lower < y && y < upper) {
                z = (f32)center->z;
                valid = lower < z && z < upper;
            }
        }
    }
    if (!valid) {
        JUT_ASSERT_fail(STR(0x10003440), 34, STR(0x100034F8));
        y = (f32)center->y;
        z = (f32)center->z;
        x = (f32)center->x;
    }
    mCenter.y = y;
    mCenter.z = z;
    mCenter.x = x;
}

/* 02018E88 */
void cM3dGSph::Set(const cXyz* center, f32 radius) {
    SetC(center);
    SetR(radius);
}

/* 02018EDC */
void cM3dGSph::SetShape(const cM3dGSphS* source) {
    gabi::Local<cXyz> center;
    f32 x = source->mCenter.x;
    f32 y = source->mCenter.y;
    f32 z = source->mCenter.z;
    center->x = x;
    center->y = y;
    center->z = z;
    SetC(center.get());
    f32 radius = source->mRadius;
    SetR(radius);
}

/* 02018F3C */
// HD: routes scalar coordinates through the checked center setter.
void cM3dGSph::SetCxyz(f32 x, f32 y, f32 z) {
    gabi::Local<cXyz> center;
    center->z = z;
    center->y = y;
    center->x = x;
    SetC(center.get());
}

/* 02018F6C */
// HD: cross methods preserve the full integer result instead of narrowing to GameCube bool.
u32 cM3dGSph::crossCylinder(const cM3dGCyl* other, cXyz* out) {
    gabi::Local<be<f32>> distance;
    return cM3d_Cross_CylSph(other, this, out, distance.get());
}

/* 02018F9C */
u32 cM3dGSph::crossSphere(const cM3dGSph* other, cXyz* out) {
    return cM3d_Cross_SphSph(other, this, out);
}

/* 02018FAC */
static void __sinit_c_m3d_g_sph_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C9AC));
    f32 lo = headerPiMin;
    f32 hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C9B8));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C9C4));
}

/* WWHD glue (tools/verify/autoglue.py)
data initialRadius 100033D4
data rangeMin 100033D8
data rangeMax 100033DC
data headerPiMin 10003564
data headerPiMax 10003568
ext 0273AD10 void* operator_new(u32)
ext 0273AA24 void JUT_ASSERT_fail(const char*, s32, const char*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
ext 02013854 s32 cM3d_Cross_CylSph(const cM3dGCyl*, const cM3dGSph*, cXyz*, be<f32>*)
ext 02014010 u32 cM3d_Cross_SphSph(const cM3dGSph*, const cM3dGSph*, cXyz*)
*/
