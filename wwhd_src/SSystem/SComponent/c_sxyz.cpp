/** Short-coordinate vector arithmetic. WWHD, verified unit c_sxyz. */
#include "SSystem/SComponent/c_sxyz.h"

/* 0201A478 */
csXyz* csXyz__ct(csXyz* i_this, s16 param_0, s16 param_1, s16 param_2) {
    // HD: GHS allocates on a NULL this and writes y before x.
    if (i_this == NULL) {
        i_this = static_cast<csXyz*>(csXyz_allocate(sizeof(csXyz)));
    }
    if (i_this != NULL) {
        i_this->y = param_1;
        i_this->x = param_0;
        i_this->z = param_2;
    }
    return i_this;
}

static SxyzResult vectorResult(const csXyz* result) {
    return {u32(u16(result->x)) << 16 | u16(result->y), u16(result->z)};
}

/* 0201A4DC */
SxyzResult csXyz_Add(csXyz* i_this, csXyz* param_0) {
    // HD: the original reads x, z, then y before constructing the return object.
    s16 x = s16(s32(i_this->x) + s16(param_0->x));
    s16 z = s16(s32(i_this->z) + s16(param_0->z));
    s16 y = s16(s32(i_this->y) + s16(param_0->y));
    gabi::Local<csXyz> temporary;
    csXyz* result = csXyz__ct(temporary.get(), x, y, z);
    return vectorResult(result);
}

/* 0201A554 */
void csXyz_AddAssign(csXyz* i_this, csXyz* param_0) {
    // HD: retain the interleaved loads and stores when the vectors overlap.
    s32 x = s16(i_this->x) + s16(param_0->x);
    s32 y = i_this->y;
    i_this->x = s16(x);
    y += s16(param_0->y);
    s32 z = i_this->z;
    i_this->y = s16(y);
    z += s16(param_0->z);
    i_this->z = s16(z);
}

/* 0201A588 */
SxyzResult csXyz_Multiply(csXyz* i_this, f32 param_0) {
    s16 x = s16(gabi::ftoi(f32(s16(i_this->x)) * param_0));
    s16 y = s16(gabi::ftoi(f32(s16(i_this->y)) * param_0));
    s16 z = s16(gabi::ftoi(f32(s16(i_this->z)) * param_0));
    gabi::Local<csXyz> temporary;
    csXyz* result = csXyz__ct(temporary.get(), x, y, z);
    return vectorResult(result);
}

static csXyz Zero;

/* 0201A668 */
// HD: header statics, destructor registrations, then csXyz::Zero.
static void __sinit_c_sxyz_cpp() {
    sinit_header_statics_rodata(0x101FFAF8, 0x1018D3AC, 0x101FFB04, 0x100036BC);
    csXyz__ct(&Zero, 0, 0, 0);
}

/* WWHD glue (tools/verify/autoglue.py): HD allocation and vector zero instance
ext 0273AD10 void* csXyz_allocate(u32)
data Zero 101FFB14
*/
