/* d_a_player_frontwall_t60.h (WWHD, true 60 build; 2026-11-24, Link lane: the
 * the mdC / mdM route, Medli carried). Decomp build only.
 * Separate file: d_a_player_main_04b.cpp keeps a thin call.
 *
 * changeFrontWallTypeProc 02418E00 (the move procs' front-wall check) counts the frames Link keeps pushing against the
 * front wall in mFrontWallTimer: each call reads it, clears it, and on the ground sets it to the previous count + 1 when
 * the wall is still there; above the HIO counts it starts the small jump (type 6: > 4), the wall catch / climb (> 7, or
 * > 14), else nothing. The count is per 30 Hz frame in the original, but the proc calls it on every step: at N 2 it ran
 * twice as fast (Link carrying Medli at the low ledge: B's count 0..7 in 4 frames where A takes 8; at
 * frame 111 B's first step saw the small-jump step (type 6) with count 4 -> 5 > 4: SMALL_JUMP 0x29, where A met the same
 * wall at count 2 (112, type 6 for one frame, count 3) and walked on: HANG_FALL_START at 121).
 * Form (the 30 Hz timer rule / the process-order read rule frame-start reads): on the ground the check decides on the frame's first step only, which
 * runs at the frame-start place (before posMove), i.e. A's place for the frame's check; the later steps of the frame keep
 * the count and make no ground decision. The midair checks (the hang / ladder / climb catches while falling) stay per
 * step (the per-step ground-check rule). setFrontWallType runs on every step as before. dt 1 / N 1: never (the original).
 * NC WWHD_T60_FRONTWALL_NEGCTRL=1: the per-step count (the form before). Host data only, no guest reads. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool lkT60_frontWallNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] {
        const char* e = getenv("WWHD_T60_FRONTWALL_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
#else
    return false;
#endif
}
/* true: a later step of an N > 1 frame on the ground: no ground decision, the count kept (the caller returns FALSE
 * before it clears mFrontWallTimer); midair = the caller's mModeFlg & ModeFlg_MIDAIR */
static inline bool lkT60_frontWallHeld(unsigned midair) {
    if (t60::dt() == 1.0f || t60::steps() <= 1 || t60::first_step() || midair != 0 || lkT60_frontWallNc())
        return false;
    return true;
}
