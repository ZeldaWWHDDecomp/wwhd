/* d_a_mo2_t60.h (WWHD, true 60 build): the Moblin's step helpers (mo2_exec, mo2_move; dt = 1/N,
 * 1.6).
 *
 * The Moblin is the Bokoblin's sibling (the same damage reaction, the same AI shape), so it takes the Bokoblin's step
 * helpers (d_a_bk_t60.h: the frame tag and set latch, bk_t60_first / bk_t60_end, bk_t60_counted, bk_t60_frame_is,
 * bk_t60_at_grid) and adds its own AI state signature for the one-state-step hold. dt = 1 (unconverted, N = 1) is the
 * original code: every helper returns the original's answer there.
 *
 * Where the per-frame work goes (the 30 Hz timer rule, 8, 11, 14), as for the Bokoblin:
 *   tick30 step  the catch timers (a timer set on an earlier step of the frame is not counted in that frame: first-step
 *                snapshot, d_a_mo2_exec.cpp), the blink (cM_rnd), the lighting (cM_rnd), smoke / spit / animation-stop
 *                counters, the parry window counter (the unconverted player reads it), the per-frame particles /
 *                sounds, the guard and push decisions (mo2_t60_frame_work), the demo camera and the catch event (event
 *                work, 30 Hz).
 *   first step   the AI timers (mTimers, side step, weapon run / view, dash stop, hit stun) and the phase counter count
 *                there, before the decisions they time; the timer `== v` one-shots and the timer- / phase-driven
 *                decisions fall there (the first-step AI-timer rule: the new state moves on every step of the frame). The collision
 *                results of the tick30 Move (the guard sphere's block, the body / head hits, the spear's At hit) are read
 *                there, so the hit gates (mNoHitTimer, mNoGuardTimer) and the paralysis delay count there, before the
 *                reads; the collider status's attacker ids shift there.
 * The spear's At spheres keep the 30 Hz grid while the player is unconverted (d_a_mo2_move.cpp yari_hit_check). */
#pragma once
#include "d/actor/d_a_bk_t60.h"
#include "t60_earlier_steps.h"
#include <cstdlib>

/* one AI state step per 30 Hz frame (d_a_bk_t60.h bk_t60_ai_state): a state
 * entered on an in-between step (by the dispatch, damage_check, damage_reaction or its result switch, the ice thaw) waits
 * for the next frame: daMo2_Execute marks the change (frame-tag latch keyed by the actor) and Mo2_move holds its
 * dispatch on the frame's remaining steps (the motion keeps the speed the frame's dispatch set). The state: the action and
 * mode of the damage reaction (the Moblin has no jump sub-steps; the attack's animation steps are animation-end
 * decisions, as the Bokoblin's mAttackStep). dt = 1 and N = 1: nothing is marked or held. */
template <class T> static inline uint64_t mo2_t60_ai_state(T* i_this) {
    return (uint64_t)(u16)(s16)i_this->mDamageReaction.mAction |
           ((uint64_t)(u16)(s16)i_this->mDamageReaction.mMode << 16);
}
template <class T> static inline u32 mo2_t60_ai_key(T* i_this) { return gabi::ea(&i_this->mDamageReaction.mAction); }
/* a state entered by a fall-through into its case body has RUN in the call that entered it (d_a_bk_t60.h bk_t60_ai_ran,
 * the 30 Hz timer rule): it is not held; its body continues on the frame's remaining steps with their dt
 * shares. Port latch: key = the mode's odd byte address, value = the frame tag mixed with the state, so a later change
 * in the same call is held again. dt = 1 / tick30: nothing. */
template <class T> static inline u32 mo2_t60_ran_tag(T* i_this) {
    uint64_t s = mo2_t60_ai_state(i_this);
    return bk_t60_frame_tag() ^ ((u32)s * 0x9E3779B1u) ^ (u32)(s >> 32) ^ 0x5A5A0000u;
}
template <class T> static inline void mo2_t60_ai_ran(T* i_this) {
    if (t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(&i_this->mDamageReaction.mMode) | 1u, mo2_t60_ran_tag(i_this));
}
template <class T> static inline void mo2_t60_ai_note(T* i_this, uint64_t before) {
    if (t60::dt() == 1.0f || t60::tick30() || mo2_t60_ai_state(i_this) == before)
        return;
    if (wwhd_t60_latch_pending(gabi::ea(&i_this->mDamageReaction.mMode) | 1u, mo2_t60_ran_tag(i_this))) {
        wwhd_t60_latch_set(gabi::ea(&i_this->mDamageReaction.mMode) | 1u, ~mo2_t60_ran_tag(i_this)); /* used: never matches again (~tag) */
        return;
    }
    wwhd_t60_latch_set(mo2_t60_ai_key(i_this), bk_t60_frame_tag());
}
template <class T> static inline bool mo2_t60_ai_held(T* i_this) {
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(mo2_t60_ai_key(i_this), bk_t60_frame_tag()) != 0;
}

