/* d_a_bk_t60.h (WWHD, true 60 build): the Bokoblin's step helpers (bk_action, bk_exec, bk_fight, bk_move;
 * dt = 1/N).
 *
 * A converted Bokoblin executes on every step with dt = 1/N; unconverted (and at N = 1) it executes on the tick30 step
 * with dt = 1, where every helper here returns the original's answer, so the dt = 1 path is the original code.
 *
 * Where the per-frame work goes (the 30 Hz timer rule, 8, 11, 14):
 *   bk_t60_end     the tick30 step: the unconverted world runs there, in the original's process order. The blink, the
 *                    lighting (it draws from cM_rnd), the smoke / spit / animation-stop counters and the per-frame
 *                    spawners count and fire there. A set of such a counter that comes
 *                    after its count in the execute order on an in-between step waits for the next frame
 *                    (bk_t60_mark_set / bk_t60_pending).
 *   bk_t60_first   the frame's first step: the collision results of the tick30 Move (hits, guard, the At hit) are read
 *                    there, so the hit gates (mNoHitTimer, mRecentHitTimer, mAtHitWait) and the paralysis delay a hit
 *                    starts count there, before the reads; the collider status's attacker ids shift there. The AI
 *                    timers and the phase counters count there too (and the per-frame work of the dispatch,
 *                    bk_t60_frame_work), before the decisions they time, and the timer
 *                    `== v` one-shots and timer- / phase-driven decisions (and their cM_rnd draws, once per frame) fall
 *                    there: the new state then moves on every step of the frame with its dt share, as the original
 *                    counts, decides, then moves its whole frame (the first-step AI-timer rule; on the tick30
 *                    step the new motion started (N-1)/N frame late). A timer an action sets on any step of the frame
 *                    is first counted next frame, as in the original.
 * The At sphere of the attack (bk_t60_at_grid): an attack's hit volume is swept once per 30 Hz frame, as the original
 * (hit checks like the original, one sweep per frame between the frame's start and end
 * positions): registered on the tick30 step (the frame's whole swing, at the frame-end attack frame), its hit read on the
 * next frame's first step. While the player was unconverted the mixed At / Tg pair met only in the tick30 Move anyway;
 * a converted player no longer meets the sword / spear per step (N chords along the arc, not the frame's one). */
#pragma once
#include "true60.h"
#include "t60_earlier_steps.h"
#include "d/actor/d_a_enemy_anm_t60.h"
#include "d/actor/d_a_bk_kdturn_t60.h" /* the knock-down's turn catch-up */
#include <unordered_map>
#include <cstdlib>

/* the set latch of (c_damagereaction.h t60dr_mark_set / t60dr_pending, the same port-owned latch and frame tag
 * g_Counter.mCounter0; repeated here so these units need not include the damage reaction's unit header): a counter set on
 * an in-between step whose phase (or count) waits for the next 30 Hz frame. dt = 1 and the tick30 step record nothing. */
static inline u32 bk_t60_frame_tag() { return *gabi::at<be<u32>>(0x101FF558); }
static inline void bk_t60_mark_set(const void* counter) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(counter), bk_t60_frame_tag());
}
static inline bool bk_t60_pending(const void* counter) {
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(gabi::ea(counter), bk_t60_frame_tag()) != 0;
}

/* the frame's first step (every step at dt = 1) */
static inline bool bk_t60_first() { return t60::dt() == 1.0f || t60::first_step(); }
/* the frame's last (tick30) step (every step at dt = 1) */
static inline bool bk_t60_end() { return t60::dt() == 1.0f || t60::tick30(); }

/* a counter counted (-1 while non-zero) on the tick30 step, read after its count position in the execute: the frame's
 * counted value on the steps before the tick30 step, the stored one on it */
static inline s32 bk_t60_counted(s32 v) { return (v != 0 && !bk_t60_end()) ? v - 1 : v; }

/* the animation crossing-check rule: `(int)frame == v` on an animation frame checked once per frame at 30 Hz: true on the step whose frame entered v
 * (the step before showed another integer frame); a stopped animation (rate 0) keeps firing once per 30 Hz frame, on the
 * tick30 step, as the original fires every frame */
static inline bool bk_t60_frame_is(f32 frame, f32 rate, int v) {
    if (gabi::ftoi(frame) != v)
        return false;
    f32 dt = t60::dt();
    if (dt == 1.0f)
        return true;
    f32 step = gabi::fmuls_ppc(rate, dt);
    if (step == 0.0f)
        return t60::tick30();
    return gabi::ftoi(frame - step) != v;
}

/* the attack's At sphere on the 30 Hz grid: at every dt < 1 (one sweep per 30 Hz frame; with an
 * unconverted player the mixed At / Tg pair is checked only by the tick30 collision Move anyway) */
static inline bool bk_t60_at_grid() { return t60::dt() != 1.0f; }

/* one AI state step per 30 Hz frame. The original runs the action / mode
 * dispatch once per frame: a state entered in frame f (a transition of the dispatch, a hit read by damage_check, a
 * change by damage_reaction) is first run in frame f + 1, and a chain of per-call transitions (fight_run 0 -> 1 -> 2 ->
 * 4, the jump steps of mJumpMove / mJumpStep) advances one link per frame. Run per step, the chain advanced one link
 * per step (N times faster: a lead that grew with N). So a state entered on an in-between step waits for the next frame:
 * the execute that changed the state marks it (bk_t60_ai_note, frame-tag latch keyed by the actor), and the dispatch of
 * Bk_move is held on the frame's remaining steps (bk_t60_ai_held); the motion keeps the speed the frame's dispatch set,
 * as the original frame moves with it. A change on the tick30 step needs no mark (the next step is the next frame's).
 * dt = 1 and N = 1: nothing is marked or held (the original). */
