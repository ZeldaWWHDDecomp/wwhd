/* d_a_tn_t60.h (WWHD, true 60 build): the Darknut's step helpers (tn#2; dt = 1/N). .
 *
 * The Darknut uses the same damage reaction as the Bokoblin and the Moblin (mDamageReaction: action, mode), so it takes
 * their step helpers (d_a_bk_t60.h: the frame tag and set latch, bk_t60_first / bk_t60_end; d_a_mo2_t60.h: the AI state
 * signature of the damage reaction and the one-state-step hold). dt = 1 (unconverted, N = 1) is the original code: every
 * helper returns the original's answer there.
 *
 * Where the per-frame work goes (the 30 Hz timer rule, 8, 11, 14; the first-step AI-timer rule), the Moblin form (daTn_Execute plays its animations in
 * executePrefix, before the dispatch Tn_move):
 *   first step   executePrefix's counts (mFrameCount, mCountDownTimers, the weapon run / search wait, armour shake, hit
 *                stun, no-damage / no-guard timers, the status timer), the collision results of the tick30 Move
 *                (prepareDamage: the collider status's attacker ids shift, the guard sphere's count, the hit reads, the
 *                damage they cause and updateKnockback's draw), Tn_move's counts (move target, paralysis delay) and its
 *                timer `== v` one-shots and phase (mFrameCount & mask) decisions with their cM_rnd draws: the new state
 *                moves on every step of the frame with its dt share, as the original counts, decides, then moves.
 *   tick30 step  the freeze timer's one-shots (enemy_ice counts it there, c_damagereaction.cpp).
 *   tick30 step  also the lighting (finishNormalEffects, last in the original: the flicker draw after the frame's AI
 *                draws, from the frame's end position) and the cut-turn timer (counted after the dispatch).
 *   parts        part_move every step: its counter, timer and the detach / release decisions (mPartDetachFlags, cleared
 *                after the first step's read) on the first step, a state entered there first flies next frame; the
 *                flight with dt (pos += speed x dt, gravity 5 x dt, the spin's whole speed per frame); placement per step.
 *   every step   the dispatch (unless held), the motion, the animation play (a new animation set before the tick30 step
 *                is a request, set on the tick30 step after its play: d_a_tn.cpp tn_t60_anm_apply),
 *                updateDamageAnimation and updateHeldWeapon (event-driven: the weapon drop draws once).
 * Cross-actor decisions (the first-step input rule addendum "converted Link earlier in the line", with animation-driven decision
 * and the whole-frame form addendum; pair: the spotting decision A 62 / B 63): Link executes before the Darknut, so A's one
 * frame reads Link's frame-END state. The frame's sensing (mPlayerDistance, mPlayerHeightDistance, mTargetAngle: Link
 * against the Darknut's frame-START position, before its own motion) is taken again on the tick30 step with Link's
 * current state there (tn_t60_sense_*), and every decision that reads Link (distance / view / way / cut type / guard
 * stance / line of sight, the nearby-bomb check with them) is taken on the tick30 step only (tn_t60_xdecide); a decision
 * body that only decides (combat mode 2, circle mode 12, the -20 jump-back draw) runs whole there at dt 1, earlier steps
 * keep the frame's motion. A mode change from such a decision is then the frame's last change, as in A.
 * Open (tn#2): the per-frame particles / sounds of the separate actions (yogan_fail, s_demo, b_nige). */
#pragma once
#include "d/actor/d_a_mo2_t60.h"

/* the animation crossing-check rule: an animation-frame `(int)frame == v` one-shot (sound, particle, smoke start) once per 30 Hz frame: on the step
 * whose play entered v (d_a_bk_t60.h bk_t60_frame_is; a stopped animation fires on the tick30 step). dt = 1: `== v`. */
static inline bool tn_t60_frame_is(const mDoExt_McaMorf* morf, int v) {
    return bk_t60_frame_is(static_cast<f32>(morf->mFrameCtrl.mFrame), static_cast<f32>(morf->mFrameCtrl.mRate), v);
}

/* a decision that reads Link's (another actor's) state: on the tick30 step (first-step input addendum).
 * dt = 1: every call (the original). */
static inline bool tn_t60_xdecide() { return bk_t60_end(); }
