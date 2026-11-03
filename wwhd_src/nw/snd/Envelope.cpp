/* Envelope parameter conversions and initial state. From WWHD's instructions only. */
#include "nw/snd/Envelope.h"
using namespace nw::snd;

void* sndEnvelopeAllocate(u32 size);
static const EnvelopeAttackRates sAttackRates;
static const EnvelopeSustainLevels sSustainLevels;

/* 028A5098 */
void Envelope::setAttack(u32 value) {
    // The 7-bit control is not range-checked; the element address wraps in 32 bits like the original's.
    attackMultiplier = *gabi::at<const be<f32>>(gabi::ea(&sAttackRates) + (value << 2));
}

/* 028A50B0 */
void Envelope::setHold(s32 value) {
    u32 adjusted = (u32)value + 1;
    s32 squared = (s32)(adjusted * adjusted);
    holdDuration = (u32)(squared / 4);
}

/* 028A50C8 */
f32 Envelope::rate(s32 value) {
    if (value == 127) return 65535.0f;
    if (value == 126) return 24.0f;
    if (value < 50) {
        f32 numerator = (f32)(s32)(((u32)value << 1) + 1);
        f32 scaled = gabi::fmuls_ppc(numerator, 0.0078125f);
        return gabi::fdivs_ppc(scaled, 5.0f);
    }
    f32 denominator = (f32)(s32)(126u - (u32)value);
    f32 scaled = gabi::fdivs_ppc(60.0f, denominator);
    return gabi::fdivs_ppc(scaled, 5.0f);
}

/* 028A5190 */
void Envelope::setDecay(s32 value) {
    gabi::AutoFrame frame{0x028A5190};
    decayRate = rate(value);
}

/* 028A51BC */
void Envelope::setSustain(u32 value) {
    sustain = value;
}

/* 028A51C4 */
void Envelope::setRelease(s32 value) {
    gabi::AutoFrame frame{0x028A51C4};
    releaseRate = rate(value);
}

/* 028A51F0 */
void Envelope::resetLevel(f64 decibels) {
    f32 scaled = gabi::fmuls_fpr(decibels, 10.0);
    stage = Attack;
    level = scaled;
}

/* 028A520C */
void Envelope::initialize(f64 decibels) {
    gabi::AutoFrame frame{0x028A520C};
    setAttack(127);
    setHold(0);
    setDecay(127);
    setSustain(127);
    setRelease(127);
    resetLevel(decibels);
}

/* 028A5260 */
Envelope* Envelope::construct(Envelope* envelope) {
    gabi::AutoFrame frame{0x028A5260};
    if (!envelope) envelope = (Envelope*)sndEnvelopeAllocate(sizeof(Envelope));
    if (envelope) envelope->initialize(-90.40000152587890625);
    return envelope;
}

/* 028A52A8 */
s32 Envelope::sustainLevel(u32 value) {
    // Unchecked index, 32-bit element address as in the original.
    return *gabi::at<const be<s16>>(gabi::ea(&sSustainLevels) + (value << 1));
}

/* 028A52BC */
void Envelope::update(s32 ticks) {
    gabi::AutoFrame frame{0x028A52BC};
    u32 currentStage = stage;
    if (currentStage == Attack) {
        if (ticks <= 0) return;
        f32 current = level;
        f32 multiplier = attackMultiplier;
        do {
            current = gabi::fmuls_ppc(current, multiplier);
            --ticks;
            if (current > -0.03125f) {
                u32 duration = holdDuration;
                level = 0.0f;
                holdRemaining = duration;
                stage = Hold;
                return;
            }
        } while (ticks > 0);
        level = current;
        return;
    }
    if (currentStage == Hold) {
        u32 remaining = holdRemaining;
        if (ticks >= (s32)remaining) {
            holdRemaining = 0;
            ticks = (s32)((u32)ticks - remaining);
            stage = Decay;
            s32 target = sustainLevel(sustain);
            f32 current = level;
            f32 speed = decayRate;
            f32 next = gabi::fnmsubs(speed, (f32)ticks, current);
            f32 destination = (f32)target;
            if (next < destination) {
                level = destination;
                stage = Sustain;
            } else {
                level = next;
            }
            return;
        }
        holdRemaining = remaining - (u32)ticks;
        if (stage != Decay) return;
        currentStage = Decay;
    }
    if (currentStage == Decay) {
        s32 target = sustainLevel(sustain);
        f32 current = level;
        f32 speed = decayRate;
        f32 next = gabi::fnmsubs(speed, (f32)ticks, current);
        f32 destination = (f32)target;
        if (next < destination) {
            level = destination;
            stage = Sustain;
        } else {
            level = next;
        }
        return;
    }
    if (currentStage == Release) {
        f32 speed = releaseRate;
        f32 current = level;
        level = gabi::fnmsubs(speed, (f32)ticks, current);
    }
}

/* 028A54C0 */
f32 Envelope::decibels() {
    if (stage == Attack && attackMultiplier == 0.0f) return 0.0f;
    f32 current = level;
    return gabi::fdivs_ppc(current, 10.0f);
}

/* WWHD glue
 data sAttackRates 10186D20
 data sSustainLevels 10186F54
 ext 0273AD10 void* sndEnvelopeAllocate(u32)
*/
