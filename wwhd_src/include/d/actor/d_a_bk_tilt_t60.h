/* d_a_bk_tilt_t60.h (WWHD, true 60 build, t60-pgp 2026-11-25, route Bokoblin CB0C frame
 * 125): the Bokoblin's ground tilt once per 30 Hz frame, the twin of the Moblin's d_a_mo2_tilt_t60.h (frame 72).
 * Decomp build only. Separate file: the source keeps
 * thin calls.
 *
 * daBk_Execute eases dr.mSlopeTilt x / z toward the ground's slope (two ground probes around the body) while dr.mAcch has a
 * ground hit (or action 19). The original eases once per frame, from the frame-end position, on frames that end on the
 * ground. Walking down the slope in  (frames 120-126, the fall term lifting the body every other frame) A eased on
 * every other frame; at N 2 the steps' own hits and positions eased on other steps and from mid-frame places: the tilt the
 * next frame's damage_reaction reads (t60dr_slope_read, the frame-start tilt) differed from frame 125 (joint 12's slope
 * target -822 / -3707 vs A's -790 / -3775; the hand, then the held weapon 1.74 off at 126). Form: the block runs on the
 * frame's tick30 step (the frame-end position and ground hit, as A's) and eases with the original one-frame arithmetic
 * (cLib_addCalcAngleS2's dt = 1 code); the earlier steps keep the frame-start tilt (the picture's tilt changes at the
 * frame end). dt = 1: the original code. Negative control WWHD_T60_BKTILT_NEGCTRL=1: the block on every step. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60bktilt {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKTILT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* true: this step skips the ground tilt block */
inline bool skip() { return !nc() && t60::dt() != 1.0f && !t60::tick30(); }
/* the frame's one ease on the tick30 step: cLib_addCalcAngleS2's dt = 1 arithmetic (as t60mtilt::ease). Returns false
 * where the caller runs cLib_addCalcAngleS2 itself (dt = 1, the negative control). */
template <class P> inline bool ease(P* p, s32 target, s32 scale, s32 maxStep) {
    if (nc() || t60::dt() == 1.0f)
        return false;
    const s32 x = (s16)*p;
    const s32 d = (s16)((u32)target - (u32)x);
    s32 step = (s16)(scale == 0 ? (d < 0 ? -1 : 0) : d / scale);
    if (step > maxStep)
        step = maxStep;
    else if (step < (s32)(0u - (u32)maxStep))
        step = (s32)(0u - (u32)maxStep);
    *p = (s16)((u32)x + (u32)step);
    return true;
}
}  // namespace t60bktilt
