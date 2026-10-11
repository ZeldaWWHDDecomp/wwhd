/**
 * c_m3d_g_lin.cpp
 * Component - line segments
 * HD emits the GameCube header-inline methods out of line in this translation unit.
 */
#include "SSystem/SComponent/c_m3d_g_lin.h"
#include "bindings.h" /* the header stays light: d/d_bg_s.h includes it */
void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out); /* moved from the header (actors define their own) */
#include <bit>

enum : u32 { lineVtable = 0x10003374 }; // HD: cM3dGLin vtable.

/* 02018780 */
// HD: GHS constructor allocates when called with NULL.
static cM3dGLin* cM3dGLin__ct(cM3dGLin* i_this, const cXyz* start, const cXyz* end) {
    if (i_this == NULL) {
        i_this = (cM3dGLin*)operator_new(sizeof(cM3dGLin));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->__vtbl = lineVtable;
    i_this->mStart = *start;
    i_this->mEnd = *end;
    return i_this;
}

/* 02018808 */
void cM3dGLin::SetStartEnd(const cXyz* start, const cXyz* end) {
    mStart = *start;
    mEnd = *end;
}

// HD: direct lfs/stfs pairs keep the original bits, even for signalling NaNs.
// Preserve the float-store event while avoiding a conversion through double.
static void copyFloatWord(be<f32>* dst, const be<f32>* src) {
    u32 word = *reinterpret_cast<const be<u32>*>(src);
    *dst = std::bit_cast<f32>(word);
}

/* 0201883C */
void cM3dGLin::SetStartEndVec(const cXyz* start, const cXyz* end) {
    copyFloatWord(&mStart.x, &start->x);
    copyFloatWord(&mStart.y, &start->y);
    copyFloatWord(&mStart.z, &start->z);
    copyFloatWord(&mEnd.x, &end->x);
    copyFloatWord(&mEnd.y, &end->y);
    copyFloatWord(&mEnd.z, &end->z);
}

/* 02018870 */
void cM3dGLin::SetEnd(const cXyz* pos) {
    mEnd = *pos;
}

/* 0201888C */
void cM3dGLin::CalcPos(cXyz* out, f32 scale) {
    gabi::Local<cXyz> tmp;
    PSVECSubtract(&mEnd, &mStart, tmp.get());
    PSVECScale(tmp.get(), tmp.get(), scale);
    PSVECAdd(tmp.get(), &mStart, out);
}

/* 0201890C */
static void __sinit_c_m3d_g_lin_cpp() {
    sinit_header_statics_rodata(0x101FF92C, 0x1018C964, 0x101FF938, 0x1000338C);
}

/* WWHD glue (tools/verify/autoglue.py)
ext 028E8DAC void PSVECSubtract(const cXyz*, const cXyz*, cXyz*)
*/
