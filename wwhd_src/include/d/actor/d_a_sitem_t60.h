/* d_a_sitem_t60.h (WWHD, true 60 build; 2026-11-18): the enemy item on a chain
 * (d_a_sitem.cpp) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_sitem.cpp keeps thin calls.
 *
 * the whole-frame form rule (springs frame-exact) with its addendum (cross-actor reads; as mant / kantera):
 * the item hangs on a 10-link chain from the point its holder writes (current.pos / angle, the holder's frame), the
 * links relax towards each other once per frame (control1 / control2: every link reads its neighbours' previous
 * positions, a per-frame solver, not a step form: the t60conv per-site form left ~11% of N-step spec inputs apart), the swing
 * phases run on mCount, the wind push decays, the cut chain falls with speed.y -= 3 a frame and breaks on the ground;
 * the hand / chain / body spheres are registered for the collision Move (one-sweep-per-frame attack) and their hits from it cut the chain,
 * blow it away (Link's facing, fopAcM_searchActorAngleY) or break it (item drop, particles, sound). So the whole
 * daSitem_Execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state: every link, timer,
 * draw, hit and effect once per frame, as the original. The earlier steps change no logic state and show the picture
 * (what daSitem_Draw reads: the two line materials' segment positions, the hand position / angles through
 * hand_mtx_set (the item model's base matrix and mHandCenter), eyePos / attention_info.position) moved on by the
 * previous frame's change times the frame's dt so far (visual); the tick30 step puts the frame-start words back first.
 * No picture when the previous frame did not run here (or the actor is a new process at a freed address) or a value
 * jumped more than kJump in that frame. dt 1: never consulted (the original). Host memory per translation unit (rule
 * the split-actor state rule (1)). Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_py.h" /* the process-order read rule addendum 2 */
#include "d/t60_whole.h"

