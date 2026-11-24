/* d_a_player_weaponthrow_t60.h (WWHD, true 60 build; 2026-11-24): Link's weapon
 * throw (procWeaponThrow, d_a_player_main_08.cpp) ends on the frame after the throw, as the original. Decomp build only.
 * Separate file: the source keeps thin calls.
 *
 * The throw (lkT60_pass(frame 10): setThrow on the stick, then deleteEquipItem, which empties mActorKeepEquip) is the
 * frame's one lookahead, taken on the frame's first step (d_a_player_anm_t60.h). The procedure ends on
 * `mActorKeepEquip.mActor == nullptr`: the original's next call reads it, the next frame. At N > 1 the frame's later
 * steps already read the empty slot and ended the throw a frame early (a Bokoblin route with Link's Boko stick: proc 80 -> 4
 * on B 3857's tick30 step, A at 3858; the next proc a frame early too). Form: the throw marks the frame (port latch keyed
 * by Link's mActorKeepEquip); the frame's later steps do not end the procedure on the empty slot it left. dt 1: nothing
 * is marked (the original). Negative control WWHD_T60_WPNTHROW_NEGCTRL=1: the form before. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lk08wt {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_WPNTHROW_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 */ }
/* the throw emptied the slot on this step (an earlier step of the frame: marked; tick30 / dt 1: nothing) */
static inline void thrown(const void* keepSlot) {
    if (nc() || t60::dt() == 1.0f || t60::tick30())
        return;
    wwhd_t60_latch_set(gabi::ea(keepSlot), frameTag());
}
/* the slot is empty from this frame's throw: not yet seen by the procedure (the original's next call sees it) */
static inline bool emptiedThisFrame(const void* keepSlot) {
    return !nc() && t60::dt() != 1.0f && wwhd_t60_latch_pending(gabi::ea(keepSlot), frameTag()) != 0;
}
}  // namespace lk08wt
