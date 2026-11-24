/* d_a_player_drawsplit_t60.h (WWHD, true 60 build; 2026-11-23). Decomp build
 * only. Separate file: d_a_player_main_03.cpp keeps
 * thin calls.
 *
 * Link's two visual height offsets, m35C4 (setStepsOffset 023FF47C: the step offset, setWorldMatrix draws him at
 * pos.y + m35C4) and mFootBgOffsetY (footBgCheck 0240005C: the feet's ground offset, added to the model base matrix y),
 * ease once per frame toward their targets: addCalc(m, target, 0.5, max, min). Under the forms
 * (d_a_player_stepsoffset_t60.h, d_a_player_footbg_t60.h) every step of a frame eases ONCE FROM THE FRAME START, so the
 * tick30 step's value is A's. The frame's earlier steps therefore already hold the whole frame's ease: the drawn root
 * takes the frame's offset change on the earlier step's pass and little on the tick30 pass (a walking route at N 2, Link
 * running up the stair slope at the cliff, steps 3536-3590: drawn root y +5.2 / +0.1, +4.3 / -1.2 per pass, alt30 of
 * the drawn root 0.111 vs interp 0.026; the offsets' change was +2.5..+3.9 on the earlier step and -2.5..-4.0 on the
 * tick30 step while pos.y follows the per-step stair snap, the per-step ground-check rule).
 * Form (even split of the frame's ease, the tick30 step unchanged): on the frame's k-th step of N (k < N) the eased
 * value is start + (k / N) x (eased - start), start = the frame-start value the form eased from; the step's own
 * snap / move terms after the ease (setStepsOffset's step-up -0.7 diff, step-down +0.7 f) stay as they are. The
 * tick30 step (k = N) and dt 1 (N = 1, the original) never come here, and every step re-eases from the frame start, so
 * no earlier-step value reaches the tick30 step's offsets. Negative control WWHD_T60_DRAWSPLIT_NEGCTRL=1 (game build):
 * the earlier steps keep the whole ease. Host memory, one player.
 *
 * Leg IK angles (2026-11-23): footBgCheck's other eases under the same form, the
 * foot records' mLegAAngle / mLegBAngle (cLib_addCalcAngleS(.., 2, 0x1800, 0x10): the thigh / knee IK, jointCB1 reads
 * them with mLegBJointAngle / mFootAngle derived from them) and field_0x004 (the foot's ground roll), also eased once from
 * the frame start on every step: on stairs / slopes the earlier step's pass bent the legs the whole frame's ease and the
 * tick30 pass almost none. The same even split, in s16 angle space (the eased change is at most 0x1800, so the short way
 * is the change): start + (k / N) x (s16)(eased - start), truncated toward zero; the tick30 step unchanged, every step
 * re-eases from the frame start (footBgBegin writes the frame's angles back), so the frame end is A's. Negative control
 * WWHD_T60_LEGSPLIT_NEGCTRL=1 (game build): the earlier steps keep the whole ease (the leg angles only). */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool lkT60_drawSplitNegCtrl() {
    static const bool nc = [] {
        const char *e = getenv("WWHD_T60_DRAWSPLIT_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
}

/* the earlier step's share of the frame's ease: v = the eased value, start = the value it eased from, stamp = the step
 * counter of the frame's first step (the form's record); the caller checks that the step is under the form (dt != 1;
 * footBgCheck's form runs the call itself at dt 1) */
static inline void lkT60_drawSplitEase(f32 &v, f32 start, uint32_t stamp) {
    if (t60::steps() <= 1 || t60::tick30() || lkT60_drawSplitNegCtrl())
        return;
    const uint32_t k = t60_step_counter() - stamp + 1;
    const uint32_t n = t60::steps();
    if (k < 1 || k >= n)
        return;
    v = start + (v - start) * ((f32)k / (f32)n);
}

static inline bool lkT60_legSplitNegCtrl() {
    static const bool nc = [] {
        const char *e = getenv("WWHD_T60_LEGSPLIT_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
}

/* the earlier step's share of the frame's angle ease: v = the eased angle, start = the angle it
 * eased from (the frame-start value), stamp = the frame's first step (footBgCheck's form record); the caller checks that
 * the step is under the form */
static inline s16 lkT60_legSplitAngle(s16 v, s16 start, uint32_t stamp) {
    if (t60::steps() <= 1 || t60::tick30() || lkT60_legSplitNegCtrl())
        return v;
    const int32_t k = (int32_t)(t60_step_counter() - stamp + 1);
    const int32_t n = (int32_t)t60::steps();
    if (k < 1 || k >= n)
        return v;
    const int32_t d = (int16_t)(uint16_t)((uint16_t)v - (uint16_t)start);
    return (s16)(uint16_t)((uint16_t)start + (uint16_t)(int16_t)(d * k / n));
}
