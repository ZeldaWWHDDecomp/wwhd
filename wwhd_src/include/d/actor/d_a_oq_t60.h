/* d_a_oq_t60.h (WWHD, true 60 build; 2026-11-18): the Octorok's step helpers (d_a_oq.cpp,
 * d_a_oq_exec.cpp; dt = 1/N). Decomp build only.
 *
 * A converted Octorok (the small / sea / delayed / child Octoroks, their rock and the spawners: one profile, PROC_OQ
 * 0xE3) executes on every step with dt = 1/N; unconverted (and at N = 1) it executes on the tick30 step with dt = 1,
 * where every helper here returns the original's answer, so the dt = 1 path is the original code. The forms as for
 * the Bokoblin / ChuChu / Darknut / Keese (d_a_bk_t60.h, d_a_cc_t60.h, d_a_ki_t60.h):
 *   first step   the AI timers mTimer[6] count there (the first-step AI-timer rule), with the timer-only decisions (the delayed
 *                Octorok's attack, the child's surface timer, the dying spin's end, the rock's life), the animation-end
 *                decisions (en_t60_isStop: the frame-start answer held over the frame, animation-driven decision), the random wait draw of
 *                the attack's end (first-step AI-timer random addendum), the rock's spin pick, the wall check (it reads the previous
 *                step's ground check), and the collision results of the tick30 Move (body_atari_check with the collider
 *                status shift, the rock's shield / sword / At hits: mixed-pair hit-flag) and the rock's sea / wall contact (A checks the
 *                frame-start position: exact there).
 *   tick30 step  every decision that reads Link (distance, height, angle, ship ride: the first-step input rule addendum): the
 *                surface / dive / attack / threaten decisions, the rise's arrival at Link's height, the small Octorok's
 *                dive after an attack, the facing target, the large Octorok's ship-near latch; the spawner's whole
 *                dispatch (every mode reads Link); the night glow frame (+1 per frame, visual); the lighting
 *                (settingTevStruct draws, draw_SUB is last in A: the frame-end position).
 *   every step   the motion (posMove speed x dt with the held push, v += g dt with the exact-arc term, the facing
 *                chases, the scale / dive / rise eases, the bob), the animation play, the matrices, the collider
 *                centres.
 * One AI state step per 30 Hz frame (the Bokoblin's hold): a state (action, mode) changed on an in-between step holds
 * the dispatch on the frame's remaining steps; the Octorok moves inside its dispatch (the bob, the facing chase, the
 * dying spin), so a held step replays the motion blocks the frame's dispatch call ran (the Keese's journal, a bit set
 * per actor: A's one call ran them over the whole frame). A state entered by a fall-through into its case body has run
 * in the call (oq_t60_ran): not held. An animation set on a middle step starts with the next frame's play (the Octorok
 * plays after its dispatch: the Bokoblin form of the split-actor state rule). A launch / new animation decided on the tick30 step
 * (it reads Link) catches up the frame's earlier steps (first-step AI-timer addendum): its turn runs the whole frame, its animation gets
 * the earlier steps' play (1 - dt) at once. Collision (the one-sweep-per-frame attack rule): every collider of the Octorok (the body's At /
 * Tg / Co, the sea Octorok's bite At, the rock's At / Tg spheres) is registered for the tick30 collision Move only; its
 * hit flags stay until the next one and are read on the next frame's first step; the tick30 Move's push is held over the
 * next frame's steps, dt of it per step (the first-step push rule). */
#pragma once
#include "true60.h"
#include "d/t60conv.h"
#include "d/actor/d_a_bk_t60.h"
#include "d/actor/d_a_oq.h"

/* the frame's first / tick30 step (every step at dt = 1). Negative control (OQ_T60_NEGCTRL, with T60C_NEGCTRL): every
 * site in the per-step form (decide, count, read and register on every step, nothing held or replayed). */
#ifdef OQ_T60_NEGCTRL
static inline bool oq_t60_first() { return true; }
static inline bool oq_t60_end() { return true; }
#else
static inline bool oq_t60_first() { return bk_t60_first(); }
static inline bool oq_t60_end() { return bk_t60_end(); }
#endif
/* a decision that reads Link: on the tick30 step (every step at dt = 1) */
static inline bool oq_t60_link() { return oq_t60_end(); }

/* the state word of the hold: action, mode */
static inline u32 oq_t60_state(const oq_class* i) { return (u32)(u8)i->mAction | ((u32)(u8)i->mMode << 8); }
static inline u32 oq_t60_ran_key(const oq_class* i) { return gabi::ea(&i->mAction) ^ 0x80000000u; }
static inline u32 oq_t60_ran_tag(const oq_class* i) {
    return bk_t60_frame_tag() ^ (oq_t60_state(i) * 0x9E3779B1u) ^ 0x5A5A0000u;
}
/* a state entered by a fall-through into its case body (mode 0 -> 1, 0xA -> 0xB, 0x18 -> 0x19, the spawner's 0x32 ->
 * 0x33) has run in the call that entered it: not held. dt = 1 / tick30: nothing. */
