/* d_a_kantera_t60.h (WWHD, true 60 build; 2026-11-18): the lantern
 * (d_a_kantera.cpp) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_kantera.cpp keeps thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads; as coming2 / stone2): the lantern's frame
 * decides and moves from its holder's state: carried (state 1) it takes the holder's hand position and facing, swings
 * by its flame's step, and leaves the hand when the holder's lantern state says dropped (1) or thrown (2, aimed at
 * Link); falling (state 5) it breaks on a ground / wall hit or an AT hit from the tick30 collision Move; broken (state
 * 10) it grows its fire sphere (registered for the collision Move, one-sweep-per-frame attack) and counts its break timer. Its own frame holds
 * per-frame counters (mBonRot, mAnimCounter, the moths' target timers), random draws (drop speed, moth targets) and the
 * joint relax chases. So the whole daKantera_Execute runs once per 30 Hz frame on the tick30 step at dt 1 from the
 * frame-start state (d/t60_whole.h): every decision, timer, draw, move and the effect / sound starts once per frame,
 * as the original. The earlier steps change no logic state and show the lantern moving on by the previous frame's
 * change of its position, flame point (mBonPos: the draw places the flame there) and facing / tumble angles times the
 * frame's dt so far (the execute's model step: base matrix and joint calc; visual; the tick30 run sets them again);
 * none while it waits for its switch or once broken (no model step in those frames). dt 1: the original. Negative
 * control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_whole.h"
#include <cstdlib>

namespace kantera_t60 {
static inline void get(kantera_class* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = a->mBonPos.x, v[4] = a->mBonPos.y, v[5] = a->mBonPos.z;
    v[6] = (f32)(s16)a->current.angle.y, v[7] = (f32)(s16)a->current.angle.x;
}
static inline void set(kantera_class* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->mBonPos.x = v[3], a->mBonPos.y = v[4], a->mBonPos.z = v[5];
    a->current.angle.y = t60w::angle(v[6]), a->current.angle.x = t60w::angle(v[7]);
}
/* the execute's model step (daKantera_Execute's tail before the flame point): base matrix, joint calc */
static inline void mtx(kantera_class* a) {
    MtxTrans(a->current.pos.x, a->current.pos.y, a->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), a->current.angle.y);
    cMtx_XrotM(calc_mtx(), a->current.angle.x);
    MtxTrans(0.0f, a->mOffsY, 0.0f, 1);
    J3DModel* model = a->mpModel1;
    J3DModel_setBaseScale(model, &a->scale);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    J3DModel_calc(model);
}
/* held: a lantern its holder places from outside (mParam0 1: the Poe's / Jalhalla's lantern, state 3: no move of its
 * own; the holder's kantera_calc writes current.pos / angle.y on the holder's run). Not held, the frame-start restore
 * on the tick30 step put back the position the first step found and wiped the holder's write of this frame (Poe before
 * the lantern in the line: the lantern never left its create place, pair from frame 73, sph 148 off in z).
 * State 1 (carried by Link, mParam0 0x23) follows the hand in its own frame: not held. */
static inline bool held(kantera_class* a) { return a->mParam0 == 1; }
static const t60w::Ops<kantera_class> kOps = {8, 6, get, set, mtx, held};
/* A held lantern (2026-11-24): a held lantern's earlier steps are picture steps. Its
 * holders (the Poe, d_a_pw: whole form, d/t60_whole.h) place it only in their tick30 run (kantera_calc), and their own
 * earlier steps are picture steps (frame interpolation): the port draws the Poe between its frame ends. The held lantern's earlier
 * steps were kept in its draw history as steps of their own ("its holder places it on every step"), with the matrices
 * of the previous frame end: drawn at N 2 it moved only on every other pass while the Poe's hand moved on every pass
 * (stair sweep G1, ghost ship: 3.23 / 0 / 3.23 / 0 units, the hand 1.6 / 1.6). Told as picture steps, the lantern is
 * drawn between its frame ends as its holder (no logic, host only). dt 1: never (no earlier steps). Negative control
 * WWHD_T60_KANTERA_HELDPIC_NEGCTRL=1: as before (no picture step). */
static inline bool heldpicNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_KANTERA_HELDPIC_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* daKantera_Execute's earlier-step return (the Guard skipped) */
static inline void skipStep(kantera_class* a) {
    if (held(a) && !heldpicNc())
        t60w_picture_step(gabi::ea(a));
}
}  // namespace kantera_t60
