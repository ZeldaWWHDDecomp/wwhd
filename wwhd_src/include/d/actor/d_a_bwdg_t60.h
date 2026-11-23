/* d_a_bwdg_t60.h (WWHD, true 60 build, 2026-11-19): Molgera's sand floor (d_a_bwdg.cpp, bwdg_class) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form: the frame reads Molgera (bwd: wave speed, dent centre and depth), advances the wave phase
 * (wave_cont) and moves its height-field background (dBgWHf MoveHf). So daBwdg_Execute 021043EC runs whole once per
 * 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier steps
 * change no logic state (the background moves once per frame, rule 14). No model of its own: the sand packet is drawn
 * from the background's vertices (the 30 Hz surface; no in-between picture).
 * daBwdg_Draw: settingTevStruct on the tick30 step's draw only (rule 14). Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bwdg_class;
namespace bwdg_t60 {
static inline const t60w::Ops<bwdg_class>& ops() { return t60objw::Logic<bwdg_class>::kOps; }
/* the draw's logic-side stores: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bwdg_t60
