/* SDK quaternion arithmetic, decompiled Cafe build; names follow zeldaret/tww. */
#include "sdk/mtx/quat.h"
/* 028E8B48 */
void PSQUATMultiply(const Quaternion *p, const Quaternion *q, Quaternion *pq) {
    f32 x = p->x, y = p->y, z = p->z, w = p->w, a = q->x, b = q->y, c = q->z, d = q->w;
    f32 rx = gabi::fmadds(y, c, w * a) + gabi::fmadds(x, d, -z * b);
    f32 ry = gabi::fmadds(-x, c, z * a) + gabi::fmadds(y, d, w * b);
    f32 rz = gabi::fmadds(w, c, -y * a) - gabi::fmadds(-z, d, -x * b);
    f32 rw = gabi::fmadds(-z, c, -x * a) - gabi::fmadds(-w, d, y * b);
    pq->x = rx;
    pq->y = ry;
    pq->z = rz;
    pq->w = rw;
}
/* 028E8BA4 */
void PSQUATInverse(const Quaternion *src, Quaternion *inv) {
    f32 x = src->x, y = src->y, z = src->z, w = src->w;
    f32 square = gabi::fmadds(z, z, x * x) + gabi::fmadds(w, w, y * y);
    f32 scale = 1.0f;
    if (square != 0.0f) {
        f32 estimate = gabi::fres(square);
        scale = estimate * gabi::fnmsubs(square, estimate, 2.0f);
    }
    inv->w = scale * w;
    inv->x = x * (-scale);
    inv->y = y * (-scale);
    inv->z = z * (-scale);
}
/* 028E8C0C */
void PSQUATNormalize(const Quaternion *src, Quaternion *unit) {
    f32 x = src->x, y = src->y, z = src->z, w = src->w;
    f32 square = gabi::fmadds(z, z, x * x) + gabi::fmadds(w, w, y * y);
    f64 estimate = gabi::frsqrte(square);
    f64 scale = gabi::fnmsub(estimate * estimate, square, 3.0) * (estimate * 0.5);
    if (!(square - 0.00001f >= 0.0f))
        scale = 0.0;
    unit->x = (f32)(x * round25(scale));
    unit->y = (f32)(y * round25(scale));
    unit->z = (f32)(z * round25(scale));
    unit->w = (f32)(w * round25(scale));
}
f64 quatSin(f64 value);
f64 quatCos(f64 value);
f64 sdkAcos(f64 value);
/* 028E9B14 */
void C_QUATRotAxisRad(Quaternion *q, const Vec *axis, f32 rad) {
    Vec unit;
    PSVECNormalize(axis, &unit);
    f32 half = rad * 0.5f;
    f64 sinA = quatSin(half);
    f64 cosA = quatCos(half);
    f32 x = unit.x, y = unit.y, z = unit.z;
    q->x = sinA * x;
    q->y = sinA * y;
    q->w = cosA;
    q->z = sinA * z;
}
/* 028E9BC0 */
void C_QUATSlerp(const Quaternion *p, const Quaternion *q, Quaternion *r, f32 t) {
    f32 dot = gabi::fmadds(p->w, q->w,
                           gabi::fmadds(p->z, q->z, gabi::fmadds(p->x, q->x, (f32)p->y * q->y)));
    f32 sign = 1.0f;
    if (dot < 0.0f) {
        dot = -dot;
        sign = -1.0f;
    }
    f32 tp = 1.0f - t, tq = t;
    if (!(dot > 0.9999899864196777f)) {
        f64 theta = sdkAcos(dot);
        f64 sinTheta = quatSin(theta);
        f64 sinP = quatSin((f32)(tp * round25(theta)));
        tp = (f32)(sinP / sinTheta);
        f64 sinQ = quatSin((f32)(t * round25(theta)));
        tq = (f32)(sinQ / sinTheta);
    }
    tq = sign * tq;
    r->x = gabi::fmadds(tp, p->x, tq * q->x);
    r->y = gabi::fmadds(tp, p->y, tq * q->y);
    r->z = gabi::fmadds(tp, p->z, tq * q->z);
    r->w = gabi::fmadds(tp, p->w, tq * q->w);
}
/* WWHD glue
ext 028F43F8 f64 quatSin(f64 value)
ext 028F4BE0 f64 quatCos(f64 value)
ext 028F4FAC f64 sdkAcos(f64 value)
*/
