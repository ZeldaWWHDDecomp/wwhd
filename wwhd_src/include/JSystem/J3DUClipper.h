/* Frustum clipper.
 * HD: accessed members sit four bytes before GC field offsets. */
#pragma once
#include "wwhd.h"
namespace Clipper {
struct Vec {
    be<f32> x, y, z;
};
WWHD_SIZE(Vec, 12);
} // namespace Clipper
struct J3DModel;
struct J3DUClipper {
    Clipper::Vec mPlane[4];
    u8 _30[0x18];
    be<f32> mFovY, mAspect, mNear, mFar;
    void init();
    void calcViewFrustum();
    u32 clipSphere(const Mtx34*, const Clipper::Vec*, f32);
    u32 clipBox(const Mtx34*, const Clipper::Vec*, const Clipper::Vec*);
    u32 clipByBox(J3DModel*);
};
WWHD_OFFSET(J3DUClipper, mFovY, 0x48);
WWHD_OFFSET(J3DUClipper, mAspect, 0x4C);
WWHD_OFFSET(J3DUClipper, mNear, 0x50);
WWHD_OFFSET(J3DUClipper, mFar, 0x54);
WWHD_SIZE(J3DUClipper, 0x58);
f64 clipperTan(f32);
void clipperNormalize(Clipper::Vec*, Clipper::Vec*);
void clipperTransform(const Mtx34*, const Clipper::Vec*, Clipper::Vec*);
void clipperRegisterStatic(void*);
