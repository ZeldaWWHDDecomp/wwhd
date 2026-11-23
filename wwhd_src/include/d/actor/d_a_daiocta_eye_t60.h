/* d_a_daiocta_eye_t60.h (WWHD, true 60 build, 2026-11-19): the Big Octo's eyes
 * (d_a_daiocta_eye.cpp, daDaiocta_Eye_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Big Octo's whole form (d_a_daiocta_t60.h): an
 * eye's frame is read by its parent (isLivingEye / isDamageEye / isDamageBombEye: mbIsDead, the per-frame flags
 * mbIsDamaged / mbIsDamagedBomb cleared at the frame start), takes its hits from the tick30 collision (checkTgHit ->
 * mStts.Move / GetTgHitObj, the bomb hit coHit_CB) and runs the damage scale anime (mScaleAnimeIdx) and the brk / btk
 * anims (play, isStop) once per frame. The parent runs whole on the tick30 step, so the eye does too: _execute runs
 * whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form), in the
 * original process order after the parent; the collider is registered once for the tick30 collision Move (one-sweep-per-frame attack). The
 * earlier steps change no logic state. The picture: frame interpolation (the eye model's base matrix is set in setMtx, so the port
 * blends it between the frame ends; a picture step's draw is kept and put back). The draw writes no logic state: the
 * light state (eyeSetLighting = settingTevStruct into tevStr) is made on the tick30 step's draw only (rule 14). Never
 * held. dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct daDaiocta_Eye_c;
namespace daiocta_eye_t60 {
static inline const t60w::Ops<daDaiocta_Eye_c>& ops() { return t60objw::Logic<daDaiocta_Eye_c>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace daiocta_eye_t60
