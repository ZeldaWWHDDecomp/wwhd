/* d_a_bk_turnfs_t60.h (WWHD, true 60 build, 2026-11-24, the Bokoblin twin of the
 * the frame-88 form t60marr::turnPlace, d_a_mo2_arrive_t60.h). Decomp build only.
 * Separate file: d_a_bk_move.cpp keeps thin calls.
 *
 * Bk_move's walking modes set dr.mTurnTargetY = atan2 of a target point (the patrol point m0320, the path point, home)
 * from the Bokoblin's place and ease current.angle.y toward it. A's call measures from the frame-start place (before the
 * frame's move); at N steps the later steps measured from their mid-frame place: the target drifted by the step's lateral
 * offset and the integer ease turned a frame early (route at N 2, Bokoblin 47C5CB0C jyunkai mode 1 frame 7:
 * B's tick30 -1848, A -1850; 0.002 off, then the fight split from frame 137, Link 134.7 off). Form: the turn target's place
 * is the frame's first-step place on every step of the frame (the arrival tests already read the first step's answer, rule
 * the animation-driven decision rule). Only the target angle's place: the distance tests keep their own place / form. A frame whose first step did
 * not record (a mode entered later) uses the step's own place. dt 1: the call's own place (the original). Negative
 * control WWHD_T60_BKTURNFS_NEGCTRL=1 (game build): each step's own place. Host memory keyed by the actor. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60bkturn {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKTURNFS_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct Place {
    u32 frame;
    f32 x, z;
};
inline std::unordered_map<u32, Place> s_places;

/* the place the turn target is measured from: *x / *z in (the step's place), the frame-start place out on later steps;
 * true when it replaced the place (the caller then measures from it; otherwise it keeps the original's own vector,
 * bit for bit: cXyz_mi is the guest's PSVECSubtract) */
inline bool place(u32 actor, f32* x, f32* z) {
    if (t60::dt() == 1.0f || nc())
        return false;
    const u32 frame = *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 (as t60marr::turnPlace) */
    Place& e = s_places[actor];
    if (t60::first_step()) {
        e.frame = frame, e.x = *x, e.z = *z;
        return false;
    }
    if (e.frame == frame) {
        *x = e.x;
        *z = e.z;
        return true;
    }
    return false;
}
} // namespace t60bkturn
