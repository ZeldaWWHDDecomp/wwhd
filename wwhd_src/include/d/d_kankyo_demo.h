#pragma once
#include "gc_api/gc_api.h"

struct DEMO_PLIGHT {
    /* 0x00 */ be<s32> field_0x00;
    /* 0x04 */ be<s32> field_0x04;
    /* 0x08 */ LIGHT_INFLUENCE mLightInfluence;
    // HD: Initial update delay follows the expanded LIGHT_INFLUENCE.
    /* 0x2C */ be<f32> mBrightness;
    /* 0x30 */ be<u32> mUpdateDelay;
};
WWHD_OFFSET(DEMO_PLIGHT, field_0x04, 4);
WWHD_OFFSET(DEMO_PLIGHT, mLightInfluence, 8);

WWHD_OFFSET(DEMO_PLIGHT, mBrightness, 0x2C);
WWHD_OFFSET(DEMO_PLIGHT, mUpdateDelay, 0x30);
WWHD_SIZE(DEMO_PLIGHT, 0x34);

f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32);
