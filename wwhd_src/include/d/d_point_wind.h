/** Capsule-driven point wind: unchanged 0x30 guest layout. */
#pragma once
#include "bindings.h"
struct dPointWindInfluence_l {
    cXyz mPos;
    cXyz mDir;
    be<f32> mRadius;
    be<f32> mStrength;
    be<f32> field_0x20;
    be<s32> mRegistIdx;
    u8 mbConstant;
    u8 pad29[3];
};
struct dPointWind_c {
    gptr<void> mpCps;
    dPointWindInfluence_l mWind;
};
WWHD_SIZE(dPointWindInfluence_l, 0x2C);
WWHD_SIZE(dPointWind_c, 0x30);
WWHD_OFFSET(dPointWind_c, mWind, 0x4);
WWHD_OFFSET(dPointWind_c, mWind.mRadius, 0x1C);
WWHD_OFFSET(dPointWind_c, mWind.field_0x20, 0x24);
