/* d_a_mtoge_t60.h (WWHD, true 60 build; 2026-11-18): the Forsaken Fortress spikes
 * (d_a_mtoge.cpp, a moving background) on a general step length (dt = 1/N). Separate file:
 * d_a_mtoge.cpp keeps one call in its execute (the action functions keep the original text: t60_test.py's
 * hand conversion of daMtoge_actionUp is built from it).
 *
 * The original, once per 30 Hz frame: Up / Down chase speedF to 30 by 4, then mHeightOffset to 0 / -300 by speedF
 * (arrived: the next action, ARRIVAL / HIND), then the model matrix and the moving background (dBgW_Move); Hind /
 * Arrival wait for the switch (then the next action, speedF 0, the start sound).
 * Exact frame move (glide form, rule (a): the frame end equals 30 Hz):
 *   - Up / Down: the frame's first step runs the frame's two chases whole (dt 1) and latches the frame-start height,
 *     the frame-end height and the arrival; every step shows the height on the line from start to end (start + (end -
 *     start) x the frame's dt so far), the tick30 step lands on the end exactly and takes the arrival (the next action
 *     first runs next frame, as in the original); matrix and dBgW_Move every step (the background moves with the
 *     picture; a rider is the coupling);
 *   - Hind / Arrival: the switch decision (and the sound) on the tick30 step, from the frame-end switches (an
 *     unconverted setter has run its frame by then; the new action starts next frame, as in the original).
 * dt 1: never consulted (the original dispatch). Host memory per actor and 30 Hz frame tag. */
#pragma once
#include "true60.h"

namespace mtogeT60 {

struct Latch {
    u32 actor, tag;
    f32 h0, h1, span;
    bool arrived;
};
enum { kSlots = 64 };
static Latch s_latch[kSlots];

static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 */ }

static inline Latch* find(u32 actor, u32 tag) {
    for (u32 i = 0; i < kSlots; i++)
        if (s_latch[i].actor == actor && s_latch[i].tag == tag)
            return &s_latch[i];
    return 0;
}
static inline Latch* make(u32 actor, u32 tag) {
    Latch* free_slot = 0;
    for (u32 i = 0; i < kSlots; i++) {
        Latch& e = s_latch[i];
        if (e.actor == actor)
            return &e;
        if (!free_slot && (e.actor == 0 || e.tag != tag))
            free_slot = &e;
    }
    return free_slot ? free_slot : &s_latch[actor % kSlots];
}

/* Up / Down at dt != 1: the frame's chases whole on its first step, the glide on every step */
static inline void move(daMtoge_c* i_this, f32 target, u8 next) {
    const u32 actor = gabi::ea(i_this), tag = frameTag();
    Latch* e = t60::first_step() ? 0 : find(actor, tag);
    if (!e) {
        const f32 h0 = i_this->mHeightOffset;
        const f32 previous = t60_set_dt(1.0f);
        cLib_chaseF(&i_this->speedF, 30.0f, 4.0f);
        const bool arrived = cLib_chaseF(&i_this->mHeightOffset, target, i_this->speedF) != 0;
        t60_set_dt(previous);
        e = make(actor, tag);
        e->actor = actor;
        e->tag = tag;
        e->h0 = h0;
        e->h1 = i_this->mHeightOffset;
        e->arrived = arrived;
        e->span = 0.0f;
    }
    e->span = gabi::fadds_ppc(e->span, t60::dt());
    if (t60::tick30()) {
        i_this->mHeightOffset = e->h1;
        if (e->arrived)
            i_this->setAction(next);
        e->actor = 0;
    } else {
        i_this->mHeightOffset =
            gabi::fadds_ppc(e->h0, gabi::fmuls_ppc(gabi::fsubs_ppc(e->h1, e->h0), e->span));
    }
    i_this->calcMtx();
    dBgW_Move(i_this->mpBgW);
}

/* the execute's hook: true when this step is handled here (dt != 1 only) */
static inline bool execute(daMtoge_c* i_this) {
#ifdef MTOGE_T60_NEGCTRL
    (void)i_this;
    return false; /* negative control: the per-step original (central dt chases), decisions on every step */
#endif
    if (t60::dt() == 1.0f)
        return false;
    switch ((u8)i_this->mAction) {
    case ACT_UP:
        move(i_this, 0.0f, ACT_ARRIVAL);
        return true;
    case ACT_DOWN:
        move(i_this, -300.0f, ACT_HIND);
        return true;
    case ACT_HIND:
    case ACT_ARRIVAL:
        return !t60::tick30(); /* the switch decision on the tick30 step (the original body) */
    default:
        return false;
    }
}

}  // namespace mtogeT60
