/* t60_held_pic.h (WWHD, true 60 build, 2026-11-23): a held whole-form carryable's
 * picture on a frame's earlier steps, from the place its holder gave it on that step. Decomp build only.
 * Separate file: actor sources keep thin calls.
 *
 * Seen in play (N 2): a little stutter on the vase itself while Link was carrying it around (fixed for the
 * pot by tsubo_t60::picture, d_a_tsubo_mode_t60.h), then the same issue with the small stones. Every whole-form
 * carryable (d/t60_whole.h: the execute runs whole once per 30 Hz frame on the tick30 step) returns from t60w::begin on
 * a frame's earlier steps and, held (o.held: carried by Link or his hookshot), shows nothing there: no extrapolated
 * picture (its holder places it) and no frame-interpolation picture step (the port keeps that step's model matrices in its draw
 * history). Its holder (Link's setGrabItemPos, the hookshot) moves current.pos / shape_angle on every step, but the
 * model's base matrix came only from the tick30 run's set_mtx: on the earlier steps it was drawn at the previous frame
 * end's place while Link's hands moved on (a 30 Hz staircase against the hand).
 *
 * The picture: on an earlier step of a held carryable, the actor's own matrix step (`pic`, its set_mtx or the like)
 * from this step's placement; `pic` writes the model's base matrix (a picture) and scratch matrices only, or puts back
 * the logic words it writes (the actor's header says which). The tick30 run sets the matrix again from its frame's end.
 * dt 1 (N 1): never (no earlier steps). Negative control WWHD_T60_HELDPIC_NEGCTRL=1: the form before (held: no
 * picture). The pot keeps its own form (tsubo_t60::picture, NC WWHD_T60_POTHELD_NEGCTRL). */
#pragma once
#include "d/t60_whole.h"
#include <cstdlib>

namespace t60hp {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_HELDPIC_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the execute's earlier-step tail (t60w::begin returned true / the Guard skipped): the held picture */
template <class A> static inline void picture(A* a, const t60w::Ops<A>& o, void (*pic)(A*)) {
    if (t60::dt() == 1.0f || t60::tick30() || !o.held(a) || nc())
        return;
    pic(a);
}
}  // namespace t60hp
