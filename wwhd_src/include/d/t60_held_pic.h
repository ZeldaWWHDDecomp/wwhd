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
 * picture). The pot keeps its own form (tsubo_t60::picture, NC WWHD_T60_POTHELD_NEGCTRL).
 * Link's carried item rides at the frame's bounce picture (with_grab_dy, also in the pot's form). */
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
/* (d_a_player_main_04.cpp lk04_grabPicNote): Link's carried item's bounce on this earlier step as
 * a picture (the per-step form from the frame-start bounce) minus the logic bounce he added; 0 when Link does not carry
 * `actor` or did not place it on this step (game build, N > 1; NC WWHD_T60_GRABPIC_NEGCTRL=1: always 0) */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" __attribute__((weak)) float wwhd_t60_grab_pic_dy(uint32_t actor);
static inline float grab_dy(uint32_t actor) { return wwhd_t60_grab_pic_dy ? wwhd_t60_grab_pic_dy(actor) : 0.0f; }
#else
static inline float grab_dy(uint32_t) { return 0.0f; }
#endif
/* (d_a_hookshot_t60.h hs60::notePic): the place the hookshot would give its carried actor on
 * this earlier step (it places a 30 Hz carried actor on the tick30 step only); false: none */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" __attribute__((weak)) bool wwhd_t60_hook_pic_pos(uint32_t actor, float* out);
static inline bool hook_pos(uint32_t actor, float* out) { return wwhd_t60_hook_pic_pos && wwhd_t60_hook_pic_pos(actor, out); }
#else
static inline bool hook_pos(uint32_t, float*) { return false; }
#endif
/* run `fn` (the actor's matrix step) at its holder's picture place: the hookshot's noted place (current.pos +0x314), or
 * current.pos.y (+0x318) raised by Link's bounce picture; the words are put back */
template <class F> static inline void with_grab_dy(uint32_t actor, F fn) {
    float hp[3];
    if (hook_pos(actor, hp)) {
        be<u32>* w = gabi::at<be<u32>>(actor + 0x314);
        u32 w0[3];
        for (int i = 0; i < 3; i++) {
            w0[i] = w[i];
            *gabi::at<be<f32>>(actor + 0x314 + 4u * i) = hp[i];
        }
        fn();
        for (int i = 0; i < 3; i++)
            w[i] = w0[i];
        return;
    }
    const float dy = grab_dy(actor);
    if (dy == 0.0f) {
        fn();
        return;
    }
    be<u32>* y = gabi::at<be<u32>>(actor + 0x318); /* fopAc_ac_c::current.pos.y */
    const u32 y0 = *y;
    *gabi::at<be<f32>>(actor + 0x318) = (f32)*gabi::at<be<f32>>(actor + 0x318) + dy;
    fn();
    *y = y0;
}
/* the execute's earlier-step tail (t60w::begin returned true / the Guard skipped): the held picture */
template <class A> static inline void picture(A* a, const t60w::Ops<A>& o, void (*pic)(A*)) {
    if (t60::dt() == 1.0f || t60::tick30() || !o.held(a) || nc())
        return;
    with_grab_dy(gabi::ea(a), [a, pic] { pic(a); });
}
}  // namespace t60hp
