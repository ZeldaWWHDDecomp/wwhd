/* Line segment: GameCube names and WWHD layout.
 * Guest references use pointers. The Vec SetStartEnd overload is named separately,
 * and CalcPos omits const, to support verifier function and member pointers. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "wwhd.h"

class cM3dGLin {
public:
    void SetStartEnd(const cXyz* start, const cXyz* end);
    void SetStartEndVec(const cXyz* start, const cXyz* end);
    void SetEnd(const cXyz* pos);
    void CalcPos(cXyz* out, f32 scale);

    /* 0x00 */ cXyz mStart;
    /* 0x0C */ cXyz mEnd;
    /* 0x18 */ be<u32> __vtbl;
};
WWHD_OFFSET(cM3dGLin, mStart, 0x00);
WWHD_OFFSET(cM3dGLin, mEnd, 0x0C);
WWHD_OFFSET(cM3dGLin, __vtbl, 0x18);
WWHD_SIZE(cM3dGLin, 0x1C);
