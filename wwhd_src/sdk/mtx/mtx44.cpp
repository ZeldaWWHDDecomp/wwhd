// SDK paired-single 4x4 matrix routines, reconstructed from WWHD.
#include "sdk/mtx/mtx44.h"
/* 028E892C */
void PSMTX44Identity(Mtx44* matrix) {
    for (u32 row = 0; row < 4; ++row)
        for (u32 col = 0; col < 4; ++col)
            matrix->m[row][col] = row == col ? 1.0f : 0.0f;
}
/* 028E8970 */
void PSMTX44Copy(const Mtx44* source, Mtx44* dest) {
    // Paired loads precede their two stores, preserving overlapping copies.
    for (u32 row = 0; row < 4; ++row) {
        for (u32 col = 0; col < 4; col += 2) {
            f32 first = source->m[row][col], second = source->m[row][col + 1];
            dest->m[row][col] = first;
            dest->m[row][col + 1] = second;
        }
    }
}
/* 028E89B4 */
void PSMTX44Concat(const Mtx44* a, const Mtx44* b, Mtx44* dest) {
    f32 right[4][4], low[4][2], high[4][2];
    // GHS retains the right-hand rows, and delays each high-half left load.
    for (u32 row = 0; row < 4; ++row)
        for (u32 col = 0; col < 4; ++col)
            right[row][col] = b->m[row][col];
    for (u32 row = 0; row < 4; ++row) {
        f32 x = a->m[row][0], y = a->m[row][1], z = a->m[row][2], w = a->m[row][3];
        for (u32 col = 0; col < 2; ++col) {
            f32 value = right[0][col] * x;
            value = gabi::fmadds(right[1][col], y, value);
            value = gabi::fmadds(right[2][col], z, value);
            low[row][col] = gabi::fmadds(right[3][col], w, value);
        }
    }
    for (u32 row = 0; row < 4; ++row) {
        f32 x = a->m[row][0], y = a->m[row][1];
        if (row == 2) {
            dest->m[row - 1][0] = low[row - 1][0];
            dest->m[row - 1][1] = low[row - 1][1];
        }
        if (row == 3) {
            dest->m[3][0] = low[3][0];
            dest->m[3][1] = low[3][1];
        }
        f32 z = a->m[row][2], w = a->m[row][3];
        if (row == 2) {
            dest->m[2][0] = low[2][0];
            dest->m[2][1] = low[2][1];
        }
        for (u32 col = 0; col < 2; ++col) {
            f32 value = right[0][col + 2] * x;
            value = gabi::fmadds(right[1][col + 2], y, value);
            value = gabi::fmadds(right[2][col + 2], z, value);
            high[row][col] = gabi::fmadds(right[3][col + 2], w, value);
        }
        if (row == 1) {
            dest->m[row - 1][0] = low[row - 1][0];
            dest->m[row - 1][1] = low[row - 1][1];
        }
    }
    for (u32 row = 0; row < 4; ++row) {
        dest->m[row][2] = high[row][0];
        dest->m[row][3] = high[row][1];
    }
}
/* 028E8AB8 */
void PSMTX44Trans(Mtx44* matrix, f32 x, f32 y, f32 z) {
    matrix->m[0][3] = x;
    matrix->m[1][3] = y;
    matrix->m[2][3] = z;
    for (u32 row = 0; row < 4; ++row)
        for (u32 col = 0; col < 4; ++col)
            if (col != 3 || row == 3)
                matrix->m[row][col] = row == col ? 1.0f : 0.0f;
}
/* WWHD glue
 */
