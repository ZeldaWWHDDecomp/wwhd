/* JPA_emitter_t60.h (WWHD, true 60 build): the particle emitter's per-frame state on a general
 * step length (dt = 1/N).
 *
 * JPABaseEmitter (JPA_emitter_calc.cpp), once JPA-10 runs the particle calc on every step at g_dt = 1/N:
 *   - mTick +0x194 (calc 0281FE40: the emitter age; key frames, max frame) and mTime +0x198 (doStartFrameProcess
 *     028200C4: the start delay) advance by dt (the 30 Hz timer rule: continuous ages, read as floats: the key values interpolate).
 *   - the rate-step timer mRateStepTimer +0x250 (calcCreatePtcls 0281F878) advances by dt; the frame's earlier steps only
 *     advance it, the tick30 step runs the original emission (RateStepEmit set by the previous frame's crossing) and the
 *     crossing (revised with: the emission follows the frame's emitter callbacks, which run on the tick30
 *     step, so an emitter that follows an actor emits where the 30 Hz frame does). The emission itself is unchanged: count = floor(mEmitCount + rate x (1 + rndm x r)), the
 *     FirstEmit minimum and the FixedInterval count, with the emitter's own RNG (+0x1EC) drawn as in the 30 Hz frame:
 *     emission keeps the 30 Hz cadence (one emission per crossing, at the frame's place), so the emitter RNG sequence and
 *     the particle counts are A's (shared-RNG rule: draws at A's place); the particles then move at the step length
 *     (JPA-07). Not equivalent by design: a rate-step timer that is not a whole number at a frame start (only reachable
 *     from a step-length change, N 2 -> 4); the N-step spec domain is whole timers.
 *   - the HD countdowns mDeleteCountdown +0x3D4 (doTerminationProcess 0282014C: the two-frame delete wait) and
 *     mClearCountdown +0x3D5 (calc: the deferred particle clear) count 30 Hz frames: they decrement on the frame's tick30
 *     step only (the 30 Hz timer rule, half-step counting); a delete wait started on an earlier step starts at 3, so the frame's
 *     tick30 step brings it to the 30 Hz frame's 2 (the frame-end value is A's).
 *   - the game's emitter callbacks (calc's CallbackExecute / ExecuteAfter, also under StopCalc) run once per
 *     30 Hz frame, on the tick30 step: they are unconverted game code (d_particle_callbacks.cpp, converts
 *     them per callback); the particle callbacks of calcParticles likewise (JPA_particle.cpp).
 *   - an emitter's first calc (fix): the manager's first visit of an emitter (mTick and
 *     mTime still 0, FirstEmit still set: no start-delay step and no calc step has run) runs at the frame time already
 *     elapsed, k / N on step k of the frame (t60::step), not 1 / N: an emitter created on the tick30 step (by the
 *     frame's 30 Hz code, before the particle calc) gets one whole frame there like A's first calc (age 1, rate-step
 *     timer +1, A's crossing); one created on an earlier step k gets k / N, so with the frame's remaining steps it has
 *     k / N + (N - k) / N = 1 at the frame end. Without it a new emitter stays 1/N behind A for life (its rate-step
 *     crossings half a frame late at N 2: the 3BBBBB0C burst emits once instead of twice, pt7). Only in converted
 *     context (dt < 1); at N = 1 (step 1, dt 1) and in N-step spec's whole-frame runs (step 1 first) it is the earlier code.
 * One-update emitters (pair): emission happens on the frame's
 *     tick30 step only, so an emitter a converted actor creates on an earlier step and stops on a later step of the same
 *     frame (Link's foot smoke: setFootEffect on a footfall step, resetFootEffect 023DF9C0 -> the callback's end:
 *     StopEmit, mMaxFrame -1) would never emit; at 30 Hz the stop comes one frame later, after the frame's emission. Form:
 *     the manager notes an emitter on its first visit on an earlier step (not stopped yet: its mMaxFrame); on that frame's
 *     tick30 step a noted emitter that was stopped since gets the frame's calc as if not stopped (StopEmit / delete wait
 *     / mMaxFrame undone for the calc), then the stop is put back: the frame's emission is A's, the next frame's calc sees
 *     the stopped emitter as A's next frame does. A stop on the creating step itself is not deferred (A: no emission
 *     either). Entries of a group not seen on its tick30 visit (the emitter was deleted) are dropped. dt 1: never noted.
 * dt = 1 / tick30: the original. */
