/* d_a_bwd_t60.h (WWHD, true 60 build; / bwd_execute / bwd_move / bwd_camera / bwd_end, 2026-11-19): Molgera (d_a_bwd*.cpp, bwd_class) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form, as Gohma's btd): the frame reads Link (distance, the hookshot pull), the
 * larvae (bwds, ko_count search), the tick30 collision hits and the sand ground check, counts mFrameCounter and the
 * timers, runs move / the demo camera / end with their random draws, starts the BGM and creates the sand (bwdg, 0xDB)
 * once. So bwd_execute (daBwd_Execute 020FA614: move, camera, end and kankyo inside) runs whole once per 30 Hz frame on the
 * tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier steps change no logic
 * state. The picture: frame interpolation (head morf, tongue and body segment models blended between the frame ends).
 * daBwd_Draw: settingTevStruct (body and platform) and the blur-rate reset (mBlurRate = 0) on the tick30 step's draw only
 * (rule 14); the blur calls themselves (graphics) stay on every draw.
 * daBwd_Create does not call the execute. Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bwd_class;
namespace bwd_t60 {
static inline const t60w::Ops<bwd_class>& ops() { return t60objw::Logic<bwd_class>::kOps; }
/* the draw's logic-side stores: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bwd_t60