static inline void oq_t60_ran(const oq_class* i) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(oq_t60_ran_key(i), oq_t60_ran_tag(i));
}
static inline void oq_t60_note(const oq_class* i, u32 before) {
    if (t60::dt() == 1.0f || t60::tick30() || oq_t60_state(i) == before)
        return;
    if (wwhd_t60_latch_pending(oq_t60_ran_key(i), oq_t60_ran_tag(i))) {
        wwhd_t60_latch_set(oq_t60_ran_key(i), ~oq_t60_ran_tag(i)); /* used: never matches again (~tag) */
        return;
    }
    wwhd_t60_latch_set(gabi::ea(&i->mAction), bk_t60_frame_tag());
}
static inline bool oq_t60_held(const oq_class* i) {
#ifdef OQ_T60_NEGCTRL
    return false;
#endif
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(gabi::ea(&i->mAction), bk_t60_frame_tag()) != 0;
}

/* sea_water_check's surface test (pos.y below the surface + 40 / the water + 20): A tests the frame-start position (its
 * call comes before the move), so the frame's first step decides and the later steps keep that answer (latch keyed by
 * mWaterPos + site; a frame whose first test falls on a later step tests there). dt = 1: the test. */
static inline bool oq_t60_below(const oq_class* i, u32 site, bool below) {
    if (t60::dt() == 1.0f)
        return below;
    /* two keys: "tested this frame" and "below" (a latch query with another frame value drops the entry) */
    const u32 key = (gabi::ea(&i->mWaterPos) ^ 0x40000000u) + site, tag = bk_t60_frame_tag();
    if (!oq_t60_first() && wwhd_t60_latch_pending(key + 1, tag))
        return wwhd_t60_latch_pending(key, tag) != 0;
    wwhd_t60_latch_set(key + 1, tag);
    wwhd_t60_latch_set(key, below ? tag : ~tag);
    return below;
}

/* the bob (sea_water_check's y ease toward the wave, gravity 0) sets pos.y on every step; A's frame adds the move's
 * speed.y once after its one ease. A step that ran the bob keeps only the ease (the move's y is taken back) and the
 * tick30 step adds the whole frame's speed.y: the frame ends on A's height (uncapped ease: the wave + speed.y; capped:
 * the cap + speed.y). Marked per step (latch keyed by mBobPhase, value: the step counter). dt = 1: nothing. */
static inline void oq_t60_bob_mark(const oq_class* i) {
    if (t60::dt() != 1.0f)
        wwhd_t60_latch_set(gabi::ea(&i->mBobPhase) ^ 0x40000000u, t60_step_counter());
}
static inline bool oq_t60_bob_ran(const oq_class* i) {
#ifdef OQ_T60_NEGCTRL
    return false;
#endif
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(gabi::ea(&i->mBobPhase) ^ 0x40000000u, t60_step_counter()) != 0;
}

/* the animation-end test as the frame's dispatch sees it (d_a_enemy_anm_t60.h) */
static inline bool oq_t60_isStop(oq_class* i) { return en_t60_isStop(i->mpMorf) != 0; }

/* the rock's own deletion (its sea / wall contact, a hit, the shrink's end): A's fopAcM_delete takes the process off the
 * execute line after its call, which still ran the frame's whole move and spin; a request on an earlier step stopped the
 * frame's later steps (pair: the rock's last frame moved half). The request decided on an earlier step is made on
 * the frame's tick30 step (oq_t60_delete_due, the end of the execute); its sound / particle stay at the decision (A's
 * call, the frame-start position). Latch keyed by mSpin. dt = 1 / tick30: the delete. */
static inline void oq_t60_delete(oq_class* i) {
#ifndef OQ_T60_NEGCTRL
    if (t60::dt() != 1.0f && !t60::tick30()) {
        wwhd_t60_latch_set(gabi::ea(&i->mSpin) ^ 0x40000000u, bk_t60_frame_tag());
        return;
    }
#endif
    fopAcM_delete(i);
}
static inline void oq_t60_delete_due(oq_class* i) {
#ifndef OQ_T60_NEGCTRL
    if (t60::dt() == 1.0f || !t60::tick30())
        return;
    const u32 key = gabi::ea(&i->mSpin) ^ 0x40000000u, tag = bk_t60_frame_tag();
    if (wwhd_t60_latch_pending(key, tag)) {
        wwhd_t60_latch_set(key, ~tag); /* used */
        fopAcM_delete(i);
    }
#else
    (void)i;
#endif
}