namespace sitem_t60 {

enum { kLine1 = 10, kLine2 = 5, kF = (kLine1 + kLine2) * 3 + 3 + 3 + 3, kN = kF + 2, kSlots = 16 };
static const f32 kJump = 500.0f;

struct Latch {
    u32 actor, tag, pcid; /* as t60w::Latch */
    u32 w0[kN];           /* frame-start words (floats; the two hand angles as s16 in the low half) */
    f32 dv[kN];           /* the previous frame's change */
    f32 span;
    bool model, shown;
};
static Latch s_latch[kSlots];
static float s_prevDt = 1.0f;
static bool s_whole = false;

static inline u32 fw(f32 f) {
    u32 w;
    __builtin_memcpy(&w, &f, 4);
    return w;
}
static inline f32 wf(u32 w) {
    f32 f;
    __builtin_memcpy(&f, &w, 4);
    return f;
}

/* the shown words: line 1 segments, line 2 segments, mHandPos, mHandCenter, eyePos (floats), then mHandAngX / Y */
template <class A> static inline be<u32>* fword(A* a, int i) {
    if (i < kLine1 * 3)
        return reinterpret_cast<be<u32>*>(&a->mChain.mpLines->mpSegments[0]) + i;
    i -= kLine1 * 3;
    if (i < kLine2 * 3)
        return reinterpret_cast<be<u32>*>(&a->mChain2.mpLines->mpSegments[0]) + i;
    i -= kLine2 * 3;
    if (i < 3)
        return reinterpret_cast<be<u32>*>(&a->mHandPos) + i;
    i -= 3;
    if (i < 3)
        return reinterpret_cast<be<u32>*>(&a->mHandCenter) + i;
    return reinterpret_cast<be<u32>*>(&a->eyePos) + (i - 3);
}
template <class A> static inline void get(A* a, u32* w) {
    for (int i = 0; i < kF; i++)
        w[i] = fword(a, i)->get();
    w[kF] = (u32)(s32)(s16)a->mHandAngX;
    w[kF + 1] = (u32)(s32)(s16)a->mHandAngY;
}
/* attention_info.position follows eyePos (control3 writes both from line 1's link 5) */
template <class A> static inline void put(A* a, const u32* w) {
    for (int i = 0; i < kF; i++)
        *fword(a, i) = w[i];
    a->mHandAngX = (s16)(s32)w[kF];
    a->mHandAngY = (s16)(s32)w[kF + 1];
    a->attention_info.position.x = wf(w[kF - 3]);
    a->attention_info.position.y = wf(w[kF - 2]);
    a->attention_info.position.z = wf(w[kF - 1]);
}

static inline Latch* find(u32 actor) {
    for (u32 i = 0; i < kSlots; i++)
        if (s_latch[i].actor == actor)
            return &s_latch[i];
    return 0;
}
static inline Latch* make(u32 actor, u32 tag, u32 pcid) {
    Latch* e = find(actor);
    if (e && e->pcid == pcid)
        return e;
    Latch* stale = e;
    for (u32 i = 0; !stale && i < kSlots; i++) {
        Latch& x = s_latch[i];
        if (x.actor == 0 || x.tag + 2 < tag)
            stale = &x;
    }
    e = stale ? stale : &s_latch[(actor >> 4) % kSlots];
    e->actor = actor;
    e->pcid = pcid;
    e->tag = ~0u;
    e->model = false;
    e->shown = false;
    for (int i = 0; i < kN; i++)
        e->dv[i] = 0.0f;
    return e;
}

/* the execute's head: true on a step before the tick30 step (the picture only); on the tick30 step: the frame-start
 * words back, dt 1 for the whole run, false */
template <class A> static inline bool begin(A* a, void (*mtx)(A*)) {
    if (!t60w::on()) {
        s_whole = false;
        return false;
    }
    const u32 actor = gabi::ea(a), tag = t60w::frameTag();
    Latch* e = make(actor, tag, *gabi::at<be<u32>>(actor + 0x04) /* base_process_class::mBsPcId */);
    if (t60::first_step() || e->tag != tag) {
        if (e->tag + 1 != tag)
            e->model = false;
        e->tag = tag;
        get(a, e->w0);
        e->span = 0.0f;
        e->shown = false;
    }
    if (!t60::tick30()) {
        e->span = gabi::fadds_ppc(e->span, t60::dt());
        if (e->model) {
            u32 w[kN];
            for (int i = 0; i < kF; i++)
                w[i] = fw(wf(e->w0[i]) + e->dv[i] * e->span);
            for (int i = kF; i < kN; i++)
                w[i] = (u32)(s32)(s16)(s32)((f32)(s32)e->w0[i] + e->dv[i] * e->span);
            put(a, w);
            mtx(a);
            e->shown = true;
        }
        return true;
    }
    if (e->shown)
        put(a, e->w0);
    s_prevDt = t60_set_dt(1.0f);
    s_whole = true;
    t60py_swap_begin(); /* (the process-order read rule addendum 2, d/t60_py.h) the run reads Link's reset flags as his whole frame */
    return false;
}

/* the execute's end on the tick30 step: dt back, the frame's change kept for the next frame's picture */
template <class A> static inline void end(A* a) {
    if (!s_whole)
        return;
    s_whole = false;
    t60_set_dt(s_prevDt);
    t60py_swap_end(); /* (the process-order read rule addendum 2) Link's live reset flags back, the run's own writes kept */
    Latch* e = find(gabi::ea(a));
    if (!e)
        return;
    u32 w[kN];
    get(a, w);
    e->model = true;
    for (int i = 0; i < kN; i++) {
        const f32 d = i < kF ? wf(w[i]) - wf(e->w0[i]) : (f32)(s16)(s32)((s32)w[i] - (s32)e->w0[i]);
        if (!(__builtin_fabsf(d) <= kJump)) /* a jump (or a non-finite value): no picture next frame */
            e->model = false;
        e->dv[i] = d;
    }
}

/* scope form (as t60w::Guard; a template so the verify build keeps it a host local) */
template <class A> struct Guard {
    A* a;
    bool skip;
    Guard(A* a_, void (*mtx)(A*)) : a(a_) { skip = begin(a, mtx); }
    ~Guard() {
        if (!skip)
            end(a);
    }
};

}  // namespace sitem_t60
