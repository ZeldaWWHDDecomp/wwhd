/* t60_npc.h (WWHD, true 60 build; NPC rows, 2026-11-18): the NPC form (the whole-frame form rule "NPC form")
 *for NPC actors, logic only, through d/t60_whole.h. Decomp build only.
 * Separate file: actor sources keep thin calls.
 *
 * An NPC's frame decides from the event system and other actors (partner search, event order / talk, event cuts, the
 * message status, Link's position) and runs the d_npc per-call helpers (JntCtrl, head swings, path index, event cut
 * procs, talk), its morf animation (setMtx / play_animation play it) and the shared mover. So the whole
 * execute runs once per 30 Hz frame on the tick30 step at dt 1 from the frame-start state (t60w::Guard); the earlier
 * steps change no logic state. No per-NPC picture hook (2026-11-18): an NPC's matrix step advances its animation,
 * so it is not a picture step; the in-between picture (base matrix and skeleton between steps) comes from the generic
 * mechanism frame interpolation (in: t60w::begin reports the earlier steps as picture steps; the port draws the
 * actor between its frame ends and keeps / puts back its picture-step draws). So: no values (n 0), an empty matrix step.
 * Use: `t60w::Guard<A> g(this, t60npc::Logic<A>::kOps); if (g.skip) return TRUE;` at the execute's head and
 * `g.model = true;` before its matrix step. dt 1: the original. Negative control: T60C_NEGCTRL (the execute on every
 * step at dt). */
#pragma once
#include "d/t60_whole.h"

namespace t60npc {
/* rule 14 (draw-side state once per logic step): an NPC in the whole form has one logic step per 30 Hz frame, the
 * tick30 step. A draw that changes state (a latch of a flag the frame's logic or collision set, e.g. Ym1's grass-cut
 * flag) does so only on the tick30 step's draw. Since frame interpolation the port keeps and puts back a picture step's draw (actor
 * memory, the models its draw calcs), so for writes into the actor this gate is redundant; it stays (harmless: the
 * picture-step draw's latch would be put back anyway) and covers a write outside the actor that no put-back reaches.
 * At N 1 and in the harness every step is a tick30 step. */
static inline bool drawStep() { return t60::tick30(); }

template <class A> struct Logic {
    static void get(A*, f32*) {}
    static void set(A*, const f32*) {}
    static void mtx(A*) {} /* the picture: frame interpolation (generic) */
    static bool held(A*) { return false; }
    static constexpr t60w::Ops<A> kOps = {0, 0, get, set, mtx, held};
};
}  // namespace t60npc
