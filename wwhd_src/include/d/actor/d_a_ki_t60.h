/* d_a_ki_t60.h (WWHD, true 60 build; 2026-11-18): the Keese's step helpers (d_a_ki.cpp,
 * d_a_ki_exec.cpp, d_a_ki_execute.cpp; dt = 1/N). Decomp build only.
 *
 * A converted Keese executes on every step with dt = 1/N; unconverted (and at N = 1) it executes on the tick30 step with
 * dt = 1, where every helper here returns the original's answer, so the dt = 1 path is the original code. The rules
 * as for the Bokoblin / ChuChu / Darknut (d_a_bk_t60.h, d_a_cc_t60.h, d_a_tn_t60.h):
 *   first step   the AI timers mTimers[4], mNoHitTimer and the phase counter mFrameCounter count there (the first-step AI-timer rule),
 *                with the timer `== v` one-shots, the timer / random decisions and draws (the attack lunge, the circle
 *                and wander targets, the attack cry), the arrivals measured before the frame's own motion (the path
 *                point, the knockback end, the flame's fall start, the wander target) and the wall checks (they read
 *                the previous step's ground check); the lighting (its flicker draw, before the dispatch as in A); the lunge's At / Co result of the tick30 Move is read there (mixed-pair hit-flag).
 *   tick30 step  every decision that reads Link (distance, his motion, his line of sight, the wall between: rule note
 *                first-step input addendum, whole-frame form addendum); the decisions that come after the frame's motion in A's call (the
 *                arrival at home, the return timer at the attack's end, the path's and the flame's Link checks, the
 *                hit reaction of daKi_Execute with the collider status shift): on the tick30 step they keep A's order
 *                (a first-step change before them no longer hides them, first-step AI-timer order addendum) and act after the
 *                frame's motion as in A; the reserve Keese's join decision (it counts the other Keese: the process-order read rule,
 *                ki_t60_fs_*), the eye pattern frame, the burning flame's sound / smoke.
 *   every step   the motion (ki_pos_move pos += speed x dt, the circling chases, the knockback, the fall, the flame's
 *                arc), the animation play, the ground check, the collider centres.
 * One AI state step per 30 Hz frame (check item, rule from): a state (action, behaviour, path, vanish)
 * changed on an in-between step (in practice the first step: every decision sits on the first or the tick30 step) holds
 * the dispatch on the frame's remaining steps. A Keese moves inside its dispatch functions, so a held step replays the
 * motion blocks the frame's dispatch call ran (the journal below: the original frame moved its whole frame with them,
 * then ran the new state's body next frame), and on the tick30 step the decisions of that call that come after its
 * motion. A state entered by a fall-through into its case body has run in the call (ki_t60_ran): not held. An animation set on a non-tick30 step starts with the next frame's play
 * (the Keese plays before its dispatch: the Moblin form of the split-actor state rule). The attack sphere's At and every Co set bit
 * are registered for the tick30 collision Move only (the one-sweep-per-frame attack rule, the Co push mask), and so is the damage sphere's
 * Tg (no converted or unconverted attacker registers an At on an in-between Move: nothing changes but that the tick30
 * Move's hit flags stay for the tick30 step's read); the tick30 Move's push is held over the next frame's steps, dt of
 * it per step (the first-step push rule). */
#pragma once
#include "true60.h"
#include "d/t60conv.h"
#include "d/t60_objw.h"
/* true60 (the whole-frame form rule addendum): the Keese whole form. The frame decides from Link (the
 * chase / attack target mPosMove is Link's position, Link runs earlier in the line), the tick30 collision hits and the
 * other Keese's state, and its eases, timers, RNG draws and morf play run once per 30 Hz frame; as per-step hand forms
 * they left the Keese off A's frame (e4c97n2 / f4c97n2: the chase eases saw Link's mid-frame position or the mid-frame
 * own position; controls strict). So daKi_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the
 * frame-start state (d/t60_whole.h, t60objw form): every decision with the other actors' current state, the colliders
 * registered once for the tick30 collision Move (one-sweep-per-frame attack), the move once. The earlier steps change no logic state; the
 * picture: frame interpolation. Nothing else needs the Keese's per-step position (Link's knockback and the Co push read its tick30
 * colliders). The per-step helpers below are inert inside the whole run (dt 1). Never held. Negative control:
 * T60C_NEGCTRL runs the execute on every step at dt. */
