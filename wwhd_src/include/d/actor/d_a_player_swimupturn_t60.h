/* d_a_player_swimupturn_t60.h (WWHD, true 60 build; 2026-11-25). Decomp game build only,
 *. Header only; the note is an inline variable (one copy across the
 * d_a_player_main units: setSpeedAndAngleSwim in main_07 notes, procSwimUp_init in main_03 reads).
 *
 * SWIM_MOVE's setSpeedAndAngleSwim turns shape_angle.y toward the stick target and current.angle.y after it
 * (cLib_addCalcAngleS: the earlier steps take a share, the tick30 step ends on the original's one step from the frame
 * start). SWIM_MOVE's body then enters SWIM_UP (changeSwimUpProc) when Link surfaces; SWIM_UP does not turn.
 * In A the frame's one turn is whole before the change. At N steps a change on an earlier step leaves both angles at
 * that step's share and nothing finishes the turn: oq  at N 4 on intgc9, frame 253 (SWIM_UP 0x35): angle A
 * 25476 / B 25576 (the first step's 300 of the frame's 400); the stroke's earlier steps moved along the whole turn
 * but the tick30 step's speed took the stuck angle: speed xz A (11.589, -13.773) / B (11.451,
 * -13.888), Link 7.97 off by 357. At N 2 the first step's share reached the turn's end (strict yes). Form: the free swim
 * turn's first step notes each angle's frame start and the original's one turn from it (the d_a_player_movecos_t60.h
 * value function); SWIM_UP entered on an earlier step of that frame sets an angle still on its turn's segment to the
 * whole turn (A's frame end). The tick30 step (eases already whole), dt 1: nothing. NC WWHD_T60_SWIMUPTURN_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

struct LkT60SwimUpTurn {
    uint32_t stamp;
    s16 shapeStart, shapeWhole, curStart, curWhole;
    bool valid;
};
inline LkT60SwimUpTurn g_lkT60SwimUpTurn;

static inline bool lkT60_swimUpTurnNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SWIMUPTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the note step: the frame's first step at N > 1 (the caller reads the frame-start angles only then) */
static inline bool lkT60_swimUpTurnStep() { return t60::dt() != 1.0f && t60::steps() > 1 && t60::first_step() && !lkT60_swimUpTurnNc(); }
static inline void lkT60_swimUpTurnNote(s16 shapeStart, s16 shapeWhole, s16 curStart, s16 curWhole) {
    if (!lkT60_swimUpTurnStep())
        return;
    g_lkT60SwimUpTurn = LkT60SwimUpTurn{t60_step_counter(), shapeStart, shapeWhole, curStart, curWhole, true};
}
/* an angle still on its turn's segment (frame start .. whole turn): the whole turn */
static inline s16 lkT60_swimUpTurnSeg(s16 angle, s16 start, s16 whole) {
    const s32 S = (s16)(whole - start), o = (s16)(angle - start);
    if (S == 0)
        return angle;
    return (S > 0 ? (o >= 0 && o <= S) : (o <= 0 && o >= S)) ? whole : angle;
}
/* procSwimUp_init on an earlier step of a frame whose first step noted the swim turn */
template <class A>
static inline void lkT60_swimUpTurnFinish(A* shape, A* cur) {
    const LkT60SwimUpTurn& n = g_lkT60SwimUpTurn;
    if (t60::dt() == 1.0f || t60::steps() <= 1 || t60::tick30() || lkT60_swimUpTurnNc() || !n.valid ||
        t60_step_counter() - n.stamp >= (uint32_t)t60::steps())
        return;
    *shape = lkT60_swimUpTurnSeg((s16)*shape, n.shapeStart, n.shapeWhole);
    *cur = lkT60_swimUpTurnSeg((s16)*cur, n.curStart, n.curWhole);
}
