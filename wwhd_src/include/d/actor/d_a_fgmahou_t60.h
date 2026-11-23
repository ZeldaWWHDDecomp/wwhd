/* d_a_fgmahou_t60.h (WWHD, true 60 build, 2026-11-19): Phantom Ganon's energy ball
 * (fgmahou, d_a_fgmahou.cpp, fgmahou_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule addendum (cross-actor reads and writes), the Magtail / Floormaster whole form: the flight, the aim at
 * Link / Phantom Ganon, the rally (deflect) window, the hit checks and the delete countdown decide from Link, Phantom
 * Ganon (fganon), the tick30 collision hits and the BG check, and the age / timers, the brk / btk frames and the morf
 * advance once per 30 Hz frame. So daFgmahou_Execute runs whole once per 30 Hz frame on the tick30 step at dt 1 from
 * the frame-start state (d/t60_whole.h, t60objw form); the earlier steps change no logic state (one AI state step per
 * 30 Hz frame, the check item, holds by construction). The picture: frame interpolation. The execute ends in the model
 * matrix, the morf calc and the emitter's matrix, so they are made on the tick30 step only; daFgmahou_Draw writes no
 * logic state. Create has no pose call. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the
 * execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct fgmahou_class;
namespace fgmahou_t60 {
static inline const t60w::Ops<fgmahou_class>& ops() { return t60objw::Logic<fgmahou_class>::kOps; }
}  // namespace fgmahou_t60
