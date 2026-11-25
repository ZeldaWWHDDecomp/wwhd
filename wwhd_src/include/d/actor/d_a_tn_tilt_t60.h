/* d_a_tn_tilt_t60.h (WWHD, true 60 build, t60-pgp 2026-11-25): the Darknut's ground tilt once
 * per 30 Hz frame, the twin of (d_a_bk_tilt_t60.h) and frame 72 (d_a_mo2_tilt_t60.h). Decomp build
 * only. Separate file: the source keeps thin calls.
 *
 * d_a_tn.cpp updateGroundTilt eases mDamageReaction.mSlopeTilt toward the ground's slope (probes around the body while it
 * is knocked down, mFallPitch != 0; else toward 0) while mAcch has a ground hit. The original eases once per frame from the
 * frame-end position on frames that end on the ground; per step the steps' own hits and positions eased on other steps.
 * Form: the function runs on the frame's tick30 step with cLib_addCalcAngleS2's one-frame arithmetic; earlier steps keep
 * the frame-start tilt. dt = 1: the original. Negative control WWHD_T60_TNTILT_NEGCTRL=1: every step. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace t60tntilt {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_TNTILT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
inline bool skip() { return !nc() && t60::dt() != 1.0f && !t60::tick30(); }
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
}  // namespace t60tntilt
