/* d_a_bmd_t60.h (WWHD, true 60 build; / bmd_exec / bmd_move / bmd_camera, 2026-11-19): Kalle
 * Demos, core (d_a_bmd*.cpp, bmd_class: one process) on a general step length (dt = 1/N). Decomp build
 * only. Separate file: the actor's sources keep thin
 * calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form; as the Helmaroc King's bdk line): the frame
 * reads Link (the eat / grab windows, the ride on the flower), the hands / feet and Makar (mk_move), the tick30
 * collision hits (core_damage_check), its timers (mTimers[4], mDamageTimer, mBgOffTimer, mVineCutTimer, mAttackTimer)
 * gate every action of move (d_a_bmd_move.cpp), and it moves background: the flower body and the five petals
 * (pm_bgw[0..5], dBgW_Move: colliders + platforms are coupled parties, rule 14). The demo camera (demo_camera,
 * d_a_bmd_camera.cpp), bmd_kankyo, eff_cont and the camera type force step once per frame. So daBmd_Execute runs whole
 * once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the
 * earlier steps change no logic state; the background moves once per frame, after the frame's rides.
 * ride_call_back (the flower's ride callback, called on each of the rider's steps): only the tick30 step's call counts
 * (t60objw::cbSkip, the msw / leaflift form): the pull to the centre (mFlowerState 3, mRideCaught) and the core hide
 * (mBgOffTimer 20) once per frame, as the tick30 execute reads them.
 * The picture: frame interpolation (body / head / dead-head / Makar morfs and the face / R00_EF models drawn by this process:
 * blended between the frame ends; a picture step's draw is kept and put back). daBmd_Draw: settingTevStruct of the
 * actor's tevStr on the tick30 step's draw only (rule 14); the BG1 light state goes into a draw-local copy (picture
 * only); mBlurRate's 1 -> 0 is visual (draw keep).
 * daBmd_Create does not call the execute. Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bmd_class;
namespace bmd_t60 {
static inline const t60w::Ops<bmd_class>& ops() { return t60objw::Logic<bmd_class>::kOps; }
/* a draw-side state change: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
/* the ride callback: only the tick30 step's call counts */
static inline bool cbSkip() { return t60objw::cbSkip(); }
}  // namespace bmd_t60
