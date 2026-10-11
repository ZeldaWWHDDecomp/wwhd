/* Local WWHD vector reconstruction. */
#include "bindings.h"
#include "gc_api/JSystem/JUTAssert.h"
#include <cmath>
// HD: retain the GHS hidden result pointer, nullable allocation and float-copy return ABI.
// Shared cXyz operators already provide the caller-facing C++ API; these Guest adapters
// implement their verified entry points without redefining the shared inline operators.
static f32 one;
static f32 zero;
static f32 nearZeroSquare;
static f32 zeroTolerance;
static f32 normScale;
static f32 normLargeScale;
static cXyz Zero;
static cXyz BaseX;
static cXyz BaseY;
static cXyz BaseZ;
static cXyz BaseXY;
static cXyz BaseXZ;
static cXyz BaseYZ;
static cXyz BaseXYZ;
void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out);
void PSVECCrossProduct(const cXyz* a, const cXyz* b, cXyz* out);
void PSVECNormalize(const cXyz* src, cXyz* dst);
void JUTAssertion_showConfirm(const char* file, s32 line, const char* message);
static cXyz* allocate(cXyz* dst) {
    return dst ? dst : static_cast<cXyz*>(operator_new(sizeof(cXyz)));
}
// HD: value-return construction copies through float registers, quieting signaling NaNs.
static void copy_float(cXyz* dst, const cXyz* src) {
    dst->x = (f32)src->x;
    dst->y = (f32)src->y;
    dst->z = (f32)src->z;
}
/* 0201AD78 */
void cXyz_addGuest(cXyz* src, cXyz* dst, cXyz* rhs) {
    cXyz temp;
    PSVECAdd(src, rhs, &temp);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201ADE0 */
void cXyz_subtractGuest(cXyz* src, cXyz* dst, cXyz* rhs) {
    cXyz temp;
    PSVECSubtract(src, rhs, &temp);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201AE48 */
void cXyz_scaleGuest(cXyz* src, cXyz* dst, f32 scale) {
    cXyz temp;
    PSVECScale(src, &temp, scale);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201AEAC */
void cXyz_divideGuest(cXyz* src, cXyz* dst, f32 divisor) {
    cXyz temp;
    f32 inverse = one / divisor;
    PSVECScale(src, &temp, inverse);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201AF1C */
void cXyz_productGuest(cXyz* src, cXyz* dst, cXyz* rhs) {
    cXyz temp;
    f32 y = (f32)src->y * (f32)rhs->y;
    f32 x = (f32)src->x * (f32)rhs->x;
    f32 z = (f32)src->z * (f32)rhs->z;
    temp.x = x;
    temp.y = y;
    temp.z = z;
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201AF98 */
u8 cXyz_equalGuest(cXyz* a, cXyz* b) {
    return (f32)a->x == (f32)b->x && (f32)a->y == (f32)b->y && (f32)a->z == (f32)b->z;
}

/* 0201AFD8 */
u8 cXyz_notEqualGuest(cXyz* a, cXyz* b) {
    return !((f32)a->x == (f32)b->x && (f32)a->y == (f32)b->y && (f32)a->z == (f32)b->z);
}

/* 0201B018 */
void cXyz_getCrossProductGuest(cXyz* src, cXyz* dst, cXyz* rhs) {
    cXyz temp;
    PSVECCrossProduct(src, rhs, &temp);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201B080 */
void cXyz_outprodGuest(cXyz* src, cXyz* dst, cXyz* rhs) {
    cXyz_getCrossProductGuest(src, dst, rhs);
}

/* 0201B084 */
void cXyz_normGuest(cXyz* src, cXyz* dst) {
    cXyz temp;
    f32 square = PSVECSquareMag(src);
    if (square < nearZeroSquare)
        JUTAssertion_showConfirm("c_xyz.cpp", 251, "isNearZeroSquare() == 0");
    PSVECNormalize(src, &temp);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201B12C */
void cXyz_normZPGuest(cXyz* src, cXyz* dst) {
    cXyz temp;
    f32 square = PSVECSquareMag(src);
    if (!(square < nearZeroSquare))
        PSVECNormalize(src, &temp);
    else
        temp.copy(Zero);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201B1E4 */
void cXyz_normZCGuest(cXyz* src, cXyz* dst) {
    cXyz temp;
    cXyz check;
    cXyz first;
    cXyz second;
    cXyz third;
    f32 epsilon = nearZeroSquare;
    f32 square = PSVECSquareMag(src);
    if (!(square < epsilon))
        PSVECNormalize(src, &temp);
    else {
        cXyz_scaleGuest(src, &first, normScale);
        cXyz_scaleGuest(&first, &second, normLargeScale);
        cXyz_normZPGuest(&second, &third);
        f32 z = third.z, x = third.x, y = third.y;
        temp.z = z;
        temp.x = x;
        temp.y = y;
        check.z = z;
        check.y = y;
        check.x = x;
        square = PSVECSquareMag(&check);
        if (square < epsilon) {
            f32 zeroValue = zero, oneValue = one;
            temp.x = zeroValue;
            temp.z = oneValue;
            temp.y = zeroValue;
        }
    }
    dst = allocate(dst);
    if (dst)
        copy_float(dst, &temp);
}

/* 0201B31C */
void cXyz_normalizeGuest(cXyz* src, cXyz* dst) {
    f32 square = PSVECSquareMag(src);
    if (square < nearZeroSquare)
        JUTAssertion_showAssert("c_xyz.cpp", 285, "isNearZeroSquare() == 0");
    PSVECNormalize(src, src);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, src);
}

/* 0201B3C0 */
void cXyz_normalizeZPGuest(cXyz* src, cXyz* dst) {
    f32 square = PSVECSquareMag(src);
    if (!(square < nearZeroSquare))
        PSVECNormalize(src, src);
    else
        src->copy(Zero);
    dst = allocate(dst);
    if (dst)
        copy_float(dst, src);
}

/* 0201B47C */
s32 cXyz_normalizeRSGuest(cXyz* src) {
    f32 square = PSVECSquareMag(src);
    if (square < nearZeroSquare)
        return 0;
    PSVECNormalize(src, src);
    return 1;
}

/* 0201B4E0 */
u8 cXyz_isZeroGuest(cXyz* src) {
    f32 epsilon = zeroTolerance;
    return std::fabs((f32)src->x) < epsilon && std::fabs((f32)src->y) < epsilon && std::fabs((f32)src->z) < epsilon;
}

/* 0201B528 */
void __sinit_c_xyz_cpp() {
    sinit_header_statics_rodata(0x101ffb8c, 0x1018d460, 0x101ffb98, 0x10003750);
    BaseX.set(one, zero, zero);
    BaseY.set(zero, one, zero);
    BaseZ.set(zero, zero, one);
    Zero.set(zero, zero, zero);
    BaseXY.set(one, one, zero);
    BaseXZ.set(one, zero, one);
    BaseYZ.set(zero, one, one);
    BaseXYZ.set(one, one, one);
}

/* WWHD glue (tools/verify/autoglue.py)
data one 100036E8
data zero 10003724
data nearZeroSquare 100036EC
data zeroTolerance 1000374C
data normScale 1000371C
data normLargeScale 10003720
data Zero 101FFBA8
data BaseX 101FFBB4
data BaseY 101FFBC0
data BaseZ 101FFBCC
data BaseXY 101FFBD8
data BaseXZ 101FFBE4
data BaseYZ 101FFBF0
data BaseXYZ 101FFBFC
str 10003708 in 0201B084
str 100036F0 in 0201B084
str 10003740 in 0201B31C
str 10003728 in 0201B31C
ext 028E8DAC void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out)
ext 028E8D4C void PSVECCrossProduct(const cXyz* a, const cXyz* b, cXyz* out)
ext 028E8EF0 void PSVECNormalize(const cXyz* src, cXyz* dst)
ext 0273AA38 void JUTAssertion_showConfirm(const char* file, s32 line, const char* message)
*/
