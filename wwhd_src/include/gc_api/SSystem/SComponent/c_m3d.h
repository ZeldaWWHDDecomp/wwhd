/* c_m3d geometry API. */
#pragma once
#include "bindings.h"

/* 020109E8 cM3d_Len2dSq (HD out-of-line copy) [v c_m2d_g_box] */
inline f32 cM3d_Len2dSq(f32 x0, f32 y0, f32 x1, f32 y1) {
    return gabi::call<f32>(0x020109E8, x0, y0, x1, y1);
}
