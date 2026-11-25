/* d_a_player_moveinit_t60.h (WWHD, true 60 build; 2026-11-25). Decomp build only,
 *. Separate file: main_02 keeps a thin call.
 *
 * setMoveAnime 023E0E50 re-sets the move blend on every update: frame control 0 keeps its phase, frame control 1 (the
 * blend partner) is put at the same phase of its own clip (a continuation, not latched; psword2d). At 30 Hz the update
 * that ENTERS a move procedure (procMove's init from the WAIT body: setBlendMoveAnime -> setMoveAnime) runs after the
 * frame's whole play, so control 1 takes the frame-end phase; the new body first runs in the next update. At N steps
 * that init came on the frame's first step (after 1/N of the play) and the body is held for the frame's later steps
 * (lkT60_procEnteredThisFrame): nothing re-sets the blend there, and control 1 played on at the frame
 * start's held rate (the WAIT blend's): a long route (N 2, frame 5072): control 1 frame A 1.7368 / B 1.3684,
 * Link's foot pose at the frame end off (toe z up to 0.11), the next frame's foot-travel chord 4.2131 / A 4.2624 (the
 * "animation-phase lead" chord residual), Link 0.047 off and off the wall 80 frames later. Form: such a set takes the switch form already
 * there for a set that changes the animation (lkSwitch): the phase projected over the frame's remaining plays
 * (lkT60_projectFrame), both controls latched (first played on the next frame's first step). The frame end is A's.
 * NC WWHD_T60_MOVEINIT_NEGCTRL=1. dt 1 / tick30 / later steps: never (no extra guest read). */
#pragma once
#include "true60.h"
#include "d/actor/d_a_player_t60.h"
#include <cstdlib>

static inline bool lkT60_moveInitNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] {
        const char* e = getenv("WWHD_T60_MOVEINIT_NEGCTRL");
        return e && e[0] == '1';
    }();
    return v;
#else
    return false;
#endif
}
/* (2026-11-25): a procedure entered on the frame's first step BEFORE the dispatch's
 * window (lkT60_procOpen): the execute's pre-dispatch checks (checkItemAction: the boomerang throw's procWait_init from
 * BOOMERANG_SUBJECT) run after the frame's animation play in A as well, so their setMoveAnime takes the frame-end phase;
 * at N steps it took the first step's (the boomerang route at N 2, frame 3635: mFrameCtrlUnder[1] A 29.159 / B 29.051, f31 A
 * 0.88361 / B 0.87459, three frames off). Such an entry is not a held body (A runs its body in the same frame's dispatch),
 * so it only takes MOVEINIT-PHASE's set form (projected phase, latched). Key: Link's mCurProc EA + 2, frame-tagged.
 * NC WWHD_T60_MOVEINITPRE_NEGCTRL=1. dt 1: never. */
static inline bool lkT60_moveInitPreNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] {
        const char* e = getenv("WWHD_T60_MOVEINITPRE_NEGCTRL");
        return e && e[0] == '1';
    }();
    return v;
#else
    return false;
#endif
}
static inline uint32_t lkT60_procPreKey(uint32_t curProcEa) { return curProcEa + 2; }
/* commonProcInit: a first-step entry before the dispatch's window */
static inline void lkT60_procEnteredPre(uint32_t curProcEa) {
    if (!lkT60_procStep() || lkT60_moveInitPreNc() || lkT60_moveInitNc() ||
        wwhd_t60_latch_pending(lkT60_procOpenKey(curProcEa), lkT60_frameTag()))
        return;
    wwhd_t60_latch_set(lkT60_procPreKey(curProcEa), lkT60_frameTag());
}
/* a setMoveAnime from a procedure entered on this (first, not tick30) step: its body is held for the rest of the frame
 * (or, MOVEINIT-PRE, entered before the dispatch: not held, the set form only) */
static inline bool lkT60_moveInitHold(uint32_t curProcEa) {
    return t60::dt() != 1.0f && !t60::tick30() && !lkT60_moveInitNc() &&
           (lkT60_procEnteredThisFrame(curProcEa) ||
            (lkT60_procStep() && !lkT60_moveInitPreNc() &&
             wwhd_t60_latch_pending(lkT60_procPreKey(curProcEa), lkT60_frameTag()) != 0));
}

/* (2026-11-25, a long route, walk start frame 5450 -> 5813): the original's phase
 * f31 reads the move animation's own frame (r3->getFrame()), which animeUpdate 023D9F6C left at the frame of the LAST
 * frame control that played it: with the wait blend both under slots hold the same animation, so control 1 (rate
 * (rate x f26) x f30, 2.3000002 where control 0 has 2.3) writes it last. The switch form projected control 0 instead:
 * A 1.0592308 + 2.3000002 = 3.3592310 (4056FDA4), B 1.0592308 + 2.3 = 3.3592308 (...A3), 1 ulp, the 1-2 ulp drift of
 * the under controls from there on, the base-foot switch at 5504 (speedF 2.3767 / 2.3765, Link 0.001 off) and the wall
 * circle 0 contact edge at 5812 (A off the wall, B on it: A's release speed at 5813). Form: the projection takes the
 * control that writes the animation last in animeUpdate's order (under 0, under 1 when it plays the same animation, upper
 * 1 when it plays it and not under 1's). NC WWHD_T60_MOVEANMFRAME_NEGCTRL=1. dt 1: not run (the switch form is not). */
static inline bool lkT60_moveAnmFrameNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] {
        const char* e = getenv("WWHD_T60_MOVEANMFRAME_NEGCTRL");
        return e && e[0] == '1';
    }();
    return v;
#else
    return false;
#endif
}
/* the frame control whose frame animeUpdate writes last into animation r3 (under slot 0's animation) */
static inline J3DFrameCtrl* lkT60_moveAnmWriter(uint32_t r3, J3DFrameCtrl* under0, J3DFrameCtrl* under1, J3DFrameCtrl* upper1,
                                                uint32_t under1Anm, uint32_t upper1Anm) {
    if (lkT60_moveAnmFrameNc())
        return under0;
    J3DFrameCtrl* w = under0;
    if (under1Anm != 0 && under1Anm == r3)
        w = under1;
    if (upper1Anm != 0 && upper1Anm == r3 && upper1Anm != under1Anm)
        w = upper1;
    return w;
}