struct ki_class;
namespace ki_t60w {
static inline const t60w::Ops<ki_class>& ops() { return t60objw::Logic<ki_class>::kOps; }
}  // namespace ki_t60w
#include "d/actor/d_a_bk_t60.h"

/* the per-step movement rule (the exact-arc term of fopAcM_posMove): an inline `pos.y += speed.y; speed.y -= g` per frame, run per step as
 * `pos.y += speed.y x dt; speed.y -= g x dt`, falls g x (N - 1) / 2N short of the frame's rise; the term g dt (1 - dt) / 2
 * per step makes every 30 Hz frame end on the original's height. dt = 1: nothing. */
static inline void ki_t60_arc_y(ki_class* i, f32 g) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return;
    const f32 dt = t60::dt();
    i->actor.current.pos.y += g * dt * (1.0f - dt) * 0.5f;
#else
    (void)i, (void)g;
#endif
}

/* the state word of the hold: action, behaviour, path index, vanish */
static inline u32 ki_t60_state(const ki_class* i) {
    return (u32)(u8)i->mAction | ((u32)(u8)(s8)i->mBehaviorType << 8) | ((u32)(u8)(s8)i->mCurrKiPathIndex << 16) |
           ((u32)(u8)i->mVanish << 24);
}
/* the state changed on an earlier step of this 30 Hz frame (not the tick30 step): the dispatch is held (port latch, keyed
 * by the action byte: every unit of the Keese sees it). dt = 1: false. */
static inline bool ki_t60_held(const ki_class* i) {
#ifdef T60C_NEGCTRL
    (void)i;
    return false;
#else
    return t60c::on() && wwhd_t60_latch_pending(gabi::ea(&i->mAction), bk_t60_frame_tag()) != 0;
#endif
}
/* a state entered by a fall-through into its case body (the set-up behaviours 0 / 10 falling into 1 / 11) has run in
 * the call that entered it: not held (port latch: key = the vanish byte's address with the top bit set, value = the
 * frame tag mixed with the state, so a later change in the same call is held again). dt = 1 / tick30: nothing. */
static inline u32 ki_t60_ran_tag(const ki_class* i) {
    return bk_t60_frame_tag() ^ (ki_t60_state(i) * 0x9E3779B1u) ^ 0x5A5A0000u;
}
static inline void ki_t60_ran(const ki_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(&i->mVanish) | 0x80000000u, ki_t60_ran_tag(i));
#else
    (void)i;
#endif
}
static inline void ki_t60_note(const ki_class* i, u32 before) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30() || ki_t60_state(i) == before)
        return;
    if (wwhd_t60_latch_pending(gabi::ea(&i->mVanish) | 0x80000000u, ki_t60_ran_tag(i)))
        return;
    wwhd_t60_latch_set(gabi::ea(&i->mAction), bk_t60_frame_tag());
#else
    (void)i, (void)before;
#endif
}

/* An animation set (anm_init) before the frame's tick30 step is a request (the Moblin's form,
 *  option (a), d_a_mo2_t60.h): daKi_Execute plays its morf BEFORE the dispatch, so A's set always follows the
 * frame's whole play of the old animation (and A's calc, after the dispatch, blends from the frame-start pose with the
 * new animation's first ratio). The request (bck, morf, mode, speed, sound) lives in d_a_ki.cpp, anm_init's unit (rule
 * the split-actor state rule); the &mpMorf latch (ki_t60_mark_anm) tells the other units one is pending this frame. The old animation plays
 * on the frame's remaining steps; on the tick30 step daKi_Execute applies the request right after its play, before the
 * dispatch, at the original's place between play and calc (ki_t60_anm_apply). A set on the tick30 step is set at once
 * and replaces an earlier request. dt = 1 (N = 1, unconverted): nothing is requested, the original code. Replaces the
 * "first played next frame" hold (ki_t60_play_held). */