template <class T> static inline uint64_t bk_t60_ai_state(T* i_this) {
    return (uint64_t)(u16)(s16)i_this->dr.mAction | ((uint64_t)(u16)(s16)i_this->dr.mMode << 16) |
           ((uint64_t)(u8)i_this->mJumpStep << 32) | ((uint64_t)((s32)i_this->mJumpMove != 0) << 40);
}
template <class T> static inline u32 bk_t60_ai_key(T* i_this) { return gabi::ea(&i_this->dr.mAction); }
/* a state entered by a fall-through into its case body has RUN in the call that entered it (the original frame ran its
 * body once, with the frame's whole smoothing / turn / attack-frame step): it is not held, its body continues on the
 * frame's remaining steps with their dt shares (kbkU1_2 frame 55: fight 0 -> 1, held, lost half the speed decay and
 * turn). bk_t60_ai_ran marks the state at the fall-through (port latch: key = the mode's odd byte address, value = the
 * frame tag mixed with the state, so a later change in the same call is held again). dt = 1 / tick30: nothing. */
template <class T> static inline u32 bk_t60_ran_tag(T* i_this) {
    uint64_t s = bk_t60_ai_state(i_this);
    return bk_t60_frame_tag() ^ ((u32)s * 0x9E3779B1u) ^ (u32)(s >> 32) ^ 0x5A5A0000u;
}
template <class T> static inline void bk_t60_ai_ran(T* i_this) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(&i_this->dr.mMode) | 1u, bk_t60_ran_tag(i_this));
}
template <class T> static inline void bk_t60_ai_note(T* i_this, uint64_t before) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_ai_state(i_this) == before)
        return;
    if (wwhd_t60_latch_pending(gabi::ea(&i_this->dr.mMode) | 1u, bk_t60_ran_tag(i_this))) {
        wwhd_t60_latch_set(gabi::ea(&i_this->dr.mMode) | 1u, ~bk_t60_ran_tag(i_this)); /* used: never matches again (~tag) */
        return;
    }
    wwhd_t60_latch_set(bk_t60_ai_key(i_this), bk_t60_frame_tag());
}
template <class T> static inline bool bk_t60_ai_held(T* i_this) {
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(bk_t60_ai_key(i_this), bk_t60_frame_tag()) != 0;
}

/* the held tick30 step completes the frame's dispatch call (pair f77: fight_run mode
 * 3 -> 2 on the timer). A's one call senses the target (Bk_move: mTargetAngleY / mPlayerDistance from the frame-start
 * Bokoblin to the target's tick30-step position, bk_t60_sense), then fight_run copies it to mTurnTargetY and turns
 * current.angle.y toward it (scale 4, 0x400 / 0x800 in mode 1, on the ground) BEFORE its mode switch; so its frame ends
 * on the tick30 sensing and the turn toward it. At N steps the switch came on the first step (on the first step's sensing,
 * target 2594); the tick30 step was held (bk_t60_ai_held), sensed nothing and did not turn: B ended on 2594 / 2678, A on
 * 2699 / 2693, and the next frames' decisions read the late target. Form: fight_run's prelude marks the frame on an
 * earlier step (the first step: which turn ran: key the mTurnTargetY address, 0x400; | 1, 0x800; ^ 0xB0000000, no turn); a held tick30
 * step senses (bk_t60_sense, any action: A's call sensed) and, when fight_run's prelude ran, sets mTurnTargetY from the
 * new target and turns once more: the integer ease ends on the frame's full step from the frame-start angle (c_lib
 * t60PinFull), A's turn. Nothing else of the held state runs. dt = 1 / N = 1: nothing is marked (the original). */
template <class T> static inline u32 bk_t60_turn_key(T* i_this, int kind) {
    const u32 a = gabi::ea(&i_this->dr.mTurnTargetY);
    return kind == 0 ? (a ^ 0xB0000000u) : kind == 0x800 ? (a | 1u) : a;
}
/* (f141 addendum, pair: fight_run 0 -> 1 on the first step; A's call entered in mode 0 and did not turn that
 * frame, B's tick30 call (mode 1) turned 2885 -> 2822 for A's 2885) the turn's condition (entry mode != 0, on the ground)
 * and its max step are the frame's first call's answer: noted on the first step, held on the later steps
 * (bk_t60_turn_due), as the ground answer of bk_t60_ground_held. */
template <class T> static inline void bk_t60_turn_note(T* i_this, int maxStep) {
    if (t60::dt() == 1.0f || !t60::first_step() || t60::tick30())
        return;
    wwhd_t60_latch_set(bk_t60_turn_key(i_this, maxStep), bk_t60_frame_tag());
}
/* on a later step of the frame: -1 fight_run did not run on the first step; 0 it ran without a turn; else the turn's max
 * step (the first step and dt 1: -1, the live answer) */
