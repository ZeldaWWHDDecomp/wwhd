/* d_a_bwds_t60.h (WWHD, true 60 build, 2026-11-19): Molgera's larvae, Molgerita (d_a_bwds.cpp / d_a_bwds_exec.cpp, bwds_class) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (as Gohma's btd): the frame reads Molgera (bwd) and Link (the hookshot), the tick30 collision
 * hits and the sand ground check, counts the timers and draws random numbers. So daBwds_Execute 0210602C runs whole once
 * per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier
 * steps change no logic state. The picture: frame interpolation (body and splash morfs blended between the frame ends).
 * daBwds_Draw: settingTevStruct on the tick30 step's draw only (rule 14). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bwds_class;
namespace bwds_t60 {
static inline const t60w::Ops<bwds_class>& ops() { return t60objw::Logic<bwds_class>::kOps; }
/* the draw's logic-side stores: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bwds_t60
