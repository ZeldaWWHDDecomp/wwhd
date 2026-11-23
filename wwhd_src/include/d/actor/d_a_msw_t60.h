/* d_a_msw_t60.h (WWHD, true 60 build; 2026-11-18): the swinging platform on four chains
 * (d_a_msw.cpp, a moving background Link rides) on a general step length (dt = 1/N). Separate file
 * d_a_msw.cpp keeps thin calls.
 *
 * The original, once per 30 Hz frame: msw_move (the sway timer +1, current.angle x / z eased to 0 (integer chases),
 * the tilt and sway sines of the timer times their amplitudes, the amplitudes eased, shape_angle = current.angle +
 * tilt, current.pos = home + sway), the platform matrix, the four chains (lower ends through the matrix, the start-up
 * timer placing the upper ends, the hit timers, a chain hit: timer 15, sound, particle; the cylinder set and
 * registered), the background moved (dBgW_Move). Riders call ride_call_back from their ground check: the platform tips
 * toward the rider and the amplitudes rise with the rider's step (old to new position).
 * Exact frame move (the frame end equals 30 Hz, the rides of the frame included):
 *   - the frame's tick30 step runs msw_move (dt 1) from the frame-start state: a rider earlier in the line has
 *     finished its frame (its ride_call_back on its tick30 step, below), so msw_move sees that ride in the same frame,
 *     as in A; a rider later in the line rides after it (next frame's msw_move, as in A);
 *   - for the picture, the frame's first step runs msw_move once as a preview (dt 1), latches the frame-start and
 *     preview-end position / shape_angle and puts every state field msw_move writes back to its frame-start value; the
 *     steps before the tick30 step show position / shape_angle on the line from start to preview end (the frame's dt so
 *     far; angles by their wrapped difference); the matrix, the chains' lower ends and cylinder centres and dBgW_Move
 *     follow on every step (the background moves with the picture);
 *   - the chains' frame work (start-up timer, hit timers, the hit check from the last tick30 collision Move with its
 *     sound and particle, the mixed-pair hit-flag rule) and the cylinder registration (one per frame for the tick30 Move, the one-sweep-per-frame attack rule) run on
 *     the tick30 step;
 *   - ride_call_back at a rider's dt != 1 (a converted rider on this converted carrier): once per 30 Hz
 *     frame, on the rider's tick30 step, from the rider's frame-END state (its current position there, the animation-driven decision rule
 *     frame-end form) and its frame-start position (old.pos, latched on its first step; a rider that lands on an
 *     in-between step has no latch: its old.pos is used); the platform position it reads is the platform's frame-start
 *     position while the platform's tick30 move is still to come (A: the platform later in the line has not moved yet),
 *     its current (frame-end) position after it.
 * dt 1: never consulted (the original). Host memory per actor and 30 Hz frame tag. *
 * WWHD_T60_PIC_RESET (f_pc_t60_picture.h, 2026-11-20): msw is excluded on purpose: its picture moves its background for
 * its riders, so its picture is part of its logic; it keeps writing it in the execute.
 */
#pragma once
#include "true60.h"

namespace mswT60 {

static inline bool on() {
#ifdef MSW_T60_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f;
#endif
}
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 */ }

struct Latch {
    u32 actor, tag;
    f32 p0[3], p1[3], span;
    s16 a0[3], a1[3];
    bool landed;
    /* the frame-start values of the state msw_move writes besides position / shape_angle */
    s16 timer, angX, angZ, tilt[3];
    f32 tiltAmp[3], swayOff[3], swayAmp[3];
};
struct RideLatch {
    u32 actor, rider, tag;
    f32 old[3];
};
enum { kSlots = 32 };
static Latch s_move[kSlots];
static RideLatch s_ride[kSlots];

template <class L> static inline L* find(L* t, u32 actor, u32 tag) {
    for (u32 i = 0; i < kSlots; i++)
        if (t[i].actor == actor && t[i].tag == tag)
            return &t[i];
    return 0;
}
template <class L> static inline L* make(L* t, u32 actor, u32 tag) {
    L* free_slot = 0;
    for (u32 i = 0; i < kSlots; i++) {
        if (t[i].actor == actor)
            return &t[i];
        if (!free_slot && (t[i].actor == 0 || t[i].tag != tag))
            free_slot = &t[i];
    }
    return free_slot ? free_slot : &t[actor % kSlots];
}

static inline f32 line(f32 a, f32 b, f32 s) { return gabi::fadds_ppc(a, gabi::fmuls_ppc(gabi::fsubs_ppc(b, a), s)); }
static inline s16 lineS(s16 a, s16 b, f32 s) { return (s16)(a + (s16)gabi::ftoi((f32)(s16)(b - a) * s)); }

