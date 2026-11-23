/* d_a_cc_t60.h (WWHD, true 60 build): the ChuChu's step helpers (cc_exec, cc_act; dt = 1/N).
 *
 *
 * A converted ChuChu executes on every step with dt = 1/N; unconverted (and at N = 1) it executes on the tick30 step with
 * dt = 1, where every helper here returns the original's answer, so the dt = 1 path is the original code. It takes the
 * Bokoblin's helpers (d_a_bk_t60.h: the frame tag and set latch, bk_t60_first / bk_t60_end, bk_t60_mark_anm).
 *
 * Where the per-frame work goes (the 30 Hz timer rule, 8, 9, 11, 14), as for the Bokoblin:
 *   tick30 step  the stone animation frame, the abyss frame count, the lighting (settingTevStruct draws cM_rnd), the
 *                FIXED behaviour's random turn (cM_rnd), the deku tree's event steps (the event manager is unconverted).
 *   first step   the AI timers mTimers[7] count there, before the dispatch, and their `== v` one-shots and the
 *                decisions they time fall there (the first-step AI-timer rule: the new state moves on every step of the frame; a
 *                timer set later in the frame is counted next frame); the collision results of the tick30 Move (body /
 *                stone hits) are read there, with the collider status's attacker-id shift; the body cylinder's centre is
 *                set there (the original sets it after the dispatch, before the frame's move: the frame's start
 *                position).
 * daCC_Execute plays its animation AFTER the dispatch: the Bokoblin form of the new-animation latch (the split-actor state rule). */
#pragma once
#include "d/actor/d_a_bk_t60.h"

/* one AI state step per 30 Hz frame (d_a_bk_t60.h bk_t60_ai_state): a state (action, mode)
 * entered on an in-between step (by the dispatch, a hit read, the shock / light checks, the water / abyss checks after
 * the dispatch) waits for the next frame: daCC_Execute notes the change at its end (frame-tag latch keyed by the actor's
 * action byte) and holds the dispatch on the frame's remaining steps; the motion keeps the speed the frame's dispatch
 * set. dt = 1 and N = 1: nothing is noted or held. */
static inline u32 cc_t60_ai_state(cc_class* i_this) {
    return (u32)(u8)i_this->mCurrAction | ((u32)(u8)i_this->mMode << 8);
}
/* a state entered by a fall-through into its case body has RUN in the call that entered it (d_a_bk_t60.h bk_t60_ai_ran,
 * the 30 Hz timer rule): not held. Port latch key: the action byte's address with the top bit set (a key no
 * guest address uses), value = the frame tag mixed with the state. dt = 1 / tick30: nothing. */
static inline u32 cc_t60_ran_key(cc_class* i_this) { return gabi::ea(&i_this->mCurrAction) ^ 0x80000000u; }
static inline u32 cc_t60_ran_tag(cc_class* i_this) {
    return bk_t60_frame_tag() ^ (cc_t60_ai_state(i_this) * 0x9E3779B1u) ^ 0x5A5A0000u;
}
static inline void cc_t60_ai_ran(cc_class* i_this) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(cc_t60_ran_key(i_this), cc_t60_ran_tag(i_this));
}
/* the current state was entered by a fall-through this step (cc_t60_ai_ran): its body runs on, the dispatch is not held,
 * so this call is not the frame's last dispatch call (the tick30 step's is). dt = 1 / tick30: false. */
static inline bool cc_t60_ai_ran_now(cc_class* i_this) {
    return t60::dt() != 1.0f && !t60::tick30() &&
           wwhd_t60_latch_pending(cc_t60_ran_key(i_this), cc_t60_ran_tag(i_this)) != 0;
}
static inline void cc_t60_ai_note(cc_class* i_this, u32 before) {
    if (t60::dt() == 1.0f || t60::tick30() || cc_t60_ai_state(i_this) == before)
        return;
    if (wwhd_t60_latch_pending(cc_t60_ran_key(i_this), cc_t60_ran_tag(i_this))) {
        wwhd_t60_latch_set(cc_t60_ran_key(i_this), ~cc_t60_ran_tag(i_this)); /* used: never matches again (~tag) */
        return;
    }
    wwhd_t60_latch_set(gabi::ea(&i_this->mCurrAction), bk_t60_frame_tag());
}
static inline bool cc_t60_ai_held(cc_class* i_this) {
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(gabi::ea(&i_this->mCurrAction), bk_t60_frame_tag()) != 0;
}

/* a per-frame s16 phase / angle step (visual: the swim bob, the damage swing, the stone shake, the squash wobble):
 * the step's share, truncated, and the rest of the frame's step on the tick30 step, so each 30 Hz frame adds exactly
 * the original's step (N = 2 / 4: exact halves / quarters of 0x800, 0x3000, 1000, 20000). dt = 1: the whole step. */
static inline s16 cc_t60_step_s16(s32 perFrame) {
    f32 dt = t60::dt();
    if (dt == 1.0f)
        return (s16)perFrame;
    s32 part = (s32)((f32)perFrame * dt);
    if (t60::tick30())
        return (s16)(perFrame - part * (t60::steps() - 1));
    return (s16)part;
}
