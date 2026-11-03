/* MTX vector arithmetic, decompiled Cafe build; names follow zeldaret/tww. */
#include "sdk/mtx/vec.h"

static inline f32 reciprocalLength(f32 square) {
    f64 estimate = gabi::frsqrte(square);
    f32 squaredEstimate = (f32)(estimate * round25(estimate));
    f32 halfEstimate = (f32)(estimate * 0.5f);
    return gabi::fnmsubs(squaredEstimate, square, 3.0f) * halfEstimate;
}
/* 028E8D4C */
void PSVECCrossProduct(const Vec *a, const Vec *b, Vec *axb) {
    f32 ax = a->x, ay = a->y, az = a->z, bx = b->x, by = b->y, bz = b->z;
    f32 x = gabi::fmsubs(ay, bz, by * az);
    f32 y = -gabi::fmsubs(ax, bz, bx * az);
    f32 z = -gabi::fmsubs(ay, bx, by * ax);
    axb->x = x;
    axb->y = y;
    axb->z = z;
}
/* 028E8D88 */
void PSVECAdd(const Vec *a, const Vec *b, Vec *ab) {
    f32 x = a->x + b->x, y = a->y + b->y;
    ab->x = x;
    ab->y = y;
    ab->z = a->z + b->z;
}
/* 028E8DAC */
void PSVECSubtract(const Vec *a, const Vec *b, Vec *a_b) {
    f32 x = a->x - b->x, y = a->y - b->y;
    a_b->x = x;
    a_b->y = y;
    a_b->z = a->z - b->z;
}
/* 028E8DD0 */
f32 PSVECSquareMag(const Vec *v) {
    f32 x = v->x, y = v->y, z = v->z;
    return gabi::fmadds(z, z, x * x) + y * y;
}
/* 028E8DE8 */
f32 PSVECSquareDistance(const Vec *a, const Vec *b) {
    f32 y = a->y - b->y, z = a->z - b->z, x = a->x - b->x;
    return gabi::fmadds(x, x, y * y) + z * z;
}
/* 028E8E10 */
f32 PSVECMag(const Vec *v) {
    f32 x = v->x, y = v->y, z = v->z;
    f32 square = gabi::fmadds(z, z, x * x) + y * y;
    return square == 0.0f ? square : square * reciprocalLength(square);
}
/* 028E8E64 */
void PSVECScale(const Vec *src, Vec *dst, f32 scale) {
    f32 x = src->x, y = src->y, z = src->z;
    dst->x = x * scale;
    dst->y = y * scale;
    dst->z = z * scale;
}
/* 028E8E80 */
f32 PSVECDistance(const Vec *a, const Vec *b) {
    f32 y = a->y - b->y, z = a->z - b->z, x = a->x - b->x;
    f32 square = gabi::fmadds(x, x, y * y) + z * z;
    return square == 0.0f ? square : square * reciprocalLength(square);
}
/* 028E8EF0 */
void PSVECNormalize(const Vec *src, Vec *unit) {
    f32 x = src->x, y = src->y, z = src->z;
    f32 scale = reciprocalLength(gabi::fmadds(z, z, x * x) + y * y);
    unit->x = x * scale;
    unit->y = y * scale;
    unit->z = z * scale;
}
/* 028E8F44 */
f32 PSVECDotProduct(const Vec *a, const Vec *b) {
    return gabi::fmadds(a->x, b->x, (f32)a->y * b->y) + (f32)a->z * b->z;
}
f64 sdkSqrt(f64 value);
/* 028E9DAC */
void C_VECAdd(const Vec *a, const Vec *b, Vec *ab) {
    ab->x = a->x + b->x;
    ab->y = a->y + b->y;
    ab->z = a->z + b->z;
}
/* 028E9DE0 */
void C_VECNormalize(const Vec *src, Vec *unit) {
    f32 x = src->x, y = src->y, z = src->z;
    f32 square = gabi::fmadds(z, z, gabi::fmadds(x, x, y * y));
    f32 scale = (f32)(1.0f / sdkSqrt(square));
    unit->x = src->x * scale;
    unit->y = src->y * scale;
    unit->z = src->z * scale;
}
/* 028E9E60 */
f32 C_VECDotProduct(const Vec *a, const Vec *b) {
    return gabi::fmadds(a->z, b->z, gabi::fmadds(a->x, b->x, (f32)a->y * b->y));
}
/* 028E9E88 */
void C_VECHalfAngle(const Vec *a, const Vec *b, Vec *half) {
    Vec aTmp;
    Vec bTmp;
    Vec hTmp;
    aTmp.x = -a->x;
    aTmp.y = -a->y;
    aTmp.z = -a->z;
    bTmp.x = -b->x;
    bTmp.y = -b->y;
    bTmp.z = -b->z;
    C_VECNormalize(&aTmp, &aTmp);
    C_VECNormalize(&bTmp, &bTmp);
    C_VECAdd(&aTmp, &bTmp, &hTmp);
    f32 square = C_VECDotProduct(&hTmp, &hTmp);
    if (square > 0.0f)
        C_VECNormalize(&hTmp, half);
    else {
        half->x = hTmp.x;
        half->y = hTmp.y;
        half->z = hTmp.z;
    }
}
/* 028E9F74 */
void C_VECReflect(const Vec *src, const Vec *normal, Vec *dst) {
    Vec vTmp;
    Vec nTmp;
    vTmp.x = -src->x;
    vTmp.y = -src->y;
    vTmp.z = -src->z;
    C_VECNormalize(&vTmp, &vTmp);
    C_VECNormalize(normal, &nTmp);
    f32 dot = C_VECDotProduct(&vTmp, &nTmp);
    dst->x = gabi::fmsubs(nTmp.x + nTmp.x, dot, vTmp.x);
    dst->y = gabi::fmsubs(nTmp.y + nTmp.y, dot, vTmp.y);
    dst->z = gabi::fmsubs(nTmp.z + nTmp.z, dot, vTmp.z);
    C_VECNormalize(dst, dst);
}
/* WWHD glue
ext 028F4384 f64 sdkSqrt(f64 value)
*/
