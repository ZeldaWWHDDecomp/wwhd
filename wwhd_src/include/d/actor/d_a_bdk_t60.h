/* d_a_bdk_t60.h (WWHD, true 60 build, 2026-11-19): Helmaroc King, battle (d_a_bdk*.cpp,
 * bdk_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form): the frame reads Link (position, the attack
 * and wind decisions, the demo placement of the player), the tick30 collision hits (damage_check: mask, tosaka, body),
 * the background (mAcch.CrrPos, the ground timer) and the shared five-timer set mTimers[] that gates every action
 * (bdk_move / bdk_act / bdk_demo); its tails are a follow-the-leader chain per frame (tail_control from the node
 * callback's root positions), the effects and objects step once per frame (my_effect_move, obj_move). So
 * daBdk_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h,
 * t60objw form, n 0); the earlier steps change no logic state.
 * The picture: frame interpolation. The body morf (mpMorf, base matrix and calc in the execute; the node callback writes the tail
 * roots there), the mask (mp8F0), the 36 tail segment models, the mask pieces and the effect / object models get their
 * base matrices in the execute or the draw and are calced in daBdk_Draw (mDoExt_modelUpdateDL); every one is a model
 * drawn by this process, so the port's draw history blends it between the frame ends; a picture step's draw is kept and
 * put back (draw keep), so the draw's own writes (tevStr, mTailDrawDelay, mBlurRate) leave nothing behind.
 * settingTevStruct (the background light state mBgTevStr) is made on the tick30 step's draw only (rule 14).
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bdk_class;
namespace bdk_t60 {
static inline const t60w::Ops<bdk_class>& ops() { return t60objw::Logic<bdk_class>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bdk_t60
