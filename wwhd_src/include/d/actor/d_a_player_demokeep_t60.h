/* d_a_player_demokeep_t60.h (WWHD, true 60 build; 2026-11-25). Decomp
 * build only. Header only: d_a_player_main_03 keeps one call. */
#pragma once
#include "true60.h"
#include <cstdlib>

/* (2026-11-25, long1 seg 8 frame 7710, the Atorizk arrival): the start demo's
 * DEMO_KEEP (procedure body) ends when Link is more than 300 from home.pos. A tests it once per 30 Hz update, before
 * the frame's move (current.pos = the previous frame's end); at N steps a later step tested the position after the
 * frame's earlier steps had moved him (half a frame ahead at N 2): the timer cleared on B's tick30 step of 7707 (A:
 * 7708), the demo ended and WAIT came a frame early (B stops at 1823.96, A walks one more frame to 1807.82). Form: the
 * test (and its linkEndEventCut) on the frame's first step only, where B's position is A's. NC
 * WWHD_T60_DEMOKEEP_LEAVE_NEGCTRL=1. dt 1: every update (the original). */
static inline bool lkT60_demoKeepLeaveNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] {
        const char* e = getenv("WWHD_T60_DEMOKEEP_LEAVE_NEGCTRL");
        return e && e[0] == '1';
    }();
    return v;
#else
    return false;
#endif
}
static inline bool lkT60_demoKeepLeaveStep() { return t60::dt() == 1.0f || t60::first_step() || lkT60_demoKeepLeaveNc(); }
