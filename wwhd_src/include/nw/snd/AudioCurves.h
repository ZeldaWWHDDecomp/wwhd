/* Sound value curves: low-pass cutoff, pan and surround attenuation curves, pitch ratios,
 * decibel amplitudes, the sound random generator and sample storage sizes, from WWHD. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
// A pan curve selection: which curve table, whether the centre is normalised to 1, whether
// gains clamp at 1 (else 2), and whether the surround variant is disabled.
struct PanCurve {
    be<u32> mCurve;
    be<u8> mNormalizeCenter, mClampUnity, mDisableSurround;
    u8 mPadding;
    f32 panGain(s32 mode, f64 pan) const;
    f32 attenuationGain(f64 attenuation) const;
};
WWHD_OFFSET(PanCurve, mNormalizeCenter, 4);
WWHD_OFFSET(PanCurve, mClampUnity, 5);
WWHD_OFFSET(PanCurve, mDisableSurround, 6);
WWHD_SIZE(PanCurve, 8);
u32 cutoffFrequency(f64 level);
f32 pitchRatio(s32 pitch);
f32 decibelAmplitude(f64 decibels);
u32 nextRandomHalfword();
u32 sampleStorageBytes(u32 samples, u32 format);
} }