/* the first-step input rule / mixed-pair hit-flag order (s7 7001589): on a frame whose tick30 step runs the rock Octorok's
 * 0x14 init (it reads Link: first-step input addendum), A's call ran that init before its hit handling (body_atari_check after the
 * dispatch), so the first-step hit read must not change the state ahead of it (the hold would skip the init). Such a
 * frame's first step defers the read (oq_t60_defer_hits) and the tick30 step reads the hits after the init, in A's order
 * (first-step AI-timer: order before first-step placement). Latch keyed by mDamageType. Returns whether this step reads the hits. */
static inline void oq_t60_defer_hits(const oq_class* i) {
#ifndef OQ_T60_NEGCTRL
    if (t60::dt() != 1.0f && oq_t60_first() && !t60::tick30())
        wwhd_t60_latch_set(gabi::ea(&i->mDamageType) ^ 0x40000000u, bk_t60_frame_tag());
#else
    (void)i;
#endif
}
static inline bool oq_t60_hit_read(const oq_class* i) {
#ifndef OQ_T60_NEGCTRL
    if (t60::dt() != 1.0f) {
        const u32 key = gabi::ea(&i->mDamageType) ^ 0x40000000u, tag = bk_t60_frame_tag();
        const bool deferred = wwhd_t60_latch_pending(key, tag) != 0;
        if (t60::tick30()) {
            if (deferred)
                wwhd_t60_latch_set(key, ~tag); /* used */
            return deferred;
        }
        return oq_t60_first() && !deferred;
    }
#endif
    return oq_t60_first();
}

/* checkFrame (a threshold on the play frame: the spit, the water rings) at most once per 30 Hz frame: a site that
 * fired keeps quiet on the frame's later steps (port latch keyed by the morf's frame control + 4 + site). dt = 1: the
 * plain checkFrame. */
static inline bool oq_t60_checkFrame(oq_class* i, f32 frame, u32 site) {
    if (t60::dt() == 1.0f)
        return i->mpMorf->checkFrame(frame) != 0;
    const u32 key = gabi::load<u32>(gabi::ea(&i->mpMorf)) + 0x98u + 4u + site;
    if (wwhd_t60_latch_pending(key, bk_t60_frame_tag()))
        return false;
    if (!i->mpMorf->checkFrame(frame))
        return false;
    wwhd_t60_latch_set(key, bk_t60_frame_tag());
    return true;
}

/* first-step AI-timer addendum: a new animation set on the tick30 step by a decision that reads Link gets the frame's earlier steps'
 * play (1 - dt) at once (the step's own play follows after the dispatch). A set on the first step plays every step. */
static inline void oq_t60_catch_up_play(oq_class* i) {
#ifdef OQ_T60_NEGCTRL
    return;
#endif
    if (t60::dt() == 1.0f || !t60::tick30() || t60::first_step())
        return;
    const f32 previous = t60_set_dt(1.0f - t60::dt());
    i->mpMorf->play(nullptr, 0, 0);
    t60_set_dt(previous);
}
/* the same for a one-call turn (search_y_check in an init body run on the tick30 step): the whole frame's turn */
template <class F> static inline void oq_t60_whole(F f) {
    if (t60::dt() == 1.0f || !t60::tick30() || t60::first_step()) {
        f();
        return;
    }
    const f32 previous = t60_set_dt(1.0f);
    f();
    t60_set_dt(previous);
}

/* the motion blocks a dispatch call ran (replayed on the frame's held steps) */
enum {
    OQ_T60_TAIL = 1,     /* action_dousa's tail: facing target, body check, sea_water_check, search_y_check 0x500 */
    OQ_T60_SEA = 2,      /* a sea_water_check in a mode body (kougeki 0x19 / 0x1A, itai 0x1F) */
    OQ_T60_SPIN = 4,     /* the dying spin (itai 0x20 / 0x21) */
    OQ_T60_BODY = 8,     /* action_kougeki's body check (the sea Octorok's bite radius ease) */
    OQ_T60_TURN1000 = 16, /* kougeki 0x14's search_y_check 0x1000 */
    OQ_T60_DIVE = 32      /* dousa 0xB's sink ease (mDrawOffsetY), when the dive goes on */
};
struct OqT60Journal {
    u32 ea, tag, blocks;
};
static OqT60Journal s_oqT60Journal[32];
static inline OqT60Journal* oq_t60_jr(const oq_class* i, bool make) {
    const u32 ea = gabi::ea(i), tag = bk_t60_frame_tag();
    OqT60Journal* free_slot = nullptr;
    for (u32 k = 0; k < 32; k++) {
        OqT60Journal& e = s_oqT60Journal[(ea / 16 + k) & 31];
        if (e.ea == ea)
            return &e;
        if (free_slot == nullptr && (e.ea == 0 || e.tag != tag))
            free_slot = &e;
    }
    if (!make)
        return nullptr;
    if (free_slot == nullptr)
        free_slot = &s_oqT60Journal[(ea / 16) & 31];
    free_slot->ea = ea;
    free_slot->tag = tag;
    free_slot->blocks = 0;
    return free_slot;
}
/* a dispatch call that runs (not held) begins the frame's record */
static inline void oq_t60_jr_begin(const oq_class* i) {
    if (t60::dt() == 1.0f)
        return;
    OqT60Journal* e = oq_t60_jr(i, true);
    e->tag = bk_t60_frame_tag();
    e->blocks = 0;
}
static inline void oq_t60_jr_ran(const oq_class* i, u32 block) {
    if (t60::dt() == 1.0f)
        return;
    OqT60Journal* e = oq_t60_jr(i, false);
    if (e != nullptr && e->tag == bk_t60_frame_tag())
        e->blocks |= block;
}
static inline u32 oq_t60_jr_blocks(const oq_class* i) {
    OqT60Journal* e = oq_t60_jr(i, false);
    return (e != nullptr && e->tag == bk_t60_frame_tag()) ? e->blocks : 0u;
}

