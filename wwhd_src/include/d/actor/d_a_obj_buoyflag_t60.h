/* d_a_obj_buoyflag_t60.h (WWHD, true 60 build): the buoy flag (d_a_obj_buoyflag*.cpp,
 * the flag on the floating barrels of daObjBarrel2) on a general step length (dt = 1/N). Converted, the
 * flag executes on every step with g_dt = 1/N; at dt = 1 (N = 1, the 30 Hz fallback, an unconverted flag) every helper
 * returns the original per-frame value, so the original instructions run (dt-1 check bit-exact). Separate file:
 * the .cpp files keep thin calls only.
 *
 * mode_afl (afloat on its barrel; the barrel writes m1090 each of its executes):
 *   - the cloth (Packet_c::calc_pos) is the d_cloth_packet form (the per-step smoothing rule): per frame speed += F
 *     (springs, gravity, the wind on the normals), speed += k (speed - wind x jitter) (= a relaxation toward the wind
 *     with drag 1 + k), pos = old + speed. A step: speed += F x cloth60_push(1 + k), k_dt = (1 + k)^dt - 1, pos = old +
 *     speed x dt (the speed stays a per-frame speed). The forces are re-evaluated on each step's positions (cloth-simulation: the
 *     cloth is checked statistically).
 *   - the wind phases mAng[0..11] (per-frame integer adds): cloth60_share (exact per 30 Hz frame).
 *   - the move push (the barrel's translation since the last execute x 0.2, a per-frame speed): / dt (a step's
 *     translation is dt of the frame's when the barrel moves every step; an unconverted barrel moves on the tick30
 *     step only: that step then carries N x the push, whose relaxation share (1 - drag^dt) x N matches the frame's
 *     1 - drag to first order).
 *   - the random draws stay once per 30 Hz frame on the tick30 step, in the original order (the per-step chance rule: the visual wind
 *     jitter cM_rnd < 0.05 phase jump, cM_rndF(0.2) on the wind power, cM_rndF(0.2) x 3 per vertex on the drag's wind);
 *     the frame's other steps take the means (no jump, + 0.1, x 1.0): stateless, visual only.
 * mode_jumpToSea (the barrel exploded: the flag spins about a random axis and sinks, then deletes itself): runs on
 *   every step since (follow-up; was the 30 Hz visual-motion rule 30 Hz grid): the sink through the
 *   converted daObj_posMoveF_stream (the exact frame move: the frame's speed update on its first step from the
 *   frame-start height (the above / below the wave resistance), the position carried between and landed on the
 *   original's single move on the tick30 step), the spin spring (integer carry, visual only) once per frame on the
 *   first step and held (30 Hz visual-motion form), the cloth in the mode_afl step form, its draws on the tick30 step.
 * No host memory. */
#pragma once
#include "d/d_cloth_packet_t60.h"

/* the cloth's step length (both modes; follow-up 2026-11-18: mode_jumpToSea runs every step too) */
template <class A> static inline f32 buoy60_dt(const A* a) {
    (void)a;
    return t60::dt();
}

/* does this step take the frame's random draws (once per 30 Hz frame, on the tick30 step) */
template <class A> static inline bool buoy60_draw(const A* a) { return buoy60_dt(a) == 1.0f || t60::tick30(); }

/* mode_jumpToSea's spin (mRotSpeed / mRotAngle: an integer-carried spring, visual only): once per 30 Hz frame, on the
 * frame's first step, held over its later steps (the 30 Hz visual-motion rule form); the sink itself moves every step through the
 * converted daObj::posMoveF_stream (the exact frame move) */
static inline bool buoy60_spin() { return t60::dt() == 1.0f || t60::first_step(); }

/* a wind phase's per-frame add for this step (cloth60_share: exact per 30 Hz frame) */
template <class A> static inline s32 buoy60_ang(const A* a, s32 perFrame) {
    return buoy60_dt(a) == 1.0f ? perFrame : cloth60_share(perFrame);
}

/* the drag factor k (speed += k (speed - wind)) for a step: (1 + k)^dt - 1; k itself at dt 1 */
template <class A> static inline f32 buoy60_dragK(const A* a, f32 k) {
    const f32 dt = buoy60_dt(a);
    if (dt == 1.0f) {
        return k;
    }
    return (f32)(__builtin_pow(1.0 + (double)k, (double)dt) - 1.0);
}

/* the step's share of the frame's force push: cloth60_push with drag 1 + k (drag-consistent); 1 at dt 1 */
template <class A> static inline f32 buoy60_push(const A* a, f32 k) {
    return buoy60_dt(a) == 1.0f ? 1.0f : cloth60_push(1.0f + k);
}