static inline void ki_t60_mark_anm(const ki_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on() || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(&i->mpMorf), bk_t60_frame_tag());
#else
    (void)i;
#endif
}
static inline bool ki_t60_anm_pending(const ki_class* i) {
#ifdef T60C_NEGCTRL
    (void)i;
    return false;
#else
    return t60c::on() && wwhd_t60_latch_pending(gabi::ea(&i->mpMorf), bk_t60_frame_tag()) != 0;
#endif
}
#if defined(GABI_GAME_BUILD) && GABI_T60 && !defined(T60C_NEGCTRL)
extern "C" __attribute__((weak)) void wwhd_t60_ki_anm_apply(void* ki);
static inline void ki_t60_anm_apply(ki_class* i) {
    if (t60::tick30() && ki_t60_anm_pending(i) && wwhd_t60_ki_anm_apply)
        wwhd_t60_ki_anm_apply(i);
}
#else
static inline void ki_t60_anm_apply(ki_class*) {}
#endif

/* The motion journal (per translation unit: each of the Keese's units replays only the blocks its own functions ran,
 * the split-actor state rule (1); the template argument is the unit's KI_T60_UNIT, so the units' tables stay apart: a template's
 * static member is one object across units otherwise). A dispatch call that runs (not held) begins the actor's journal and records each motion block it
 * runs (a function of the unit and up to three integer arguments); a held step replays them in order with its own dt.
 * Entries of an older frame are dead. dt = 1: nothing is recorded, a block just runs. */
typedef void (*KiT60Block)(ki_class*, s32, s32, s32);
#ifndef KI_T60_UNIT
#error "define KI_T60_UNIT (1 d_a_ki.cpp, 2 d_a_ki_exec.cpp, 3 d_a_ki_execute.cpp) before including d_a_ki_t60.h"
#endif
/* the journal of this unit began in this frame: a port latch per actor and unit (the host table alone cannot tell this
 * frame from an older one with the same frame tag, e.g. the verification harness's inputs; the harness clears the
 * latch per input) */
