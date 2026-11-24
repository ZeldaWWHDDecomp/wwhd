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
 * The picture: frame interpolation (the model's joints blended between the frame ends; the picture-step draw kept / put back);
 * held by a carrier, the carrier's hand of the step (below).
 * The draw's light state (settingTevStruct) on the tick30 step's draw only (rule 14). No create-time execute call.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"
#include "d/actor/d_a_boko.h"
#include <cstdlib>

namespace boko_t60 {
/* (2026-11-23, the carryables' held picture d/t60_held_pic.h): while a carrier
 * holds the weapon (actor_status 0x2000: a Bokoblin / Moblin / Darknut, or Link with the weapon in his hand) its carrier
 * writes the model's base matrix from its hand on each of its steps (daBoko_setMatrix / Link's setItemModel), but the
 * weapon's earlier steps were frame interpolation picture steps: drawn between its frame ends, it cut the chord of the swing's arc
 * while the hand followed the arc. Held, an earlier step is a step of its own (t60w::begin notes no picture step) and its
 * picture is the model's calc from this step's base matrix (heldPicture: the joints only, a picture; the tick30 run
 * calcs again from its frame end). held() is false on the tick30 step, so the tick30 run and the latch keep the form
 * before (bit-exact logic). Negative control WWHD_T60_WPNHELD_NEGCTRL=1: the form before (never held, frame interpolation). */
static inline bool heldNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_WPNHELD_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
static inline bool held(daBoko_c* a) {
    return !t60::tick30() && (static_cast<u32>(a->actor_status) & 0x2000) != 0 && !heldNc();
}
static constexpr t60w::Ops<daBoko_c> kOps = {0, 0, t60objw::Logic<daBoko_c>::get, t60objw::Logic<daBoko_c>::set,
                                             t60objw::Logic<daBoko_c>::mtx, held};
static inline const t60w::Ops<daBoko_c>& ops() { return kOps; }
/* the execute's earlier-step tail (the Guard skipped): held, the model's joints from this step's base matrix */
static inline BOOL heldPicture(daBoko_c* a) {
    if (t60::dt() == 1.0f || t60::tick30() || !held(a))
        return 1;
    /* the latch as the form before leaves it (n 0: nothing is shown, but `shown` makes the tick30 step put old.pos /
     * old.angle back to the frame's first step's: a carrier that drops the weapon on an earlier step writes its
     * current.angle there, fopAc_Execute copies it to old; bkF / bkN 47C58678 old.angle.y at the drop, steps 3667-3673) */
    t60w::Latch* e = t60w::find(gabi::ea(a));
    if (e && e->model)
        e->shown = true;
    if (a->mpModel.get())
        bokoModelCalculate(a->mpModel.get());
    return 1;
}
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace boko_t60
