/* d_a_bk_failhold_t60.h (WWHD, true 60 build, 2026-11-25, holder 47C5CE68
 * frame 222). Decomp build only. Separate file:
 * d_a_bk_move.cpp keeps thin calls.
 *
 * Bk_move takes its knocked-down branch (dr.mMode <= -100: no dispatch, no speed from speedF) from the state it is called
 * in. A's call of the frame in which damage_reaction ends the knock-down (case 1, health 0: FAIL, action 20 mode 0) runs
 * Bk_move still knocked down (-100) and returns there; the speed vector stays 0 and the frame does not move. At N steps
 * FAIL was set on the frame's first step; its tick30 step is held (bk_t60_ai_held) but entered Bk_move with mode 0: the
 * general path's tail turned speedF (-20) into the speed and dr_damage_set moved the Bokoblin a whole frame (-1.99 /
 * -4.59) that A never moves (bkL at N 2 + REACT5 / DMGTURN / KDHOLD). Form: the mode Bk_move is entered with on the
 * frame's first step is noted; a held later step of that frame whose first-step mode was knocked down takes the
 * knocked-down branch as A's call did (its held part: the weapon sphere carry, DMGTURN's tick30 sensing / turn).
 * dt 1: never. Negative control WWHD_T60_BKFAILHOLD_NEGCTRL=1. Host memory keyed by the actor, frame-tagged. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60bkfh {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKFAILHOLD_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct Entry {
    u32 frame;
    bool knocked;
};
inline std::unordered_map<u32, Entry> s_first;

inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); } /* g_Counter.mCounter0 */

/* Bk_move's entry, every step: mode the dr.mMode it is entered with, held this step's bk_t60_ai_held. Returns true when
 * this held step must take the knocked-down branch (the frame's first-step entry was knocked down) */
inline bool knockedFrame(u32 actor, s32 mode, bool held) {
    if (t60::dt() == 1.0f || nc())
        return false;
    if (t60::first_step()) {
        s_first[actor] = Entry{frameTag(), mode <= -100};
        return false;
    }
    if (!held || mode <= -100)
        return false;
    auto it = s_first.find(actor);
    return it != s_first.end() && it->second.frame == frameTag() && it->second.knocked;
}
} // namespace t60bkfh
