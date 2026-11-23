/* d_a_bmdhand_t60.h (WWHD, true 60 build, 2026-11-19): Kalle Demos, vine hand (d_a_bmdhand.cpp, bmdhand_class) on a
 * general step length (dt = 1/N). Decomp build only. Separate
 * file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form; the core d_a_bmd_t60.h has the same form):
 * the frame reads the core (the boss found by search: its mode, the vine command) and Link (the grab / cut checks,
 * cut_control), the tick30 collision hits, and writes into the core (mVineCutTimer, mVineCount: the process-order read rule addendum, with
 * every party whole on the tick30 step the writes land in process order as in A); its timers (mTimers[4], mCcOffTimer,
 * mBloodTimer, mCloseTimer) gate hand_move. So daBmdhand_Execute runs whole once per 30 Hz frame on the tick30 step
 * at dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier steps change no logic state.
 * The picture: frame interpolation (the hand morf is calculated in the draw by updateDL from the execute's matrices, no joint
 * callback: picture only; blended between the frame ends, a picture step's draw kept and put back). settingTevStruct
 * in daBmdhand_Draw on the tick30 step's draw only (rule 14).
 * The create does not call the execute. Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bmdhand_class;
namespace bmdhand_t60 {
static inline const t60w::Ops<bmdhand_class>& ops() { return t60objw::Logic<bmdhand_class>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bmdhand_t60
