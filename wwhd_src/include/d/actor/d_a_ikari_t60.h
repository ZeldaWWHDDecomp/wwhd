/* d_a_ikari_t60.h (WWHD, true 60 build): the anchor (d_a_ikari.cpp) on a general step length
 * (dt = 1/N). Converted, the anchor executes on every step with g_dt = 1/N; at dt = 1 (N = 1, the 30 Hz
 * fallback, an unconverted anchor) every helper returns the original per-frame value, so the original instructions run
 * (dt-1 check bit-exact). Separate file: d_a_ikari.cpp keeps thin calls only.
 *
 * _execute's per-frame work: mTimer += 1 (the swing's phase counter, visual: nothing else reads it), the swing angle
 * x = rotX (1 + sin(mTimer x k)), k = REG0_S(5) + 500, rotX from the wind power (the environment, unconverted: per frame).
 *   - mTimer stays a per-frame counter, counted on the tick30 step (a visual counter: the first-step AI-timer rule leaves those on the
 *     tick30 step); the frame's other steps read the frame's progress p (dt per step since the frame's first step) as a
 *     fractional phase: sin(mTimer x k + p x k). On the tick30 step p is 1 and mTimer has advanced: the original value
 *     (mTimer x k), so every 30 Hz frame ends on the original angle.
 *   - the progress is host memory keyed by the actor (a few anchors per room; a lost slot restarts at the step's dt).
 * Draw interpolation:. */
#pragma once
#include "true60.h"

struct Ikari60Slot {
    const void* owner;
    u32 stamp; /* t60_step_counter() of the last step */
    f32 acc;   /* the frame's progress after that step */
};

/* the frame's progress after this step (dt per step since the frame's first step); 1 at dt 1 */
static inline f32 ikari60_progress(const void* a) {
    const f32 dt = t60::dt();
    if (dt == 1.0f) {
        return 1.0f;
    }
    static Ikari60Slot slots[16];
    const u32 now = t60_step_counter();
    Ikari60Slot* s = nullptr;
    Ikari60Slot* oldest = &slots[0];
    for (Ikari60Slot& e : slots) {
        if (e.owner == a) {
            s = &e;
            break;
        }
        if (now - e.stamp > now - oldest->stamp) {
            oldest = &e;
        }
    }
    if (s == nullptr) {
        s = oldest;
        s->owner = a;
        s->acc = 0.0f;
        s->stamp = now - 1;
    }
    const bool cont = now - s->stamp == 1 && !t60::first_step();
    s->acc = (cont ? s->acc : 0.0f) + dt;
    s->stamp = now;
    if (t60::tick30() || s->acc > 1.0f) {
        s->acc = 1.0f;
    }
    return s->acc;
}

/* does this step count the frame's mTimer (the tick30 step; every step at dt 1) */
static inline bool ikari60_count() { return t60::dt() == 1.0f || t60::tick30(); }

/* the swing phase for cM_ssin: mTimer x k at dt 1 and on the tick30 step (mTimer already counted), else
 * mTimer x k + progress x k */
static inline s32 ikari60_phase(const void* a, s32 timer, s32 k) {
    const f32 p = ikari60_progress(a);
    if (ikari60_count()) {
        return timer * k;
    }
    return timer * k + (s32)(p * (f32)k);
}
