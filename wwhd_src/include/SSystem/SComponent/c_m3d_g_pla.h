/* Plane with normal: cM3dGPla, GameCube and WWHD layout (0x14).
 * Guest reference arguments use pointers for autoglue. HD constructors initialize mD. */
#pragma once
#include "bindings.h"

class cM3dGPla {
public:
    void SetupNP0(const cXyz* pNormal, const cXyz* pPoint);
    u32 getCrossY(const cXyz* i_axis, be<f32>* i_value);
    u32 getCrossYLessD(const cXyz* i_axis, be<f32>* i_value);
    void Set(const cM3dGPla* pla);
    s16 CalcAngleXz(be<s16>* pAngleX, be<s16>* pAngleY);

    /* 0x00 */ cXyz mNormal;
    /* 0x0C */ be<f32> mD;
    /* 0x10 */ be<u32> __vtbl;
};
WWHD_OFFSET(cM3dGPla, mNormal, 0x00);
WWHD_OFFSET(cM3dGPla, mD, 0x0C);
WWHD_OFFSET(cM3dGPla, __vtbl, 0x10);
WWHD_SIZE(cM3dGPla, 0x14);

void PSVECNormalize(const cXyz* in, cXyz* out);
s16 cM3d_CalcVecAngle(const cXyz* normal, be<s16>* x, be<s16>* z);
