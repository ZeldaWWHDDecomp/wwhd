/* d_a_bpw_t60.h (WWHD, true 60 build, 2026-11-19): Jalhalla (d_a_bpw.cpp, bpw_class:
 * the body, its lantern, the damage balls and the possessed Poes, one profile 0x00D3 by mType) on a general step length
 * (dt = 1/N). Decomp build only. Separate file:
 * the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form, as Gohma's btd): every type's frame reads Link (position, the lantern /
 * Poe split windows, the curse), the parent found by search (mParentId), the tick30 collision hits (body_atari_check,
 * kantera_atari_check) and its ten countdown timers that gate every action (start demo, move, bunri split, fire, end);
 * the blur rate, the vibration (vib_mode_check), the effects and the environment change (kankyou_hendou) step once per
 * frame. So daBPW_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state
 * (d/t60_whole.h, t60objw form, n 0); the earlier steps change no logic state.
 * The picture: frame interpolation. The morf is a model drawn by this process (the draw places its base matrix and calcs it), so
 * the port's draw history blends it between the frame ends; a picture step's draw is kept and put back (draw keep).
 * daBPW_Draw: settingTevStruct (body_draw_SUB, kantera_draw_SUB) and the blur-rate reset (mBlureRate = 0) on the tick30
 * step's draw only (rule 14); the blur calls themselves (graphics) stay on every draw.
 * daBPW_Create does not call the execute. Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bpw_class;
namespace bpw_t60 {
static inline const t60w::Ops<bpw_class>& ops() { return t60objw::Logic<bpw_class>::kOps; }
/* the draw's light state and logic stores: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bpw_t60
