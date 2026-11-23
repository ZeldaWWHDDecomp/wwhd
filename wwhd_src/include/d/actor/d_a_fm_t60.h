/* d_a_fm_t60.h (WWHD, true 60 build, 2026-11-19): Floormaster (d_a_fm*.cpp, daFm_c) on a general
 * step length (dt = 1/N). Decomp build only. Separate file:
 *the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Magtail's form: the frame decides from other
 * actors' state (the base target Link, the grab target found by process id: a bomb's state, an NPC, a pot; Link's
 * grab / control status, the event demos) and the tick30 collision hits, and it writes into the grabbed actor (grabPlayer:
 * daPy_setPlayerPosAndAngle_mtx from the hand joint; grabNPC / grabBomb / grabTsubo place theirs). So _execute runs
 * whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form): every
 * decision with the other actors' current state, the writes into the grabbed actor once per frame, the collider
 * registered once for the tick30 collision Move (one-sweep-per-frame attack), the push / move through fopAcM_posMoveF once. The earlier steps
 * change no logic state. The picture: frame interpolation (the body morf and the hole model are drawn by this process: blended
 * between the frame ends; a picture step's draw is kept and put back). The draws (holeDraw / bodyDraw) write no logic
 * state. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs _execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct daFm_c;
namespace fm_t60 {
static inline const t60w::Ops<daFm_c>& ops() { return t60objw::Logic<daFm_c>::kOps; }
}  // namespace fm_t60
