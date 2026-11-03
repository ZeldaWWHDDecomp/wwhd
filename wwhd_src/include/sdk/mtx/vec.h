/* Cafe SDK vector layout matches the GameCube MTX API. */
#pragma once
#include "wwhd.h"
struct Vec {
    be<f32> x, y, z;
};
WWHD_OFFSET(Vec, x, 0);
WWHD_OFFSET(Vec, y, 4);
WWHD_OFFSET(Vec, z, 8);
WWHD_SIZE(Vec, 12);
void PSVECNormalize(const Vec *, Vec *);
void C_VECNormalize(const Vec *, Vec *);
void C_VECAdd(const Vec *, const Vec *, Vec *);
f32 C_VECDotProduct(const Vec *, const Vec *);
