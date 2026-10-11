/* Frustum clipping in tww style. */
#include "JSystem/J3DUClipper.h"
using Clipper::Vec;
static f32 l_near;
static f32 l_far;
static f32 l_half;
static f32 Deg2Rad;
static f32 l_zero;
static u32 l_firstLow;
static u32 l_firstHigh;
static u32 l_secondLow;
static u32 l_secondHigh;
struct ClipperRecord {
    u8 bytes[1];
};
static ClipperRecord l_record;
// HD: retain fused dot-product order and the extended tangent rounded to GHS precision.
static f32 dot(const Vec* p, f32 x, f32 y, f32 z) {
    f32 vy = gabi::fmuls_ppc(y, p->y);
    f32 xy = gabi::fmadds(x, p->x, vy);
    return gabi::fmadds(z, p->z, xy);
}
/* 02838000 */
void J3DUClipper::init() {
    f32 near = l_near, far = l_far;
    mNear = near;
    mFar = far;
}

/* 0283801C */
void J3DUClipper::calcViewFrustum() {
    // HD: GHS interleaves cross-product component stores; keep its products and rounding.

    f32 angle = gabi::fmuls_ppc(mFovY, l_half);
    angle = gabi::fmuls_ppc(angle, Deg2Rad);
    f64 tangent = clipperTan(angle);
    f32 near = mNear, ny = f32(f64(near) * round25(tangent)), minusY = -ny, minusNear = -near;
    f32 nx = gabi::fmuls_ppc(mAspect, ny), minusX = -nx;
    f32 twiceA = gabi::fmuls_ppc(minusNear, minusY), twiceB = gabi::fmuls_ppc(ny, minusNear);
    f32 nX = gabi::fmuls_ppc(minusNear, minusX), xY = gabi::fmuls_ppc(ny, minusX),
        yX = gabi::fmuls_ppc(minusX, minusY), xy = gabi::fmuls_ppc(nx, minusNear),
        yx = gabi::fmuls_ppc(nx, ny);
    mPlane[1].x = gabi::fsubs_ppc(twiceB, twiceB);
    mPlane[0].z = gabi::fsubs_ppc(yX, xY);
    mPlane[0].x = gabi::fsubs_ppc(twiceB, twiceA);
    mPlane[2].x = gabi::fsubs_ppc(twiceA, twiceB);
    mPlane[2].y = gabi::fsubs_ppc(xy, xy);
    mPlane[0].y = gabi::fsubs_ppc(nX, nX);
    mPlane[1].y = gabi::fsubs_ppc(nX, xy);
    mPlane[1].z = gabi::fsubs_ppc(yx, xY);
    mPlane[2].z = gabi::fsubs_ppc(gabi::fmuls_ppc(nx, ny), gabi::fmuls_ppc(minusY, nx));
    mPlane[3].x = gabi::fsubs_ppc(twiceA, twiceA);
    mPlane[3].y = gabi::fsubs_ppc(xy, nX);
    mPlane[3].z = gabi::fsubs_ppc(yX, gabi::fmuls_ppc(minusY, nx));
    for (u32 i = 0; i < 4; ++i)
        clipperNormalize(&mPlane[i], &mPlane[i]);
}

/* 02838148 */
u32 J3DUClipper::clipSphere(const Mtx34* matrix, const Clipper::Vec* pos, f32 radius) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<Clipper::Vec> out;
    clipperTransform(matrix, pos, out.get());
    f32 z = out->z, y = out->y, x = out->x;
    if (-z < gabi::fsubs_ppc(mNear, radius))
        return 1;
    if (-z > gabi::fadds_ppc(mFar, radius))
        return 1;
    for (u32 i = 0; i < 4; ++i)
        if (dot(&mPlane[i], x, y, z) > radius)
            return 1;
    return 0;
}

/* 02838274 */
u32 J3DUClipper::clipBox(const Mtx34* matrix, const Clipper::Vec* minimum,
                         const Clipper::Vec* maximum) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<Clipper::Vec[8]> corners;
    gabi::Local<Clipper::Vec> out;
    f32 minZ = minimum->z, maxY = maximum->y, maxZ = maximum->z, minX = minimum->x,
        maxX = maximum->x, minY = minimum->y;
    f32 xs[8] = {maxX, maxX, minX, minX, maxX, maxX, minX, minX};
    f32 ys[8] = {maxY, maxY, maxY, maxY, minY, minY, minY, minY};
    f32 zs[8] = {minZ, maxZ, maxZ, minZ, minZ, maxZ, maxZ, minZ};
    for (u32 i = 0; i < 8; ++i) {
        (*corners)[i].x = xs[i];
        (*corners)[i].y = ys[i];
        (*corners)[i].z = zs[i];
    }
    u32 counts[6] = {};
    f32 zero = l_zero;
    for (u32 i = 0; i < 8; ++i) {
        clipperTransform(matrix, &(*corners)[i], out.get());
        f32 z = out->z, y = out->y, x = out->x;
        u32 any = 0;
        if (-z < mNear) {
            ++counts[4];
            ++any;
        }
        if (-z > mFar) {
            ++counts[5];
            ++any;
        }
        for (u32 j = 0; j < 4; ++j)
            if (dot(&mPlane[j], x, y, z) > zero) {
                ++counts[j];
                ++any;
            }
        if (!any)
            return 0;
    }
    for (u32 i : {0u, 2u, 1u, 3u, 4u, 5u})
        if (counts[i] == 8)
            return 1;
    return 0;
}

/* 028384FC */
static void __sinit_J3DUClipper_cpp() {
    l_firstLow = 0;
    l_secondLow = 0;
    l_firstHigh = 0;
    l_secondHigh = 0;
    clipperRegisterStatic(&l_record);
}

/* 02838524 */
u32 J3DUClipper::clipByBox(J3DModel* model) { return 0; }

// HD: overload names distinguish verifier member pointers; clipByBox is a stub.
// clang-format off
/* WWHD glue
 data l_near 10172B48
 data l_far 10172B4C
 data l_half 10172B50
 data Deg2Rad 10172B54
 data l_zero 10172B58
 data l_firstLow 104B6004
 data l_firstHigh 104B6008
 data l_secondLow 104B5FFC
 data l_secondHigh 104B6000
 data l_record 101FAD08
 ext 028F423C f64 clipperTan(f32)
 ext 028E8EF0 void clipperNormalize(Clipper::Vec*, Clipper::Vec*)
 ext 028E8F64 void clipperTransform(const Mtx34*, const Clipper::Vec*, Clipper::Vec*)
 ext 028F026C void clipperRegisterStatic(void*)
*/