#pragma once
#include "true60.h"

namespace jpa_t60 {
/* a per-frame count moves on this step: dt 1 or the frame's tick30 step */
inline bool frame_count() { return t60::dt() == 1.0f || t60::tick30(); }
/* a countdown of n 30 Hz frames started on this step: n, or n + 1 on an earlier step (its frame's tick30 decrements) */
inline unsigned countdown_start(unsigned n) { return frame_count() ? n : n + 1; }
/* the dt of the manager's first visit of an emitter: the frame time elapsed by the end of this step (k / N); 0 when
 * the step is not a later converted step (dt 1, step 1: the emitter is not read) or the emitter is not on its first
 * visit (the caller keeps the step's dt) */
template <class Emitter> inline float first_calc_dt(const Emitter* e, unsigned firstEmit) {
    float dt = t60::dt();
    unsigned k = t60::step();
    if (!(dt < 1.0f) || k <= 1u) return 0.0f;
    if ((float)e->mTick != 0.0f || (float)e->mTime != 0.0f || !((unsigned)e->mFlags & firstEmit)) return 0.0f;
    return k >= t60::steps() ? 1.0f : (float)k * dt;
}
/* one-update emitters (see above): the emitters first visited on an earlier step of this frame, per group */
struct Fresh { unsigned ea; int maxFrame; unsigned group; };
static Fresh s_fresh[64];
static unsigned s_freshN = 0;
/* first visit on an earlier step, not stopped yet: note it (its mMaxFrame, before a stop overwrites it) */
template <class Emitter> inline void fresh_note(const Emitter* e, unsigned group, unsigned firstEmit, unsigned stopEmit) {
    if (frame_count() || s_freshN >= 64u) return;
    if ((float)e->mTick != 0.0f || (float)e->mTime != 0.0f || !((unsigned)e->mFlags & firstEmit)) return;
    if (((unsigned)e->mFlags & stopEmit) || (int)e->mMaxFrame < 0) return;
    const unsigned ea = gabi::ea(e);
    for (unsigned i = 0; i < s_freshN; i++)
        if (s_fresh[i].ea == ea) return;  /* (noted already: a StopCalc emitter stays at age 0) */
    s_fresh[s_freshN++] = Fresh{ea, (int)e->mMaxFrame, group};
}
/* the tick30 visit: a noted emitter stopped since gets this calc unstopped; returns 1 when the stop is to be put back */
template <class Emitter> inline bool fresh_unstop(Emitter* e, unsigned group, unsigned stopEmit, unsigned enableDelete) {
    if (!s_freshN || !frame_count() || !(t60::dt() < 1.0f)) return false;
    const unsigned ea = gabi::ea(e);
    for (unsigned i = 0; i < s_freshN; i++) {
        if (s_fresh[i].ea != ea || s_fresh[i].group != group) continue;
        const int maxFrame = s_fresh[i].maxFrame;
        s_fresh[i] = s_fresh[--s_freshN];
        if (!((unsigned)e->mFlags & stopEmit) && (int)e->mMaxFrame >= 0) return false;
        e->mFlags = (unsigned)e->mFlags & ~(stopEmit | enableDelete);
        e->mMaxFrame = maxFrame;
        e->mDeleteCountdown = 0;  /* (only a stop on a later step can have started it: the emitter has no particles) */
        return true;
    }
    return false;
}
template <class Emitter> inline void fresh_restop(Emitter* e, unsigned stopEmit) {
    e->mFlags = (unsigned)e->mFlags | stopEmit;
    e->mMaxFrame = -1;
}
/* end of a group's tick30 visit: entries not seen belonged to deleted emitters */
inline void fresh_end(unsigned group) {
    if (!s_freshN || !frame_count()) return;
    for (unsigned i = 0; i < s_freshN;)
        if (s_fresh[i].group == group) s_fresh[i] = s_fresh[--s_freshN];
        else i++;
}
}  // namespace jpa_t60
