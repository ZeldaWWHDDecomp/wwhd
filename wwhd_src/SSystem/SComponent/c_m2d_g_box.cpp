/**
 * c_m2d_g_box.cpp - two dimensional box geometry
 * WWHD, written in the style of zeldaret/tww and verified against the original
 * (tools/verify, unit c_m2d_g_box).
 */
#include "SSystem/SComponent/c_m2d_g_box.h"
#include "gc_api/SSystem/SComponent/c_m3d.h"
#include "gc_api/MSL_C/string.h"
#include <cmath>

static const f32 l_zero = 0.0f;
static const f32 l_circleZero = 0.0f;

/* 0201060C */
void cM2dGBox::Set(cXy* p0, cXy* p1) {
    mP0 = *p0;
    mP1 = *p1;
}

/* 02010630 */
f32 cM2dGBox::GetLen(const cXy* p) {
    // HD: combine the inside / above / below tests, preserving the original load order.
    f32 x = p->x;
    f32 x0 = mP0.x;
    if (x0 < x && x < mP1.x) {
        f32 y0 = mP0.y;
        f32 y = p->y;
        f32 y1 = mP1.y;
        if (y0 < y && y < y1) {
            return l_zero;
        }
        f32 distY0 = std::fabs(y0 - y);
        f32 distY1 = std::fabs(y1 - y);
        return distY0 > distY1 ? distY1 : distY0;
    }
    f32 y0 = mP0.y;
    f32 y = p->y;
    if (y0 < y && y < mP1.y) {
        f32 x1 = mP1.x;
        f32 distX0 = std::fabs(x0 - x);
        f32 distX1 = std::fabs(x1 - x);
        return distX0 > distX1 ? distX1 : distX0;
    }
    // HD: diagonal distance uses an out-of-line squared-distance call before sqrtf.
    f32 cornerX = x < x0 ? x0 : (f32)mP1.x;
    f32 cornerY = y < y0 ? y0 : (f32)mP1.y;
    f32 squared = cM3d_Len2dSq(x, y, cornerX, cornerY);
    return std_sqrtf(squared);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02010594 */
cM2dGBox* cM2dGBox__ct(cM2dGBox* self) {
    // HD: GHS constructor allocates when this is NULL and stores the TU's trailing vtable.
    if (!self) self = (cM2dGBox*)operator_new(sizeof(cM2dGBox));
    if (self) {
        self->vtable = 0x10001F50;
        bzero(&self->mP0, (u32)sizeof(cXy));
        bzero(&self->mP1, (u32)sizeof(cXy));
    }
    return self;
}

/* 020105F8 */
void cM2dGBox__dt(cM2dGBox* self, s32 flags) {
    if (self && (flags & 1)) operator_delete(self);
}

/* 020107A8 */
static void __sinit_c_m2d_g_box_cpp() {
    sinit_header_statics_rodata(0x101FF84C, 0x1018C844, 0x101FF858, 0x10001F44);
}

/* 0201083C */
u32 cM2dGCir__ct(u32 self) {
    // HD: separate circle TU hosted here; preserve its integer address ABI and initialization order.
    if (!self) {
        self = gabi::ea(operator_new(sizeof(cM2dGCir)));
        if (!self) return 0;
    }
    cM2dGCir* circle = gabi::at<cM2dGCir>(self);
    f32 zero = l_circleZero;
    circle->vtable = 0x10001F64;
    circle->mPosX = zero;
    circle->mRadius = zero;
    circle->mPosY = zero;
    return self;
}

/* 02010890 */
static void __sinit_c_m2d_g_cir_cpp() {
    sinit_header_statics_rodata(0x101FF868, 0x1018C868, 0x101FF874, 0x10001F7C);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's constants
data l_zero 10001F3C
data l_circleZero 10001F74
*/
