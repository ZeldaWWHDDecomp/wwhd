// Local bloom parameter layout from the assigned executable instructions.
#pragma once
#include "wwhd.h"
namespace agl::environment {
struct BloomColorWeights {
    u8 mBeforeWeights[0x228]; be<f32> mRed,mGreen,mBlue;
    u8 mBeforeMode[0x64]; be<s32> mMode; u8 mBeforeNormalized[4];
    be<f32> mNormalizedRed,mNormalizedGreen,mNormalizedBlue;
    void normalize();
};
static_assert(offsetof(BloomColorWeights,mMode)==0x298);
static_assert(offsetof(BloomColorWeights,mNormalizedRed)==0x2A0);
static_assert(sizeof(BloomColorWeights)==0x2AC);
struct BloomParameters {
    u8 mBeforeOwner[0x18]; gptr<void> mOwner,mTable; BloomColorWeights mColorWeights;
    void reset();
};
static_assert(offsetof(BloomParameters,mColorWeights)==0x20);
static_assert(sizeof(BloomParameters)==0x2CC);
}
