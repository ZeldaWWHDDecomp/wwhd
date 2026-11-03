/* MTX affine matrices, decompiled Cafe build; names follow zeldaret/tww. */
#include "sdk/mtx/mtx.h"
/* 028E9098 */
void PSMTXIdentity(Mtx34 *m) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++)
            m->m[i][j] = i == j ? 1.0f : 0.0f;
}
/* 028E90D4 */
void PSMTXCopy(const Mtx34 *src, Mtx34 *dst) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j += 2) {
            f32 a = src->m[i][j], b = src->m[i][j + 1];
            dst->m[i][j] = a;
            dst->m[i][j + 1] = b;
        }
}
/* 028E9108 */
void PSMTXConcat(const Mtx34 *a, const Mtx34 *b, Mtx34 *ab) {
    f32 left[3][4], right[3][4], result[3][4];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++) {
            left[i][j] = a->m[i][j];
            right[i][j] = b->m[i][j];
        }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++) {
            f32 sum = gabi::fmadds(right[1][j], left[i][1], right[0][j] * left[i][0]);
            sum = gabi::fmadds(right[2][j], left[i][2], sum);
            result[i][j] = j >= 2 ? gabi::fmadds(j == 3 ? 1.0f : 0.0f, left[i][3], sum) : sum;
        }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++)
            ab->m[i][j] = result[i][j];
}
/* 028E91EC */
u32 PSMTXInverse(const Mtx34 *src, Mtx34 *inv) {
    f32 a = src->m[0][0], b = src->m[0][1], c = src->m[0][2];
    f32 d = src->m[1][0], e = src->m[1][1], f = src->m[1][2];
    f32 g = src->m[2][0], h = src->m[2][1], i = src->m[2][2];
    f32 cof[3][3];
    cof[0][0] = gabi::fmsubs(e, i, h * f);
    cof[1][0] = gabi::fmsubs(f, g, i * d);
    cof[2][0] = gabi::fmsubs(d, h, e * g);
    cof[0][1] = gabi::fmsubs(h, c, b * i);
    cof[1][1] = gabi::fmsubs(i, a, c * g);
    cof[2][1] = gabi::fmsubs(b, g, a * h);
    cof[0][2] = gabi::fmsubs(b, f, e * c);
    cof[1][2] = gabi::fmsubs(c, d, f * a);
    cof[2][2] = gabi::fmsubs(a, e, b * d);
    f32 det = gabi::fmadds(g, cof[0][2], gabi::fmadds(d, cof[0][1], a * cof[0][0]));
    if (det == 0.0f)
        return 0;
    f32 estimate = gabi::fres(det);
    f32 reciprocal = gabi::fnmsubs(estimate, det * estimate, estimate + estimate);
    f32 tx = src->m[0][3], ty = src->m[1][3], tz = src->m[2][3];
    for (int r = 0; r < 3; r++)
        for (int col = 0; col < 3; col++)
            cof[r][col] *= reciprocal;
    for (int r = 0; r < 3; r++) {
        inv->m[r][0] = cof[r][0];
        inv->m[r][1] = cof[r][1];
        inv->m[r][2] = cof[r][2];
        inv->m[r][3] = gabi::fnmadds(cof[r][2], tz, gabi::fmadds(cof[r][1], ty, cof[r][0] * tx));
    }
    return 1;
}
/* 028E93CC */
void PSMTXTrans(Mtx34 *m, f32 x, f32 y, f32 z) {
    m->m[0][3] = x;
    m->m[1][3] = y;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m->m[i][j] = i == j ? 1.0f : 0.0f;
    m->m[2][3] = z;
}
/* 028E9410 */
void PSMTXTransApply(const Mtx34 *src, Mtx34 *dst, f32 x, f32 y, f32 z) {
    // Keep paired loads and stores in order for partially overlapping matrices.
    f32 xx = src->m[0][0], xy = src->m[0][1];
    f32 xz = src->m[0][2], xt = src->m[0][3];
    f32 yz = src->m[1][2], yt = src->m[1][3];
    f32 zz = src->m[2][2], zt = src->m[2][3];
    dst->m[0][0] = xx;
    dst->m[0][1] = xy;
    f32 yx = src->m[1][0], yy = src->m[1][1];
    dst->m[0][2] = xz;
    dst->m[0][3] = x + xt;
    f32 zx = src->m[2][0], zy = src->m[2][1];
    dst->m[1][0] = yx;
    dst->m[1][1] = yy;
    dst->m[1][2] = yz;
    dst->m[1][3] = y + yt;
    dst->m[2][0] = zx;
    dst->m[2][1] = zy;
    dst->m[2][2] = zz;
    dst->m[2][3] = z + zt;
}
/* 028E945C */
void PSMTXScale(Mtx34 *m, f32 x, f32 y, f32 z) {
    m->m[0][0] = x;
    m->m[1][1] = y;
    m->m[2][2] = z;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++)
            if (i != j)
                m->m[i][j] = 0.0f;
}
/* 028E948C */
// HD: preserve the double results from sin/cos until the explicit single rounding.
void PSMTXRotTrig(Mtx34 *m, u32 axis, f64 sine, f64 cosine) {
    f32 sinA = sine, cosA = cosine;
    axis |= 0x20;
    if (axis != 'x' && axis != 'y' && axis != 'z')
        return;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++)
            m->m[i][j] = 0.0f;
    if (axis == 'x') {
        m->m[0][0] = 1.0f;
        m->m[1][1] = cosA;
        m->m[2][2] = cosA;
        m->m[1][2] = -sinA;
        m->m[2][1] = sinA;
    }
    if (axis == 'y') {
        m->m[1][1] = 1.0f;
        m->m[0][0] = cosA;
        m->m[2][2] = cosA;
        m->m[0][2] = sinA;
        m->m[2][0] = -sinA;
    }
    if (axis == 'z') {
        m->m[2][2] = 1.0f;
        m->m[0][0] = cosA;
        m->m[1][1] = cosA;
        m->m[0][1] = -sinA;
        m->m[1][0] = sinA;
    }
}

