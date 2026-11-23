/* d_a_npc_gp1_t60.h (WWHD, true 60 build, 2026-11-18): Gp1 (Windfall) (d_a_npc_gp1*.cpp,
 * daNpc_Gp1_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule NPC form: the NPC's frame reads Link (attention / talk start), the tick30
 * message status and the event manager's order window, and changes mode within the frame. So the whole _execute runs
 * once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h). The earlier steps change
 * no logic state and show the NPC moved on by the previous frame's change (position, model angle mAngle.y) times the frame's dt so far:
 * setMtx's matrix part (base matrix, skeleton calc) without its morf play, ground check, room / colour or setAttention (no animation play, no ground check, no attention / eye position: those are logic). Picture of the skeleton
 * between frame ends: frame interpolation. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on
 * every step at dt. */
#pragma once
#include "d/t60_whole.h"
#include <cstring>

namespace gp1_t60 {
/* rule 14 (draw-side state once per logic step; 2026-11-18, as the t60npc::drawStep): the head joint
 * callback stores logic state (attention base / eye position) when the model is calculated in the draw. As a converted
 * process the NPC is drawn on every step and those draws are not put back, so the stores are made on the tick30 step's
 * draw only (the frame's own draw, as at 30 Hz). At N 1 and in the harness every step is a tick30 step. */
static inline bool drawStep() { return t60::tick30(); }
static inline void get(daNpc_Gp1_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = (f32)(s16)a->mAngle.y;
}
static inline void set(daNpc_Gp1_c* a, const f32* v) {
    a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
    a->mAngle.y = t60w::angle(v[3]);
}
/* the morf's kept state (pairs / gprP; the process-order read rule, readers get the frame's value): a
 * picture calc leaves what the next logic calc reads as it found it.
 *   - the kept pose: mDoExt_McaMorf::calc blends the animation into the pose it keeps (mpTransformInfo 0x20 / joint,
 *     mpQuat 0x10 / joint) and keeps the result; the picture's calc puts it back (host copy; fix 1);
 *   - the morf blend's frame snapshot: the picture's calc at the step's dt (< 1) took the path
 *     (m_Do_ext_morf_t60.h morf_t60_snap) and left a snapshot of this frame with the frame-start mPrevMorf; the logic's
 *     calc (setMtx, dt 1 in the whole run, after the tick30 play) then took morf_t60_draw_snap and blended with that
 *     older prev: ratio (cur - prev_{k-1}) / (1 - prev_{k-1}), two blend steps a frame (gp1's 8-frame morfs at its
 *     head turns: x (n-2) / n per frame for A's (n-1) / n; probe: morf mCurMorf 0.125 .. 1 in the three windows,
 *     the morf object, its control, the animation objects and the joint matrices byte-exact A vs B). The picture's calc
 *     runs at dt 1: no snapshot, the original code (its blend is put back with the pose).
 * A calcs once a frame (setMtx); the logic's own calc is the original. dt 1 (N 1): never called. */
static inline void calcKeep(mDoExt_McaMorf* m) {
    enum { kJoints = 128 };
    static u8 s_pose[kJoints * 0x20], s_quat[kJoints * 0x10];
    const u32 t = m->mpTransformInfo, q = m->mpQuat;
    const u32 n = (t && q && m->getModel()) ? (u32)m->getModel()->getModelData()->getJointNum() : 0;
    const bool keep = n && n <= kJoints;
    if (keep) {
        memcpy(s_pose, gabi::at<u8>(t), n * 0x20);
        memcpy(s_quat, gabi::at<u8>(q), n * 0x10);
    }
    const f32 dt = t60_set_dt(1.0f);
    m->calc();
    t60_set_dt(dt);
    if (keep) {
        memcpy(gabi::at<u8>(t), s_pose, n * 0x20);
        memcpy(gabi::at<u8>(q), s_quat, n * 0x10);
    }
}
/* the picture: setMtx's matrix part (base matrix, skeleton calc) without its morf play, ground check, room / colour or setAttention */
static inline void mtx(daNpc_Gp1_c* a) {
    mDoMtx_stack_c::transS(a->current.pos.x, a->current.pos.y, a->current.pos.z);
    mDoMtx_stack_c::YrotM(a->mAngle.y);
    J3DModel_setBaseTRMtx(a->mpMorf->getModel(), mDoMtx_stack_c::get());
    calcKeep(a->mpMorf.get());
}
static inline bool held(daNpc_Gp1_c*) { return false; }
static const t60w::Ops<daNpc_Gp1_c> kOps = {4, 3, get, set, mtx, held};
}  // namespace gp1_t60
