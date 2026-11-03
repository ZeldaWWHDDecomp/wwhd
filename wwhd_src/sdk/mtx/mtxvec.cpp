/* MTX vector transforms, decompiled Cafe build; names follow zeldaret/tww. */
#include "sdk/mtx/mtxvec.h"
/* 028E8F64 */
void PSMTXMultVec(const Mtx34 *m, const Vec *src, Vec *dst) {
    f32 x = src->x, y = src->y, z = src->z;
    f32 even = gabi::fmadds(m->m[0][2], z, (f32)m->m[0][0] * x);
    f32 odd = gabi::fmadds(m->m[0][3], 1.0f, (f32)m->m[0][1] * y);
    f32 rightX = m->m[1][0], rightY = m->m[1][1];
    f32 rightZ = m->m[1][2], rightW = m->m[1][3];
    dst->x = even + odd;
    even = gabi::fmadds(rightZ, z, rightX * x);
    odd = gabi::fmadds(rightW, 1.0f, rightY * y);
    f32 upX = m->m[2][0], upY = m->m[2][1];
    f32 upZ = m->m[2][2], upW = m->m[2][3];
    dst->y = even + odd;
    dst->z = gabi::fmadds(upZ, z, upX * x) + gabi::fmadds(upW, 1.0f, upY * y);
}
/* 028E8FB8 */
void PSMTXMultVecArray(const Mtx34 *m, const Vec *srcBase, Vec *dstBase, u32 count) {
    f32 matrix[3][4];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++)
            matrix[i][j] = m->m[i][j];
    f32 x = srcBase[0].x, y = srcBase[0].y, z = srcBase[0].z;
    for (u32 n = 0; n < count; n++) {
        f32 rx = gabi::fmadds(
            matrix[0][2], z,
            gabi::fmadds(matrix[0][1], y, gabi::fmadds(matrix[0][0], x, matrix[0][3])));
        f32 ry = gabi::fmadds(
            matrix[1][2], z,
            gabi::fmadds(matrix[1][1], y, gabi::fmadds(matrix[1][0], x, matrix[1][3])));
        f32 rz = gabi::fmadds(matrix[2][2], z, matrix[2][0] * x) +
                 gabi::fmadds(matrix[2][3], 1.0f, matrix[2][1] * y);
        if (n + 1 < count) {
            x = srcBase[n + 1].x;
            y = srcBase[n + 1].y;
            z = srcBase[n + 1].z;
        }
        dstBase[n].x = rx;
        dstBase[n].y = ry;
        dstBase[n].z = rz;
    }
}
/* 028E9044 */
void PSMTXMultVecSR(const Mtx34 *m, const Vec *src, Vec *dst) {
    f32 x = src->x, y = src->y, z = src->z;
    f32 sums[3];
    for (int i = 0; i < 3; i++)
        sums[i] = (f32)m->m[i][0] * x + (f32)m->m[i][1] * y;
    f32 z1 = m->m[1][2], z2 = m->m[2][2], z0 = m->m[0][2];
    dst->x = gabi::fmadds(z0, z, sums[0]);
    dst->y = gabi::fmadds(z1, z, sums[1]);
    dst->z = gabi::fmadds(z2, z, sums[2]);
}
/* WWHD glue
 */
