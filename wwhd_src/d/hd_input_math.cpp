/* HD orientation matrix operations, kept separate from the merged input controller source. */
#include "d/hd_input_math.h"
static f32 sOne;
static f32 sZero;
/* 02617590 */
void hdOrientationMatrix_c::setColumn(u32 column, hdInputVector* vector) {
    // GHS pure float copies preserve the source bits, including signalling NaNs.
    for (u32 row = 0; row < 3; ++row) mBits[row][column] = vector->mBits[row];
}
/* 026175B4 */
void hdOrientationMatrix_c::setColumns(hdInputVector* first, hdInputVector* second, hdInputVector* third) {
    setColumn(0, first);
    setColumn(1, second);
    setColumn(2, third);
}
/* 02618944 */
void hdOrientationMatrix_c::copy(hdOrientationMatrix_c* source) {
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 3; ++column) mBits[row][column] = source->mBits[row][column];
}
/* 02618990 */
void hdOrientationMatrix_c::multiply(hdOrientationMatrix_c* left, hdOrientationMatrix_c* right) {
    f32 a[3][3], b[3][3];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 3; ++column) {
            a[row][column] = left->mElements[row][column];
            b[row][column] = right->mElements[row][column];
        }
    const f32 firstX = gabi::fmadds(a[0][2], b[2][0],
        gabi::fmadds(a[0][0], b[0][0], gabi::fmuls_ppc(a[0][1], b[1][0])));
    const f32 firstY = gabi::fmadds(a[0][2], b[2][1],
        gabi::fmadds(a[0][0], b[0][1], gabi::fmuls_ppc(a[0][1], b[1][1])));
    const f32 firstZ = gabi::fmadds(a[0][2], b[2][2],
        gabi::fmadds(a[0][0], b[0][2], gabi::fmuls_ppc(a[0][1], b[1][2])));
    const f32 secondX = gabi::fmadds(a[1][2], b[2][0],
        gabi::fmadds(a[1][0], b[0][0], gabi::fmuls_ppc(a[1][1], b[1][0])));
    const f32 secondY = gabi::fmadds(a[1][2], b[2][1],
        gabi::fmadds(a[1][0], b[0][1], gabi::fmuls_ppc(a[1][1], b[1][1])));
    const f32 secondZ = gabi::fmadds(a[1][2], b[2][2],
        gabi::fmadds(a[1][0], b[0][2], gabi::fmuls_ppc(a[1][1], b[1][2])));
    const f32 thirdX = gabi::fmadds(a[2][2], b[2][0],
        gabi::fmadds(a[2][0], b[0][0], gabi::fmuls_ppc(a[2][1], b[1][0])));
    const f32 thirdY = gabi::fmadds(a[2][2], b[2][1],
        gabi::fmadds(a[2][0], b[0][1], gabi::fmuls_ppc(a[2][1], b[1][1])));
    const f32 thirdZ = gabi::fmadds(a[2][2], b[2][2],
        gabi::fmadds(a[2][0], b[0][2], gabi::fmuls_ppc(a[2][1], b[1][2])));
    mElements[0][1] = firstY;
    mElements[0][2] = firstZ;
    mElements[1][1] = secondY;
    mElements[0][0] = firstX;
    mElements[1][2] = secondZ;
    mElements[1][0] = secondX;
    mElements[2][0] = thirdX;
    mElements[2][1] = thirdY;
    mElements[2][2] = thirdZ;
}
/* 02618B04 */
void hdOrientationMatrix_c::identity() {
    const f32 one = sOne, zero = sZero;
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 3; ++column) mElements[row][column] = row == column ? one : zero;
}
/* 02618B3C */
void hdOrientationMatrix_c::inverse(hdOrientationMatrix_c* source) {
    const f32 a = source->mElements[0][0], b = source->mElements[0][1], c = source->mElements[0][2];
    const f32 d = source->mElements[1][0], e = source->mElements[1][1], f = source->mElements[1][2];
    const f32 g = source->mElements[2][0], h = source->mElements[2][1], i = source->mElements[2][2];
    const f32 fh = gabi::fmuls_ppc(h, f), fg = gabi::fmuls_ppc(f, g);
    const f32 di = gabi::fmuls_ppc(d, i), ge = gabi::fmuls_ppc(g, e);
    const f32 cofactorA = gabi::fmsubs(e, i, fh);
    const f32 cofactorC = gabi::fmsubs(d, h, ge);
    const f32 determinant = gabi::fmadds(c, cofactorC, gabi::fmadds(a, cofactorA, gabi::fmuls_ppc(b, gabi::fsubs_ppc(fg, di))));
    if (determinant == sZero) { identity(); return; }
    const f32 reciprocal = sOne / determinant;
    const f32 biCh = gabi::fmsubs(b, i, gabi::fmuls_ppc(h, c));
    const f32 aiCg = gabi::fmsubs(a, i, gabi::fmuls_ppc(g, c));
    const f32 afCd = gabi::fmsubs(a, f, gabi::fmuls_ppc(d, c));
    const f32 bfCe = gabi::fmsubs(b, f, gabi::fmuls_ppc(e, c));
    const f32 ahBg = gabi::fmsubs(a, h, gabi::fmuls_ppc(g, b));
    const f32 aeDb = gabi::fmsubs(a, e, gabi::fmuls_ppc(d, b));
    mElements[1][1] = gabi::fmuls_ppc(aiCg, reciprocal);
    mElements[1][0] = -gabi::fmuls_ppc(gabi::fsubs_ppc(di, fg), reciprocal);
    mElements[0][1] = -gabi::fmuls_ppc(biCh, reciprocal);
    mElements[0][2] = gabi::fmuls_ppc(bfCe, reciprocal);
    mElements[0][0] = gabi::fmuls_ppc(cofactorA, reciprocal);
    mElements[2][0] = gabi::fmuls_ppc(cofactorC, reciprocal);
    mElements[2][1] = -gabi::fmuls_ppc(ahBg, reciprocal);
    mElements[1][2] = -gabi::fmuls_ppc(afCd, reciprocal);
    mElements[2][2] = gabi::fmuls_ppc(aeDb, reciprocal);
}
/* WWHD glue: local constants from the matrix translation unit.
data sOne 100E2980
data sZero 100E2984
*/
