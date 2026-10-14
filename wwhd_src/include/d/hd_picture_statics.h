// Per-translation-unit HD header state. Private decomp-derived source.
#pragma once
#include "d/d_picture_box.h"
struct hdPictureStaticState_c {
    be<f32> mMinimumAngle, mMaximumAngle;
    be<u8> mInitFlagA, mInitFlagB;
    u8 mReserved[2];
    be<u32> mZeroState[4];
};
WWHD_OFFSET(hdPictureStaticState_c, mInitFlagA, 8);
WWHD_OFFSET(hdPictureStaticState_c, mInitFlagB, 9);
WWHD_OFFSET(hdPictureStaticState_c, mZeroState, 12);
WWHD_SIZE(hdPictureStaticState_c, 0x1C);
struct hdPictureStaticTokens_c { dPictureInitTokenHD mTokens[3]; };
WWHD_SIZE(hdPictureStaticTokens_c, 36);
struct hdPictureAngles_c { be<f32> mMinimum, mMaximum; };
WWHD_SIZE(hdPictureAngles_c, 8);
inline void initializePictureHeaderStatics(hdPictureStaticState_c* state,
        hdPictureStaticTokens_c* tokens, const hdPictureAngles_c* angles) {
    for (u32 i = 0; i < 4; i++) state->mZeroState[i] = 0;
    pictureTokenInit(&tokens->mTokens[0]);
    f32 minimum = angles->mMinimum;
    f32 maximum = angles->mMaximum;
    state->mMinimumAngle = minimum;
    state->mMaximumAngle = maximum;
    pictureInitFlagA(&state->mInitFlagA);
    pictureTokenInit(&tokens->mTokens[1]);
    pictureInitFlagB(&state->mInitFlagB);
    pictureTokenInit(&tokens->mTokens[2]);
}
