/* d_a_mo2_arrive_t60.h (WWHD, true 60 build, 2026-11-24): the Moblin patrol's (jyunkai
 * mode 1, d_a_mo2_move.cpp) arrival / stop tests on the frame-start answer, as the Bokoblin's (the animation-driven decision rule).
 * Decomp build only. Separate file: the source keeps
 * thin calls.
 *
 * A's call tests the distance to the path target from the frame-start position (before the frame's move). At N > 1 a
 * later step tested its mid-frame position, 0.5 frame closer, and stopped a frame early: Moblin route on w15 (moF),
 * Moblin 47C5CECC patrolling: B's frame-94 tick30 step reached the point (mode 2, wait_set's mTimers[1] 51) where A's
 * frame 95 does. Form: the tests answer on the frame's first step; the first step's stop changes the state, which holds
 * the later steps (mo2_t60_ai_held). dt = 1: every call. Negative control WWHD_T60_MOARRIVE_NEGCTRL=1 (game build): the
 * tests on every step. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60marr {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_MOARRIVE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* true: this step's call runs the arrival / stop tests */
inline bool test() { return nc() || t60::dt() == 1.0f || t60::first_step(); }
}  // namespace t60marr
