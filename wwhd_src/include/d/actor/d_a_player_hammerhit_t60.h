/* d_a_player_hammerhit_t60.h (WWHD, true 60 build, 2026-11-24): the Skull
 * Hammer's ground hit (procHammerFrontSwing, d_a_player_main_07.cpp) sets the swing's frame control on an earlier step of
 * the frame. Decomp build only. Separate file: the
 * source keeps thin calls.
 *
 * The hit backs the swing up to the hit frame (setFrame) and stops it (setRate(0)): the hold. A's update plays before the
 * dispatch, so the set frame stays until the next frame's play (rate 0: for good). At N steps a hit on an earlier step
 * was followed by the frame's later plays, and the rate hold (d_a_player_t60.h lkT60_rateHoldBegin)
 * played them at the frame-start rate: hammer route (h2, hit on frame 195's first step) set 7.39 and
 * the tick30 step's play moved it to 9.39 (A: 6.79, the swing pose 2.6 frames off for the hold's 10 frames).
 * Form: the hit's set is a set of the frame control within the frame (the split-actor state rule (3), lkT60_markAnmSet): no further
 * play this frame; the next frame plays it at rate 0 as A's does. A hit on the tick30 step: nothing (no play follows).
 * dt 1: nothing. Negative control WWHD_T60_HAMMERHIT_NEGCTRL=1 (game build): the later plays as before. */
#pragma once
#include "true60.h"
#include "d/actor/d_a_player_t60.h"
#include <cstdlib>

namespace lkT60HammerHit {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_HAMMERHIT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* after the hit's setFrame / setRate(0) on the swing's frame control */
inline void frameSet(uint32_t frameCtrlEa) {
    if (nc())
        return;
    lkT60_markAnmSet(frameCtrlEa);
}
}  // namespace lkT60HammerHit