/* fight_run's head turn on the held steps (t60_earlier_steps.h; pair, Moblin 47C59A80 frame 109):
 * A's one call turns current.angle.y toward mTurnTargetY at fight_run's head (scale 4, 0x400; 0x800 in mode 1) and then
 * switches mode (3 -> 2 on its timer). At N steps the switch came on step 1 after that step's turn share (+32 at N 4); the
 * frame's remaining steps were held (mo2_t60_ai_held) and never turned: 4674 for A's 4755, and a frame behind from then
 * on. Form: the head marks the frame when it turns on an earlier step (port latch, key the mTurnTargetY address ^
 * 0xE0000000, | 1 for 0x800); every held step after it turns once more (Mo2_move's held branch): the integer ease moves
 * the frame's split on the steps between and pins its full step from the frame start on the tick30 step, A's turn. The
 * held tick30 step first takes the tick30 sensing (mo2_t60_sense) and the head's target from it, as A's call (moF44,
 * 47C5CECC frame 300, mode 4 -> 2 on step 1: B turned toward the first step's target, 15627 for A's 15817).
 * dt 1: nothing is marked (the original). NC WWHD_T60_MO2HELDTURN_NEGCTRL=1: the held steps neither sense nor turn (the
 * form before). */
static inline bool mo2_t60_heldturn_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_MO2HELDTURN_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
template <class T> static inline u32 mo2_t60_headturn_key(T* i_this, bool wide) {
    return (gabi::ea(&i_this->mDamageReaction.mTurnTargetY) ^ 0xE0000000u) | (wide ? 1u : 0u);
}
template <class T> static inline void mo2_t60_headturn_note(T* i_this, s16 maxSpeed) {
    if (!mo2_t60_heldturn_nc())
        t60es::mark(mo2_t60_headturn_key(i_this, maxSpeed == 0x800));
}
/* on a held later step: the head turn's max step when the head turned on an earlier step of this frame, else 0 */
template <class T> static inline s16 mo2_t60_headturn_due(T* i_this) {
    if (mo2_t60_heldturn_nc() || !t60es::later())
        return 0;
    if (t60es::marked(mo2_t60_headturn_key(i_this, true)))
        return 0x800;
    if (t60es::marked(mo2_t60_headturn_key(i_this, false)))
        return 0x400;
    return 0;
}

/* the frame's last dispatch call. The per-frame work inside the dispatch (counts, the decisions drawn once per frame)
 * goes on the tick30 step; but when the state changes on an in-between step, the dispatch is held on the frame's remaining
 * steps (tick30 included), so the call that made the change is the frame's last dispatch call and does the frame's work,
 * as the original's one call per frame does it after the change (e.g. fight_run's push count in the call whose mode 2
 * chose mode 4). The change can come from damage_check before Mo2_move in the same execute (daMo2_Execute marks it,
 * key: the mode's address) or from Mo2_move itself (its entry state, d_a_mo2_move.cpp). */
template <class T> static inline void mo2_t60_note_before_move(T* i_this, uint64_t before) {
    if (t60::dt() == 1.0f || t60::tick30() || mo2_t60_ai_state(i_this) == before)
        return;
    wwhd_t60_latch_set(gabi::ea(&i_this->mDamageReaction.mMode), bk_t60_frame_tag());
}
template <class T> static inline bool mo2_t60_changed_before_move(T* i_this) {
    return t60::dt() != 1.0f &&
           wwhd_t60_latch_pending(gabi::ea(&i_this->mDamageReaction.mMode), bk_t60_frame_tag()) != 0;
}

/* a per-frame push (a line-check correction of a fraction of an offset per frame): x dt (the per-step movement rule) */
static inline f32 mo2_t60_rate(f32 perFrame) { return t60::dt() == 1.0f ? perFrame : gabi::fmuls_ppc(perFrame, t60::dt()); }

/* An animation set before the frame's tick30 step becomes a request, applied on the tick30 step after its play / calc
 * (option (a); replaces the "first played next frame" latch of).
 * daMo2_Execute plays and calcs the morf BEFORE its dispatch (damage_check, Mo2_move, damage_reaction), so A's anm_init
 * always follows the frame's whole play: the morf blends from the old animation's frame-END pose. Set on an earlier
 * step, the tick30 calc restored the frame-start pose (m_Do_ext_morf_t60.h) and the morf blended from there (pair
 * mopk2 frame 195: the left hand -325.79 in B, -327.93 in A, 36 off at 196). With the request the old animation plays
 * to the frame end on the remaining steps, the tick30 calc gives A's frame-end pose, and the request's setAnm follows it
 * at A's place (before Mo2_move). The request (bck, morf, mode, speed, sound, and a setFrame / setPlaySpeed made right
 * after the set in the same call) lives in d_a_mo2.cpp, anm_init's unit (the split-actor state rule: no state shared through inline /
 * static globals); the set latch on &mpMorf (bk_t60_mark_set) tells the other units that one is pending this frame.
 * The apply is exported from d_a_mo2.cpp (default visibility, the fast build keeps it global); daMo2_Execute calls it
 * weakly (game build only; verify builds have no request to apply: the units' anm_init calls are the original code).
 * dt = 1 (N = 1, unconverted): nothing is requested, the original code. */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" __attribute__((weak)) void wwhd_t60_mo2_anm_apply(void* mo2);
template <class T> static inline void mo2_t60_anm_apply(T* i_this) {
    if (bk_t60_end() && bk_t60_pending(&i_this->mpMorf) && wwhd_t60_mo2_anm_apply)
        wwhd_t60_mo2_anm_apply(i_this);
}
#else
template <class T> static inline void mo2_t60_anm_apply(T*) {}
#endif
