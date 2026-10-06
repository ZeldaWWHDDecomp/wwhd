/* Local WWHD vector reconstruction. */
#include "SSystem/SComponent/c_xyz.h"
#include <cmath>
static f32 constant(u32 a) { return gabi::load<f32>(a); }
static cXyz* allocate(cXyz* dst) { return dst ? dst : gabi::call<cXyz*>(0x0273AD10, 12u); }
static void copy_float(cXyz* dst, const cXyz* src) {
    dst->x = (f32)src->x;
    dst->y = (f32)src->y;
    dst->z = (f32)src->z;
}
void xyz_add(cXyz* src, cXyz* dst, cXyz* rhs) {
    WWHD_FUNC(0x0201AD78, void, src, dst, rhs);
    gabi::Local<cXyz> temp;
    gabi::call(0x028E8D88, src, rhs, temp.get());
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201AD78, xyz_add);
void xyz_sub(cXyz* src, cXyz* dst, cXyz* rhs) {
    WWHD_FUNC(0x0201ADE0, void, src, dst, rhs);
    gabi::Local<cXyz> temp;
    gabi::call(0x028E8DAC, src, rhs, temp.get());
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201ADE0, xyz_sub);
void xyz_scale(cXyz* src, cXyz* dst, f32 scale) {
    WWHD_FUNC(0x0201AE48, void, src, dst, scale);
    gabi::Local<cXyz> temp;
    gabi::call(0x028E8E64, src, temp.get(), scale);
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201AE48, xyz_scale);
void xyz_div(cXyz* src, cXyz* dst, f32 divisor) {
    WWHD_FUNC(0x0201AEAC, void, src, dst, divisor);
    gabi::Local<cXyz> temp;
    f32 inverse = constant(0x100036E8) / divisor;
    gabi::call(0x028E8E64, src, temp.get(), inverse);
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201AEAC, xyz_div);
void xyz_product(cXyz* src, cXyz* dst, cXyz* rhs) {
    WWHD_FUNC(0x0201AF1C, void, src, dst, rhs);
    gabi::Local<cXyz> temp;
    f32 y = (f32)src->y * (f32)rhs->y;
    f32 x = (f32)src->x * (f32)rhs->x;
    f32 z = (f32)src->z * (f32)rhs->z;
    temp->x = x; temp->y = y; temp->z = z;
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201AF1C, xyz_product);
u8 xyz_equal(cXyz* a, cXyz* b) {
    WWHD_FUNC(0x0201AF98, u8, a, b);
    return (f32)a->x == (f32)b->x && (f32)a->y == (f32)b->y && (f32)a->z == (f32)b->z;
}
VERIFY(0x0201AF98, xyz_equal);
u8 xyz_unequal(cXyz* a, cXyz* b) {
    WWHD_FUNC(0x0201AFD8, u8, a, b);
    return !((f32)a->x == (f32)b->x && (f32)a->y == (f32)b->y && (f32)a->z == (f32)b->z);
}
VERIFY(0x0201AFD8, xyz_unequal);
void xyz_cross(cXyz* src, cXyz* dst, cXyz* rhs) {
    WWHD_FUNC(0x0201B018, void, src, dst, rhs);
    gabi::Local<cXyz> temp;
    gabi::call(0x028E8D4C, src, rhs, temp.get());
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201B018, xyz_cross);
void xyz_outprod(cXyz* src, cXyz* dst, cXyz* rhs) {
    WWHD_FUNC(0x0201B080, void, src, dst, rhs);
    gabi::call(0x0201B018, src, dst, rhs);
}
VERIFY(0x0201B080, xyz_outprod);
void xyz_norm(cXyz* src, cXyz* dst) {
    WWHD_FUNC(0x0201B084, void, src, dst);
    gabi::Local<cXyz> temp;
    f32 square = gabi::call<f32>(0x028E8DD0, src);
    if (square < constant(0x100036EC))
        gabi::call(0x0273AA38, STR(0x10003708), 251, STR(0x100036F0));
    gabi::call(0x028E8EF0, src, temp.get());
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201B084, xyz_norm);
void xyz_normZP(cXyz* src, cXyz* dst) {
    WWHD_FUNC(0x0201B12C, void, src, dst);
    gabi::Local<cXyz> temp;
    f32 square = gabi::call<f32>(0x028E8DD0, src);
    if (!(square < constant(0x100036EC))) gabi::call(0x028E8EF0, src, temp.get());
    else temp->copy(*gabi::at<cXyz>(0x101FFBA8));
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201B12C, xyz_normZP);
void xyz_normZC(cXyz* src, cXyz* dst) {
    WWHD_FUNC(0x0201B1E4, void, src, dst);
    gabi::Local<cXyz> temp, check, first, second, third;
    f32 epsilon = constant(0x100036EC);
    f32 square = gabi::call<f32>(0x028E8DD0, src);
    if (!(square < epsilon)) gabi::call(0x028E8EF0, src, temp.get());
    else {
        xyz_scale(src, first.get(), constant(0x1000371C));
        xyz_scale(first.get(), second.get(), constant(0x10003720));
        xyz_normZP(second.get(), third.get());
        f32 z = third->z, x = third->x, y = third->y;
        temp->z = z; temp->x = x; temp->y = y;
        check->z = z; check->y = y; check->x = x;
        square = gabi::call<f32>(0x028E8DD0, check.get());
        if (square < epsilon) {
            f32 zero = constant(0x10003724), one = constant(0x100036E8);
            temp->x = zero; temp->z = one; temp->y = zero;
        }
    }
    dst = allocate(dst);
    if (dst) copy_float(dst, temp.get());
}
VERIFY(0x0201B1E4, xyz_normZC);
void xyz_normalize(cXyz* src, cXyz* dst) {
    WWHD_FUNC(0x0201B31C, void, src, dst);
    f32 square = gabi::call<f32>(0x028E8DD0, src);
    if (square < constant(0x100036EC))
        gabi::call(0x0273AA24, STR(0x10003740), 285, STR(0x10003728));
    gabi::call(0x028E8EF0, src, src);
    dst = allocate(dst);
    if (dst) copy_float(dst, src);
}
VERIFY(0x0201B31C, xyz_normalize);
void xyz_normalizeZP(cXyz* src, cXyz* dst) {
    WWHD_FUNC(0x0201B3C0, void, src, dst);
    f32 square = gabi::call<f32>(0x028E8DD0, src);
    if (!(square < constant(0x100036EC))) gabi::call(0x028E8EF0, src, src);
    else src->copy(*gabi::at<cXyz>(0x101FFBA8));
    dst = allocate(dst);
    if (dst) copy_float(dst, src);
}
VERIFY(0x0201B3C0, xyz_normalizeZP);
s32 xyz_normalizeRS(cXyz* src) {
    WWHD_FUNC(0x0201B47C, s32, src);
    f32 square = gabi::call<f32>(0x028E8DD0, src);
    if (square < constant(0x100036EC)) return 0;
    gabi::call(0x028E8EF0, src, src);
    return 1;
}
VERIFY(0x0201B47C, xyz_normalizeRS);
u8 xyz_isZero(cXyz* src) {
    WWHD_FUNC(0x0201B4E0, u8, src);
    f32 epsilon = constant(0x1000374C);
    return std::fabs((f32)src->x) < epsilon && std::fabs((f32)src->y) < epsilon && std::fabs((f32)src->z) < epsilon;
}
VERIFY(0x0201B4E0, xyz_isZero);
void xyz_initializer() {
    WWHD_FUNC(0x0201B528, void);
    for (u32 off : {12u,8u,4u,0u}) gabi::store<u32>(0x101FFB98+off, 0);
    gabi::call(0x028F026C, gabi::at<void>(0x1018D460));
    f32 a = constant(0x10003750), b = constant(0x10003754);
    gabi::store<f32>(0x101FFB8C,a); gabi::store<f32>(0x101FFB90,b);
    gabi::call(0x028ED6F8, gabi::at<void>(0x101FFB94));
    gabi::call(0x028F026C, gabi::at<void>(0x1018D46C));
    gabi::call(0x028EAB2C, gabi::at<void>(0x101FFB95));
    gabi::call(0x028F026C, gabi::at<void>(0x1018D478));
    f32 zero = constant(0x10003724), one = constant(0x100036E8);
    gabi::at<cXyz>(0x101FFBB4)->set(one,zero,zero);
    gabi::at<cXyz>(0x101FFBC0)->set(zero,one,zero);
    gabi::at<cXyz>(0x101FFBCC)->set(zero,zero,one);
    gabi::at<cXyz>(0x101FFBA8)->set(zero,zero,zero);
    gabi::at<cXyz>(0x101FFBD8)->set(one,one,zero);
    gabi::at<cXyz>(0x101FFBE4)->set(one,zero,one);
    gabi::at<cXyz>(0x101FFBF0)->set(zero,one,one);
    gabi::at<cXyz>(0x101FFBFC)->set(one,one,one);
}
VERIFY(0x0201B528, xyz_initializer);
