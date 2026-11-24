/* d_a_player_grabthrow_t60.h (WWHD, true 60 build, 2026-11-24): procGrabThrow's
 * next-mode threshold (d_a_player_main_07.cpp) on the frame-end frame. Decomp build only.
 * Separate file: the Link source keeps a thin call.
 *
 * procGrabThrow ends the throw with `frameCtrl.getFrame() > 7.0f` -> checkNextMode(1): A's once-a-frame test on the
 * frame-end frame (A plays the animation before the dispatch). At N steps the raw per-step frame crossed 7 on a LATER
 * step of the frame (a bomb-throw pair at N 4: frame 93's steps 6.8 / 7.0 / ...: the threshold on step 2), so
 * checkNextMode's WAIT was entered on a later step, which the procedure hold (s_lkT60ProcHold: bodies entered on the
 * frame's FIRST step) does not hold: step 3 ran procWait and entered MOVE in the same frame (B: WAIT 93 -> MOVE 93;
 * A: WAIT 93 -> MOVE 94; link-motion 8.9 at 96, link-procs FAIL; the NC run showed the same: Link's, not the bomb's).
 * At N 2 the first step's play happened to reach 7.0 + rounding. Form: the frame-end frame threshold (lkT60_endFrame: the
 * frame-end frame of the projection on every step with a first-step record, the per-step frame without one), as the
 * orca pair's next-mode threshold. NC WWHD_T60_GRABTHROW_NEGCTRL=1: the per-step frame. dt 1: getFrame() (the original). */
#pragma once
#include "d/actor/d_a_player_anm_t60.h"
#include <cstdlib>

static inline bool lkT60_grabThrowNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_GRABTHROW_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the frame procGrabThrow's next-mode threshold reads */
static inline f32 lkT60_grabThrowFrame(J3DFrameCtrl* fc) {
    return lkT60_grabThrowNc() ? fc->getFrame() : lkT60_endFrame(fc);
}
