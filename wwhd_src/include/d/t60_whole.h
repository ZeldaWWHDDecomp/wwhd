/* t60_whole.h (WWHD, true 60 build; 2026-11-18): the whole 30 Hz frame of an actor on the frame's tick30
 * step (the whole-frame form rule addendum, cross-actor reads; first case d_a_obj_barrel2_t60.h). Decomp build
 * only. Separate file: actor sources keep thin calls.
 *
 * For actors whose frame decides from another actor's state (Link's grab status / speed, hits from the tick30 collision
 * Move) and moves through the shared mover (daObj::posMoveF_*):
 *   - the tick30 step runs the original execute whole at dt 1 from the frame-start state the earlier steps showed over
 *     (put back first, with old = current as fopAc_Execute set it and the matrices re-run): every decision with the other actors' current state (what A reads for either line order), the
 *     mover's original move, the collider registered once for the tick30 collision Move (the one-sweep-per-frame attack rule); the frame end is
 *     the original's bit for bit;
 *   - the frame's earlier steps change no logic state: they show the actor moving on from its frame-start values by the
 *     previous frame's change times the frame's dt so far (visual extrapolation through the actor's set_mtx), nothing
 *     when the previous frame had no model step, the actor did not run in the frame just before (or is a new process
 *     at a freed address), or the actor is held (carried: its holder places it on every step).
 * The actor's header gives the shown values (positions as floats, angles as s16 carried in floats: index >= nPos) and
 * its matrix step. dt 1: never consulted (the original). Host memory per translation unit (the split-actor state rule (1)).
 * Negative control: with T60C_NEGCTRL (tools/t60conv) the original execute runs on every step at dt. */
#pragma once
#include "true60.h"
#include <cstring>
#include "f_pc/f_pc_t60_picture.h" /* WWHD_T60_PIC_RESET: the shown values only around the draw */
#include "d/t60_whole_carrier.h" /* extension: the frame's whole-form actors, for their riders */
#include "d/t60_py.h" /* the process-order read rule addendum 2: Link reset-flag frame-OR for the tick30 run */

/* frame interpolation (generic in-between picture, 2026-11-18): an earlier step's execute of a whole-form actor is a
 * picture step, no step of its own. The port draws converted processes from a per-step history of their joint matrices
 * (runtime/src); told here, it keeps this step's matrices out of that history, so the actor's skeleton is
 * drawn between its frame ends (the frame-end skeletons blended joint by joint), smooth without a per-actor picture hook
 * (an NPC's setMtx plays its animation: no picture there). Host memory only (no guest word, no logic); game build only. */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) void wwhd_t60_whole_picture(uint32_t actor);
}
static inline void t60w_picture_step(uint32_t actor) { wwhd_t60_whole_picture(actor); }
#else
static inline void t60w_picture_step(uint32_t) {}
#endif

namespace t60w {

static inline bool on() {
#ifdef T60C_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f;
#endif
}
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 */ }

enum { kMax = 8, kSlots = 128 };
struct Latch {
    u32 actor, tag;      /* tag: the frame whose start v0 holds */
    u32 pcid;            /* the actor's process id (fpc_ProcID, base +0x04): a new actor at a freed address (or the
                          * harness's next generated input) starts a fresh latch, no stale change / model step */
    f32 v0[kMax], span;  /* frame start, the frame's dt so far */
    u32 old0[5];         /* the actor's old place (pos, angle, room: five words) as the frame's first step found it */
    f32 dv[kMax];        /* the previous frame's change */
    bool model;          /* the previous frame reached its model step (and was not held) */
    bool shown;          /* an earlier step of this frame moved the shown values */
    u32 step;            /* the step counter (true60.h) on the tick30 step that set dv / model: the next frame's
                          * first step follows it by one; anything else (a frame without a run, or the verify harness's
                          * next generated input: it advances the counter by two before an input's first step) shows
                          * nothing. Keys the latch per input, so no picture runs from the previous input's frame (
                          * gk1 / gp1 / zk1 seed-1 input 1007766: same address, process id and the next frame tag) */
};
static Latch s_latch[kSlots];
static float s_prevDt = 1.0f;
static bool s_whole = false; /* inside the tick30 step's whole run */
/* the depth counter (2026-11-19, the t60 statics lint): 1 = the tick30 step's whole run is open; each Guard that
 * begins inside it (at dt 1, the !on() path: an actor fast-created and executed from the run) counts one more level and
 * its end pops only that level, so a nested run never clears the outer run's s_whole / s_prevDt / latch update.
 * s_wholeStep: the step counter the open run began on (a run from another step is an aborted one, left behind) */
