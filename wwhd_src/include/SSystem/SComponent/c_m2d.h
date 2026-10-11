/* Circle/line intersection. */
#pragma once
#include "bindings.h"

class cM2dGCir {
public:
    be<f32> mPosX;
    be<f32> mPosY;
    be<f32> mRadius;
    be<u32> __vtbl;
};
WWHD_OFFSET(cM2dGCir, mPosX, 0);
WWHD_OFFSET(cM2dGCir, mPosY, 4);
WWHD_OFFSET(cM2dGCir, mRadius, 8);
WWHD_OFFSET(cM2dGCir, __vtbl, 0xC);
WWHD_SIZE(cM2dGCir, 0x10);
// HD: guest references use pointers for autoglue.
void cM2d_CrossCirLin(cM2dGCir*, f32, f32, f32, f32, be<f32>*, be<f32>*);
f64 hd_sqrt(f64);
void hd_assert(void*, s32, void*);
