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
#include <unordered_map>

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

/* the patrol's turn target (frame 88, 2026-11-24; pair, Moblin route at N 4 +
 * Moblin 47C5CECC walking toward its path point): mode 1 sets mTurnTargetY = atan2 of the path target from
 * the Moblin's place and eases current.angle.y toward it. A's call takes that place at the frame start (before the frame's
 * move); B's later steps took their mid-frame place: the target drifted by the step's lateral offset, the integer ease
 * turned on steps 3-4 of frame 88 (-1143 -> -1146) a frame before A (frame 89), and the tick30 step's pin moved the frame
 * end on the late target (x 0.0023 off from then on). Form: the turn target's place is the frame's first-step place on
 * every step of the frame (the same frame-start answer as the arrival tests). dt 1: the call's own place (the original).
 * NC WWHD_T60_MOTURNFS_NEGCTRL=1 (game build): each step's own place. Host memory keyed by the actor, d_a_mo2_move only. */
inline bool turnNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_MOTURNFS_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct TurnPlace {
    u32 frame;
    f32 x, z;
};
inline std::unordered_map<u32, TurnPlace>& turnPlaces() {
    static std::unordered_map<u32, TurnPlace> m;
    return m;
}
/* the place the turn target is measured from: *x / *z in, the frame-start place out on the frame's later steps */
inline void turnPlace(u32 actor, f32* x, f32* z) {
    if (t60::dt() == 1.0f || turnNc())
        return;
    const u32 frame = *gabi::at<be<u32>>(0x101FF558);
    TurnPlace& e = turnPlaces()[actor];
    if (t60::first_step()) {
        e.frame = frame, e.x = *x, e.z = *z;
        return;
    }
    if (e.frame == frame)
        *x = e.x, *z = e.z;
}
}  // namespace t60marr
