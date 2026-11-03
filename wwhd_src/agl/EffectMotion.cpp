/* AGL#8 offset, angular, and periodic motion update. */
#include "agl/EffectMotion.h"
#include <atomic>
void updateEffectMotionState(agl::EffectMotionPrefix_l*);
f64 effectMotionCosine(f64);
f64 effectMotionSine(f64);
/* 02792640 */
void agl::EffectMotionPrefix_l::advanceMotion(f64 elapsedFrames) {
    gabi::AutoFrame frame{0x02792640};
    // Original stack slots preserve scalar stores used by the paired-vector update.
    struct MotionScratch_l {
        be<u32> mConversionHigh, mConversionLow;
        be<f32> mHorizontalRaw, mVerticalRaw;
        be<f32> mHorizontalStep, mVerticalStep;
        be<f32> mHorizontalFinal, mVerticalFinal;
        be<f32> mHorizontalCopy, mVerticalCopy;
    };
    gabi::FrameLocal<MotionScratch_l> scratch(8);
    auto periodAsFloat = [&](s32 period) -> f32 {
        scratch->mConversionLow = (u32)period ^ 0x80000000u;
        scratch->mConversionHigh = 0x43300000u;
        u64 bits = ((u64)(u32)scratch->mConversionHigh << 32) | (u32)scratch->mConversionLow;
        f64 encoded;
        __builtin_memcpy(&encoded,&bits,sizeof(encoded));
        return (f32)(encoded - 4503601774854144.0);
    };
    constexpr f32 pi = 3.1415927f;
    constexpr f32 twoPi = 6.2831855f;
    constexpr f32 secondsPerFrame = 0.016666668f;
    f32 horizontal = mHorizontalOffset;
    f32 vertical = mVerticalOffset;
    if (horizontal < 0.0f) {
        horizontal = gabi::fadds_ppc(horizontal,1.0f);
        mHorizontalOffset = horizontal;
    }
    if (vertical < 0.0f) {
        vertical = gabi::fadds_ppc(vertical,1.0f);
        horizontal = mHorizontalOffset;
        mVerticalOffset = vertical;
    }
    if (horizontal > 1.0f) {
        mHorizontalOffset = gabi::fsubs_ppc(horizontal,1.0f);
        vertical = mVerticalOffset;
    }
    if (vertical > 1.0f) mVerticalOffset = gabi::fsubs_ppc(vertical,1.0f);
    f32 verticalStep = gabi::fmuls_fpr((f64)(f32)mVerticalRate,elapsedFrames);
    f32 horizontalStep = gabi::fmuls_fpr((f64)(f32)mHorizontalRate,elapsedFrames);
    scratch->mVerticalRaw = verticalStep;
    f32 verticalRaw = scratch->mVerticalRaw;
    scratch->mHorizontalRaw = horizontalStep;
    f32 horizontalRaw = scratch->mHorizontalRaw;
    scratch->mHorizontalCopy = horizontalRaw;
    scratch->mVerticalCopy = verticalRaw;
    verticalRaw = scratch->mVerticalCopy;
    horizontalRaw = scratch->mHorizontalCopy;
    verticalStep = gabi::fmuls_ppc(verticalRaw,secondsPerFrame);
    scratch->mVerticalStep = verticalStep;
    horizontalStep = gabi::fmuls_ppc(horizontalRaw,secondsPerFrame);
    horizontal = mHorizontalOffset;
    f32 angle = mAngle;
    verticalStep = scratch->mVerticalStep;
    scratch->mHorizontalStep = horizontalStep;
    horizontalStep = scratch->mHorizontalStep;
    scratch->mHorizontalFinal = horizontalStep;
    horizontalStep = scratch->mHorizontalFinal;
    scratch->mVerticalFinal = verticalStep;
    verticalStep = scratch->mVerticalFinal;
    horizontal = gabi::fadds_ppc(horizontal,horizontalStep);
    vertical = mVerticalOffset;
    f32 angularRate = mAngularRate;
    vertical = gabi::fadds_ppc(vertical,verticalStep);
    mHorizontalOffset = horizontal;
    mVerticalOffset = vertical;
    if (angle > pi) {
        angle = gabi::fsubs_ppc(angle,twoPi);
        mAngle = angle;
    }
    if (angle < -pi) {
        angle = gabi::fadds_ppc(angle,twoPi);
        mAngle = angle;
    }
    f32 angularStep = gabi::fmuls_fpr((f64)angularRate,elapsedFrames);
    angularStep = gabi::fdivs_ppc(angularStep,60.0f);
    s32 horizontalPeriod = mHorizontalPeriod;
    mAngle = gabi::fadds_ppc(angle,angularStep);
    if (horizontalPeriod > 1) {
        f32 phaseStep = gabi::fmuls_fpr((f64)twoPi,elapsedFrames);
        phaseStep = gabi::fdivs_ppc(phaseStep,periodAsFloat(horizontalPeriod));
        f32 phase = gabi::fadds_ppc((f32)mHorizontalPhase,phaseStep);
        mHorizontalPhase = phase;
        f64 cosine = effectMotionCosine((f64)phase);
        mHorizontalOscillation = gabi::fmuls_fpr((f64)(f32)mHorizontalAmplitude,cosine);
    }
    s32 verticalPeriod = mVerticalPeriod;
    if (verticalPeriod > 1) {
        f32 phaseStep = gabi::fmuls_fpr((f64)twoPi,elapsedFrames);
        phaseStep = gabi::fdivs_ppc(phaseStep,periodAsFloat(verticalPeriod));
        f32 phase = gabi::fadds_ppc((f32)mVerticalPhase,phaseStep);
        mVerticalPhase = phase;
        f64 sine = effectMotionSine((f64)phase);
        mVerticalOscillation = gabi::fmuls_fpr((f64)(f32)mVerticalAmplitude,sine);
    }
    updateEffectMotionState(this);
    std::atomic_thread_fence(std::memory_order_acquire);
}
/* WWHD glue
ext 028F4BE0 f64 effectMotionCosine(f64)
ext 028F43F8 f64 effectMotionSine(f64)
ext 02791E24 void updateEffectMotionState(agl::EffectMotionPrefix_l*)
*/
