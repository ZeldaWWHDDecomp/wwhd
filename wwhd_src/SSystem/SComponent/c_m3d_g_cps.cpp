/** Capsule geometry. */
#include "SSystem/SComponent/c_m3d_g_cps.h"

static const f32 initialRadius = 0.0f;
enum : u32 { capsuleVtable = 0x10003138 }; // HD: capsule vtable.
static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
// HD: translation-unit header statics; existing API from the minimal cylinder header.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject;
    u8 padding[2];
};
struct HeaderZeroObject { be<u32> words[4]; };
#define headerStatics (*gabi::at<HeaderStatics>(0x101FF8F4))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x101FF900))

/* 02018150 */
// HD: GHS allocates when called with NULL and initializes radius.
static cM3dGCps* cM3dGCps__ct(cM3dGCps* i_this) {
    if (i_this == NULL) {
        i_this = (cM3dGCps*)operator_new(sizeof(cM3dGCps));
    }
    if (i_this != NULL) {
        f32 radius = initialRadius;
        i_this->__vtbl = capsuleVtable;
        i_this->mRadius = radius;
    }
    return i_this;
}

/* 0201819C */
// HD: GHS deleting destructor tests the low flag bit.
static void cM3dGCps__dt(cM3dGCps* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 020181B0 */
void cM3dGCps::Set(const cXyz* start, const cXyz* end, f32 radius) {
    cM3dGLin_SetStartEnd(this, start, end);
    mRadius = radius;
}

/* 020181FC */
void cM3dGCps::SetShape(const cM3dGCpsS* shape) {
    cM3dGLin_SetStartEndVec(this, &shape->mStart, &shape->mEnd);
    f32 radius = shape->mRadius;
    mRadius = radius;
}

/* 02018240 */
void cM3dGCps::SetCps(const cM3dGCps* source) {
    f32 radius = source->mRadius;
    Set(&source->mStart, &source->mEnd, radius);
}

/* 0201824C */
static void __sinit_c_m3d_g_cps_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C91C));
    f32 lo = headerPiMin;
    f32 hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C928));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C934));
}

/* WWHD glue (tools/verify/autoglue.py)
data initialRadius 10003124
data headerPiMin 1000312C
data headerPiMax 10003130
ext 0273AD10 void* operator_new(u32)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
ext 0273AF40 void operator_delete(void*)
ext 02018808 void cM3dGLin_SetStartEnd(cM3dGCps*, const cXyz*, const cXyz*)
ext 0201883C void cM3dGLin_SetStartEndVec(cM3dGCps*, const cXyz*, const cXyz*)
*/
