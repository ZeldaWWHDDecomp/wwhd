/* d_a_kita_t60.h (WWHD, true 60 build, 2026-11-19): the Forbidden Woods hanging flower platform
 * (d_a_kita.cpp, kita_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's source keeps thin calls.
 *
 * the whole-frame form rule + addendum, whole form (the rope lift klft's form, d/t60_objw.h): the frame decides from its ropes
 * (mRopeState, cut by the SHAND actors), Link's ride (ride_call_back: tilt / sway / sink, mExecuteCount), the tick30
 * collision hit (the push on the water), the ground / water checks, and counts per frame (mMoveCounter drives the tilt
 * and sway sines, mExecuteCount, mPushTimer). So daKita_Execute runs whole once per 30 Hz frame on the tick30 step at
 * dt 1 from the frame-start state; the earlier steps change no logic state. The picture: frame interpolation (the base matrix
 * between the frame ends). The BG (dBgW_Move) moves once a frame, so a rider follows it once a frame (accepted
 * staircase).
 * ride_call_back: called from the rider's ground check on each of its steps: only the tick30 step's call counts
 * (t60objw::cbSkip); the rider's old.pos as A reads it is its frame-start position (the process-order read rule,
 * t60objw::fsOldEarlier), as klft.
 * daKita_Create runs the execute twice (the first frame's pose): the original at dt 1 (cbBegin / cbEnd).
 * The draw's light state (settingTevStruct) on the tick30 step's draw only (rule 14).
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct kita_class;
namespace kita_t60 {
static inline const t60w::Ops<kita_class>& ops() { return t60objw::Logic<kita_class>::kOps; }
/* a run at dt 1, as plain calls (a struct local with a constructor / destructor does not run them under the source glue:
 * the CreateRun / CbRun scope objects left dt at the rider's 1/N, kitD1_2 [kitadbg]): cbBegin() sets dt 1 and returns
 * the previous value, cbEnd() puts it back. Used by daKita_Create's two execute calls (the first frame's pose; t60w::on()
 * is false at dt 1, so the Guard does nothing) and by ride_call_back's counted call (the rider's tick30 step: A's
 * once-a-frame call, whose chases (mPosRel.y cLib_addCalc2, the tilt angles, the sway) moved half the way at the
 * rider's dt, kitP1_2 frame 78: mPosRel.y -5 vs A -10). dt 1: nothing changes. */
static inline f32 cbBegin() { return t60_set_dt(1.0f); }
static inline void cbEnd(f32 previous) { t60_set_dt(previous); }
/* ride_call_back: only the tick30 step's call counts */
static inline bool cbSkip() { return t60objw::cbSkip(); }
/* the rider's old.pos as A reads it (process-order read): its frame-start position when it has executed on this step, else the live one */
static inline cXyz* riderOld(fopAc_ac_c* rider, cXyz* buf) {
    return t60objw::fsOldEarlier(gabi::ea(rider), buf) ? buf : &rider->old.pos;
}
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace kita_t60
