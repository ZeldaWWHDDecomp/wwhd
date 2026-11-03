/* JAudio track vibrato; Cafe keeps the GameCube three-float layout. */
#pragma once
#include "bindings.h"
namespace JASystem {
class TVibrate {
public:
    TVibrate();
    void init();
    void incCounter();
    f32 getValue() const;
    /* 0x00 */ be<f32> mPhase;
    /* 0x04 */ be<f32> mDepth;
    /* 0x08 */ be<f32> mPitch;
};
WWHD_OFFSET(TVibrate, mPhase, 0);
WWHD_OFFSET(TVibrate, mDepth, 4);
WWHD_OFFSET(TVibrate, mPitch, 8);
WWHD_SIZE(TVibrate, 12);
}
f64 vibratoSin(f64 angle);
f32 vibratoPitchToCent(f32 value, f32 scale);
