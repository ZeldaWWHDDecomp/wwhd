/**
 * c_m3d_g_cyl.cpp
 * Component - cylinder geometry
 */
#include "SSystem/SComponent/c_m3d_g_cyl.h"
#include <bit>
static const f32 rangeMin = -1.0e32;
static const f32 rangeMax = 1.0e32;
static const f32 initialSize = 0.0f;
static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
// HD: translation-unit header statics, kept separate from the cylinder class.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject;
    u8 padding[2];
};
struct HeaderZeroObject { be<u32> words[4]; };
#define headerStatics (*gabi::at<HeaderStatics>(0x101FF910))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x101FF91C))
enum : u32 { cylinderVtable = 0x1000334C }; // HD: cylinder vtable.
// HD: preserve the GHS NaN-bit test, including signalling NaNs.
static bool isNan(f32 v) {
    return (std::bit_cast<u32>(v) << 1) > 0xFF000000u;
}

/* 020182E0 */
// HD: diagnostic strings and line numbers differ from GameCube.
void cM3dGCyl::SetC(const cXyz* center) {
    f32 x = center->x;
    if (isNan(x)) {
        JUT_ASSERT_fail(STR(0x10003150), 80, STR(0x10003160));
    }
    f32 y = center->y;
    if (isNan(y)) {
        JUT_ASSERT_fail(STR(0x10003150), 81, STR(0x1000319C));
    }
    f32 z = center->z;
    if (isNan(z)) {
        JUT_ASSERT_fail(STR(0x10003150), 82, STR(0x100031D8));
    }
    x = center->x;
    f32 low = rangeMin;
    f32 high = rangeMax;
    bool valid = low < x && x < high;
    if (valid) {
        y = center->y;
        valid = low < y && y < high;
    }
    if (valid) {
        z = center->z;
        valid = low < z && z < high;
    }
    if (!valid) {
        JUT_ASSERT_fail(STR(0x10003150), 83, STR(0x10003214));
        y = center->y;
        z = center->z;
        x = center->x;
    }
    mCenter.y = y;
    mCenter.z = z;
    mCenter.x = x;
}

/* 02018428 */
void cM3dGCyl::SetH(f32 height) {
    if (isNan(height)) {
        JUT_ASSERT_fail(STR(0x10003288), 95, STR(0x100032B4));
    }
    f32 low = rangeMin;
    f32 high = rangeMax;
    if (!(low < height && height < high)) {
        JUT_ASSERT_fail(STR(0x10003288), 96, STR(0x10003298));
    }
    mHeight = height;
}

/* 020184DC */
void cM3dGCyl::SetR(f32 radius) {
    if (isNan(radius)) {
        JUT_ASSERT_fail(STR(0x100032E8), 119, STR(0x10003314));
    }
    f32 low = rangeMin;
    f32 high = rangeMax;
    if (!(low < radius && radius < high)) {
        JUT_ASSERT_fail(STR(0x100032E8), 120, STR(0x100032F8));
    }
    mRadius = radius;
}

/* 02018590 */
// HD: GHS allocates when called with NULL and initializes radius and height.
static cM3dGCyl* cM3dGCyl__ct(cM3dGCyl* self) {
    if (self == NULL) {
        self = (cM3dGCyl*)operator_new(sizeof(cM3dGCyl));
    }
    if (self != NULL) {
        f32 zero = initialSize;
        self->__vtbl = cylinderVtable;
        self->mHeight = zero;
        self->mRadius = zero;
    }
    return self;
}

/* 020185E0 */
void cM3dGCyl::SetShape(const cM3dGCylS* src) {
    gabi::Local<cXyz> center;
    center->x = (f32)src->mCenter.x;
    center->y = (f32)src->mCenter.y;
    center->z = (f32)src->mCenter.z;
    SetC(center.get());
    f32 radius = src->mRadius;
    SetR(radius);
    f32 height = src->mHeight;
    SetH(height);
}

/* 0201864C */
void cM3dGCyl::Set(const cXyz* center, f32 radius, f32 height) {
    SetC(center);
    SetR(radius);
    SetH(height);
}

/* 020186C4 */
s32 cM3dGCyl::crossCylinder(const cM3dGCyl* other, be<f32>* distance) {
    return cM3d_Cross_CylCyl(this, other, distance);
}

/* 020186C8 */
s32 cM3dGCyl::crossSphere(const cM3dGSph* sphere, cXyz* position) {
    gabi::Local<be<f32>> distance;
    return cM3d_Cross_CylSph(this, sphere, position, distance.get());
}

/* 020186EC */
static void __sinit_c_m3d_g_cyl_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C940));
    f32 lo = headerPiMin;
    f32 hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C94C));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C958));
}

/* WWHD glue (tools/verify/autoglue.py): data and overloaded collision helpers
data rangeMin 10003148
data rangeMax 1000314C
data initialSize 1000335C
data headerPiMin 10003364
data headerPiMax 10003368
ext 0273AD10 void* operator_new(u32)
ext 0273AA24 void JUT_ASSERT_fail(const char*, s32, const char*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
ext 02014E18 s32 cM3d_Cross_CylCyl(const cM3dGCyl*, const cM3dGCyl*, be<f32>*)
ext 02013854 s32 cM3d_Cross_CylSph(const cM3dGCyl*, const cM3dGSph*, cXyz*, be<f32>*)
*/
