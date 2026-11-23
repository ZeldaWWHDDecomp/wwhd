/* d_a_oship_t60.h (WWHD, true 60 build, 2026-11-19): the Great Sea enemy warship
 * (d_a_oship.cpp, daOship_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Magtail's / Floormaster's whole form: the
 * frame decides from other actors' state (the target position mTargetPos from Link's ship / Link, the range modes by
 * Link's distance, plFireRepeat by the boat, the wave distance to Link), the tick30 collision hits (checkTgHit: the
 * damage / delete modes) and the bombs it fired (found by process id; their flight registers), and it starts the
 * cannon bombs (attackCannon) and the smoke / wave / splash effects. So _execute runs whole once per 30 Hz frame on the
 * tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form): every decision with the other actors'
 * current state, the path move, the aim chases (cLib_addCalcAngleS2 on mAimRotX / mAimRotY), the bombs and effects
 * started once per frame, the collider registered once for the tick30 collision Move (one-sweep-per-frame attack). The earlier steps change no
 * logic state. The picture: frame interpolation (the ship model is calculated in _execute, its node callback sets the two cannon
 * joints from mAimRotX / mAimRotY inside that calc; the model is drawn by this process, so the port blends its joint
 * matrices between the frame ends; a picture step's draw is kept and put back). The draw (_draw) writes no logic state:
 * its light state (settingTevStruct into tevStr) is made on the tick30 step's draw only (rule 14). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct daOship_c;
namespace oship_t60 {
static inline const t60w::Ops<daOship_c>& ops() { return t60objw::Logic<daOship_c>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace oship_t60