static int s_depth = 0;
static u32 s_wholeStep = 0;

template <class A> struct Ops;
/* WWHD_T60_PIC_RESET thunks: the form's set (+ mtx) and get through its Ops (f_pc_t60_picture.h) */
template <class A> struct PicThunk {
    static void apply(uint32_t ea, const void* ops, const float* v, int mtx);
    static void get(uint32_t ea, const void* ops, float* out);
};
template <class A> static inline void t60pic_record(A* a, const Ops<A>& o, const f32* v);
template <class A> struct Ops {
    int n, nPos;                   /* values, of which the first nPos are positions (floats), the rest s16 angles */
    void (*get)(A*, f32*);
    void (*set)(A*, const f32*);
    void (*mtx)(A*);               /* the picture: the actor's set_mtx */
    bool (*held)(A*);              /* the actor is placed by its holder on every step (carried): nothing is shown or
                                    * put back for it; read only at dt != 1 (dt-1 check: no extra guest reads at dt 1) */
};

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
    Latch* stale = e; /* the same address, another process: reuse its slot, fresh */
    for (u32 i = 0; !stale && i < kSlots; i++) {
        Latch& x = s_latch[i];
        if (x.actor == 0 || x.tag + 2 < tag) /* free, or not seen for two frames: a removed actor's slot */
            stale = &x;
    }
    e = stale ? stale : &s_latch[(actor >> 4) % kSlots];
    e->actor = actor;
    e->pcid = pcid;
    e->tag = ~0u;
    e->model = false;
    e->shown = false;
    e->step = 0;
    for (int i = 0; i < kMax; i++)
        e->dv[i] = 0.0f;
    return e;
}

/* the execute's head: true when this step is handled here (a step before the tick30 step: the picture only); on the
 * tick30 step it puts the shown values back, sets dt 1 for the whole run and returns false */
template <class A> static inline bool begin(A* a, const Ops<A>& o) {
    if (!on()) {
        if (s_whole && s_wholeStep == t60_step_counter()) { /* nested inside this step's open whole run: one level more */
            s_depth++;
            return false;
        }
        s_whole = false; /* a whole run that never reached its end (an aborted run) leaves nothing behind */
        s_depth = 0;
        return false;
    }
    const bool held = o.held(a);
    const u32 actor = gabi::ea(a), tag = frameTag();
    t60wc::note(actor); /* extension: a 30 Hz carrier for its riders this frame (d/t60_whole_carrier.h) */
    Latch* e = make(actor, tag, *gabi::at<be<u32>>(actor + 0x04) /* base_process_class::mBsPcId */);
    if (t60::first_step() || e->tag != tag) {
        if (e->tag + 1 != tag || t60_step_counter() != e->step + 1) /* not seen in the frame before (or another
                                                                       * generated input): no previous frame's change to show */
            e->model = false;
        e->tag = tag;
        o.get(a, e->v0);
        for (u32 i = 0; i < 5; i++)
            e->old0[i] = *gabi::at<be<u32>>(actor + 0x300 + 4 * i);
        e->span = 0.0f;
        e->shown = false;
    }
    if (!t60::tick30()) {
        if (!held) /* (held: its holder places it on every step; those are steps of their own) */
            t60w_picture_step(actor);
        e->span = gabi::fadds_ppc(e->span, t60::dt());
        if (e->model && !held) {
            f32 v[kMax];
            for (int i = 0; i < o.n; i++)
                v[i] = e->v0[i] + e->dv[i] * e->span;
            if (t60pic_on()) { /* WWHD_T60_PIC_RESET: recorded for the step's draw, the logic fields stay */
                t60pic_record(a, o, v);
            } else {
                o.set(a, v);
                o.mtx(a);
            }
            e->shown = true;
        }
        return true;
    }
    if (e->shown && !held) {
        o.set(a, e->v0);
        /* fopAc_Execute copied old = current (five words: pos, angle, room) before this step's execute, i.e. the
         * picture of the step before: back to the old place the frame's first step found (in the game the frame-start
         * place, as A has it: the landing / ground checks run from old.pos; stone / pot landed 0.6 apart, the FF barrel
         * rested at another angle). Latched, not copied from current: the N-step spec harness calls execute without
         * fopAc_Execute's copy (d_a_stone N-step spec r3+0x300 after) */
        for (u32 i = 0; i < 5; i++)
            *gabi::at<be<u32>>(actor + 0x300 + 4 * i) = e->old0[i];
        /* the picture moved the matrices: back to the frame-start pose, as A has them from the previous frame's set_mtx
         * (an execute path that skips set_mtx, e.g. a cull check, keeps it; pair,-obj-tv
         *) */
        o.mtx(a);
    }
    s_prevDt = t60_set_dt(1.0f);
    s_whole = true;
    s_depth = 1;
    s_wholeStep = t60_step_counter();
    t60py_swap_begin(); /* (the process-order read rule addendum 2, d/t60_py.h) the run reads Link's reset flags as his whole frame */
    return false;
}

