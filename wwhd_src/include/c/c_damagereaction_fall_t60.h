/* c_damagereaction_fall_t60.h (WWHD, true 60 build, 2026-11-24): the damage reaction's
 * fall term on a downhill slope. Decomp build only. Separate file:
 * c_damagereaction.cpp keeps thin calls.
 *
 * The reaction moves the enemy y += speed.y / 4 a frame (speed.y -1 while it stands: -0.25), then its body ground check
 * (CrrPos) puts it onto the ground. Walking down a slope A drops the frame's 0.25, lands below the ground and is put
 * back on it: on the ground, speed.y -1 again. Per step (t60dr_fall_step: the frame's start speed x dt) the earlier
 * step's ground check already lifted the enemy back onto the slope; the tick30 step's share then fell short of the
 * ground the frame had walked down to, and the frame ended in the air (pair, Bokoblin 47C5E81C frame 267: A y
 * 29.772 on the ground, B 29.798 in the air with speed.y -7; the next frames alternated, the hit at 311 then knocked it
 * another way). The tick30 step now moves to the frame start's height plus the frame's term (A's place before its one
 * ground check), so the earlier steps' ground lifts do not shorten it. Not after a speed set between the steps (a launch:
 * the frame's term is the new speed's, t60dr_fall_step) or a move of more than kJump between the steps (put somewhere).
 * Negative control WWHD_T60_DRFALL_NEGCTRL=1: the form before. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60drFall {
static const f32 kJump = 50.0f;
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRFALL_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the tick30 step's fall: to the frame start's height y0 + the frame's term; `share` is the form before (the frame's term
 * minus the earlier steps' shares), `y` the height now */
static inline f32 tick30(f32 y0, f32 frameTerm, f32 y, f32 share, f32 sumMoved) {
    if (nc())
        return share;
    const f32 expected = y0 + sumMoved; /* where the earlier steps' shares alone left it */
    if (!(__builtin_fabsf(y - expected) <= kJump))
        return share;
    return (y0 + frameTerm) - y;
}
}  // namespace t60drFall
