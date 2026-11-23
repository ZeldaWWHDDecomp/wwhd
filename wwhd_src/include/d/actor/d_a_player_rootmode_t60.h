#pragma once
/* true60 (2026-11-23; d_a_player_main_04.cpp execute). Separate file:
 * execute keeps two thin calls.
 *
 * execute resets mRootMoveMode to 0 before the procedure (modes 8 / 0xC excepted) and the procedure's body sets it again
 * every frame (procCutL / the cut bodies: 1, root held in x / z by jointBeforeCB). A procedure entered on the frame's
 * first step has its body held over the frame's later steps (rule 1, 2026-11-17, s_lkT60ProcHold), so such a step
 * ran posMove and mpCLModel->calc in mode 0: the root not held, the animation's root translation in the pose. setCollision
 * then put mCyl's centre (root / neck joints) where the pose's root was: pairmo-at frame 61 (procCutL entered on step 1 of
 * 2, CUT_L's lunge), step 2's centre 11.3 units ahead of Link (step 1 and A: 1.1), and that step is the tick30 step, whose
 * collision Move is the only one that lists an unconverted actor: the Moblin's co overlap 22.4 instead of A's 15.3, its
 * push 2.24 instead of 1.53 and Link's next-frame push ~6.5 longer (Link 15.5 vs 9.4, Moblin 1.66 vs 1.04 at frame 62).
 * Form: the mode the reset cleared is noted; a held step (the body skipped) puts it back, as if the held body had set it
 * again, before posMove / the model calc / setCollision. dt 1 (N = 1): nothing is ever held, never consulted beyond the
 * note. Negative control WWHD_T60_ROOTMODE_NEGCTRL=1 (game build): the held step keeps mode 0. */
#include <cstdlib>

/* host state (Link's execute is not re-entered): the mode the reset cleared on this step, 0xFF = none */
static unsigned char s_lkT60RootModeCleared = 0xFF;

/* the reset: m = mRootMoveMode before it, cleared = the reset set 0 */
static inline void lkT60RootMode_noteReset(unsigned char m, bool cleared) { s_lkT60RootModeCleared = cleared ? m : 0xFF; }

/* the held step (body skipped): the cleared mode back */
template <class M> static inline void lkT60RootMode_held(M& mode) {
    static const bool negctrl = [] {
        const char* e = getenv("WWHD_T60_ROOTMODE_NEGCTRL");
        return e && e[0] == '1';
    }();
    const unsigned char m = s_lkT60RootModeCleared;
    s_lkT60RootModeCleared = 0xFF;
    if (negctrl || m == 0xFF || (unsigned char)mode != 0)
        return;
    mode = m;
}
