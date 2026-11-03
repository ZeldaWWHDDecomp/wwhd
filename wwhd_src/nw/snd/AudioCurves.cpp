/* Table-based sound conversions reconstructed from WWHD. The tables stay in the game image. */
#include "nw/snd/AudioCurves.h"
#include "nw/snd/AdpcmDecode.h"
using namespace nw::snd;
// The conversion tables in the image (views; their contents stay in the game).
struct CutoffTable { be<u16> hertz[24]; };                 // per 1/30 step of the low-pass level
struct SemitoneTable { be<f32> ratios[12]; };
struct FineTuneTable { be<f32> ratios[256]; };              // 1/256-semitone steps
struct DecibelTable { be<f32> amplitudes[965]; };           // -90.4 dB .. +6.0 dB in 0.1 dB steps
struct PanCurveSet { gptr<const be<f32>> curves[3]; };      // 257 points each
static CutoffTable sCutoffFrequencies;
static SemitoneTable sSemitoneRatios;
static FineTuneTable sFineRatios;
static DecibelTable sDecibelAmplitudes;
static PanCurveSet sPanCurves;
static PanCurveSet sSurroundPanCurves;
static u32 sRandomState = 0x12345678;
static const s32 kOctave = 12 * 256;            // pitch units per octave

// Element `index` of a guest table with the original's 32-bit address arithmetic.
template <class T> static const T& tableEntry(const T* table, u32 index) {
    return *gabi::at<const T>(gabi::ea(table) + index * u32(sizeof(T)));
}

/* 0289CA18 */
u32 nw::snd::cutoffFrequency(f64 level) {
    gabi::AutoFrame frame{0x0289CA18u};
    if (level > 1.0f) level = 1.0f;
    else level = level >= 0.0f ? level : 0.0f;
    if (level < 0.13561438f) return 80;
    if (!(level < 0.9f)) return 16000;
    const f32 distance = gabi::fsubs_fpr(level, 0.13561438f);
    const u32 index = gabi::ftoi(gabi::fdivs_ppc(distance, 0.033333335f));
    return tableEntry(sCutoffFrequencies.hertz, index);
}

/* 0289CAB4 */
f32 PanCurve::panGain(s32 mode, f64 pan) const {
    gabi::AutoFrame frame{0x0289CAB4u};
    // Map the pan -1..1 to 0..1.
    f32 position;
    if (pan > 1.0f) {
        position = gabi::fmuls_ppc(gabi::fadds_ppc(1.0f, 1.0f), 0.5f);
    } else {
        const f64 clamped = gabi::fsubs_fpr(pan, -1.0f) >= 0.0f ? pan : -1.0;
        position = gabi::fmuls_ppc(gabi::fadds_fpr(clamped, 1.0f), 0.5f);
    }
    const gptr<const be<f32>>* curves = (!mDisableSurround && mode == 2) ? sSurroundPanCurves.curves : sPanCurves.curves;
    const u32 index = gabi::ftoi(gabi::fmadds(position, 256.0f, 0.5f));
    const be<f32>* curve = tableEntry(curves, mCurve);
    f32 gain = tableEntry(curve, index);
    if (mNormalizeCenter) gain = gabi::fdivs_ppc(gain, curve[128]);
    const f32 maximum = mClampUnity ? 1.0f : 2.0f;
    if (gain > maximum) return maximum;
    return gain >= 0.0f ? gain : 0.0f;
}

/* 0289CC00 */
f32 PanCurve::attenuationGain(f64 attenuation) const {
    gabi::AutoFrame frame{0x0289CC00u};
    f64 clamped = attenuation;
    if (clamped > 2.0f) clamped = 2.0f;
    else clamped = clamped >= 0.0f ? clamped : 0.0f;
    const f32 position = gabi::fmuls_fpr(clamped, 0.5f);
    const u32 index = gabi::ftoi(gabi::fmadds(position, 256.0f, 0.5f));
    const be<f32>* curve = tableEntry(sPanCurves.curves, mCurve);
    const f32 gain = tableEntry(curve, index);
    if (gain > 2.0f) return 2.0f;
    return gain >= 0.0f ? gain : 0.0f;
}

/* 0289CCCC */
f32 nw::snd::pitchRatio(s32 pitch) {
    s32 octaves = 0;
    if (pitch < 0) {
        const s32 down = s32((~u32(pitch)) / u32(kOctave)) + 1;
        pitch = s32(u32(pitch) + u32(down) * u32(kOctave));
        octaves = -down;
    }
    if (pitch >= kOctave) {
        const s32 up = pitch / kOctave;
        pitch -= up * kOctave;
        octaves += up;
    }
    const s32 semitones = pitch / 256;
    const u32 fine = u32(pitch - semitones * 256);
    f32 ratio = 1.0f;
    for (s32 octave = 0; octave < octaves; ++octave) ratio = gabi::fadds_ppc(ratio, ratio);
    for (s32 octave = 0; octave > octaves; --octave) ratio = gabi::fmuls_ppc(ratio, 0.5f);
    if (semitones) ratio = gabi::fmuls_ppc(ratio, sSemitoneRatios.ratios[semitones]);
    if (fine) ratio = gabi::fmuls_ppc(ratio, sFineRatios.ratios[fine]);
    return ratio;
}

/* 0289CDBC */
f32 nw::snd::decibelAmplitude(f64 decibels) {
    gabi::AutoFrame frame{0x0289CDBCu};
    f64 clamped = 6.0f;
    if (!(decibels > clamped)) clamped = gabi::fsubs_fpr(decibels, -90.4f) >= 0.0f ? decibels : f64(-90.4f);
    const u32 index = u32(gabi::ftoi(gabi::fmuls_fpr(clamped, 10.0f))) + 904u;
    return tableEntry(sDecibelAmplitudes.amplitudes, index);
}

/* 0289CE14 */
u32 nw::snd::nextRandomHalfword() {
    const u32 next = sRandomState * 1664525u + 1013904223u;
    sRandomState = next;
    return next >> 16;
}

/* 0289CE3C */
u32 nw::snd::sampleStorageBytes(u32 samples, u32 format) {
    if (format == SamplePcm8) return samples;
    if (format == SamplePcm16) return samples * 2u;
    if (format != SampleAdpcm) return 0;
    // 14 samples per 8-byte ADPCM frame; a partial frame takes its header byte too.
    const u32 frames = samples / 14u;
    const u32 remainder = samples - frames * 14u;
    return frames * 8u + (remainder ? ((remainder + 1u) >> 1) + 1u : 0u);
}

/* WWHD glue
data sCutoffFrequencies 101854A0
data sSemitoneRatios 10182428
data sFineRatios 10182458
data sDecibelAmplitudes 10182858
data sPanCurves 101FC924
data sSurroundPanCurves 101FC930
data sRandomState 101FC920
*/