template <class T> static inline int bk_t60_turn_due(T* i_this) {
    if (t60::dt() == 1.0f || t60::first_step())
        return -1;
    const u32 tag = bk_t60_frame_tag();
    if (wwhd_t60_latch_pending(bk_t60_turn_key(i_this, 0x800), tag) != 0)
        return 0x800;
    if (wwhd_t60_latch_pending(bk_t60_turn_key(i_this, 0x400), tag) != 0)
        return 0x400;
    if (wwhd_t60_latch_pending(bk_t60_turn_key(i_this, 0), tag) != 0)
        return 0;
    return -1;
}

/* (2026-11-25; long2 seg 3 + at N 2, Bokoblin 47C5E81C frame
 * 1012): fight_run's jump-run landing (jump step 1 -> 2, case 1) breaks out of the jump switch, and the same call then
 * decides the fight distance (mPlayerDistance < mFightDist: action 5, mode 0, the attack). A's landing call attacked at
 * 1012. At N steps the landing came on the first step (the ground answer held), where the decision is
 * not taken (it reads Link: tick30 only), and the jump step change held the tick30 step (bk_t60_ai_held): no decision
 * that frame; the next frame's case 2 stopped the run (speedF 0) and attacked a frame late, from a standstill (the
 * spear's swing 142 off at Link's 1050 knock-back, 8.4 off by 1106). Form: the landing on an earlier step is noted
 * (frame tag, keyed by the jump step's address); the held tick30 step (Bk_move, after its sensing) makes the landing
 * call's decision with the tick30 sensing, as A's call. dt 1 / a landing on the tick30 step: nothing (the call decides
 * itself). Negative control WWHD_T60_BKJLANDATK_NEGCTRL=1. */
static inline bool bk_t60_jlandatk_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_BKJLANDATK_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
template <class T> static inline u32 bk_t60_jland_key(T* i_this) { return gabi::ea(&i_this->mJumpStep) ^ 0xD1000000u; }
/* fight_run case 1, the landing (jump step -> 2) */
template <class T> static inline void bk_t60_jland_note(T* i_this) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_jlandatk_nc())
        return;
    wwhd_t60_latch_set(bk_t60_jland_key(i_this), bk_t60_frame_tag());
}
/* the held tick30 step: the landing call's tail decision is due */
template <class T> static inline bool bk_t60_jland_due(T* i_this) {
    if (t60::dt() == 1.0f || !t60::tick30() || bk_t60_jlandatk_nc())
        return false;
    return wwhd_t60_latch_pending(bk_t60_jland_key(i_this), bk_t60_frame_tag()) != 0;
}

/* fight_run's jump-run launch (pair frame 50 = A 3552, Bokoblin 47C5CE68). In
 * jump steps 0 / 2 A's one call turns current.angle.y twice: the prelude's turn (scale 4, 0x800) and the jump step's
 * (scale 2, 0x2000); step 2's launch then adds its random yaw. The c_lib integer ease keeps one frame entry per value:
 * the tick30 prelude turn pins the frame's full step from the frame-start angle and ends the entry, so the jump step's
 * turn after it began a partial entry and moved only its dt share (3549: A 2239 -> 2201, B -> 2210; 3550 2145 / 2163).
 * (a) fight_run bk_t60_jump_turn: on the tick30 step after a pinned prelude turn (noted on the first step,
 * bk_t60_turn_due > 0) the jump step's turn is the original's one step from there (dt 1): A's frame.
 * (b) a launch on an earlier step (mJumpStep 2 -> 1 holds the tick30 step, bk_t60_ai_held): the held step's prelude
 * turn (Bk_move) ran after the random add, a write from outside the ease (partial entry): B turned 1045 -> 1168 for A's
 * 988, 7 frames; a tick30 step that is not held does the same in fight_run's prelude, 1028 -> 1154).
 * bk_t60_jlaunch_note keeps the launch's pre-add angle; the tick30 step (held: Bk_move; else fight_run's prelude,
 * bk_t60_jlaunch_complete) puts it back, turns the
 * frame's two turns (the prelude's pins its full step from the frame start, the jump step's at dt 1, as (a)) and adds
 * the launch's yaw again (cur - pre): A's order.
 * dt = 1: nothing is noted (the original). */
/* negative control WWHD_T60_BKLAUNCH_NEGCTRL=1: the launch form off (no note, no dt-1 jump turn, no ramp
 * catch-up): the form before it */
/* negative control WWHD_T60_BKLAND_NEGCTRL=1: fight_run's jump landing (step 1 -> 2) reads the ground
 * live on every step: the form before it */
static inline bool bk_t60_land_nc() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKLAND_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
/* (2026-11-25, bkL frame 165, holder 47C5CE68 / its stick 47C58678): a hit read on
 * the frame's first step puts the knock-down (mMode -100) in place there; its tick30 step is held (first run next
 * frame) and returned before the held step's sensing and fight_run's turn. A's one call made the turn (toward the
 * tick30 sensing) before damage_check's switch: 3861 -> 3964, B stayed on the first step's half turn (3924) and the
 * stick's throw left the hand from there (2.5 off where it lands). Form: the held knock-down tick30 step senses and
 * turns as the other held steps (bk_t60_turn_due). NC WWHD_T60_BKDMGTURN_NEGCTRL=1: the return as before. */
static inline bool bk_t60_dmgturn_nc() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKDMGTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
static inline bool bk_t60_launch_nc() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKLAUNCH_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
/* the note lives in the port's latch (shared by the bk TUs: a header-static or inline host map is one copy per TU in
 * this build, so fight_run's note never reached Bk_move's held step, 1028 -> 1154): key ea(&current.angle.y) ^
 * 0xC0000000, + 16 the launch mark, + 0..15 the pre-add angle's bits (set: the frame tag; clear: ~tag) */
