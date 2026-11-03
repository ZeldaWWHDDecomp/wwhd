/* Sound envelope state and 7-bit controls, reconstructed from WWHD's binary. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct EnvelopeAttackRates { be<f32> values[128]; };
struct EnvelopeSustainLevels { be<s16> values[128]; };
WWHD_SIZE(EnvelopeAttackRates, 512);
WWHD_SIZE(EnvelopeSustainLevels, 256);
struct Envelope {
    enum Stage { Attack, Hold, Decay, Sustain, Release };
    be<u32> stage;
    be<f32> level;
    be<f32> decayRate;
    be<f32> releaseRate;
    be<f32> attackMultiplier;
    be<u16> holdDuration;
    be<u16> holdRemaining;
    be<u8> sustain;
    u8 padding[3];
    void setAttack(u32 value);
    void setHold(s32 value);
    static f32 rate(s32 value);
    void setDecay(s32 value);
    void setSustain(u32 value);
    void setRelease(s32 value);
    void resetLevel(f64 decibels);
    void initialize(f64 decibels);
    static Envelope* construct(Envelope* envelope);
    static s32 sustainLevel(u32 value);
    void update(s32 ticks);
    f32 decibels();
};
WWHD_OFFSET(Envelope, level, 4);
WWHD_OFFSET(Envelope, attackMultiplier, 0x10);
WWHD_OFFSET(Envelope, holdDuration, 0x14);
WWHD_OFFSET(Envelope, holdRemaining, 0x16);
WWHD_OFFSET(Envelope, sustain, 0x18);
WWHD_SIZE(Envelope, 0x1C);
} }
