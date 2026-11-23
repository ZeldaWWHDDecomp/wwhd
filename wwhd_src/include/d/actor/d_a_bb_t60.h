/* d_a_bb_t60.h (WWHD, true 60 build, 2026-11-19): Kargaroc (d_a_bb.cpp, d_a_bb_exec.cpp,
 * d_a_bb_move.cpp, bb_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads and writes), the Magtail / Floormaster whole form: the frame decides from
 * Link (distance / angles each frame, the view and BG line checks, the swoop at him), the bait (search_esa), the tick30
 * collision hits (damage_check) and the switch it waits on, and it writes into the actor it carries (mCarryState 3:
 * the carried Bokoblin / Moblin's position, angles and carry offsets, the drop) - as the Floormaster's grab. Its six
 * timers, the frame counter, the RNG draws, the tail chain (tail_control: follow-the-leader per frame) and the morf play
 * advance once per 30 Hz frame. So daBb_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the
 * frame-start state (d/t60_whole.h, t60objw form): every decision with the other actors' current state, the writes into
 * the carried actor once per frame, the colliders registered once for the tick30 collision Move (one-sweep-per-frame attack), the move once,
 * a delete request at the frame's end (first-step AI-timer addendum 2). The earlier steps change no logic state.
 * The picture: frame interpolation. The body morf is calculated in the execute; daBb_Draw enters it, the eye btp frame and the
 * nine tail models (base matrices from the execute's tail positions / angles), writing no logic state; the port's draw
 * history blends every model the process draws between the frame ends. daBb_Create calls daBb_Execute once (the first
 * frame's pose): that call is the original at dt 1 (createBegin / createEnd). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bb_class;
namespace bb_t60 {
static inline const t60w::Ops<bb_class>& ops() { return t60objw::Logic<bb_class>::kOps; }
/* daBb_Create's execute call: the original at dt 1 (t60w::on() is false at dt 1, so the Guard does nothing) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
}  // namespace bb_t60