template <class T> static inline u32 bk_t60_jlaunch_key(T* i_this, u32 i) {
    return (gabi::ea(&i_this->current.angle.y) ^ 0xC0000000u) + i;
}
template <class T> static inline void bk_t60_jlaunch_note(T* i_this) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_launch_nc())
        return;
    const u32 tag = bk_t60_frame_tag(), pre = (u16)(s16)i_this->current.angle.y;
    for (u32 i = 0; i < 16; i++)
        wwhd_t60_latch_set(bk_t60_jlaunch_key(i_this, i), (pre >> i) & 1u ? tag : ~tag);
    wwhd_t60_latch_set(bk_t60_jlaunch_key(i_this, 16), tag);
}
/* the tick30 step: true (and *pre) when a launch on an earlier step of this frame was noted; the note is used up */
template <class T> static inline bool bk_t60_jlaunch_take(T* i_this, s16* pre) {
    if (t60::dt() == 1.0f || !t60::tick30())
        return false;
    const u32 tag = bk_t60_frame_tag();
    if (wwhd_t60_latch_pending(bk_t60_jlaunch_key(i_this, 16), tag) == 0)
        return false;
    wwhd_t60_latch_set(bk_t60_jlaunch_key(i_this, 16), ~tag); /* used */
    u32 v = 0;
    for (u32 i = 0; i < 16; i++)
        if (wwhd_t60_latch_pending(bk_t60_jlaunch_key(i_this, i), tag) != 0)
            v |= 1u << i;
    *pre = (s16)(u16)v;
    return true;
}

/* jyunkai's walk (pair f171 / 187 / 198, three patrolling Bokoblins): mode 0 / -1 falls
 * through into mode 1, whose tail turns current.angle.y toward mTurnTargetY (scale 4, 0x400 / 0x1000) and ramps speedF
 * toward the patrol speed (cLib_addCalc2 by 5): A's entry frame ends on +1024 / 5.0. At N steps the entry came on the
 * first step (that step's dt share: +512 / 2.5) and the tick30 step was held (bk_t60_ai_held): no turn, no ramp; B stayed
 * 2.5 slower and 512 behind until the caps. Form: mode 1's tail marks the frame on an earlier step (which turn: key the
 * m0320 address, | 1 for 0x1000); a held tick30 step runs the tail once more (the c_lib eases end on the frame's full
 * step). Nothing else of the held state runs. dt = 1 / N = 1: nothing is marked (the original). */
template <class T> static inline u32 bk_t60_walk_key(T* i_this, bool wide) {
    return gabi::ea(&i_this->m0320) | (wide ? 1u : 0u);
}
template <class T> static inline void bk_t60_walk_note(T* i_this, s16 maxStep) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(bk_t60_walk_key(i_this, maxStep == 0x1000), bk_t60_frame_tag());
}
/* (t60_earlier_steps.h) every held step after the noting step runs the tail, not only the tick30 step:
 * at N 4 the steps between (2..N-1) were held without the ramp and the turn (pair, Bokoblin 47C59B00 frame 144,
 * jyunkai 0 -> 1 on step 1: speedF 1.25, 1.25, 1.25, 2.5 for A's 5.0; the float ramp moves its dt share per step, so it
 * needs every step; the integer turn pins the frame's full step on the tick30 step either way). N 2: the tick30 step only,
 * as before. NC WWHD_T60_BKWALKN_NEGCTRL=1: the tick30 step only (the form before). */
static inline bool bk_t60_walk_n_nc() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKWALKN_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
/* 0: jyunkai's mode-1 tail did not run on an earlier step of this frame; else its turn's max step */
template <class T> static inline s16 bk_t60_walk_due(T* i_this) {
    if (t60::dt() == 1.0f || (bk_t60_walk_n_nc() ? !t60::tick30() : !t60es::later()))
        return 0;
    const u32 tag = bk_t60_frame_tag();
    if (wwhd_t60_latch_pending(bk_t60_walk_key(i_this, true), tag) != 0)
        return 0x1000;
    if (wwhd_t60_latch_pending(bk_t60_walk_key(i_this, false), tag) != 0)
        return 0x400;
    return 0;
}

/* jyunkai's notice on a held tick30 step (pair frame 346, Bokoblin 47C5B4B4): the
 * held step runs the notice (Bk_move, jyunkai_notice) as the tail of A's call after a mode switch jyunkai made on an
 * earlier step. When jyunkai was only ENTERED on the earlier step (aite_miru 14 -> 0 on its timer), A's call ran
 * aite_miru, not jyunkai: no notice that frame; jyunkai (and its notice -> fight_run) first runs next frame. B ran it on
 * the held tick30 step: 14 -> 0 -> 4 in one frame, fight_run one frame early (bk 1189@347). Form: jyunkai marks the frame
 * when its body runs on an earlier step (key m0320's address ^ 0xD0000000); the held step's notice needs the mark.
 * dt = 1: nothing is marked (the original). NC WWHD_T60_BKCHAIN_NEGCTRL=1: the held step's notice unconditional. */
