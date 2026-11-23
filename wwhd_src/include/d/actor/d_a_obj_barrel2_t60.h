/* d_a_obj_barrel2_t60.h (WWHD, true 60 build; 2026-11-18): the floating barrel /
 * sea mine with the buoy flag (d_a_obj_barrel2.cpp) on a general step length (dt = 1/N). Separate file
 * d_a_obj_barrel2.cpp keeps thin calls.
 *
 * the whole-frame form rule (springs frame-exact) with the cross-actor rule of the same day: the
 * barrel is springs and per-frame decisions throughout (set_water_pos' wobble spring with its two cM_rndFX kicks,
 * afl_sway's tilt spring-damper, mode_afl's float spring with its cM_rnd bob step and the drift spring with the
 * collision push m_cc_move, the exit / explode sinks; the mine range checks against the ship, the item hand-over by
 * Link's distance, hits, the mode changes and their timers, the event orders). Several decisions read another actor
 * (the ship, Link) and a mine check's mode change runs its new mode in the same frame, so the whole frame runs at one
 * point, the frame's tick30 step:
 *   - the tick30 step runs the original execute whole at dt 1 from the frame-start position and tilt axis (put back
 *     first): every spring once, its random kicks once in the original order, every decision with the other actors'
 *     current state (frame-end for one that executes earlier in the line, frame-start for one later: what A reads),
 *     the cylinder registered for the tick30 collision Move (the one-sweep-per-frame attack rule); the frame end is the original's bit for bit;
 *   - the frame's earlier steps change no state: they show the barrel moving on from its frame-start position and tilt
 *     by the previous frame's change times the frame's dt so far (a visual extrapolation, set_mtx: model and the buoy
 *     flag's matrix), nothing when the previous frame had no model step or no previous frame is known.
 * A frame whose execute removed the barrel or left it without its model step (mode_proc_call false, hit 3, a delete
 * request) shows nothing more. dt 1: never consulted (the original). Host memory per actor. Negative control:
 * -DBARREL2_T60_NEGCTRL runs the original execute on every step at dt. */
#pragma once
#include "f_pc/f_pc_t60_picture.h" /* WWHD_T60_PIC_RESET */
#include "true60.h"

namespace b2T60 {

static inline bool on() {
#ifdef BARREL2_T60_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f;
#endif
}
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 */ }

struct Latch {
    u32 actor, tag;            /* tag: the frame whose start p0 / t0 hold */
    u32 pcid;                  /* the barrel's process id (base +0x04): a new barrel at a freed address starts fresh */
    f32 p0[3], t0[2], span;    /* frame start, the frame's dt so far */
    f32 dp[3], dtilt[2];       /* the previous frame's change */
    bool model;                /* the previous frame reached its model step (set_mtx, the cylinder) */
};
enum { kSlots = 128 };
static Latch s_latch[kSlots];
static float s_prevDt = 1.0f;
static bool s_whole = false;  /* inside the tick30 step's whole run */
static bool s_model = false;  /* the whole run reached the model step */
static int s_depth = 0;       /* 1 = the tick30 step's whole run is open; more = runs nested inside it (at dt 1), whose
                               * end pops only their own level (2026-11-19, as d/t60_whole.h) */
static u32 s_wholeStep = 0;   /* the step counter the open run began on */

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
    Latch* stale = e; /* the same address, another process (a new barrel at a freed address): its slot, fresh */
    for (u32 i = 0; !stale && i < kSlots; i++) {
        Latch& x = s_latch[i];
        if (x.actor == 0 || x.tag + 2 < tag)  /* free, or not seen for two frames: a removed barrel's slot */
            stale = &x;
    }
    e = stale ? stale : &s_latch[(actor >> 4) % kSlots];
    e->actor = actor;
    e->pcid = pcid;
    e->tag = ~0u;
    e->model = false;
    e->dp[0] = e->dp[1] = e->dp[2] = 0.0f;
    e->dtilt[0] = e->dtilt[1] = 0.0f;
    return e;
}
static inline f32 ext(f32 a, f32 d, f32 s) { return gabi::fadds_ppc(a, gabi::fmuls_ppc(d, s)); }

/* execute_sub's model step (the cylinder is registered there, as in the original) */
static inline bool registerHere() {
    if (s_whole && s_depth == 1) /* the outer run's own model step, not a nested run's */
        s_model = true;
    return true;
}

/* _execute's head: true when this step is handled here (a step before the tick30 step: the picture only); on the
 * tick30 step it puts the frame start back, sets dt 1 for the whole run and returns false */
