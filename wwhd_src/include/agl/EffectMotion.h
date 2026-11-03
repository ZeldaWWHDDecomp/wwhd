/* AGL#8 effect motion prefix, reconstructed from the assigned update body. */
#pragma once
#include "wwhd.h"
namespace agl {
struct EffectMotionPrefix_l {
    u8 mPrefix[0x394];
    be<f32> mHorizontalOffset, mVerticalOffset, mAngle;
    be<f32> mHorizontalOscillation, mVerticalOscillation;
    be<f32> mHorizontalPhase, mVerticalPhase;
    u8 mRatePrefix[0x80];
    be<f32> mHorizontalRate, mVerticalRate;
    u8 mHorizontalPeriodPrefix[12];
    be<s32> mHorizontalPeriod;
    u8 mVerticalPeriodPrefix[12];
    be<s32> mVerticalPeriod;
    u8 mAmplitudePrefix[12];
    be<f32> mHorizontalAmplitude, mVerticalAmplitude;
    u8 mAngularRatePrefix[0x20];
    be<f32> mAngularRate;
    void advanceMotion(f64 elapsedFrames);
};
WWHD_OFFSET(EffectMotionPrefix_l,mHorizontalOffset,0x394);
WWHD_OFFSET(EffectMotionPrefix_l,mVerticalOffset,0x398);
WWHD_OFFSET(EffectMotionPrefix_l,mAngle,0x39C);
WWHD_OFFSET(EffectMotionPrefix_l,mHorizontalOscillation,0x3A0);
WWHD_OFFSET(EffectMotionPrefix_l,mVerticalOscillation,0x3A4);
WWHD_OFFSET(EffectMotionPrefix_l,mHorizontalPhase,0x3A8);
WWHD_OFFSET(EffectMotionPrefix_l,mVerticalPhase,0x3AC);
WWHD_OFFSET(EffectMotionPrefix_l,mHorizontalRate,0x430);
WWHD_OFFSET(EffectMotionPrefix_l,mVerticalRate,0x434);
WWHD_OFFSET(EffectMotionPrefix_l,mHorizontalPeriod,0x444);
WWHD_OFFSET(EffectMotionPrefix_l,mVerticalPeriod,0x454);
WWHD_OFFSET(EffectMotionPrefix_l,mHorizontalAmplitude,0x464);
WWHD_OFFSET(EffectMotionPrefix_l,mVerticalAmplitude,0x468);
WWHD_OFFSET(EffectMotionPrefix_l,mAngularRate,0x48C);
WWHD_SIZE(EffectMotionPrefix_l,0x490);
}