static inline u32 ki_t60_jr_key(const ki_class* i) { return gabi::ea(i) ^ (0x80000000u | ((u32)KI_T60_UNIT << 28)); }
static inline bool ki_t60_jr_live(const ki_class* i) {
    return wwhd_t60_latch_pending(ki_t60_jr_key(i), bk_t60_frame_tag()) != 0;
}
template <int U> struct KiT60Journal {
    enum { kActors = 64, kOps = 16 };
    struct Op {
        KiT60Block fn;
        s32 a, b, c;
    };
    struct Ent {
        u32 ea, tag;
        s32 id; /* which dispatch function ran (the unit's own numbering) */
        u32 n;
        Op op[kOps];
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
template <int U> typename KiT60Journal<U>::Ent KiT60Journal<U>::s_ent[KiT60Journal<U>::kActors];

static inline void ki_t60_jr_begin(ki_class* i, s32 id) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return;
    typename KiT60Journal<KI_T60_UNIT>::Ent* e = KiT60Journal<KI_T60_UNIT>::find(gabi::ea(i), true);
    if (e) {
        e->tag = bk_t60_frame_tag();
        e->id = id;
        e->n = 0;
        wwhd_t60_latch_set(ki_t60_jr_key(i), bk_t60_frame_tag());
    }
#else
    (void)i, (void)id;
#endif
}
/* the id of the dispatch function this unit ran in this frame, -1 when none */
static inline s32 ki_t60_jr_id(ki_class* i) {
#ifndef T60C_NEGCTRL
    typename KiT60Journal<KI_T60_UNIT>::Ent* e = KiT60Journal<KI_T60_UNIT>::find(gabi::ea(i), false);
    if (e && e->tag == bk_t60_frame_tag() && ki_t60_jr_live(i))
        return e->id;
#else
    (void)i;
#endif
    return -1;
}
/* run a motion block and record it for the frame's held steps */
static inline void ki_t60_do(ki_class* i, KiT60Block fn, s32 a = 0, s32 b = 0, s32 c = 0) {
    fn(i, a, b, c);
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return;
    typename KiT60Journal<KI_T60_UNIT>::Ent* e = KiT60Journal<KI_T60_UNIT>::find(gabi::ea(i), false);
    if (e && e->tag == bk_t60_frame_tag() && e->n < (u32)KiT60Journal<KI_T60_UNIT>::kOps)
        e->op[e->n++] = typename KiT60Journal<KI_T60_UNIT>::Op{fn, a, b, c};
#endif
}
/* drop the recorded block `fn` (a later statement of the same call undid it: the flame's x spin set back to 0) */
static inline void ki_t60_undo(ki_class* i, KiT60Block fn) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return;
    typename KiT60Journal<KI_T60_UNIT>::Ent* e = KiT60Journal<KI_T60_UNIT>::find(gabi::ea(i), false);
    if (!e || e->tag != bk_t60_frame_tag())
        return;
    u32 w = 0;
    for (u32 r = 0; r < e->n; r++)
        if (e->op[r].fn != fn)
            e->op[w++] = e->op[r];
    e->n = w;
#else
    (void)i, (void)fn;
#endif
}
static inline void ki_t60_jr_replay(ki_class* i) {
#ifndef T60C_NEGCTRL
    typename KiT60Journal<KI_T60_UNIT>::Ent* e = KiT60Journal<KI_T60_UNIT>::find(gabi::ea(i), false);
    if (!e || e->tag != bk_t60_frame_tag() || !ki_t60_jr_live(i))
        return;
    for (u32 r = 0; r < e->n; r++)
        e->op[r].fn(i, e->op[r].a, e->op[r].b, e->op[r].c);
#else
    (void)i;
#endif
}

/* the process-order read rule for the reserve Keese's count (ki_check / ki_a_d_sub reads the other Keese's action and behaviour):
 * each Keese stamps the step it executes on, and on its frame's first step whether it began the frame fighting. A
 * reader on the tick30 step takes, for a Keese that has not executed on this step yet (later in the process line), its
 * frame-start answer; else (earlier in the line, unconverted, N = 1) its live state. Port latch (a query that does not
 * match drops the key, so each answer has its own key): the Keese's mFallSpinDir byte address = fighting at the frame's
 * start, the same with the top bit set = a frame-start record exists, its mEyeLoop byte address = the step stamp. */
static inline bool ki_t60_fighting(const ki_class* k) {
    return k->mAction == ki_class::ACT_ATTACK_MOVE_INDEX_e && (s8)k->mBehaviorType < 10;
}
static inline void ki_t60_fs_stamp(const ki_class* i) {
#ifndef T60C_NEGCTRL
    if (!t60c::on())
        return;
    if (t60::first_step()) {
        wwhd_t60_latch_set(gabi::ea(&i->mFallSpinDir) | 0x80000000u, bk_t60_frame_tag());
        if (ki_t60_fighting(i))
            wwhd_t60_latch_set(gabi::ea(&i->mFallSpinDir), bk_t60_frame_tag());
    }
    wwhd_t60_latch_set(gabi::ea(&i->mEyeLoop), t60_step_counter());
#else
    (void)i;
#endif
}
/* the fight answer the reader sees for Keese `k` */
static inline bool ki_t60_fs_fighting(const ki_class* k) {
#ifndef T60C_NEGCTRL
    if (t60c::on() && !wwhd_t60_latch_pending(gabi::ea(&k->mEyeLoop), t60_step_counter()) &&
        wwhd_t60_latch_pending(gabi::ea(&k->mFallSpinDir) | 0x80000000u, bk_t60_frame_tag()))
        return wwhd_t60_latch_pending(gabi::ea(&k->mFallSpinDir), bk_t60_frame_tag()) != 0;
#endif
    return ki_t60_fighting(k);
}
