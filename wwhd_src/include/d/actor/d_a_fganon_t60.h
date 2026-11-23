/* d_a_fganon_t60.h (WWHD, true 60 build, 2026-11-19): Phantom Ganon (d_a_fganon.cpp,
 * fganon_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's source keeps thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Gohdan / Big Octo whole form: Phantom Ganon decides from Link's
 * position, his sword hits (the tick30 collision Move: damage_check, the parried energy ball's return), the energy balls
 * it spawns and searches (fgmahou 0xF2, mahou_se_set), the event / demo system (start / end / last_end demos and
 * demo_camera) and its own frame timers, anim frame checks and cM_rnd draws; the actions of both board parts (#1 / #2:
 * move, shot2, spinattack2, down, damage_check, energy_ball_move, execute_collision) run inside daFganon_Execute. So
 * daFganon_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h,
 * t60objw form): every decision with Link's and the balls' state as A reads it (process-order read: Link earlier in the line, current;
 * the balls later, unchanged before their own tick30 step), every timer, anim frame and random draw once per frame, the
 * colliders registered once for the tick30 collision Move, the demo camera set once per frame. The earlier steps change
 * no logic state. The picture: frame interpolation (the morf and sword models are drawn by this process). The draw: the sword's
 * light state (mKenTevStr, settingTevStruct) is made on the tick30 step's draw only (rule 14); the model visibility flags
 * (from mbIsMaterialized), the brk / btk entries and the snap registration are picture / idempotent. Never held. dt 1:
 * the original. Negative control: T60C_NEGCTRL runs daFganon_Execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct fganon_class;
namespace fganon_t60 {
static inline const t60w::Ops<fganon_class>& ops() { return t60objw::Logic<fganon_class>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace fganon_t60