static inline bool bk_t60_chain_nc() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKCHAIN_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
template <class T> static inline u32 bk_t60_jyunkai_key(T* i_this) { return gabi::ea(&i_this->m0320) ^ 0xD0000000u; }
template <class T> static inline void bk_t60_jyunkai_ran(T* i_this) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(bk_t60_jyunkai_key(i_this), bk_t60_frame_tag());
}
template <class T> static inline bool bk_t60_jyunkai_ran_here(T* i_this) {
    return bk_t60_chain_nc() || wwhd_t60_latch_pending(bk_t60_jyunkai_key(i_this), bk_t60_frame_tag()) != 0;
}

/* (the form before the first-step AI-timer rule) the frame's last dispatch call (the Moblin's pattern). The per-frame
 * work inside the dispatch (fight_run's phase count m1212, the guard / dodge decision, the push count mOshiCount) goes on
 * the tick30 step; but when the AI state changes on an in-between step, the dispatch is held on the frame's remaining
 * steps (tick30 included), so the call that made the change is the frame's last dispatch call and does the frame's work,
 * as the original's one call per frame does it. The change can come from damage_check before Bk_move in the same execute
 * (daBk_Execute marks it, key: the mode's address) or from the dispatched function itself (its entry state, a local of
 * the caller: the Bokoblin's parts are separate units, so no shared static). Bk_move's own decisions before the dispatch
 * (hit stun end, paralysis) do not dispatch in the same call. dt = 1: always true (the original). */
template <class T> static inline void bk_t60_note_before_move(T* i_this, uint64_t before) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_ai_state(i_this) == before)
        return;
    wwhd_t60_latch_set(gabi::ea(&i_this->dr.mMode), bk_t60_frame_tag());
}
/* the first-step AI-timer rule: the per-frame work of the dispatch (the phase count m1212 that times the
 * run's jump start, the guard / dodge decision; the push count moved to the tick30 step, first-step AI-timer
 * addendum) is the original call's deciding part: it goes on the
 * frame's FIRST step, before the frame's motion, once per frame (a state change on a later step does not repeat it; its
 * new state's work starts next frame). On the tick30 step (the form above) the run start it times moved dt of its frame:
 * kbk3_1_2 frame 42 / 50 (ldisp.py). The entry state and the latch are no longer read. dt = 1: always true. */
template <class T> static inline bool bk_t60_frame_work(T* i_this, uint64_t entry) {
    (void)i_this, (void)entry;
    return bk_t60_first();
}

/* the first-step AI-timer rule addendum (order beats first-step placement): a per-frame count that comes after
 * tick30 decisions in A's call (the push count) runs on the frame's LAST dispatch call: the tick30 step, or the
 * in-between step whose call changed the AI state before it (changedHere; the tick30 step is then held, and A's call
 * counted after the change). Once per frame: a latch keyed by the counter (a fall-through state that ran is not held, its
 * tick30 call then skips). kmoG2_2 frame 33: mode 3 -> 2 on the first step (timer), the held tick30 step lost the count
 * (A 9, B 8), OSHI one frame late. dt = 1: always (the original). */
static inline bool bk_t60_last_call_once(const void* counter, bool changedHere) {
    if (t60::dt() == 1.0f)
        return true;
    const u32 key = gabi::ea(counter), tag = bk_t60_frame_tag();
    if (t60::tick30())
        return wwhd_t60_latch_pending(key, tag) == 0;
    if (!changedHere || wwhd_t60_latch_pending(key, tag) != 0)
        return false;
    wwhd_t60_latch_set(key, tag);
    return true;
}

/* an animation set (anm_init) on a middle step (neither the frame's first nor its tick30 step: N >= 4 only) is first
 * played next frame. daBk_Execute plays AFTER the dispatch (the Moblin plays before it, so its latch,
 * d_a_mo2.cpp, keys on every non-tick30 step). A set on the first step comes from what the original's call of this frame
 * sees (the hits of the tick30 Move, the state the previous frame left): played on every step, one frame by the frame's
 * end, as the original. A set on a middle step comes from a condition crossed within the frame (the original's next
 * call sees it): its plays on the frame's remaining steps would run the new animation (N-k+1)/N frame early; it starts
 * with the next frame's play. A set on the tick30 step (the frame's LAST step: timer decisions of this frame, or
 * crossings) plays its first morf there as A does in the same execute: that step's play runs at dt 1
 * (pair: the Miniblin's mode 2 -> 3 anm_init on the tick30 step played one step at dt 1/2,
 * so its frame-end head joint = eyePos = the body sphere's centre lagged A's (f211: 240.82 / 564.30 vs 242.51 / 569.35),
 * the tick30 Move missed the f213 overlap and the Co push came a frame late; was "keeps its one step of play"). The play
 * site asks bk_t60_play (below). The split-actor state "plays next frame" rule stays for middle steps only. dt = 1, N = 1: nothing is
 * marked. Any set at dt < 1 also ends the frame's held stop answer of that morf (d_a_enemy_anm_t60.h
 * en_t60_stop_reset). */
