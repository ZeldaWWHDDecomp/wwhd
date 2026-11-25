/* d_a_player_ladder_t60.h (WWHD, true 60 build, 2026-11-25). Decomp build only,
 *. Separate file: d_a_player_main_06.cpp keeps thin calls.
 *
 * changeLadderMoveProc eases Link onto the ladder's hold point m370C (cLib_addCalc x / y / z: 0.5 of the gap, at most 0.5,
 * at least 0.05) and then, with the stick held, enters the next ladder procedure (LADDER_MOVE / UP_END / DOWN_END, or
 * falls). A's one call per frame eases the whole frame's share. At N steps the call ran on the frame's first step with
 * the step's dt share and entered the procedure there; the frame's later steps are held (lkT60_procOpen),
 * so the frame kept only the first step's share (Outset lookout ladder, route ld, N 2, frame 85: LADDER_UP_START ->
 * LADDER_MOVE; y 187.792 for A's 187.707, the follow camera and the climb then 0.085 off).
 * Form: the callers note Link's position and the hold point before the call; when the call entered a procedure on the
 * frame's first step (lkT60_procEnteredThisFrame: the rest of the frame is held), the eases are redone from the noted
 * position at dt 1 (the frame's whole share, A's call), toward the noted hold point (the entered procedure's init moves
 * m370C). A call that stays in its procedure keeps the per-step eases. NC WWHD_T60_LADDEREASE_NEGCTRL=1. dt 1: never. */
#pragma once
#include "true60.h"
#include "d/actor/d_a_player_t60.h"
#include <cstdlib>

static inline bool lkT60_ladderEaseNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_LADDEREASE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct LkT60LadderEase {
    f32 p[3], t[3];
    bool on;
};
/* the note of the call in flight (one call at a time; no local object in the glued unit: gabi_construct) */
inline LkT60LadderEase g_lkT60LadderEase;
template <class L> static inline void lkT60_ladderEaseNote(const L* lk) {
    LkT60LadderEase& e = g_lkT60LadderEase;
    e.on = false;
    if (!lkT60_procStep() || lkT60_ladderEaseNegctrl())
        return;
    e.p[0] = lk->current.pos.x, e.p[1] = lk->current.pos.y, e.p[2] = lk->current.pos.z;
    e.t[0] = lk->m370C.x, e.t[1] = lk->m370C.y, e.t[2] = lk->m370C.z;
    e.on = true;
}
template <class L> static inline void lkT60_ladderEaseRedo(L* lk) {
    LkT60LadderEase& e = g_lkT60LadderEase;
    const bool on = e.on;
    e.on = false;
    if (!on || !lkT60_procEnteredThisFrame(gabi::ea(&lk->mCurProc)))
        return;
    lk->current.pos.x = e.p[0], lk->current.pos.y = e.p[1], lk->current.pos.z = e.p[2];
    const f32 previousDt = t60_set_dt(1.0f);
    cLib_addCalc(&lk->current.pos.x, e.t[0], 0.5f, 0.5f, 0.05f);
    cLib_addCalc(&lk->current.pos.y, e.t[1], 0.5f, 0.5f, 0.05f);
    cLib_addCalc(&lk->current.pos.z, e.t[2], 0.5f, 0.5f, 0.05f);
    t60_set_dt(previousDt);
}
