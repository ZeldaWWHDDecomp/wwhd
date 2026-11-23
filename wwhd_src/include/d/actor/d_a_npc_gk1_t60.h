/* d_a_npc_gk1_t60.h (WWHD, true 60 build, 2026-11-18): Gk1 (Windfall) (d_a_npc_gk1*.cpp,
 * daNpc_Gk1_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule NPC form: the NPC's frame reads Link (attention / talk start), the tick30
 * message status and the event manager's order window, and changes mode within the frame. So the whole _execute runs
 * once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h). The earlier steps change
 * no logic state and show the NPC moved on by the previous frame's change (position, model angle mAngle.y) times the frame's dt so far:
 * setMtx's matrix part (base matrix, skeleton calc, item and hat models on the head joint) without setAttention (no animation play, no ground check, no attention / eye position: those are logic). Picture of the skeleton
 * between frame ends: frame interpolation. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on
 * every step at dt. */
#pragma once
#include "d/t60_whole.h"

namespace gk1_t60 {
/* rule 14 (draw-side state once per logic step; 2026-11-18, as the t60npc::drawStep): the head joint
 * callback stores logic state (attention base / eye position, head position) when the model is calculated in the draw. As a converted
 * process the NPC is drawn on every step and those draws are not put back, so the stores are made on the tick30 step's
 * draw only (the frame's own draw, as at 30 Hz). At N 1 and in the harness every step is a tick30 step. */
static inline bool drawStep() { return t60::tick30(); }
static inline void get(daNpc_Gk1_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->mAngle.y;
}
static inline void set(daNpc_Gk1_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->mAngle.y = t60w::angle(v[3]);
}
/* the picture: setMtx's matrix part (base matrix, skeleton calc, item and hat models on the head joint) without setAttention */
static inline void mtx(daNpc_Gk1_c* a) {
    J3DModel_setBaseScale(a->mpMorf->getModel(), &a->scale);
    mDoMtx_stack_c::transS(a->current.pos.x, a->current.pos.y, a->current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), a->mAngle.x, a->mAngle.y, a->mAngle.z);
    J3DModel_setBaseTRMtx(a->mpMorf->getModel(), mDoMtx_stack_c::get());
    t60w::morfPictureCalc(a->mpMorf.get()); /* at dt 1, the kept pose put back */
    if (a->mpItemModel.get()) {
        PSMTXCopy(a->mpMorf->getModel()->getAnmMtx(a->m_hed_jnt_num), mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(a->mpItemModel, mDoMtx_stack_c::get());
        J3DModel_calc(a->mpItemModel);
    }
    if (a->mpHatModel.get()) {
        PSMTXCopy(a->mpMorf->getModel()->getAnmMtx(a->m_hed_jnt_num), mDoMtx_stack_c::get());
        J3DModel_setBaseTRMtx(a->mpHatModel, mDoMtx_stack_c::get());
        J3DModel_calc(a->mpHatModel);
    }
}
static inline bool held(daNpc_Gk1_c*) { return false; }
static const t60w::Ops<daNpc_Gk1_c> kOps = {4, 3, get, set, mtx, held};
}  // namespace gk1_t60