static inline u32 bk_t60_anm_t30_key(const void* morf) { return gabi::ea(morf) ^ 0xA0000000u; }
static inline void bk_t60_mark_anm(const void* morf) {
    if (t60::dt() == 1.0f)
        return;
    en_t60_stop_reset(gabi::load<u32>(gabi::ea(morf))); /* the frame's held stop answer (d_a_enemy_anm_t60.h) ends */
    if (t60::first_step())
        return;
    if (t60::tick30()) {
        wwhd_t60_latch_set(bk_t60_anm_t30_key(morf), bk_t60_frame_tag()); /* the tick30 set: its play at dt 1 */
        return;
    }
    wwhd_t60_latch_set(gabi::ea(morf), bk_t60_frame_tag());
}
/* the morf's play for a bk_t60_mark_anm user (`morf` = the same morf pointer field it marks): after a set on the tick30
 * step the play runs at dt 1 (A's one play of the new animation in its execute), else the step's play (dt 1: the
 * original). The middle-step latch (bk_t60_pending) stays the caller's own test. */
template <class F, class P> static inline u32 bk_t60_play(F* morf, P position, u32 sound, s32 reverb) {
    if (t60::dt() != 1.0f && t60::tick30() &&
        wwhd_t60_latch_pending(bk_t60_anm_t30_key(morf), bk_t60_frame_tag()) != 0) {
        const f32 previousDt = t60_set_dt(1.0f);
        const u32 ret = (*morf)->play(position, sound, reverb);
        t60_set_dt(previousDt);
        return ret;
    }
    return (*morf)->play(position, sound, reverb);
}

/* the side-step wait's draw at A's place in the frame (after pair
 * kbkG0_2: shared-RNG draws happen at A's place in the frame). fight_run's tail decides the side step when mSideStepWait
 * ran out (counted on the first step) and draws the next wait, cM_rndF(100) + 30: in A's call AFTER the mode >= 3
 * range decisions (the attack chance, cM_rndF(100), tick30 at N steps, it reads Link). At N steps the tail ran on the
 * first step, so its draw took the stream's value before the attack chance (kbkL0_2 frame 209, keyed log: the wait took
 * idx 5 (0.661) and the attack chance idx 6 (0.035 -> attack), A the reverse: no attack). The side step itself (direction,
 * mSideStepTimer: motion) stays on the first step (the first-step AI-timer rule); only the wait's draw moves to the frame's tick30 step,
 * after the range decisions: the first step marks the frame (latch keyed by mSideStepWait) and holds a non-zero
 * placeholder (no later step decides again; the wait is counted on the next frame's first step, as A's value would be);
 * the tick30 tail draws it (daBk_Execute after Bk_move when the tail did not run: a held tick30 step, another action).
 * dt = 1: the original. */
template <class T> static inline bool bk_t60_sswait_defer(T* i_this) {
    if (t60::dt() == 1.0f || t60::tick30())
        return false;
    wwhd_t60_latch_set(gabi::ea(&i_this->mSideStepWait), bk_t60_frame_tag());
    i_this->mSideStepWait = 1;
    return true;
}
template <class T> static inline bool bk_t60_sswait_due(T* i_this) {
    return t60::dt() != 1.0f && t60::tick30() &&
           wwhd_t60_latch_pending(gabi::ea(&i_this->mSideStepWait), bk_t60_frame_tag()) != 0;
}
template <class T> static inline void bk_t60_sswait_clear(T* i_this) {
    wwhd_t60_latch_set(gabi::ea(&i_this->mSideStepWait), ~bk_t60_frame_tag());
}

/* the weapon sphere on a held tick30 step (2026-11-20 after the collider audit:
 * a collider registered only on an earlier step misses the tick30 Move, the one that meets unconverted Tg / At). The
 * original registers mWeaponSph inside the dispatch call that also changes the state: the attack's At (yari_hit_check:
 * MoveCAt + Set, then the At hit read, then the caller's change: the guard drop, the wall bounce), the knock-down's
 * sphere (Bk_move, before damage_reaction ends the knock-down in the same execute) and bk_action's carry spheres. So the
 * frame's Move has it. At N steps the state changed on the frame's first step and the tick30 step is held (no dispatch):
 * the frame's Move had no weapon sphere. The registering call marks the frame (frame-tag latch, key: the sphere's
 * address + the kind); a held tick30 step registers the sphere again as that call left it, once, without reading or
 * reacting (its hit is read on the next frame's first step, where the new state reads none, as the original's next call).
 * Kinds: 1 the attack's grid sweep (the first step read inside the window: the tick30 step would have swept, MoveCAt to
 * the frame-start mAtPos, which the held steps keep), 2 the knock-down sphere (Set + SetMass 3, VsPlayer off, Spl 1),
 * 3 a plain Set (bk_action). dt = 1 / N = 1: nothing is marked (the original). */
template <class T> static inline u32 bk_t60_wsph_key(T* i_this, int kind) { return gabi::ea(&i_this->mWeaponSph) + (u32)kind; }
template <class T> static inline void bk_t60_wsph_note(T* i_this, int kind) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(bk_t60_wsph_key(i_this, kind), bk_t60_frame_tag());
}
template <class T> static inline bool bk_t60_wsph_due(T* i_this, int kind) {
    return t60::dt() != 1.0f && t60::tick30() && bk_t60_ai_held(i_this) &&
           wwhd_t60_latch_pending(bk_t60_wsph_key(i_this, kind), bk_t60_frame_tag()) != 0;
}

