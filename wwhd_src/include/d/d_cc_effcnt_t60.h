/* d_cc_effcnt_t60.h (WWHD, true 60 build, t60-pgp 2026-11-25): the collider's hit-mark
 * repeat guard counts 30 Hz frames. Decomp build only. Separate file:
 *d_cc_d.cpp keeps thin calls.
 *
 * dCcD_GAtTgCoCommonBase::mEffCounter (At / Tg / Co, +0x14) is set to 5 when an At / Tg pair makes a hit mark
 * (dCcS::SetAtTgGObjInf: a new mark only while one side's counter is 0) and counted down by one in every ClrAtHit /
 * ClrTgHit / ClrCoHit (dCcS's Move clears every collider's hit at its start; actors clear their own). dCcS runs its Move
 * on every step, so at N steps the guard ran out N times as fast. Exact-dump pair (ccfe8, N 2), ChuChu de70: the
 * sword hit of frame 286 left the Tg counter 3 for A's 4 from f287 and 0 at f295 (A 2, then 1 at f296), so a hit there
 * makes a new mark (spark, SE) that A does not. Form: the count-down runs on the frame's tick30 step only (the step
 * that ends the frame: its own Move's clear and any actor clear on it, A's per-frame clears). At N = 1 tick30 is always
 * set: the original. Negative control WWHD_T60_EFFCNT_NEGCTRL=1: every step. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60effcnt {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_EFFCNT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* this clear counts the guard down */
inline bool counts() { return t60::tick30() || nc(); }
} // namespace t60effcnt