f64 sdkSin(f64 value);
f64 sdkCos(f64 value);
f64 sdkTan(f64 value);
void PSVECCrossProduct(const Vec *, const Vec *, Vec *);
/* 028E92E4 */
void __PSMTXRotAxisRadInternal(Mtx34 *m, const Vec *axis, f64 sine, f64 cosine) {
    f32 sinA = sine, cosA = cosine;
    f32 x = axis->x, y = axis->y, z = axis->z;
    f32 square = gabi::fmadds(z, z, x * x) + y * y;
    f64 estimate = gabi::frsqrte(square);
    f32 scale =
        gabi::fnmsubs((f32)(estimate * round25(estimate)), square, 3.0f) * (f32)(estimate * 0.5f);
    x *= scale;
    y *= scale;
    z *= scale;
    f32 t = 1.0f - cosA;
    f32 tx = x * t, ty = y * t, tz = z * t;
    f32 sx = x * sinA, sy = y * sinA;
    f32 xy = tx * y;
    m->m[0][2] = tx * z + sy;
    m->m[0][3] = 0.0f;
    m->m[0][0] = tx * x + cosA;
    m->m[0][1] = gabi::fnmsubs(z, sinA, xy);
    m->m[1][0] = gabi::fmadds(z, sinA, xy);
    m->m[1][1] = ty * y + cosA;
    m->m[1][2] = ty * z - sx;
    m->m[1][3] = 0.0f;
    m->m[2][0] = tx * z - sy;
    m->m[2][1] = ty * z + sx;
    m->m[2][2] = tz * z + cosA;
    m->m[2][3] = 0.0f;
}
/* 028E954C */
void C_MTXRotAxisRad(Mtx34 *m, const Vec *axis, f32 rad) {
    f64 sinA = sdkSin(rad);
    f64 cosA = sdkCos(rad);
    f32 t = 1.0f - cosA;
    Vec unit;
    C_VECNormalize(axis, &unit);
    f32 x = unit.x, y = unit.y, z = unit.z;
    f32 tx = t * x, ty = t * y;
    f32 xz = tx * z, yz = ty * z, xy = tx * y;
    f32 sx = (f32)(sinA * x), sy = (f32)(sinA * y), sz = (f32)(sinA * z);
    m->m[2][3] = 0.0f;
    m->m[0][3] = 0.0f;
    m->m[1][3] = 0.0f;
    m->m[2][0] = xz - sy;
    m->m[0][2] = xz + sy;
    m->m[1][1] = (f32)((f64)t * (y * y) + cosA);
    m->m[0][0] = (f32)((f64)t * (x * x) + cosA);
    m->m[1][0] = xy + sz;
    m->m[0][1] = xy - sz;
    m->m[1][2] = yz - sx;
    m->m[2][1] = yz + sx;
    m->m[2][2] = (f32)((f64)t * (z * z) + cosA);
}
/* 028E9684 */
void C_MTXLookAt(Mtx34 *m, const Vec *camPos, const Vec *camUp, const Vec *target) {
    Vec look;
    Vec right;
    Vec up;
    look.x = gabi::fsubs_ppc(camPos->x, target->x);
    look.y = gabi::fsubs_ppc(camPos->y, target->y);
    look.z = gabi::fsubs_ppc(camPos->z, target->z);
    PSVECNormalize(&look, &look);
    PSVECCrossProduct(camUp, &look, &right);
    PSVECNormalize(&right, &right);
    PSVECCrossProduct(&look, &right, &up);
    f32 rx = right.x, ry = right.y, rz = right.z;
    m->m[0][1] = ry;
    m->m[0][0] = rx;
    m->m[0][2] = rz;
    f32 cx = camPos->x, cy = camPos->y, cz = camPos->z;
    f32 ux = up.x, uy = up.y, uz = up.z, lx = look.x, ly = look.y, lz = look.z;
    m->m[1][0] = ux;
    m->m[1][1] = uy;
    m->m[1][2] = uz;
    m->m[2][2] = lz;
    m->m[2][0] = lx;
    m->m[2][1] = ly;
    m->m[0][3] = -gabi::fmadds(cz, rz, gabi::fmadds(cx, rx, cy * ry));
    m->m[1][3] = -gabi::fmadds(cz, uz, gabi::fmadds(cx, ux, cy * uy));
    m->m[2][3] = -gabi::fmadds(cz, lz, gabi::fmadds(cx, lx, cy * ly));
}
/* 028E97BC */
void C_MTXLightOrtho(Mtx34 *m, f32 t, f32 b, f32 l, f32 r, f32 scaleS, f32 scaleT, f32 transS,
                     f32 transT) {
    f32 rw = 1.0f / (r - l), rh = 1.0f / (t - b);
    m->m[0][1] = 0.0f;
    m->m[2][2] = 0.0f;
    m->m[2][1] = 0.0f;
    m->m[1][2] = 0.0f;
    m->m[0][2] = 0.0f;
    m->m[2][0] = 0.0f;
    m->m[1][0] = 0.0f;
    m->m[2][3] = 1.0f;
    m->m[0][0] = (rw + rw) * scaleS;
    m->m[1][1] = (rh + rh) * scaleT;
    m->m[0][3] = gabi::fnmsubs((r + l) * rw, scaleS, transS);
    m->m[1][3] = gabi::fnmsubs((t + b) * rh, scaleT, transT);
}
/* 028E9838 */
void PSMTXRotAxisRad(Mtx34 *m, const Vec *axis, f32 rad) {
    f64 sinA = sdkSin(rad);
    f64 cosA = sdkCos(rad);
    __PSMTXRotAxisRadInternal(m, axis, sinA, cosA);
}
/* 028E98C0 */
void PSMTXRotRad(Mtx34 *m, u32 axis, f32 rad) {
    f64 sinA = sdkSin(rad);
    f64 cosA = sdkCos(rad);
    PSMTXRotTrig(m, axis, sinA, cosA);
}
/* 028E9948 */
void C_MTXPerspective(Mtx44 *m, f32 fovY, f32 aspect, f32 n, f32 f) {
    f32 angle = (fovY * 0.5f) * 0.01745329238474369f;
    f64 tangent = sdkTan(angle);
    f32 cot = (f32)(1.0f / tangent);
    f32 range = 1.0f / (f - n);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m->m[i][j] = 0.0f;
    m->m[0][0] = cot / aspect;
    m->m[1][1] = cot;
    m->m[2][2] = -((f + n) * range);
    m->m[2][3] = -(((f + f) * n) * range);
    m->m[3][2] = -1.0f;
}
/* 028E9A70 */
void C_MTXOrtho(Mtx44 *m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f) {
    f32 rw = 1.0f / (r - l), rh = 1.0f / (t - b), rd = 1.0f / (f - n);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m->m[i][j] = 0.0f;
    m->m[0][0] = rw + rw;
    m->m[0][3] = -((r + l) * rw);
    m->m[1][1] = rh + rh;
    m->m[1][3] = -((t + b) * rh);
    m->m[2][2] = -2.0f * rd;
    m->m[2][3] = -((f + n) * rd);
    m->m[3][3] = 1.0f;
}
/* 028E8B04 */
void PSMTX44Scale(Mtx44 *m, f32 x, f32 y, f32 z) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m->m[i][j] = i == j ? (i == 0 ? x : i == 1 ? y : i == 2 ? z : 1.0f) : 0.0f;
}
/* 028E8C78 */
void PSMTXQuat(Mtx34 *m, const Quaternion *q) {
    f32 x = q->x, y = q->y, z = q->z, w = q->w;
    f32 xx = x * x, yy = y * y, zz = z * z;
    f32 square = gabi::fmadds(z, z, xx) + gabi::fmadds(w, w, yy);
    f32 estimate = gabi::fres(square);
    f32 scale = (estimate * gabi::fnmsubs(square, estimate, 2.0f)) * 2.0f;
    f32 xyPlus = gabi::fmadds(x, y, z * w), xyMinus = gabi::fmsubs(x, y, z * w);
    f32 yzPlus = gabi::fmadds(y, z, x * w), xzPlus = gabi::fmadds(x, z, y * w);
    f32 yzMinus = gabi::fnmsubs(x * w, 2.0f, yzPlus), xzMinus = gabi::fnmsubs(y * w, 2.0f, xzPlus);
    m->m[0][3] = 0.0f;
    m->m[2][3] = 0.0f;
    m->m[2][2] = gabi::fnmsubs(xx + yy, scale, 1.0f);
    m->m[1][0] = xyPlus * scale;
    m->m[1][1] = gabi::fnmsubs(gabi::fmadds(z, z, xx), scale, 1.0f);
    m->m[0][0] = gabi::fnmsubs(zz + yy, scale, 1.0f);
    m->m[0][1] = xyMinus * scale;
    m->m[0][2] = xzPlus * scale;
    m->m[1][2] = yzMinus * scale;
    m->m[1][3] = 0.0f;
    m->m[2][0] = xzMinus * scale;
    m->m[2][1] = yzPlus * scale;
}
/* WWHD glue
ext 028F43F8 f64 sdkSin(f64 value)
ext 028F4BE0 f64 sdkCos(f64 value)
ext 028F423C f64 sdkTan(f64 value)
*/
