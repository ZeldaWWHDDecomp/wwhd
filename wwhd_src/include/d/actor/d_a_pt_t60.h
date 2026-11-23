/* d_a_pt_t60.h (WWHD, true 60 build; 2026-11-18): the Miniblin's step helpers (d_a_pt.cpp,
 * d_a_pt_action.cpp; dt = 1/N). Decomp build only.
 *
 * A converted Miniblin executes on every step with dt = 1/N; unconverted (and at N = 1) it executes on the tick30 step
 * with dt = 1, where every helper here returns the original's answer, so the dt = 1 path is the original code. The
 * rules as for the Keese (d_a_ki_t60.h) and the Bokoblin (d_a_bk_t60.h):
 *   first step   the AI timers mTimers[3], mNoHitTimer, the fall-out counter mFrameCounter, the blink timer, the smoke
 *                countdown and the spawn delay count there (the first-step AI-timer rule), with the timer `== v` one-shots, the timer /
 *                random decisions and draws, the arrivals measured before the frame's own motion (the jump target, the
 *                knockback's end in the spin) and the checks that read the previous step's ground / wall check (the
 *                hard landing, the run start, the bat's end); the animation stop tests (d_a_enemy_anm_t60.h).
 *   tick30 step  every decision that reads Link or the bait (distance, angle, his spin; the first-step input rule addendum, whole-frame form
 *                addendum): the wake-up, the jump's end near the target, the attack start and the attack's end; the
 *                decisions after the frame's motion in A's call (the knockback death, the water / ground check) and the
 *                collider status shift with the hit reaction (damage_check: mStts.Move, At / Tg hits, in A's order).
 *   every step   the motion (pos += speed x dt with the exact-arc term, the jump chase, the knockback), the angle chases,
 *                the hop, the animation play, the ground check, the collider centres.
 * One AI state step per 30 Hz frame (check item, rule from): a state (action, mode) changed on an
 * in-between step (every decision sits on the first or the tick30 step: in practice the first step) holds the dispatch
 * on the frame's remaining steps. The Miniblin moves inside its dispatch functions, so a held step replays the motion
 * blocks the frame's dispatch call ran (the journal below, as the Keese's) and, on the tick30 step, the decisions of that
 * call that come after its motion. A state entered by a fall-through into its case body (pt_ples / pt_bat set-up) has
 * run in the call: not held. The Miniblin plays its animation AFTER the dispatch: the Bokoblin form of the split-actor state rule
 * (bk_t60_mark_anm). The attack sphere's At, the body sphere's Co and Tg are registered for the tick30 collision Move only
 * (the one-sweep-per-frame attack rule); the collision push is added once per frame, whole, on the tick30 step after the action, as A's call
 * adds it after its damage_check's status shift (the first-step push rule, the Miniblin's order). */
#pragma once
#include "true60.h"
#include "d/t60conv.h"
#include "d/actor/d_a_bk_t60.h"
#include "d/actor/d_a_enemy_anm_t60.h"

/* the per-step movement rule (the exact-arc term): an inline `pos += speed; speed -= g` per frame, run per step as `pos += speed x dt;
 * speed -= g x dt`, falls g x (N - 1) / 2N short of the frame's rise; the term g dt (1 - dt) / 2 per step makes every
 * 30 Hz frame end on the original's height. dt = 1: 0. */
static inline f32 pt_t60_arc(f32 g) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return 0.0f;
    const f32 dt = t60::dt();
    return g * dt * (1.0f - dt) * 0.5f;
#else
    (void)g;
    return 0.0f;
#endif
}

/* the state word of the hold: action, mode */
static inline u32 pt_t60_state(const pt_class* i) { return (u32)(u16)(s16)i->mAction | ((u32)(u16)(s16)i->mMode << 16); }
/* the state changed on an earlier step of this 30 Hz frame (not the tick30 step): the dispatch is held (port latch keyed
 * by the action's address). dt = 1: false. */
static inline bool pt_t60_held(const pt_class* i) {
#ifdef T60C_NEGCTRL
    (void)i;
    return false;
#else
    return t60c::on() && wwhd_t60_latch_pending(gabi::ea(&i->mAction), bk_t60_frame_tag()) != 0;
#endif
}
/* a state entered by a fall-through into its case body has run in the call that entered it: not held (port latch: key
 * = the mode's address with the top bit set, value = the frame tag mixed with the state) */
static inline u32 pt_t60_ran_tag(const pt_class* i) {
    return bk_t60_frame_tag() ^ (pt_t60_state(i) * 0x9E3779B1u) ^ 0x5A5A0000u;
}
static inline void pt_t60_ran(const pt_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(&i->mMode) | 0x80000000u, pt_t60_ran_tag(i));
#else
    (void)i;
#endif
}
static inline void pt_t60_note(const pt_class* i, u32 before) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30() || pt_t60_state(i) == before)
        return;
    if (wwhd_t60_latch_pending(gabi::ea(&i->mMode) | 0x80000000u, pt_t60_ran_tag(i)))
        return;
    wwhd_t60_latch_set(gabi::ea(&i->mAction), bk_t60_frame_tag());
