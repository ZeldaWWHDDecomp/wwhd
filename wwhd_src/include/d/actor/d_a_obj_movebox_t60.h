/* d_a_obj_movebox_t60.h (WWHD, true 60 build; 2026-11-18): the push box
 * (d_a_obj_movebox.cpp: crates, black boxes, metal / spring boxes, mirror, mossy cube) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * d_a_obj_movebox.cpp keeps thin calls.
 *
 * the whole-frame form rule + addendum (cross-actor coupling): Link pushes / pulls the box (PPCallBack counts
 * his push frames in mMomentCnt, check_to_walk starts the walk at the threshold and sets his PUSH_PULL_KEEP flag; he
 * follows the box's position in setPushPullKeepData) and rides it (RideCallBack sums his sink / tilt into
 * mRideSinkAccel / mSwayTarget, which mode_afl / afl_sway use up once per frame); the box moves through the shared mover
 * (daObj_posMoveF_stream), its afloat spring (afl_sway), one cM_rnd bob draw a frame, the walk cosine timer, the land /
 * water / break sounds and effects, the hit-to-break check after the collision Move and its cylinder registration (one-sweep-per-frame attack).
 * So the whole Execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state, as the original.
 *
 * Collision: the earlier steps leave mMtx (the moving background's matrix, Execute's output) at the frame's
 * value, so dBgS_MoveBgActor::MoveBGExecute moves the background by nothing there (cBgW::Move with an unchanged matrix:
 * old = base, translation 0) and by the whole frame's move on the tick30 step: a rider's MoveBgCrrPos gets the frame's
 * move in one piece, as at 30 Hz (Link takes it on his next step). The box's logic state (current.pos, which Link reads
 * while pushing) also moves once a frame. The two callbacks Link calls on each of his steps count once a frame, on the
 * tick30 step (Link's tick30 step and the box's whole run fall in the same step, so the box reads them in the same
 * frame as at 30 Hz for either process order): PPCallBack (push frames, label, hit flag) and RideCallBack (sink / tilt
 * sums).
 *
 * Picture: the earlier steps show the model (mpModel's base matrix only, what the draw reads) moved on from the frame
 * start by the previous frame's change times the frame's dt so far: position, the afloat sway (mSwayX / Z) and sink
 * depth. The actor's memory, mMtx and the child (spring / mirror / Mkie / buoy, set_mtx's tail) stay at the frame's
 * values (the child follows on the tick30 step, as its logic does). No picture when the previous frame had no model
 * step, the box did not run in the frame just before, or the shown position would be more than kJump from the frame
 * start (a path reset). dt 1: never consulted (the original). Host memory per translation unit (the split-actor state rule (1)).
 * Negative control: T60C_NEGCTRL runs Execute and both callbacks on every step at dt. */
#pragma once
#include "d/t60_whole.h"

namespace movebox_t60 {
using daObjMovebox::Act_c;

static const f32 kJump = 200.0f;
static f32 s_pic[6]; /* the shown values of the step (host only: the actor's memory is not touched) */

static inline void get(Act_c* a, f32* v) {
    v[0] = a->current.pos.x, v[1] = a->current.pos.y, v[2] = a->current.pos.z;
    v[3] = a->mSwayX, v[4] = a->mSwayZ, v[5] = a->mSinkDepth;
}
static inline void set(Act_c*, const f32* v) {
    for (int i = 0; i < 6; i++)
        s_pic[i] = v[i];
}
static inline f32 absf(f32 x) { return x < 0.0f ? -x : x; }
/* the picture: set_mtx on the shown values with the child link off, then the actor's values, mMtx and the child link
 * back: only mpModel's base matrix keeps the shown pose */
static inline void mtx(Act_c* a) {
    if (a->mpModel.get() == nullptr)
        return;
    const f32 x = a->current.pos.x, y = a->current.pos.y, z = a->current.pos.z;
    if (absf(s_pic[0] - x) > kJump || absf(s_pic[1] - y) > kJump || absf(s_pic[2] - z) > kJump)
        return;
    const f32 sx = a->mSwayX, sz = a->mSwayZ, sink = a->mSinkDepth;
    const u32 child = a->mChildPID;
    f32 keep[12]; /* mMtx (guest fields: copied as values) */
    for (int i = 0; i < 12; i++)
        keep[i] = a->mMtx.m[i / 4][i % 4];
    a->current.pos.x = s_pic[0], a->current.pos.y = s_pic[1], a->current.pos.z = s_pic[2];
    a->mSwayX = s_pic[3], a->mSwayZ = s_pic[4], a->mSinkDepth = s_pic[5];
    a->mChildPID = 0xFFFFFFFF;
    a->set_mtx();
    a->mChildPID = child;
    for (int i = 0; i < 12; i++)
        a->mMtx.m[i / 4][i % 4] = keep[i];
    a->current.pos.x = x, a->current.pos.y = y, a->current.pos.z = z;
    a->mSwayX = sx, a->mSwayZ = sz, a->mSinkDepth = sink;
}
static inline bool held(Act_c*) { return false; }
static const t60w::Ops<Act_c> kOps = {6, 6, get, set, mtx, held};

/* PPCallBack / RideCallBack: true when this call is on a step before the frame's tick30 step (Link calls them on each
 * of his steps): it counts nothing, the tick30 step's call counts the frame. dt 1: false (the original). */
static inline bool cbSkip() { return t60w::on() && !t60::tick30(); }
}  // namespace movebox_t60
