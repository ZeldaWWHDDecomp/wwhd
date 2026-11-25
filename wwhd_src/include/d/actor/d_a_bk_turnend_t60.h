/* d_a_bk_turnend_t60.h (WWHD, true 60 build, 2026-11-25):
 * the attack's turn-target copy reads the frame's attack frame. Decomp build only.
 * Separate file: d_a_bk_fight.cpp keeps thin calls.
 *
 * fight_attack copies mTargetAngleY to dr.mTurnTargetY while mAttackFrame < mTurnEndFrame (or mTurnAlways), after the
 * frame's attack advance, then turns current.angle.y toward it (scale 4, 0x800). A's one call reads the frame-end
 * attack frame. Per step the earlier steps read a partial advance: on the frame whose advance crosses mTurnEndFrame the
 * first step (dt share below the end) still copied, with the first step's sensing (Link after his first step), and the
 * tick30 step no longer did. At N 4, Bokoblin 47C5CE68 frame 61: B turned toward 6247, A kept
 * the previous frame's 6206 (angle 5538 / A 5528 at f61, 5715 / 5697 at f62; the Co cylinder 35 ahead moved with it).
 * Form: every step reads the frame-end attack frame (bk_t60_attack_advance's frameEnd), so the copy runs on every step
 * of a frame A copies on (the tick30 step's sensing ends it, A's target) and on none of a frame A does not. dt = 1: the
 * live value (the original). Negative control WWHD_T60_BKTURNEND_NEGCTRL=1: the live per-step value. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60bkte {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKTURNEND_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the attack frame the turn-target copy reads */
inline f32 frame(f32 live, f32 frameEnd) { return (t60::dt() == 1.0f || nc()) ? live : frameEnd; }
} // namespace t60bkte