/* WWHD_T60_PIC_RESET (f_pc_t60_picture.h): the picture's position and tilt recorded for the step's draw */
template <class A> struct PicB2 {
    static void (*s_mtx)(A*);
    static void apply(uint32_t ea, const void*, const float* v, int withMtx) {
        A* a = gabi::at<A>(ea);
        a->current.pos.x = v[0], a->current.pos.y = v[1], a->current.pos.z = v[2];
        a->mTiltX = v[3], a->mTiltZ = v[4];
        if (withMtx && s_mtx)
            s_mtx(a);
    }
    static void get(uint32_t ea, const void*, float* out) {
        A* a = gabi::at<A>(ea);
        out[0] = a->current.pos.x, out[1] = a->current.pos.y, out[2] = a->current.pos.z;
        out[3] = a->mTiltX, out[4] = a->mTiltZ;
    }
};
template <class A> void (*PicB2<A>::s_mtx)(A*) = nullptr;
template <class A> static inline bool begin(A* a, void (*mtx)(A*)) {
    if (!on()) {
        if (s_whole && s_wholeStep == t60_step_counter()) /* nested inside this step's open whole run: one level more */
            s_depth++;
        return false;
    }
    const u32 actor = gabi::ea(a), tag = frameTag();
    Latch* e = make(actor, tag, *gabi::at<be<u32>>(actor + 0x04) /* base_process_class::mBsPcId */);
    if (t60::first_step() || e->tag != tag) {
        if (e->tag + 1 != tag) /* not seen in the frame before: no previous frame's change to show */
            e->model = false;
        e->tag = tag;
        e->p0[0] = a->current.pos.x, e->p0[1] = a->current.pos.y, e->p0[2] = a->current.pos.z;
        e->t0[0] = a->mTiltX, e->t0[1] = a->mTiltZ;
        e->span = 0.0f;
    }
    if (!t60::tick30()) {
        e->span = gabi::fadds_ppc(e->span, t60::dt());
        if (e->model) {
#if defined(GABI_GAME_BUILD) && GABI_T60
            if (t60pic_on()) { /* WWHD_T60_PIC_RESET: recorded for the step's draw; the logic fields stay at the frame start */
                float v[5] = {ext(e->p0[0], e->dp[0], e->span), ext(e->p0[1], e->dp[1], e->span),
                              ext(e->p0[2], e->dp[2], e->span), ext(e->t0[0], e->dtilt[0], e->span),
                              ext(e->t0[1], e->dtilt[1], e->span)};
                PicB2<A>::s_mtx = mtx;
                wwhd_t60_pic_record(actor, nullptr, 5, v, &PicB2<A>::apply, &PicB2<A>::get);
                return true;
            }
#endif
            a->current.pos.x = ext(e->p0[0], e->dp[0], e->span);
            a->current.pos.y = ext(e->p0[1], e->dp[1], e->span);
            a->current.pos.z = ext(e->p0[2], e->dp[2], e->span);
            a->mTiltX = ext(e->t0[0], e->dtilt[0], e->span);
            a->mTiltZ = ext(e->t0[1], e->dtilt[1], e->span);
            mtx(a);
        }
        return true;
    }
    a->current.pos.x = e->p0[0], a->current.pos.y = e->p0[1], a->current.pos.z = e->p0[2];
    a->mTiltX = e->t0[0], a->mTiltZ = e->t0[1];
    /* the picture moved the matrices (the model's base matrix, mBaseMtx, the buoy flag's mMtx): back to the
     * frame-start pose, as A has them from the previous frame's set_mtx. The cull check (cullMtx = the model's base
     * matrix) runs before execute_sub, and a culled afloat barrel skips execute_sub and its set_mtx (pair,
     * frame 442: B kept the picture's matrix while A kept the frame-start one) */
    if (e->model && e->span > 0.0f)
        mtx(a);
    s_prevDt = t60_set_dt(1.0f);
    s_whole = true;
    s_model = false;
    s_depth = 1;
    s_wholeStep = t60_step_counter();
    return false;
}

/* _execute's end on the tick30 step: dt back, the frame's change kept for the next frame's picture */
template <class A> static inline void end(A* a) {
    if (!s_whole)
        return;
    if (s_depth > 1) { /* a nested run's end: only its own level */
        s_depth--;
        return;
    }
    s_depth = 0;
    s_whole = false;
    t60_set_dt(s_prevDt);
    Latch* e = find(gabi::ea(a));
    if (!e)
        return;
    e->model = s_model;
    e->dp[0] = gabi::fsubs_ppc(a->current.pos.x, e->p0[0]);
    e->dp[1] = gabi::fsubs_ppc(a->current.pos.y, e->p0[1]);
    e->dp[2] = gabi::fsubs_ppc(a->current.pos.z, e->p0[2]);
    e->dtilt[0] = gabi::fsubs_ppc(a->mTiltX, e->t0[0]);
    e->dtilt[1] = gabi::fsubs_ppc(a->mTiltZ, e->t0[1]);
}

}  // namespace b2T60