/* the first-step push rule: the push m_cc_move of the tick30 collision Move, held from the value the frame's first step sees (or
 * the first step that sees one after a frame without it), dt of it per step (fopAcM_posMove scales by dt), dropped on
 * the frame's last step (d_a_cc_exec.cpp cc_t60_push_step, the same form). A applies the push iff the body Co collider
 * is on (mSPrm bit 0) at the move; a tick30 decision can switch that bit within the frame, so the steps keep the push
 * they applied and the tick30 step applies the rest of the frame's amount (the held push if the bit is on then, else
 * nothing): every 30 Hz frame moves by A's push. Returns whether posMove gets `out` (*dt) this step. */
struct OqT60Push {
    u32 ea, tag;
    f32 held[3];
    f32 applied[3];
};
static OqT60Push s_oqT60Push[32];
static inline bool oq_t60_push_step(oq_class* i, cXyz* out, bool coOn) {
    const cXyz* cc = &i->mStts.m_cc_move;
    const u32 ea = gabi::ea(i), tag = bk_t60_frame_tag();
    OqT60Push* e = nullptr;
    for (u32 k = 0; k < 32 && e == nullptr; k++) {
        OqT60Push& s = s_oqT60Push[(ea / 16 + k) & 31];
        if (s.ea == ea)
            e = &s;
    }
    const bool known = e != nullptr && e->tag == tag;
    if (e == nullptr) {
        for (u32 k = 0; k < 32 && e == nullptr; k++) {
            OqT60Push& s = s_oqT60Push[(ea / 16 + k) & 31];
            if (s.ea == 0 || s.tag != tag)
                e = &s;
        }
        if (e == nullptr)
            e = &s_oqT60Push[(ea / 16) & 31];
        e->ea = ea;
        e->held[0] = e->held[1] = e->held[2] = 0.0f;
    }
    const f32 x = cc->x, y = cc->y, z = cc->z;
    const bool fresh = x != 0.0f || y != 0.0f || z != 0.0f;
    const bool held = known && (e->held[0] != 0.0f || e->held[1] != 0.0f || e->held[2] != 0.0f);
    if (!known || t60::first_step()) {
        e->applied[0] = e->applied[1] = e->applied[2] = 0.0f;
    }
    if (!known || t60::first_step() || (fresh && !held)) {
        e->tag = tag;
        e->held[0] = x, e->held[1] = y, e->held[2] = z;
    }
    const f32 dt = t60::dt();
    bool use;
    if (!t60::tick30()) {
        use = coOn;
        if (use) {
            out->x = e->held[0];
            out->y = e->held[1];
            out->z = e->held[2];
            for (int k = 0; k < 3; k++)
                e->applied[k] = gabi::fadds_ppc(e->applied[k], gabi::fmuls_ppc(e->held[k], dt));
        }
    } else {
        /* the rest of the frame's push: (bit on ? held : 0) - applied, as a per-step amount (posMove multiplies by dt) */
        f32 rest[3];
        for (int k = 0; k < 3; k++)
            rest[k] = gabi::fsubs_ppc(coOn ? (f32)e->held[k] : 0.0f, e->applied[k]);
        use = coOn || rest[0] != 0.0f || rest[1] != 0.0f || rest[2] != 0.0f;
        if (use) {
            out->x = rest[0] / dt;
            out->y = rest[1] / dt;
            out->z = rest[2] / dt;
        }
        e->ea = 0;
    }
    return use;
}

/* the one-sweep-per-frame attack rule: the Octorok's colliders are registered for the tick30 collision Move only */
static inline bool oq_t60_cc_set() { return oq_t60_end(); }
