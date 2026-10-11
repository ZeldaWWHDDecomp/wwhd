/** Circle/line intersection. WWHD, verified unit c_m2d. */
#include "SSystem/SComponent/c_m2d.h"
#include <cmath>
static f32 zeroValue;
static f32 epsilonValue;
static f32 fourValue;
static f32 oneValue;
static u8 assertFile;
static u8 assertInsideCircle;
static u8 assertForward;
// Preserve the reference's addend-first NaN selection when overlapping output
// words make a payload observable; finite arithmetic still uses fmadds.
// HD: preserve PowerPC operand order and NaN payloads.
static f32 circleMultiplyAdd(f32 a, f32 factor, f32 addend) {
    if (std::isnan(addend)) {
        return gabi::ppc_qnan(addend);
    }
    if (std::isnan(a)) {
        return gabi::ppc_qnan(a);
    }
    if (std::isnan(factor)) {
        return gabi::ppc_qnan(factor);
    }
    return gabi::fmadds(a, factor, addend);
}
/* 02010284 */
void cM2d_CrossCirLin(cM2dGCir* pCircle, f32 x0, f32 y0, f32 x1, f32 y1, be<f32>* pDstX, be<f32>* pDstY) {
    f32 cy = pCircle->mPosY;
    f32 dy = gabi::fsubs_ppc(y0, cy);
    f32 cx = pCircle->mPosX;
    f32 dx = gabi::fsubs_ppc(x0, cx);
    f32 distance = circleMultiplyAdd(dx, dx, gabi::fmuls_ppc(dy, dy));
    f32 radius = pCircle->mRadius;
    f32 c = f32(-(f64(radius)*f64(radius)-f64(distance)));

    f32 zero = zeroValue;
    f32 dot = circleMultiplyAdd(x1, dx, gabi::fmuls_ppc(y1, dy));
    f32 b = dot+dot;
    f32 a = circleMultiplyAdd(x1, x1, gabi::fmuls_ppc(y1, y1));
    if (!(c < zero)) {
        hd_assert(&assertFile, 0x47, &assertInsideCircle);
    }

    f32 epsilon = epsilonValue;
    f32 t = zero;
    if (std::fabs(a) < epsilon) {
        if (!(std::fabs(b) < epsilon)) {
            t = -(c / b);
        }
    } else {
        f32 four = fourValue;
        f32 ac = (four*a)*c;
        f32 discriminant = gabi::fmsubs(b, b, ac);
        if (std::fabs(discriminant) < epsilon) {
            t = -(b / (a + a));
        } else if (!(discriminant < zero)) {
            f32 one = oneValue;
            f32 reciprocal = one/(a+a);
            f64 root1 = hd_sqrt(discriminant);
            f32 first = gabi::fmuls_ppc(f32(root1-f64(b)), reciprocal);
            f64 root2 = hd_sqrt(discriminant);
            f64 negativeB = -f64(b);
            f32 second = gabi::fmuls_ppc(f32(std::isnan(negativeB) ? negativeB : std::isnan(root2) ? root2 : negativeB-root2), reciprocal);
            t = ppc_fsel(first-second, second, first);
        }
    }
    if (!(std::fabs(t) < epsilon)) {
        if (t < zero) {
            hd_assert(&assertFile, 0x8D, &assertForward);
        }
        y0 = std::isnan(y0) ? gabi::ppc_qnan(y0) : std::isnan(t) ? gabi::ppc_qnan(t) : gabi::fmadds(t, y1, y0);
        x0 = std::isnan(x0) ? gabi::ppc_qnan(x0) : std::isnan(t) ? gabi::ppc_qnan(t) : gabi::fmadds(t, x1, x0);
    }
    *pDstX = x0;
    *pDstY = y0;
}

/* 02010500 */
// HD: math/header globals and their registration records.
static void __sinit_c_m2d_cpp() {
    sinit_header_statics_rodata(0x101FF830, 0x1018C820, 0x101FF83C, 0x10001F30);
}

/* WWHD glue (tools/verify/autoglue.py): arithmetic constants and runtime helpers
ext 028F4384 f64 hd_sqrt(f64)
ext 0273AA24 void hd_assert(void*, s32, void*)
data zeroValue 10001EFC
data epsilonValue 100030B8
data fourValue 10001F04
data oneValue 10001F08
data assertFile 10001F0C
data assertInsideCircle 10001F24
data assertForward 10001F18
*/
