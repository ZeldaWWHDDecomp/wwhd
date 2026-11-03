/* Modulation defaults and simple waveform callbacks. From WWHD's binary only. */
#include "nw/snd/Modulator.h"
#include "nw/snd/AudioCurves.h"
using namespace nw::snd;

static ModulationCurveTable sModulationCurves;
static const ModulationSineTable sModulationSine;
static const ModulationLinearPieces sTriangleSlopes;
static const ModulationLinearPieces sTriangleOffsets;
void clearModulationTable(void* data, u32 size);

/* 028A54F4 */
void Modulator::initialize() {
    range = 1;
    phaseOffset = 0;
    frequency = 6.25f;
    delay = 0;
    depth = 0.0f;
    shape = 0;
}

/* 028A5528 */
void Modulator::reset() {
    started = 0;
    wrapped = 0;
    elapsed = 0;
    phase = 0.0f;
    cached = 1.0f;
}

/* 028A5554 */
void Modulator::update(s32 ticks) {
    u32 wait = delay;
    u32 current = elapsed;
    if (current < wait) {
        u32 next = current + (u32)ticks;
        if (next <= wait) {
            elapsed = next;
            return;
        }
        ticks = (s32)((u32)ticks + current - wait);
        elapsed = wait;
    }
    f32 speed = frequency;
    if (!(speed > 0.0f)) return;
    if (!started) {
        u32 offset = phaseOffset;
        started = 1;
        phase = gabi::fdivs_ppc((f32)offset, 127.0f);
        speed = frequency;
    }
    f32 advance = gabi::fmuls_ppc(speed, (f32)ticks);
    f32 normalized = gabi::fdivs_ppc(advance, 1000.0f);
    f32 next = gabi::fadds_ppc(phase, normalized);
    s32 whole = gabi::ftoi(next);
    wrapped = !(next < 1.0f);
    phase = gabi::fsubs_ppc(next, (f32)whole);
}

/* 028A567C */
f32 Modulator::value() {
    gabi::AutoFrame frame{0x028A567C};
    if (depth == 0.0f) return 0.0f;
    u32 time = elapsed;
    if (time < (u32)delay) return 0.0f;
    u32 waveform = shape;
    u32 callback = sModulationCurves.functions[waveform];
    f64 sample;
    if (!callback) {
        sample = 1.0;
    } else if (waveform == 4 && !wrapped) {
        sample = cached;
    } else {
        sample = gabi::call_ptr<f64>(callback, (f32)phase);
        if (waveform == 4) cached = gabi::f32_from_bits(ppc_stfs_bits(sample));
    }
    f32 amount = depth;
    u32 scale = range;
    f32 scaled = gabi::fmuls_fpr(sample, amount);
    return gabi::fmuls_ppc(scaled, (f32)scale);
}

/* 028A57E0 */
f32 ModulationCurve::sine(f64 phase) {
    f32 scaled = gabi::fmuls_fpr(phase, 4294967296.0);
    u32 position;
    if (scaled < 2147483648.0f) {
        position = (u32)gabi::ftoi(scaled);
    } else {
        position = (u32)gabi::ftoi(gabi::fsubs_ppc(scaled, 2147483648.0f)) + 0x80000000u;
    }
    u32 fraction = position & 0xFFFFFF;
    const ModulationSineEntry* entry = &sModulationSine.entries[position >> 24];
    f32 slope = entry->slope;
    f32 product = gabi::fmuls_ppc(slope, (f32)fraction);
    f32 base = entry->base;
    return gabi::fmadds(product, 0.000000059604644775390625f, base);
}

/* 028A58CC */
f32 ModulationCurve::triangle(f64 phase) {
    gabi::AutoFrame frame{0x028A58CC};
    gabi::FrameLocal<be<s32>> converted(8);
    gabi::FrameLocal<ModulationLinearPieces> slopes(0x10);
    gabi::FrameLocal<ModulationLinearPieces> offsets(0x24);
    for (u32 i = 0; i < 5; ++i) offsets->words[i] = sTriangleOffsets.words[i];
    for (u32 i = 0; i < 5; ++i) slopes->words[i] = sTriangleSlopes.words[i];
    f32 scaled = gabi::fmuls_fpr(phase, 4.0);
    u32 index;
    if (scaled < 2147483648.0f) {
        *converted = gabi::ftoi(scaled);
        index = (u32)(s32)*converted;
    } else {
        *converted = gabi::ftoi(gabi::fsubs_ppc(scaled, 2147483648.0f));
        index = (u32)(s32)*converted + 0x80000000u;
    }
    f64 slope = *gabi::at<const be<f32>>(gabi::ea(slopes.get()) + (index << 2));
    f64 offset = *gabi::at<const be<f32>>(gabi::ea(offsets.get()) + (index << 2));
    // Scalar-single multiply-add retains the incoming FPR and rounds its multiplier to 25 bits.
    return (f32)gabi::ppc_nan3(slope, offset, phase,
        slope * round25(phase) + offset, gabi::ppc_dnan64());
}

/* 028A59AC */
f32 ModulationCurve::step(f64 phase) {
    f32 centered = gabi::fsubs_fpr(phase, 0.5);
    return centered >= 0.0f ? 1.0f : 0.0f;
}

/* 028A59D0 */
f32 ModulationCurve::random() {
    gabi::AutoFrame frame{0x028A59D0};
    u32 value = nextRandomHalfword();
    f32 converted = (f32)value;
    return gabi::fdivs_ppc(converted, 65535.0f);
}

/* 028A5A1C */
void ModulationCurve::initializeTable() {
    gabi::AutoFrame frame{0x028A5A1C};
    clearModulationTable(&sModulationCurves, 128);
    sModulationCurves.functions[0] = gabi::guest_fn((const void*)&sine);
    sModulationCurves.functions[1] = gabi::guest_fn((const void*)&triangle);
    sModulationCurves.functions[2] = gabi::guest_fn((const void*)&identity);
    sModulationCurves.functions[3] = gabi::guest_fn((const void*)&step);
    sModulationCurves.functions[4] = gabi::guest_fn((const void*)&random);
}

/* 028A5A90 */
f64 ModulationCurve::identity(f64 phase) {
    return phase;
}

/* WWHD glue
 data sModulationCurves 101FC93C
 data sModulationSine 1017E9C4
 data sTriangleSlopes 10187094
 data sTriangleOffsets 101870A8
 ext 028F5914 void clearModulationTable(void*,u32)
 ext 0289CE14 u32 nw::snd::nextRandomHalfword()
*/
