/* cM3dGAab: axis-aligned bounding box, GameCube and WWHD layout (0x1C).
 * HD emits the inline GameCube operations out of line. Guest reference arguments use pointers
 * for autoglue; SetMinMaxAab names the GameCube box overload separately. */
#pragma once
#include "bindings.h"

class cM3dGAab {
public:
    void Set(const cXyz* min, const cXyz* max);
    BOOL CrossY(const cXyz* v);
    BOOL UnderPlaneYUnder(f32 y);
    BOOL TopPlaneYUnder(f32 y);
    void ClearForMinMax();
    void ClearForMinMaxY();
    void SetMin(const cXyz* pMin);
    void SetMax(const cXyz* pMax);
    void SetMinMax(const cXyz* pMinMax);
    void SetMinMaxAab(const cM3dGAab* aab);
    void SetMinY(f32 y);
    void SetMaxY(f32 y);
    void SetMinMaxY(f32 y);
    void CalcCenter(cXyz* pOut);
    void PlusR(f32 r);

    /* 0x00 */ cXyz mMin;
    /* 0x0C */ cXyz mMax;
    /* 0x18 */ be<u32> __vtbl;
};
WWHD_OFFSET(cM3dGAab, mMin, 0x00);
WWHD_OFFSET(cM3dGAab, mMax, 0x0C);
WWHD_OFFSET(cM3dGAab, __vtbl, 0x18);
WWHD_SIZE(cM3dGAab, 0x1C);
