/* d_a_obj_t60.h (WWHD, true 60 build; 2026-11-18): daObj::posMoveF_grade / posMoveF_stream
 * (d_a_obj.cpp, the shared mover of barrels, pots, bombs, stones, boxes, tiles ...) on a general step length
 * (dt = 1/N). Separate file: d_a_obj.cpp keeps thin calls.
 *
 * The original, once per 30 Hz frame: speed = speed + resistance (linear + quadratic, against the stream) + grade
 * (slope push and friction from the ground normal) + gravity + speedF along angle.y (+ an extra acceleration), clamped
 * at maxFallSpeed; speedF and angle.y are rewritten from it; then fopAcM_posMove adds the speed and the collision push
 * (addVelocity, m_cc_move of the tick30 collision Move) to the position. The resistance and grade terms are nonlinear
 * in the speed, so no per-step dt form ends the frame where the original does.
 *
 * Exact frame move (hold form, the first-step input rule / rule (a), the t60c::moveBegin / moveEnd idea for all three axes):
 *   - the frame's first call of the mover (the frame's first step, or the step a mode that moves first runs on) runs the
 *     original speed update whole, from the frame-start state and the caller's frame-start inputs (stream, ground
 *     normal, extra acceleration), and latches the frame-start position, the frame's speed / speedF / angle.y and the
 *     push (addVelocity, copied: the caller's push is the tick30 Move's, the first-step push rule);
 *   - the frame's later calls keep that speed (speed / speedF / angle.y written back: what a caller or the ground check
 *     changed on an in-between step does not reach the frame end) and run no speed update;
 *   - every step that is not the frame's tick30 step carries the position to start + (speed + push) x (the frame's dt
 *     so far): display and the in-between steps' reads only;
 *   - the tick30 step puts the frame-start position back and runs the original fopAcM_posMove at dt 1: the position is
 *     the original's single add (start + speed, then + push) bit for bit, and the caller's ground check and decisions
 *     that follow on that step see the original's state.
 * A caller that leaves the mover mode on an in-between step (no call on the tick30 step) leaves the in-between position
 * (the caller's one-state-change-per-frame hold covers that, not this unit).
 * dt = 1 (N = 1, the 30 Hz fallback, every unconverted caller): never consulted, the original instructions run (dt-1 check).
 * Host memory per actor (keyed by guest address and the 30 Hz frame tag g_Counter.mCounter0), one table in d_a_obj.cpp's
 * unit (the split-actor state rule (1): the mover is one function in one unit).
 * Negative control (N-step spec): -DOBJ_T60_NEGCTRL runs the per-step form (speed update and posMove every step at dt). */
#pragma once
#include "true60.h"

namespace objT60 {

struct MoveLatch {
    u32 actor;     /* guest address of the actor, 0 = free */
    u32 tag;       /* 30 Hz frame tag of the latch */
    f32 px, py, pz;  /* frame-start position */
    f32 vx, vy, vz;  /* the frame's speed */
    f32 speedF;
    u16 angleY;
    bool push;      /* addVelocity given */
    f32 ax, ay, az; /* the push */
    f32 span;       /* the frame's dt so far */
};

enum { kSlots = 256, kProbe = 32 };
static MoveLatch s_latch[kSlots];

/* the 30 Hz frame tag (g_Counter.mCounter0), read only at dt != 1 */
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); }

static inline bool on() {
#ifdef OBJ_T60_NEGCTRL
    return false;
#else
    return t60::dt() != 1.0f;
#endif
}

/* this frame's latch of the actor, or 0 */
static inline MoveLatch* find(u32 actor, u32 tag) {
    u32 h = (actor >> 2) * 2654435761u;
    for (u32 i = 0; i < kProbe; i++) {
        MoveLatch& e = s_latch[(h + i) & (kSlots - 1)];
        if (e.actor == actor && e.tag == tag)
            return &e;
    }
    return 0;
}

/* a slot for a new latch: the actor's own, else a free or stale one (another frame's) */
static inline MoveLatch* make(u32 actor, u32 tag) {
    u32 h = (actor >> 2) * 2654435761u;
    MoveLatch* free_slot = 0;
    for (u32 i = 0; i < kProbe; i++) {
        MoveLatch& e = s_latch[(h + i) & (kSlots - 1)];
        if (e.actor == actor)
            return &e;
        if (!free_slot && (e.actor == 0 || e.tag != tag))
            free_slot = &e;
    }
    return free_slot ? free_slot : &s_latch[h & (kSlots - 1)];
}

/* the in-between step's position: start + (speed + push) x span */
static inline f32 carry(f32 p, f32 v, bool push, f32 a, f32 span) {
    f32 m = push ? gabi::fadds_ppc(v, a) : v;
    return gabi::fadds_ppc(p, gabi::fmuls_ppc(m, span));
}

}  // namespace objT60