/* the execute's end on the tick30 step: dt back, the frame's change kept for the next frame's picture. model: the frame
 * reached its model step (a host flag) */
template <class A> static inline void end(A* a, const Ops<A>& o, bool model) {
    if (!s_whole)
        return;
    if (s_depth > 1) { /* a nested run's end: only its own level */
        s_depth--;
        return;
    }
    s_depth = 0;
    s_whole = false;
    t60_set_dt(s_prevDt);
    t60py_swap_end(); /* (the process-order read rule addendum 2) Link's live reset flags back, the run's own writes kept */
    Latch* e = find(gabi::ea(a));
    if (!e)
        return;
    e->model = model && !o.held(a);
    e->step = t60_step_counter();
    f32 v[kMax];
    o.get(a, v);
    for (int i = 0; i < o.n; i++)
        e->dv[i] = i < o.nPos ? v[i] - e->v0[i] : (f32)(s16)(s32)((s32)v[i] - (s32)e->v0[i]);
}

/* scope form for an execute with several returns: begin at construction, end at every return (the destructor).
 * Set `model` where the frame reaches its model step. dt 1: begin returns false and end does nothing (host only). */
template <class A> struct Guard {
    A* a;
    const Ops<A>& o;
    bool skip, model;
    Guard(A* a_, const Ops<A>& o_) : a(a_), o(o_), model(false) { skip = begin(a, o); }
    ~Guard() {
        if (!skip)
            end(a, o, model);
    }
};

/* angle carried in a float (index >= nPos) back to s16 */
static inline s16 angle(f32 v) { return (s16)(s32)v; }

template <class A> void PicThunk<A>::apply(uint32_t ea, const void* ops, const float* v, int mtx) {
    const Ops<A>& o = *static_cast<const Ops<A>*>(ops);
    A* a = gabi::at<A>(ea);
    f32 w[kMax];
    for (int i = 0; i < o.n; i++)
        w[i] = v[i];
    o.set(a, w);
    if (mtx)
        o.mtx(a);
}
template <class A> void PicThunk<A>::get(uint32_t ea, const void* ops, float* out) {
    const Ops<A>& o = *static_cast<const Ops<A>*>(ops);
    f32 w[kMax];
    o.get(gabi::at<A>(ea), w);
    for (int i = 0; i < o.n; i++)
        out[i] = w[i];
}
template <class A> static inline void t60pic_record(A* a, const Ops<A>& o, const f32* v) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    float w[kMax];
    for (int i = 0; i < o.n; i++)
        w[i] = v[i];
    wwhd_t60_pic_record(gabi::ea(a), &o, o.n, w, &PicThunk<A>::apply, &PicThunk<A>::get);
#else
    (void)a, (void)o, (void)v;
#endif
}
/* the picture's skeleton calc of a whole-form NPC (the gp1 for every NPC whose picture calcs
 * its mDoExt_McaMorf; the process-order read rule, readers get the frame's value): the picture's calc leaves what
 * the logic's calc reads as it found it.
 *   - the morf blend's frame snapshot: a picture calc at the step's dt (< 1) takes the path (m_Do_ext_morf_t60.h
 *     morf_t60_snap) and leaves a snapshot of this frame with the frame-start mPrevMorf; the logic's calc (the whole
 *     _execute on the tick30 step at dt 1, after the frame's play) then takes morf_t60_draw_snap and blends with that
 *     older prev: two blend steps a frame while a morf blend runs (pairs / gpdP). The picture's calc runs at
 *     dt 1: no snapshot, the original code;
 *   - the kept pose: mDoExt_McaMorf::calc blends the animation into the pose it keeps (mpTransformInfo 0x20 / joint,
 *     mpQuat 0x10 / joint); the picture's calc puts it back (host copy; gp1 fix 1).
 * The model's own matrices (the picture) keep the calc's result. dt 1 (N 1): never called (no picture step). */
template <class M> static inline void morfPictureCalc(M* m) {
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
}  // namespace t60w