template <class A> static inline void save(Latch* e, A* a) {
    e->timer = a->mSwayTimer;
    e->angX = a->current.angle.x, e->angZ = a->current.angle.z;
    e->tilt[0] = a->mTiltAngle.x, e->tilt[1] = a->mTiltAngle.y, e->tilt[2] = a->mTiltAngle.z;
    e->tiltAmp[0] = a->mTiltAmp.x, e->tiltAmp[1] = a->mTiltAmp.y, e->tiltAmp[2] = a->mTiltAmp.z;
    e->swayOff[0] = a->mSwayOffset.x, e->swayOff[1] = a->mSwayOffset.y, e->swayOff[2] = a->mSwayOffset.z;
    e->swayAmp[0] = a->mSwayAmp.x, e->swayAmp[1] = a->mSwayAmp.y, e->swayAmp[2] = a->mSwayAmp.z;
}
template <class A> static inline void restore(const Latch* e, A* a) {
    a->mSwayTimer = e->timer;
    a->current.angle.x = e->angX, a->current.angle.z = e->angZ;
    a->mTiltAngle.x = e->tilt[0], a->mTiltAngle.y = e->tilt[1], a->mTiltAngle.z = e->tilt[2];
    a->mTiltAmp.x = e->tiltAmp[0], a->mTiltAmp.y = e->tiltAmp[1], a->mTiltAmp.z = e->tiltAmp[2];
    a->mSwayOffset.x = e->swayOff[0], a->mSwayOffset.y = e->swayOff[1], a->mSwayOffset.z = e->swayOff[2];
    a->mSwayAmp.x = e->swayAmp[0], a->mSwayAmp.y = e->swayAmp[1], a->mSwayAmp.z = e->swayAmp[2];
}

/* the execute's move: true when handled here (dt != 1); MOVE is msw_move */
template <class A, class M> static inline bool move(A* i_this, M msw_move) {
    if (!on())
        return false;
    const u32 actor = gabi::ea(i_this), tag = frameTag();
    Latch* e = t60::first_step() ? 0 : find(s_move, actor, tag);
    if (!e) {
        e = make(s_move, actor, tag);
        e->actor = actor;
        e->tag = tag;
        e->landed = false;
        e->p0[0] = i_this->current.pos.x, e->p0[1] = i_this->current.pos.y, e->p0[2] = i_this->current.pos.z;
        e->a0[0] = i_this->shape_angle.x, e->a0[1] = i_this->shape_angle.y, e->a0[2] = i_this->shape_angle.z;
        save(e, i_this);
        if (!t60::tick30()) { /* the preview for the picture, its state put back */
            const f32 previous = t60_set_dt(1.0f);
            msw_move(i_this);
            t60_set_dt(previous);
            restore(e, i_this);
        }
        e->p1[0] = i_this->current.pos.x, e->p1[1] = i_this->current.pos.y, e->p1[2] = i_this->current.pos.z;
        e->a1[0] = i_this->shape_angle.x, e->a1[1] = i_this->shape_angle.y, e->a1[2] = i_this->shape_angle.z;
        e->span = 0.0f;
    }
    e->span = gabi::fadds_ppc(e->span, t60::dt());
    if (t60::tick30()) {
        /* the frame's move: from the frame-start state and this frame's rides, at dt 1 */
        const f32 previous = t60_set_dt(1.0f);
        msw_move(i_this);
        t60_set_dt(previous);
        e->landed = true;
    } else {
        i_this->current.pos.x = line(e->p0[0], e->p1[0], e->span);
        i_this->current.pos.y = line(e->p0[1], e->p1[1], e->span);
        i_this->current.pos.z = line(e->p0[2], e->p1[2], e->span);
        i_this->shape_angle.x = lineS(e->a0[0], e->a1[0], e->span);
        i_this->shape_angle.y = lineS(e->a0[1], e->a1[1], e->span);
        i_this->shape_angle.z = lineS(e->a0[2], e->a1[2], e->span);
    }
    return true;
}

/* the chains' frame work and the cylinder registration: once per 30 Hz frame (tick30 step) */
static inline bool frameWork() { return !on() || t60::tick30(); }

/* ride_call_back: whether it runs on this rider step, the rider's old position and the platform position to use (the
 * original's &i_pt->old.pos / &i_ac->current.pos at dt 1); the scratch vectors hold latched positions */
template <class A, class P>
static inline bool ride(A* i_ac, P* i_pt, cXyz* riderScratch, const cXyz** riderOld, cXyz* platScratch,
                        const cXyz** platPos) {
    *riderOld = &i_pt->old.pos;
    *platPos = &i_ac->current.pos;
    if (!on())
        return true;
    const u32 actor = gabi::ea(i_ac), rider = gabi::ea(i_pt), tag = frameTag();
    if (t60::first_step()) {
        RideLatch* r = make(s_ride, actor, tag);
        r->actor = actor;
        r->rider = rider;
        r->tag = tag;
        r->old[0] = i_pt->old.pos.x, r->old[1] = i_pt->old.pos.y, r->old[2] = i_pt->old.pos.z;
    }
    if (!t60::tick30())
        return false;
    RideLatch* r = find(s_ride, actor, tag);
    if (r && r->rider == rider) {
        riderScratch->x = r->old[0], riderScratch->y = r->old[1], riderScratch->z = r->old[2];
        *riderOld = riderScratch;
        r->actor = 0;
    }
    const Latch* e = find(s_move, actor, tag);
    if (e && !e->landed) { /* the platform's tick30 move is still to come: its frame-start position */
        platScratch->x = e->p0[0], platScratch->y = e->p0[1], platScratch->z = e->p0[2];
        *platPos = platScratch;
    }
    return true;
}

/* ride_call_back's once-a-frame body runs at dt 1 (the original frame's tip and amplitude raises, its eases unscaled):
 * the gate above already made it one call per 30 Hz frame; with dt-aware c_lib eases (t60cand) the rider's step dt
 * would scale them a second time (s45 c_lib sweep: current.angle.z one step 2B, two 2D). Plain calls (no scope object:
 * the source glue rewrites locals): rideDtBegin returns the dt to put back with rideDtEnd. dt 1: nothing changes. */
static inline f32 rideDtBegin() { return on() ? t60_set_dt(1.0f) : 1.0f; }
static inline void rideDtEnd(f32 previous) { t60_set_dt(previous); }

}  // namespace mswT60
