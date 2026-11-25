/* d_a_mo2_tilt_t60.h (WWHD, true 60 build; frame 72, 2026-11-24): the Moblin's ground
 * tilt (daMo2_Execute, d_a_mo2_exec.cpp: mDamageReaction.mSlopeTilt x / z eased toward the ground's slope while
 * mAcch has a ground hit) once per 30 Hz frame. Decomp build only. Separate file
 * the source keeps thin calls.
 *
 * The original eases once per frame, from its frame-end position, when that frame ends on the ground. Walking down the
 * stairs (Moblin route on kan6c, moKm, Moblin 47C59A80) the Moblin ends every other frame on the ground (A's tilt x 6,
 * -250, -505, -687 at frames 70 / 72 / 74 / 76); at N > 1 only the tick30 step of those frames hits the ground (the first
 * step is still in the air), and its eases (cLib_addCalcAngleS2 converted: a step's share, the frame's whole value pinned
 * on the tick30 step only when the frame's first step began it) moved half (B -123, -251, -362): the body matrix, the
 * joints and mLeftHandPos (the spear's AT) off from frame 72.
 * Form: the block runs on the frame's tick30 step (the frame-end position and ground hit, as A's) and eases with the
 * original one-frame arithmetic (cLib_addCalcAngleS2's dt = 1 code); the earlier steps keep the frame-start tilt (the
 * picture's tilt changes at the frame end). dt = 1: the original code. Negative control WWHD_T60_MOTILT_NEGCTRL=1
 * (game build): the block on every step with the converted ease. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60mtilt {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_MOTILT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* true: this step skips the ground tilt block */
inline bool skip() { return !nc() && t60::dt() != 1.0f && !t60::tick30(); }
/* the frame's one ease on the tick30 step: cLib_addCalcAngleS2's dt = 1 arithmetic (c_lib.cpp 0200F428). Returns false
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
}  // namespace t60mtilt
