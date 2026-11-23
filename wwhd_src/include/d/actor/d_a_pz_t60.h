/* d_a_pz_t60.h (WWHD, true 60 build, 2026-11-19): Princess Zelda in Ganon's Tower (pz,
 * d_a_pz*.cpp, daPz_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule "NPC form" (an NPC actor, fopNpc_npc_c; same form as the Magtail / Floormaster whole form): the modes (wait, talk,
 * afraid, attack wait / attack with the bow, down) and the demo decide from Link, Ganondorf (his position, battle
 * phase and demo state), the event system and the tick30 collision, and the talk-wait timer, the eye / btk / brk
 * frames, the posMove / BG check and the morf advance once per 30 Hz frame. So daPz_c::_execute runs whole once per
 * 30 Hz frame on the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60npc form); the earlier steps
 * change no logic state (one AI state step per 30 Hz frame, the check item, holds by construction). The
 * picture: frame interpolation. The execute ends in setMtx and the light state, so the light state is made on the tick30 step
 * only; daPz_c::_draw writes no logic state. Never held. dt 1: the original. Negative control: T60C_NEGCTRL runs the
 * execute on every step at dt. */
#pragma once
#include "d/t60_npc.h"

struct daPz_c;
namespace pz_t60 {
static inline const t60w::Ops<daPz_c>& ops() { return t60npc::Logic<daPz_c>::kOps; }
}  // namespace pz_t60
