/* d_a_btd_t60.h (WWHD, true 60 build, 2026-11-19): Gohma (d_a_btd.cpp,
 * d_a_btd_damage.cpp, btd_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form; same as the Helmaroc King's bdk line): the frame
 * reads Link (position, eye, cut type: the eyelid close, the track / attack choices, the demo placement of the player),
 * Valoo's tail (dr2: the ceiling hits) and the kui actor found by search, the tick30 collision hits (damage_check in
 * d_a_btd_damage.cpp: eye, body, the stun window), the boost item near the eye, and its four timers mTimers[0/1],
 * mBlinkTimer, mStunTimer plus mActionState that gate every action (move: wait / jab / punch / fire / high fire /
 * turning punch / damage / start demo / end); the demo camera (btdExecCamera: dCamera_c Set / Start / Stop), the quakes,
 * the debris / splash chains, the effects (btd_effect) and the environment light (btd_kankyo) step once per frame. So
 * daBtd_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h,
 * t60objw form, n 0); the earlier steps change no logic state.
 * The picture: frame interpolation. The body morf (base matrix and calc in the execute; nodeCallBack writes mJointPos /
 * mEyeJointPos / eyePos / attention there, on the execute's calc), the dead-head morf and the 35 debris models are
 * models drawn by this process, so the port's draw history blends them between the frame ends; a picture step's draw
 * is kept and put back (draw keep), so the draw's own writes (mTevstr, mBlurRate's 1 -> 0, the btk / brk entries) leave
 * nothing behind. settingTevStruct in daBtd_Draw is made on the tick30 step's draw only (rule 14).
 * daBtd_Create does not call the execute. Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct btd_class;
namespace btd_t60 {
static inline const t60w::Ops<btd_class>& ops() { return t60objw::Logic<btd_class>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace btd_t60