/* the weapon's throw / drop (yari_off_check, in daBk_Execute after the frame's play, calc and carry matrix;
 * pairs guard route: the 50 % drop after a guard bounce). The original's call
 * throws after the whole frame's animation play: the Boko leaves the hand at the frame-end pose (the carry's
 * daBoko_setMatrix from the BUKI joint gives its start and its direction; the throw angle is Link's shape angle after his
 * frame). At N steps a drop decided on the frame's first step threw at the first step's pose (the reversed bounce
 * animation half a frame short: the Boko started 72 units off, 27 degrees off, bounced 3 frames early, Link's guard slip
 * followed). Form: the throw waits for the frame's tick30 step (mDropWeapon stays set, the carry keeps the Boko in the
 * hand through the frame's play; the tick30 step's carry sets the frame-end pose, then the throw; the Boko, later in the
 * line, whole form whole-frame form, runs its frame after it, as in A). Its four draws stay where A's call makes them, right after the
 * drop chance (shared RNG at A's place): made on the deciding step, kept per Bokoblin for the frame, used by the tick30
 * throw. A drop set on the tick30 step throws at once (the frame's end pose). dt = 1: the original. */
struct BkT60Throw {
    bool drawn = false;
    s16 rot = 0, angle = 0;
    f32 speedF = 0.0f, speedY = 0.0f;
    u32 tag = 0;
};
static inline std::unordered_map<u32, BkT60Throw>& bk_t60_throw_map() {
    static std::unordered_map<u32, BkT60Throw> m;
    return m;
}
/* an earlier step of the frame (dt < 1, not the tick30 step): this frame's slot, the throw waits (slot->drawn false =
 * the deciding step: the caller makes the draws into it, in yari_off_check's order); else nullptr: throw now */
template <class T> static inline BkT60Throw* bk_t60_throw_hold(T* i_this) {
    if (t60::dt() == 1.0f || t60::tick30())
        return nullptr;
    BkT60Throw& t = bk_t60_throw_map()[gabi::ea(i_this)];
    if (t.tag != bk_t60_frame_tag()) {
        t = BkT60Throw();
        t.tag = bk_t60_frame_tag();
    }
    return &t;
}
/* the tick30 throw: the draws an earlier step of this frame made (used once), or nullptr (draw now, the original) */
template <class T> static inline const BkT60Throw* bk_t60_throw_kept(T* i_this) {
    if (t60::dt() == 1.0f)
        return nullptr;
    auto& m = bk_t60_throw_map();
    auto it = m.find(gabi::ea(i_this));
    if (it == m.end() || it->second.tag != bk_t60_frame_tag() || !it->second.drawn)
        return nullptr;
    it->second.tag = ~bk_t60_frame_tag(); /* used */
    return &it->second;
}

/* the bare-handed jump attack's launch (fight mode 0: attack_set 2, speedF 70, speed.y 105, falling through into
 * fight_attack, pair frame 159, after the weapon drop). A's call decays the launch
 * speed once in fight_attack (cLib_addCalc2 by 20, dt 1: 70 -> 50) while the Bokoblin is still on the ground, then moves
 * the frame at 50. At N steps the launching (first) step decayed by 20 x dt (70 -> 60) and the later steps of the frame
 * no longer decay (airborne, kind 2: fight_attack's ground condition), so the whole jump flew at 60 (z 2.5 per frame
 * more, 30 units by the landing). Form: the launching step makes the frame's whole decay (dt 1), the frame's later steps
 * none (a frame-tag latch keyed by speedF). dt = 1: the original. */
template <class T> static inline u32 bk_t60_launch_key(T* i_this) { return gabi::ea(&i_this->speedF); }
template <class T> static inline void bk_t60_launch_note(T* i_this) {
    if (t60::dt() == 1.0f)
        return;
    wwhd_t60_latch_set(bk_t60_launch_key(i_this), bk_t60_frame_tag());
    wwhd_t60_latch_set(bk_t60_launch_key(i_this) + 1u, ~bk_t60_frame_tag()); /* not yet decayed this frame */
}
/* the decay's dt: 1 on the launch frame's first decay (marks it done), else the step's dt (unchanged); returns the dt
 * to put back */
template <class T> static inline f32 bk_t60_launch_decay_dt(T* i_this) {
    if (t60::dt() == 1.0f || wwhd_t60_latch_pending(bk_t60_launch_key(i_this), bk_t60_frame_tag()) == 0)
        return t60::dt();
    wwhd_t60_latch_set(bk_t60_launch_key(i_this) + 1u, bk_t60_frame_tag());
    return t60_set_dt(1.0f);
}
/* a later step of the launch frame, after its whole decay: no decay */
template <class T> static inline bool bk_t60_launch_decay_skip(T* i_this) {
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(bk_t60_launch_key(i_this), bk_t60_frame_tag()) != 0 &&
           wwhd_t60_latch_pending(bk_t60_launch_key(i_this) + 1u, bk_t60_frame_tag()) != 0;
}

/* fight_attack's ground test (the attack frame's advance and the speed decay run only on the ground for the jump kinds
 * 1 / 2, pair frame 350, the weaponless jump attack's landing). A's call reads the
 * ground state the previous frame's BG check left, then moves and lands after it: the landing frame neither advances nor
 * decays (speedF 50 through frame 350, 30 at 351). At N steps the landing on the frame's first step made the tick30
 * step's test true: B decayed half a frame early (40 at 350, 20 at 351; x 1.8 off at 350, 5.3 at the stop) and advanced
 * the attack frame. Form: the first step's answer (the frame-start state) is held over the frame (frame-tag latch keyed
 * by the Bk's mAcch: +2 held true, +3 held false); a later step without a first-step answer this frame keeps its live
 * test. dt = 1: the live test (the original). */
