/* d_a_boko_t60.h (WWHD, true 60 build, 2026-11-19): the dropped / carried weapon (Boko stick,
 * machete, Moblin spear, Darknut sword, ...: d_a_boko.cpp, daBoko_c) on a general step length (dt = 1/N).
 * Decomp build only. Separate file: the actor's source
 * keeps thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's / the carryables' form): the frame decides from its carrier (the base matrix
 * the carrier sets through daBoko_setMatrix on each of its steps, actor_status 0x2000, the throw flag), Link's position
 * and shape angle (the throw, the put-down line check), the tick30 collision hits (the flame lit / put out by the hit's
 * attack type), the ground / wall / water checks and the mover (bokoCallMove), and counts per frame (mSpawnDelay, the
 * flame timer, mChainTimer, the chain line keCalc). So daBoko_c::execute runs whole once per 30 Hz frame on the tick30
 * step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier steps change no logic state.
 * The carrier's matrix as the tick30 step reads it: the carrier's latest write; the weapon is created after its carrier
 * (later in the process line), so that is the carrier's tick30 write, the original's frame end (pairs check it).
 * The picture: frame interpolation (the model's joints blended between the frame ends; the picture-step draw kept / put back).
 * The draw's light state (settingTevStruct) on the tick30 step's draw only (rule 14). No create-time execute call.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct daBoko_c;
namespace boko_t60 {
static inline const t60w::Ops<daBoko_c>& ops() { return t60objw::Logic<daBoko_c>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace boko_t60
