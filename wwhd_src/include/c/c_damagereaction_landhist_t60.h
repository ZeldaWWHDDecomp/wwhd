/* c_damagereaction_landhist_t60.h (WWHD, true 60 build, 2026-11-25): the
 * enemy twin of Link's LANDHIST, d_a_player_landhist_t60.h). Decomp build only.
 * Separate file: c_damagereaction.cpp keeps a thin call.
 *
 * dr_body_bg_check runs the enemy's ground check (mAcch.CrrPos) once a frame in A. Acch's GroundCheckInit stores the last
 * check's ground hit (mPrevGroundHit) and GroundCheck sets GROUND_LANDING on a hit after none and GROUND_AWAY on no hit
 * after one: the edges of the frame-to-frame ground history. At N steps the edge falls on the step where it happens: an
 * enemy walking down a slope (at N 2, Bokoblin CB0C frames 121-125: A air / landing / air / landing /
 * ground, the fall term lifting and landing each frame) landed or left on the frame's first step, and the tick30 check
 * then saw the step's history: no AWAY where A left the ground, no LANDING where A landed, LANDING where A was already
 * down; mPrevGroundHit the first step's. Heights stayed A's, but the joints followed from 122 and the held weapon was
 * 1.74 off at 126. Form (flags only): on the tick30 step, after the check, mPrevGroundHit becomes the last frame-end
 * check's hit (the previous tick30 check) and GROUND_LANDING / GROUND_AWAY are set from it and this check's hit as A's
 * GroundCheck sets them; GROUND_HIT, GROUND_FIND, the position and the speed are the step's own. The frame-end hit is
 * recorded per damage reaction on every tick30 check. dt 1: never. Negative control WWHD_T60_DRLANDHIST_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include "d/d_bg_s.h"
#include <cstdlib>
#include <unordered_map>

namespace t60drLand {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRLANDHIST_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
inline std::unordered_map<u32, bool> s_lastHit; /* damage reaction -> the last tick30 check's GROUND_HIT */

/* right after mAcch.CrrPos in dr_body_bg_check, every step: key the damage reaction, acch its Acch */
template <class A> inline void apply(u32 key, A& acch) {
    if (t60::dt() == 1.0f || !t60::tick30() || nc())
        return;
    u32 f = (u32)acch.m_flags;
    const bool hit = (f & (u32)dBgS_Acch::GROUND_HIT) != 0;
    auto it = s_lastHit.find(key);
    if (it != s_lastHit.end()) {
        const bool prev = it->second;
        f &= ~(u32)(dBgS_Acch::GROUND_LANDING | dBgS_Acch::GROUND_AWAY);
        if (hit && !prev)
            f |= (u32)dBgS_Acch::GROUND_LANDING;
        if (!hit && prev)
            f |= (u32)dBgS_Acch::GROUND_AWAY;
        acch.m_flags = f;
        acch.mPrevGroundHit = prev ? 1 : 0;
    }
    s_lastHit[key] = hit;
}
} // namespace t60drLand
