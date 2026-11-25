/* d_a_player_atnentry_t60.h (WWHD, true 60 build; 2026-11-25). Decomp build
 * only. Separate file: d_a_player_main_04b.cpp keeps a thin call.
 *
 * procAtnActorWait turns (setSpeedAndAngleAtnActor), then checkNextMode may enter a procedure whose init reads the
 * angles: ATN_ACTOR_MOVE's setBlendAtnMoveAnime(2.4) picks the strafe side (mDirection: the sign of
 * sin(current.angle.y - shape_angle.y)) and its animations from them. A's update turns the frame's whole share before
 * the check; at N steps the first step's check came after its dt share, and the atnturn catch-up
 * (d_a_player_atnturn_t60.h) redid the turn after the init. long2 seg 15 (A 7509): the init picked the other side; on
 * the next frame procAtnActorMove's own setBlendAtnMoveAnime(-1) saw the side change and re-inited the old-frame morph
 * (counter 2.4 -> 1.4 where A was at 0.4): B's pose at 7510 lagged A's, the foot-travel base foot flipped (A 1 / B 0),
 * speedF 9.06 / 9.26 (0.195), 2.57 at the damage of 7819. (A redo of the init's set after the catch-up broke other
 * entries: the second set read the first one's state, long2 frame 182.)
 * Form: on the frame's first step (the atnturn record of this step) the check runs at A's angles: the frame's whole
 * turn from the record (at dt 1, as the catch-up computes it) before checkNextMode; an entry keeps them (the catch-up
 * is not needed), no entry puts the step's own angles back. NC WWHD_T60_ATNENTRYDIR_NEGCTRL=1. dt 1: never. */
#pragma once
#include "true60.h"
#include "d/actor/d_a_player_atnturn_t60.h"
#include <cstdlib>

static inline bool lkT60_atnEntryDirNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_ATNENTRYDIR_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
/* this step's atnturn record (first step, not consumed), for the pre-check whole turn; nullptr: no form */
static inline const LkT60AtnTurn* lkT60_atnEntryPre() {
    if (lkT60_atnEntryDirNc() || !lkT60_atnTurnRecordStep() || !s_lkT60AtnTurn.valid ||
        s_lkT60AtnTurn.stamp != t60_step_counter())
        return nullptr;
    return &s_lkT60AtnTurn;
}
