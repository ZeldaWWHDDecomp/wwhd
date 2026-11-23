/* d_a_mant_t60.h (WWHD, true 60 build; 2026-11-18): the cape of the Darknuts and Phantom
 * Ganon (d_a_mant.cpp) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_mant.cpp keeps thin calls.
 *
 * the whole-frame form rule (springs frame-exact) with its addendum (cross-actor reads; as kantera / stone2):
 * the cape is a per-frame cloth: 9 strips of 9 nodes hang from the shoulder line its wearer writes (the Darknut on every
 * step, mShoulderPosL / R), each node keeps its velocity as (new - old) x mSpeedRate (a spring-damper with gravity and
 * the wind / gust / wave forces, mTick phases), the mesh spheres and the wind sphere are registered for the collision
 * Move (one-sweep-per-frame attack) and their hits from it start the wind, the burn (mBurnTimer, particles, delete) and the tear texture, with
 * Link's facing as the wind heading; the wearer reads mWindTimer / mMeshHitOn / mHitWait. So the whole daMant_Execute
 * runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state: the shoulders the wearer set by then
 * (its frame end: what A reads), every node, timer, hit and effect once per frame, as the original. The earlier steps
 * change no logic state and show the cape's 81 vertices (mPacket.mPos, what the packet draw reads) moved on by the
 * previous frame's change times the frame's dt so far (visual); the tick30 step puts the frame-start words back first.
 * No picture when the previous frame had no vertex step, the cape did not run in the frame just before (or is a new
 * process at a freed address), or a vertex jumped more than kJump in that frame (the wearer hides the cape far away /
 * brings it back). Normals hold (the next tick30 run sets them). dt 1: never consulted (the original). Host memory per
 * translation unit (the split-actor state rule (1)). Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_py.h" /* the process-order read rule addendum 2 */
#include "d/t60_whole.h"

namespace mant_t60 {

enum { kN = 81 * 3, kSlots = 16 };
static const f32 kJump = 500.0f;

struct Latch {
    u32 actor, tag, pcid; /* as t60w::Latch */
    u32 w0[kN];           /* frame-start words of mPacket.mPos */
    f32 dv[kN];           /* the previous frame's change */
    f32 span;
    bool model, shown;
};
static Latch s_latch[kSlots];
static float s_prevDt = 1.0f;
static bool s_whole = false;

static inline be<u32>* words(mant_class* a) { return reinterpret_cast<be<u32>*>(&a->mPacket.mPos[0]); }
static inline be<f32>* floats(mant_class* a) { return reinterpret_cast<be<f32>*>(&a->mPacket.mPos[0]); }
static inline f32 asf(u32 w) {
    f32 f;
    __builtin_memcpy(&f, &w, 4);
    return f;
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
static inline bool begin(mant_class* a) {
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
        be<u32>* w = words(a);
        for (int i = 0; i < kN; i++)
            e->w0[i] = w[i].get();
        e->span = 0.0f;
        e->shown = false;
    }
    if (!t60::tick30()) {
        e->span = gabi::fadds_ppc(e->span, t60::dt());
        if (e->model) {
            be<f32>* f = floats(a);
            for (int i = 0; i < kN; i++)
                f[i] = asf(e->w0[i]) + e->dv[i] * e->span;
            e->shown = true;
        }
        return true;
    }
    if (e->shown) {
        be<u32>* w = words(a);
        for (int i = 0; i < kN; i++)
            w[i] = e->w0[i];
    }
    s_prevDt = t60_set_dt(1.0f);
    s_whole = true;
    t60py_swap_begin(); /* (the process-order read rule addendum 2, d/t60_py.h) the run reads Link's reset flags as his whole frame */
    return false;
}

/* the execute's end on the tick30 step: dt back, the frame's change kept for the next frame's picture */
static inline void end(mant_class* a, bool model) {
    if (!s_whole)
        return;
    s_whole = false;
    t60_set_dt(s_prevDt);
    t60py_swap_end(); /* (the process-order read rule addendum 2) Link's live reset flags back, the run's own writes kept */
    Latch* e = find(gabi::ea(a));
    if (!e)
        return;
    e->model = model;
    be<f32>* f = floats(a);
    for (int i = 0; i < kN && e->model; i++) {
        const f32 d = (f32)f[i] - asf(e->w0[i]);
        if (!(__builtin_fabsf(d) <= kJump)) /* a jump (or a non-finite value): no picture next frame */
            e->model = false;
        e->dv[i] = d;
    }
}

/* scope form (as t60w::Guard; a template so the verify build keeps it a host local) */
template <class A> struct Guard {
    A* a;
    bool skip, model;
    explicit Guard(A* a_) : a(a_), model(false) { skip = begin(a); }
    ~Guard() {
        if (!skip)
            end(a, model);
    }
};

}  // namespace mant_t60
