/* d_a_player_jumpfall_t60.h (WWHD, true 60 build; 2026-11-24). Decomp build only,
 *. Header only (no new functions in the glued unit).
 *
 * procSmallJump's fall test (`speed.y < -gravity` -> procFall_init) reads speed.y, which A's body sees once a frame at the
 * frame start (the previous frame's posMove left it). At N steps a later step's body reads the step before it, a share
 * of gravity further down: at N 4 the test fired on the tick30 step of a frame whose A test fires on the next frame
 * (Forest Haven, frame 391: FALL a frame early, its speed decay a frame ahead, Link 56 off by 541). Form (decisions
 * on the frame grid): the test is taken on the frame's first step only (that step reads the previous frame end, A's
 * value); a later step's test answers false. dt 1: the original test. NC WWHD_T60_JUMPFALL_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool lkT60_jumpFallNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_JUMPFALL_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
/* cond: the original test's answer on this step */
static inline bool lkT60_jumpFallTest(bool cond) {
    if (t60::dt() == 1.0f || lkT60_jumpFallNegctrl())
        return cond;
    return t60::first_step() && cond;
}
