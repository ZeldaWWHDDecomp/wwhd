/* c_damagereaction_push_t60.h (WWHD, true 60 build, 2026-11-24).
 * Decomp build only. Separate file:
 * c_damagereaction.cpp keeps thin calls.
 *
 * t60dr_push_step (the first-step push rule) holds the collision push m_cc_move the frame's first step sees: the tick30 Move's of the
 * frame before, i.e. A's push for this frame. Its second clause also took a push first seen on a later step of a frame
 * whose first step saw none. dCcS runs its Move on every step; a later step's value is an in-between Move's, which pairs
 * only converted colliders (another converted enemy, Link) at in-between positions. A meets that overlap only in its
 * frame-end Move and moves by it in the next frame. A Moblin pair at N 4, frame 170: two Moblins walking towards each other
 * (about 170 apart, their bodies just touching) took each other's push from step 2, equal and opposite (-0.174 / -0.197
 * per step), one frame before A. Form: only the first step's value is held (a later step of the frame adds no push);
 * a call first reached on a later step still takes the live value, as before. Negative control
 * WWHD_T60_DRPUSH_LATE_NEGCTRL=1: the form before. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60drPush {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRPUSH_LATE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* a push first seen on a later step of the frame (none held from its first step) is taken: the form before only */
static inline bool takeLate() { return nc(); }
} // namespace t60drPush