template <class T> static inline bool bk_t60_ground_held(T* i_this, bool live) {
    if (t60::dt() == 1.0f)
        return live;
    const u32 key = gabi::ea(&i_this->dr.mAcch), tag = bk_t60_frame_tag();
    if (t60::first_step()) {
        wwhd_t60_latch_set(key + (live ? 2u : 3u), tag);
        wwhd_t60_latch_set(key + (live ? 3u : 2u), ~tag);
        return live;
    }
    if (wwhd_t60_latch_pending(key + 2u, tag))
        return true;
    if (wwhd_t60_latch_pending(key + 3u, tag))
        return false;
    return live;
}

/* (2026-11-24; a lock-on pair, Bokoblin 47C5E81C frame 316): fight_run's jump step 2 (the
 * landing's stop) zeroes speedF. The run / jump-run choice is the tick30 step's (the way check reads Link, t60Way): an
 * earlier step that is still in the jump branch runs step 2 and zeroes speedF although A's one call takes the run switch
 * (break before the jump switch) and ramps its speed from the frame start's (90 -> 85). B's tick30 step then ramped from
 * 0 (5), the frame moved ~1 unit for A's 21 and the Bokoblin fell behind (20 at 316, 90 by 323). The earlier step notes
 * the frame start's speedF before it zeroes it; the tick30 step's mode-1 call puts it back before its ramp, so the run
 * switch ramps from A's value and a frame that stays in the jump zeroes it again in step 2 (A's form). The tick30 step's
 * move is the frame's at the tick30 speed (c_damagereaction t60dr_speed_step), so the position ends where A's does.
 * Host memory per including unit (d_a_bk_fight.cpp only). dt 1: nothing. NC WWHD_T60_BKJSTOP_NEGCTRL=1. */
static inline bool bk_t60_jstop_nc() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKJSTOP_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
struct BkT60JStop {
    u32 tag;
    f32 speedF;
    s16 turn;  /* the jump step's turn on the earlier steps */
};
static std::unordered_map<u32, BkT60JStop> s_bkT60JStop;
/* jump step 2 on an earlier step, before it zeroes speedF: the frame start's speedF (the first note of the frame) */
template <class T> static inline void bk_t60_jstop_note(T* i_this) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_jstop_nc())
        return;
    const u32 tag = bk_t60_frame_tag();
    BkT60JStop& e = s_bkT60JStop[gabi::ea(i_this)];
    if (e.tag != tag)
        e.tag = tag, e.speedF = i_this->speedF, e.turn = 0;
}
/* (the same pair, 47C5E81C frame 316: angle 7175 for A's 6365): step 2 also turns (bk_t60_jump_turn, 0x2000)
 * after its stop; A's call took the run switch and made no such turn. After that turn on an earlier step: its share is
 * kept, and the tick30 restore takes it off again (the other turns of the frame stay). */
template <class T> static inline void bk_t60_jstop_turn(T* i_this, s16 before) {
    if (t60::dt() == 1.0f || t60::tick30() || bk_t60_jstop_nc())
        return;
    auto it = s_bkT60JStop.find(gabi::ea(i_this));
    if (it == s_bkT60JStop.end() || it->second.tag != bk_t60_frame_tag())
        return;
    it->second.turn = (s16)(it->second.turn + (s16)((s16)i_this->current.angle.y - before));
}
/* (2026-11-24; the sweep's route at N 2, Bokoblin 47C5CE68 frame 47: A 2973 -> 3058
 * (the prelude turn, scale 4 / 0x800) -> 3187 (jump step 2's turn, scale 2 / 0x2000 at dt 1); B's tick30 step pinned the
 * prelude turn to 3058 from the frame start, then the restore below took the earlier step's jump-turn share (73) off again
 * (2985) and the dt-1 jump turn ended on 3150; Link's look at it 18 off at 48, the route diverged from there). The prelude
 * turn's pin writes the frame's full step from the frame-start angle, which already drops every earlier step's share,
 * the jump turn's included: the share is taken off only when the tick30 step's prelude turn did not pin (fight_run passes
 * preludePinned: the tick30 prelude turn ran with the turn the frame's first step noted, so its c_lib entry began on the
 * first step and the tick30 call wrote the frame's full step). (The first form compared current.angle.y with the value
 * the earlier turn left; at N 4, frame 48, the pinned value equalled it (3242) and the share came off: 3296 for A's 3325.)
 * NC WWHD_T60_BKJTURNPIN_NEGCTRL=1: always taken off (as before). */
static inline bool bk_t60_jturn_pin_nc() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_BKJTURNPIN_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
/* the tick30 step's mode-1 call, before its ramp: the noted speedF back (used up) */
template <class T> static inline void bk_t60_jstop_restore(T* i_this, bool preludePinned = false) {
    if (t60::dt() == 1.0f || !t60::tick30() || bk_t60_jstop_nc())
        return;
    auto it = s_bkT60JStop.find(gabi::ea(i_this));
    if (it == s_bkT60JStop.end())
        return;
    if (it->second.tag == bk_t60_frame_tag()) {
        i_this->speedF = it->second.speedF;
        if (bk_t60_jturn_pin_nc() || !preludePinned) /* */
            i_this->current.angle.y = (s16)((s16)i_this->current.angle.y - it->second.turn); /* */
    }
    s_bkT60JStop.erase(it);
}
