/* d_a_grid_t60.h (WWHD, true 60 build; 2026-11-18): the ship's sail cloth (d_a_grid.cpp,
 * profile 0xAB, created by the ship) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: d_a_grid.cpp keeps thin calls.
 *
 * the whole-frame form rule (springs frame-exact) with its addendum (cross-actor reads; the mant / barrel2
 * form, 2026-11-18: the grid couples with the ship): the sail's frame reads the ship (daShip_c's sail angle
 * through the grid's ship pointer; the ship writes the grid's position, angles, scale.y, furl and the forced wind angle
 * on every one of its steps, the ship executes earlier in the line), the wind (KYEFF, tick30), the camera eye (the alpha)
 * and the play's alternate-sail flag; its state is per-frame chases and counters (relativeWindAngle cLib_addCalcAngleS2,
 * openingGain / oscillationGain cLib_addCalc2 / addCalc0, the opening and oscillation timers, wavePhase / twistAngle
 * adds, the alpha's +-5 step), from which ho_move rebuilds the 85 cloth vertices and their normals. So the whole
 * daGrid_c::_execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state: the ship's frame
 * end (what A reads), every chase, counter and the vertices once per frame, as the original. The earlier steps change no
 * logic state and show the 85 vertices (mPacket.mPos, which the draw's updateMaterial copies on every pass) moved on by
 * the previous frame's change times the frame's dt so far (visual); the tick30 step puts the frame-start words back
 * first. The ship keeps placing the grid (position, angles, scale) on every step, so the sail follows the hull per step.
 * No picture when the previous frame did not reach ho_move (sail down: scale.y < 0.06), the grid did not run in the
 * frame just before (or is a new process at a freed address), or a vertex jumped more than kJump in that frame. Normals
 * hold (the next tick30 run sets them). dt 1: never consulted (the original). Host memory per translation unit (rule
 * the split-actor state rule (1)). Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60conv.h"

namespace grid_t60 {

enum { kN = 85 * 3, kSlots = 4 };
static const f32 kJump = 500.0f;

static inline bool on() {
#ifdef T60C_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f;
#endif
}

struct Latch {
    u32 actor, tag, pcid; /* tag: the frame whose start w0 holds; pcid: the process id at that address */
    u32 w0[kN];           /* frame-start words of mPacket.mPos */
    f32 dv[kN];           /* the previous frame's change */
    f32 span;             /* the frame's dt so far */
    bool model, shown;    /* the previous frame reached ho_move; an earlier step moved the words */
};
static Latch s_latch[kSlots];
static float s_prevDt = 1.0f;
static bool s_whole = false; /* inside the tick30 step's whole run */

static inline be<u32>* words(daGrid_c* a) { return reinterpret_cast<be<u32>*>(&a->mPacket.mPos[0]); }
static inline be<f32>* floats(daGrid_c* a) { return reinterpret_cast<be<f32>*>(&a->mPacket.mPos[0]); }
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
static inline bool begin(daGrid_c* a) {
    if (!on()) {
        s_whole = false; /* a whole run that never reached its end leaves nothing behind */
        return false;
    }
    const u32 actor = gabi::ea(a), tag = t60c::frameTag();
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
    return false;
}

/* the execute's end on the tick30 step: dt back, the frame's change kept for the next frame's picture. model: the frame
 * ran ho_move (the vertices were rebuilt) */
static inline void end(daGrid_c* a, bool model) {
    if (!s_whole)
        return;
    s_whole = false;
    t60_set_dt(s_prevDt);
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

}  // namespace grid_t60
