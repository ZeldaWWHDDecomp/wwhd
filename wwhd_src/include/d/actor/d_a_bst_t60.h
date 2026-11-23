/* d_a_bst_t60.h (WWHD, true 60 build; / bst_camera / bst_execute / bst_move, 2026-11-19):
 * Gohdan (d_a_bst*.cpp, bst_class: the head and the two hands) on a general step length (dt = 1/N).
 * Decomp build only. Separate file: the actor's
 * sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads), the Magtail's / Big Octo's whole form: the three
 * parts read each other through the shared globals (boss / hand[0..1]: mActionType, mDamage, mDemoCamMode), Link's
 * position and height, the event / message system (the intro and defeat demos, bst_camera's demo camera, the nose item
 * message) and their own frame timers (mTimers[0..4], mState, mDamageTimer, mStateTimer, the hurt / shake timers,
 * mUpdateLastFacingDirIfMultipleOf32), anim frame checks and cM_rnd draws; the actions in d_a_bst_move.cpp and the
 * attack / demo steps in d_a_bst_{paa,beam,hana,harai,kumi}.cpp all run inside daBst_Execute. So daBst_Execute runs
 * whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form): every
 * decision with the other parts' current state, every timer, anim frame and random draw once per frame, the colliders
 * registered once for the tick30 collision Move (one-sweep-per-frame attack), the demo camera set once per frame. The earlier steps change no
 * logic state. The picture: frame interpolation (the morf, pedestal and tongue models are drawn by this process, so the port
 * blends their joint matrices between the frame ends; a picture step's draw is kept and put back). The draw: the three
 * light states (tevStr, mEnvLight, mBgTevStr) are made on the tick30 step's draw only (rule 14), and so is the blur's
 * end (mBlurRate 1 -> 0, set by the execute); the blur rate (blur > 1) and the one-time centre position are picture /
 * idempotent. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs daBst_Execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bst_class;
namespace bst_t60 {
static inline const t60w::Ops<bst_class>& ops() { return t60objw::Logic<bst_class>::kOps; }
/* the draw's light states and the blur's end: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bst_t60
