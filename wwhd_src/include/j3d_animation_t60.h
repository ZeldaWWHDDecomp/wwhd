/* j3d_animation_t60.h (WWHD, true 60 build, 2026-11-24): the frame-start form of
 * J3DFrameCtrl::update (J3DAnimation.cpp j3d_t60::advance) aligned to the 30 Hz frame. Decomp build only.
 * Separate file (and no getenv static in the glued unit).
 *
 * The form shows frameStart + rate x (k x dt) on a control's k-th play since its latch began, and keeps the latch while
 * the plays are consecutive and within N steps of the latch's first play. A latch that began on a LATER step of a frame
 * (a restart: a new animation, a rate change, a setFrame) therefore ran across the frame boundary: its window ended on
 * the matching later step of the next frame, which restarted it there again, and so on: from then on every frame of
 * that control was three per-step adds (N 4) instead of A's one add (debug log, the throw route at N 4: Link's
 * mFrameCtrlUnder[0] latched at steps 3530 / 3534 / 3538 ..., the frame's first steps 3533 / 3537 / 3541: the frame-end
 * frame 1 - 4 ulp off A, Link's hands 1 ulp, the carried bomb 1 ulp, pair btU frame 78). Fix: the frame's first step
 * always starts a new latch (a play on the first step never continues one from the frame before), so the frame-start
 * form holds from the first frame the control plays from its first step on.
 * Second cause (the same log after the first fix): Link's setMoveAnime re-sets the running walk / wait blend on every
 * update (a continuation: player02SetFrameCtrl with frame = (anm frame / frame max) x frame max), a float round trip
 * that moves the frame by an ulp now and then; at 30 Hz once a frame after the play, at N steps after every step's play,
 * so the next step's play found frame != the latch's last and restarted (3724 / 3735: last 40C2F041 / 4146AB53, read
 * 40C2F040 / 4146AB54). A frame within 2 ulp of the latch's last (same sign) counts as the latch's own: the step plays
 * from the frame start as before and the tick30 step's round trip of the frame-end value is A's. NC
 * WWHD_T60_FCALIGN_NEGCTRL=1: both off (the window relative to the latch's first play, exact last). dt 1: never
 * consulted. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool j3d_t60_fcAlignNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_FCALIGN_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* a play on the frame's first step starts a new latch */
static inline bool j3d_t60_fcFirstStepRestart() { return t60::first_step() && !j3d_t60_fcAlignNc(); }
/* the frame the control holds is the latch's last play's (exact, or within 2 ulp: a phase round trip of a re-set) */
static inline bool j3d_t60_fcSameFrame(uint32_t last, uint32_t now) {
    if (last == now)
        return true;
    if (j3d_t60_fcAlignNc() || ((last ^ now) & 0x80000000u) != 0)
        return false;
    const uint32_t d = last > now ? last - now : now - last;
    return d <= 2u;
}
