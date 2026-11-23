/* d_a_kb_t60.h (WWHD, true 60 build, 2026-11-19): pig (d_a_kb.cpp, d_a_kb_exec.cpp,
 * d_a_kb_move.cpp, kb_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum, carryables form: the pig's frame decides from Link (his
 * attack hits and the anger counter, the carry check / throw, the bait demo), the tick30 collision hits and pushes
 * (m_cc_move, first-step push), the ground / water checks and the event system, and its eight timers, RNG draws and morf play
 * advance once per 30 Hz frame. So daKb_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the
 * frame-start state (d/t60_whole.h): every decision with the other actors' current state, the colliders registered
 * once for the tick30 collision Move (one-sweep-per-frame attack), the move once, a delete request at the frame's end (first-step AI-timer addendum 2). The
 * earlier steps change no logic state. Held while carried (mMode 1, carry_move): Link places it on every step, so no
 * picture step is announced for it then.
 * The picture: frame interpolation (n 0: no values latched). The morf is calculated in the execute; daKb_Draw makes the light
 * state (settingTevStruct) on the tick30 step's draw only (rule 14), the rest of the draw (btp entry, the anger tint
 * into the copy mTevStr) writes no logic state. daKb_Create's draw_SUB pose is the original at dt 1 (createBegin / createEnd).
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct kb_class;
namespace kb_t60 {
static inline void get(kb_class*, f32*) {}
static inline void set(kb_class*, const f32*) {}
static inline void mtx(kb_class*) {}
/* carried by Link (carry_move): placed by its holder on every step (read only at dt != 1) */
static inline bool held(kb_class* a) { return *gabi::at<be<s16>>(gabi::ea(a) + 0x766) /* mMode */ == 1; }
static constexpr t60w::Ops<kb_class> kOps = {0, 0, get, set, mtx, held};
static inline const t60w::Ops<kb_class>& ops() { return kOps; }
/* daKb_Create's draw_SUB call: the original at dt 1 (create's pose, not a frame step) */
/* as plain calls (a struct local's constructor / destructor never runs under the source glue: the CreateRun scope
 * object left the call at the step's dt, scopelint): createBegin() sets dt 1 and returns the previous value,
 * createEnd() puts it back */
static inline f32 createBegin() { return t60_set_dt(1.0f); }
static inline void createEnd(f32 previous) { t60_set_dt(previous); }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace kb_t60
