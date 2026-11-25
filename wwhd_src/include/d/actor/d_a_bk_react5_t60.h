/* d_a_bk_react5_t60.h (WWHD, true 60 build, 2026-11-25). Decomp build
 * only. Separate file: d_a_bk_exec.cpp keeps thin calls.
 *
 * daBk_Execute runs Bk_move (the AI) and then damage_reaction; its result 5 (a hit that only flinches) sends the Bokoblin
 * back to fight_run (action 4, mode 0, mTimers[1] 30) unless it already runs (4) or is in action 11. In A's one call the
 * AI's decision of the frame comes first: fight_run's tick30-only attack chance (the first-step input rule addendum) sets action 5,
 * then the frame's hit returns 5 and cancels it. At N steps the hit is read on the frame's first step, before the
 * tick30 AI decision: the case-5 check saw action 4 and did nothing, the tick30 step then rolled the attack and nothing
 * cancelled it (at N 2, Bokoblin 47C5CB0C frame 138: A back to fight_run mode 0, B attacked for 47 frames;
 * the fight split, Link hit at 158 instead of 169). Form: a case-5 result on an earlier step of the frame is noted; on the
 * frame's tick30 step (after its AI, in A's order) the case-5 check runs again on the frame-end action: action not 4 / 11
 * -> wait_set, action 4, mode 0, mTimers[1] 30 (the caller's code). The effects that ran on the earlier step (spit timer,
 * effect angle) stay as they were. dt 1: never. Negative control WWHD_T60_BKREACT5_NEGCTRL=1. Host memory keyed by the
 * actor, tagged with the 30 Hz frame. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60bkr5 {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKREACT5_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
inline std::unordered_map<u32, u32> s_noted; /* actor -> 30 Hz frame tag of an earlier step's case-5 result */

inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); } /* g_Counter.mCounter0 (as t60marr / t60bkturn) */

/* after damage_reaction, every step: r3 the step's result. Returns true on the tick30 step when an earlier step of this
 * frame returned 5 and this step did not (the caller then runs the case-5 check on the frame-end action) */
inline bool recheck(u32 actor, s32 r3) {
    if (t60::dt() == 1.0f || nc())
        return false;
    if (!t60::tick30()) {
        if (r3 == 5)
            s_noted[actor] = frameTag();
        return false;
    }
    auto it = s_noted.find(actor);
    if (it == s_noted.end())
        return false;
    const bool hit = it->second == frameTag() && r3 != 5;
    s_noted.erase(it);
    return hit;
}
} // namespace t60bkr5
