/* Capsule geometry, cM3dGLin layout followed by mRadius (GameCube and WWHD).
 * Flat base fields avoid importing bindings.h's caller-only cM3dGCps class.
 * Guest references use pointers; SetShape names the shape overload for verifier registration. */
#pragma once
#include "SSystem/SComponent/c_m3d_g_cyl.h"

struct cM3dGCpsS {
    cXyz mStart;
    cXyz mEnd;
    be<f32> mRadius;
};
class cM3dGCps {
public:
    void Set(const cXyz* start, const cXyz* end, f32 radius);
    void SetShape(const cM3dGCpsS* shape);
    void SetCps(const cM3dGCps* source);
    /* 0x00 */ cXyz mStart;
    /* 0x0C */ cXyz mEnd;
    /* 0x18 */ be<u32> __vtbl;
    /* 0x1C */ be<f32> mRadius;
};
WWHD_OFFSET(cM3dGCps, mStart, 0x00);
WWHD_OFFSET(cM3dGCps, mEnd, 0x0C);
WWHD_OFFSET(cM3dGCps, __vtbl, 0x18);
WWHD_OFFSET(cM3dGCps, mRadius, 0x1C);
WWHD_SIZE(cM3dGCps, 0x20);
WWHD_SIZE(cM3dGCpsS, 0x1C);
void cM3dGLin_SetStartEnd(cM3dGCps* line, const cXyz* start, const cXyz* end);
void cM3dGLin_SetStartEndVec(cM3dGCps* line, const cXyz* start, const cXyz* end);
void operator_delete(void* object);
