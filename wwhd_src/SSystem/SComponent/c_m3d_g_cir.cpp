/** Circle geometry. */
#include "SSystem/SComponent/c_m3d_g_cir.h"

static const f32 initialRadius = 0.0f;
enum : u32 { circleVtable = 0x10003100 }; // HD: circle vtable.
static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
// HD: translation-unit header statics; existing API from the minimal cylinder header.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject;
    u8 padding[2];
};
struct HeaderZeroObject { be<u32> words[4]; };
#define headerStatics (*gabi::at<HeaderStatics>(0x101FF8D8))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x101FF8E4))

/* 02018034 */
// HD: GHS deleting destructor forwards both this and flags to operator delete.
static void cM3dGCir__dt(cM3dGCir* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1)) {
        deleteCircle(i_this, flags);
    }
}

/* 02018048 */
// HD: GHS allocates when called with NULL, then initializes the base and radius.
static cM3dGCir* cM3dGCir__ct(cM3dGCir* i_this) {
    if (i_this == NULL) {
        i_this = (cM3dGCir*)operator_new(sizeof(cM3dGCir));
    }
    if (i_this != NULL) {
        constructCircleBase(i_this);
        f32 radius = initialRadius;
        i_this->__vtbl = circleVtable;
        i_this->mRadius = radius;
    }
    return i_this;
}

/* 020180A8 */
void cM3dGCir::Set(f32 x, f32 y, f32 radius, f32 z) {
    mPosZ = z;
    mPosY = y;
    mRadius = radius;
    mPosX = x;
}

/* 020180BC */
static void __sinit_c_m3d_g_cir_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C8F8));
    f32 lo = headerPiMin;
    f32 hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C904));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C910));
}

/* WWHD glue (tools/verify/autoglue.py)
data initialRadius 100030EC
data headerPiMin 100030F4
data headerPiMax 100030F8
ext 0273AD10 void* operator_new(u32)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
ext 0201083C void constructCircleBase(cM3dGCir*)
ext 0273AF40 void deleteCircle(cM3dGCir*, s32)
*/