#else
    (void)i, (void)before;
#endif
}

/* a deletion (fopAcM_delete: the fall out of the world, a death) on a step that is not the frame's last: the frame's
 * later steps do nothing (A's call that deleted it was its last; port latch keyed by the frame counter's address with
 * the top bit set). dt = 1 / tick30: nothing. */
static inline void pt_t60_mark_deleted(const pt_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(&i->mFrameCounter) | 0x80000000u, bk_t60_frame_tag());
#else
    (void)i;
#endif
}
static inline bool pt_t60_deleted(const pt_class* i) {
#ifdef T60C_NEGCTRL
    (void)i;
    return false;
#else
    return t60c::on() && wwhd_t60_latch_pending(gabi::ea(&i->mFrameCounter) | 0x80000000u, bk_t60_frame_tag()) != 0;
#endif
}

/* The motion journal of the dispatch (d_a_pt_action.cpp only). A dispatch call that runs (not held) begins the actor's
 * journal and records each motion block it runs; a held step replays them in order with its own dt. Entries of an older
 * frame are dead; validity per frame via a port latch (the verification harness reuses the frame tag across inputs and
 * clears the latches per input). dt = 1: nothing is recorded, a block just runs. */
typedef void (*PtT60Block)(pt_class*);
static inline u32 pt_t60_jr_key(const pt_class* i) { return gabi::ea(i) ^ 0xC0000000u; }
static inline bool pt_t60_jr_live(const pt_class* i) {
    return wwhd_t60_latch_pending(pt_t60_jr_key(i), bk_t60_frame_tag()) != 0;
}
template <int U> struct PtT60Journal {
    enum { kActors = 64, kOps = 16 };
    struct Ent {
        u32 ea, tag;
        u32 n;
        PtT60Block op[kOps];
    };
    static Ent s_ent[kActors];
    static Ent* find(u32 ea, bool make) {
        const u32 tag = bk_t60_frame_tag();
        Ent* free_slot = 0;
        for (u32 k = 0; k < kActors; k++) {
            Ent& e = s_ent[(ea / 16 + k) & (kActors - 1)];
            if (e.ea == ea)
                return &e;
            if (!free_slot && (e.ea == 0 || e.tag != tag))
                free_slot = &e;
        }
        if (!make || !free_slot)
            return 0;
        free_slot->ea = ea;
        free_slot->tag = ~tag;
        free_slot->n = 0;
        return free_slot;
    }
};
template <int U> typename PtT60Journal<U>::Ent PtT60Journal<U>::s_ent[PtT60Journal<U>::kActors];

static inline void pt_t60_jr_begin(pt_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return;
    typename PtT60Journal<0>::Ent* e = PtT60Journal<0>::find(gabi::ea(i), true);
    if (e) {
        e->tag = bk_t60_frame_tag();
        e->n = 0;
        wwhd_t60_latch_set(pt_t60_jr_key(i), bk_t60_frame_tag());
    }
#else
    (void)i;
#endif
}
/* run a motion block and record it for the frame's held steps */
static inline void pt_t60_do(pt_class* i, PtT60Block fn) {
    fn(i);
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return;
    typename PtT60Journal<0>::Ent* e = PtT60Journal<0>::find(gabi::ea(i), false);
    if (e && e->tag == bk_t60_frame_tag() && e->n < (u32)PtT60Journal<0>::kOps)
        e->op[e->n++] = fn;
#endif
}
static inline void pt_t60_jr_replay(pt_class* i) {
#ifndef T60C_NEGCTRL
    typename PtT60Journal<0>::Ent* e = PtT60Journal<0>::find(gabi::ea(i), false);
    if (!e || e->tag != bk_t60_frame_tag() || !pt_t60_jr_live(i))
        return;
    for (u32 r = 0; r < e->n; r++)
        e->op[r](i);
#else
    (void)i;
#endif
}

/* the one-sweep-per-frame attack rule (the collider mask): the attack sphere's At and the body sphere's Co / Tg set bits are registered for
 * the tick30 collision Move only (the tick30 Move's hit flags then stay for the tick30 step's read); the bits are put
 * back at the next execute's start. Host table of d_a_pt.cpp. dt = 1 / tick30: nothing. */
template <int U> struct PtT60Mask {
    enum { kSize = 64 };
    struct Ent {
        u32 ea, bits;
    };
    static Ent s_ent[kSize];
    static Ent* find(u32 ea, bool make) {
        Ent* free_slot = 0;
        for (u32 k = 0; k < (u32)kSize; k++) {
            Ent& e = s_ent[(ea / 16 + k) & (kSize - 1)];
            if (e.ea == ea)
                return &e;
            if (!free_slot && e.ea == 0)
                free_slot = &e;
        }
        if (!make || !free_slot)
            return 0;
        free_slot->ea = ea;
        free_slot->bits = 0;
        return free_slot;
    }
};
template <int U> typename PtT60Mask<U>::Ent PtT60Mask<U>::s_ent[PtT60Mask<U>::kSize];
static inline void pt_t60_cc_mask(pt_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30())
        return;
    u32 m = 0;
    if (i->mAtSph.mObjAt.mSPrm & 1)
        i->mAtSph.OffAtSPrmBit(1), m |= 1;
    if (i->mSph.mObjCo.mSPrm & 1)
        i->mSph.OffCoSPrmBit(1), m |= 2;
    if (i->mSph.mObjTg.mSPrm & 1)
        i->mSph.OffTgSPrmBit(1), m |= 4;
    if (m == 0)
        return;
    typename PtT60Mask<0>::Ent* e = PtT60Mask<0>::find(gabi::ea(i), true);
    if (e)
        e->bits = m;
