/* Sound modulation oscillator state from WWHD's instructions. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct Modulator {
    be<f32> depth;
    be<f32> frequency;
    be<u32> delay;
    be<u8> range;
    be<u8> shape;
    be<u8> phaseOffset;
    u8 padding;
    be<u32> elapsed;
    be<f32> phase;
    be<f32> cached;
    be<u8> started;
    be<u8> wrapped;
    u8 trailingPadding[2];
    static Modulator* construct(Modulator* modulator);
    void initialize();
    void reset();
    void update(s32 ticks);
    f32 value();
};
WWHD_OFFSET(Modulator, delay, 8);
WWHD_OFFSET(Modulator, range, 0xC);
WWHD_OFFSET(Modulator, elapsed, 0x10);
WWHD_OFFSET(Modulator, phase, 0x14);
WWHD_OFFSET(Modulator, started, 0x1C);
WWHD_SIZE(Modulator, 0x20);
struct ModulationCurveTable { be<u32> functions[32]; };
WWHD_SIZE(ModulationCurveTable, 128);
struct ModulationSineEntry { be<f32> base, slope; be<u32> reserved[2]; };
struct ModulationSineTable { ModulationSineEntry entries[256]; };
struct ModulationLinearPieces { be<u32> words[5]; };
WWHD_SIZE(ModulationSineEntry, 16);
WWHD_SIZE(ModulationSineTable, 4096);
WWHD_SIZE(ModulationLinearPieces, 20);
struct ModulationCurve {
    static f32 sine(f64 phase);
    static f32 triangle(f64 phase);
    static void initializeTable();
    static f32 step(f64 phase);
    static f32 random();
    static f64 identity(f64 phase);
};
} }
