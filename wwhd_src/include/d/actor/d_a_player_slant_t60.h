/* d_a_player_slant_t60.h (WWHD, true 60 build, 2026-11-24): Link's running lean
 * (setMoveSlantAngle 02400990: mMoveSlantAngle, shape_angle.z = half of it, mBodyAngle.z) once a 30 Hz frame. Decomp build
 * only. Separate file: d_a_player_main_04.cpp keeps a thin
 * call.
 *
 * The original leans toward 1.6 x the frame's turn (m34DE, the facing at the execute's start = the frame start, minus
 * the frame-end facing) with cLib_addCalcAngleS(4, 0xC8, 0x64), or decays by 0.35 a frame. Per step (the
 * step's turn / dt as the rate, the decay 1 - 0.65^dt) the target changes between the frame's steps: Link's integer turn
 * lands on some steps only (a small turn on the first step, none on the next), so step 1 leaned to A's value and step 2
 * (turn 0, target 0) chased it back (a bomb carry pair, frames 70 / 72: A mMoveSlantAngle -16 / shape.z
 * -8, B 0 / 0): the lean tilts the hands (mLeft / RightHandPos), so the carried bomb ended 0.002 - 0.03 off A in y on
 * 44 frames (its logic words). Form (the whole-frame form / 14b frame-exact form): the frame's earlier steps hold the lean (the
 * frame-start lean on the 30 Hz grid: visual secondary motion, the 30 Hz visual-motion rule), the tick30 step runs the original
 * setMoveSlantAngle at dt 1 with m34DE = the frame's start facing (latched on the first step): the frame end is A's.
 * NC WWHD_T60_SLANT_NEGCTRL=1: the per-step form. dt 1: the original. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool lkT60_slantNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SLANT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static struct { uint32_t stamp; int16_t start; bool valid; } s_lkT60Slant;
static float s_lkT60SlantDt = 1.0f;
/* true on an earlier step of the frame (the lean holds); the frame's first step latches the frame-start facing */
static inline bool lkT60_slantEarlier(const be<s16>* m34DE) {
    if (t60::dt() == 1.0f || lkT60_slantNc())
        return false;
    if (t60::first_step()) {
        s_lkT60Slant.stamp = t60_step_counter();
        s_lkT60Slant.start = (int16_t)*m34DE;
        s_lkT60Slant.valid = true;
    }
    return !t60::tick30();
}
/* the tick30 step: m34DE = the frame-start facing, dt 1 (returns m34DE's own value for lkT60_slantEnd) */
static inline int16_t lkT60_slantBegin(be<s16>* m34DE) {
    const int16_t own = (int16_t)*m34DE;
    s_lkT60SlantDt = 1.0f;
    if (t60::dt() == 1.0f || lkT60_slantNc())
        return own;
    if (s_lkT60Slant.valid && t60_step_counter() - s_lkT60Slant.stamp < t60::steps())
        *m34DE = s_lkT60Slant.start;
    s_lkT60SlantDt = t60_set_dt(1.0f);
    return own;
}
static inline void lkT60_slantEnd(be<s16>* m34DE, int16_t own) {
    if (s_lkT60SlantDt != 1.0f) {
        t60_set_dt(s_lkT60SlantDt);
        s_lkT60SlantDt = 1.0f;
        *m34DE = own;
    }
}