#else
    (void)i;
#endif
}
static inline void pt_t60_cc_restore(pt_class* i) {
    typename PtT60Mask<0>::Ent* e = PtT60Mask<0>::find(gabi::ea(i), false);
    if (!e)
        return;
    const u32 m = e->bits;
    e->ea = 0, e->bits = 0;
    if (m & 1)
        i->mAtSph.OnAtSPrmBit(1);
    if (m & 2)
        i->mSph.OnCoSPrmBit(1);
    if (m & 4)
        i->mSph.OnTgSPrmBit(1);
}

/* the first-step push rule, the Miniblin's form: A's call adds the push AFTER its action, whose damage_check shifts the collider
 * status (mStts.Move, on the tick30 step here): the push A adds is the one read after that shift. So the frame's push is
 * added once, whole, on the tick30 step after the action (as A's order); the earlier steps add none. (A held first-step
 * push read the value before the shift: another push than A's.) dt = 1: the live push. */
static inline void pt_t60_push(pt_class* i, f32* x, f32* y, f32* z) {
#ifndef T60C_NEGCTRL
    (void)i;
    if (!t60c::on() || t60::tick30())
        return;
    *x = 0.0f, *y = 0.0f, *z = 0.0f;
#else
    (void)i, (void)x, (void)y, (void)z;
#endif
}

/* the gravity + ground check in the frame form (pair: per-step gravity left
 * B's frame-end speed.y -3.5 where A alternates -7 / 0 on flat ground, so the Acch ground flags differed and
 * pt_attack's `groundHit` mode 1 -> 2 came a frame early, f422; same class as the esa threshold rule): the tails'
 * move + gravity (pt_t60_tail, fall_move) and the execute's push / mFallSpeedY / mAcch.CrrPos run once per frame, whole
 * on the tick30 step at dt 1 (the whole-frame form mover form); the earlier steps change neither (the picture between comes from the
 * interpolation). The ground check's line runs from the frame-start position (A's old.pos): the first step keeps it
 * (fopAc_Execute copied current to old before the step), the tick30 step puts it back into old.pos before CrrPos.
 * dt = 1 (N = 1, unconverted): every step, the original. T60C_NEGCTRL: the per-step form. */
struct PtT60Old { u32 ea = 0, tag = 0; f32 p[3] = {0, 0, 0}; };
static inline PtT60Old* pt_t60_old_ent(pt_class* i) {
    static PtT60Old s_ent[64];
    const u32 ea = gabi::ea(i);
    for (u32 k = 0; k < 64u; k++) {
        PtT60Old& e = s_ent[(ea / 16 + k) & 63u];
        if (e.ea == ea || e.ea == 0) return &e;
    }
    return &s_ent[(ea / 16) & 63u];
}
/* the execute's top: the frame's first step keeps old.pos (= the frame-start position) */
static inline void pt_t60_old_note(pt_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || !t60::first_step())
        return;
    PtT60Old* e = pt_t60_old_ent(i);
    e->ea = gabi::ea(i);
    e->tag = bk_t60_frame_tag();
    e->p[0] = i->old.pos.x, e->p[1] = i->old.pos.y, e->p[2] = i->old.pos.z;
#else
    (void)i;
#endif
}
/* the execute's push / ground-check block runs on this step (and on the tick30 step old.pos is the frame start) */
static inline bool pt_t60_bg_frame(pt_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return true;
    PtT60Old* e = pt_t60_old_ent(i);
    if (!t60::tick30())
        return false;
    if (e->ea == gabi::ea(i) && e->tag == bk_t60_frame_tag()) {
        i->old.pos.x = e->p[0];
        i->old.pos.y = e->p[1];
        i->old.pos.z = e->p[2];
    }
    return true;
#else
    (void)i;
    return true;
#endif
}
/* a tail's move + gravity: false on the steps before the tick30 step (nothing moves), true where it runs; the caller
 * runs the original code at dt 1 between t60_set_dt(1) and the restore */
static inline bool pt_t60_tail_frame(f32* prevDt) {
#ifndef T60C_NEGCTRL
    *prevDt = -1.0f;
    if (!t60c::on())
        return true;
    if (!t60::tick30())
        return false;
    *prevDt = t60_set_dt(1.0f);
    return true;
#else
    *prevDt = -1.0f;
    return true;
#endif
}
