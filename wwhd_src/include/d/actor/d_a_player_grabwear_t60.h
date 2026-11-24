/* d_a_player_grabwear_t60.h (WWHD, true 60 build, 2026-11-24): Link's
 * checkGrabWear (`mGrabItemOffsetY < 0`: a barrel worn over the head) read at A's point in the frame. Decomp build
 * only. Separate file: the sources keep thin calls.
 *
 * setGrabItemPos (d_a_player_main_04.cpp, late in execute) eases mGrabItemOffsetY toward the procedure's target
 * (addCalc, converted: per step). Every logic reader runs earlier in the execute (the procedure body and its inits,
 * setHatAngle, setCollision), so in A it reads the value the previous frame left: the frame-start value. At N > 1 the
 * frame's later steps read the mid-frame value of the first step's ease: procGrabPut's wait (mProcVar6 1) left on B's
 * tick30 step of the frame before A's (a barrel put-down pair: GRABUP from B 3795 / A 3796, WAIT a frame early).
 * Form: the frame's first step records the frame-start wear in setGrabItemPos before its ease (port latch keyed by
 * &mGrabItemOffsetY, frame tag g_Counter.mCounter0, as d_a_player_weaponthrow_t60.h); a reader on a later step of the
 * frame takes the record. The first step itself (its readers run before the ease), a frame without a record (no grab:
 * no ease) and dt 1 read the live value (the original). draw keeps the live value (the picture of each pass).
 * Negative control WWHD_T60_GRABWEAR_NEGCTRL=1: the live value everywhere (the form before). */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkgw {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_GRABWEAR_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline u32 frameTag() { return *gabi::at<be<u32>>(0x101FF558); /* g_Counter.mCounter0 */ }
/* setGrabItemPos, before the ease: the frame-start wear (first step of a frame at N > 1 only) */
static inline void frameStart(const void* offsetY, f32 v) {
    if (nc() || t60::dt() == 1.0f || !t60::first_step())
        return;
    const u32 k = gabi::ea(offsetY);
    wwhd_t60_latch_set(k, frameTag()); /* recorded */
    if (v < 0.0f)
        wwhd_t60_latch_set(k + 1, frameTag()); /* worn at the frame start */
}
/* checkGrabWear() at A's point: `v < 0` live, or the frame's first-step record on a later step */
static inline bool wear(const void* offsetY, f32 v) {
    if (nc() || t60::dt() == 1.0f || t60::first_step())
        return v < 0.0f;
    const u32 k = gabi::ea(offsetY);
    if (!wwhd_t60_latch_pending(k, frameTag()))
        return v < 0.0f;
    return wwhd_t60_latch_pending(k + 1, frameTag()) != 0;
}

/* (2026-11-24): the later-step release on a later step of the frame
 * (lkT60_grabRelease, d_a_player_t60.h) puts the carried actor back at the frame-start placement, for a throw or a void
 * where A leaves the carry before its frame's placement (a pot pair's frame 293). procGrabPut's put-down release is not that
 * case: the frame-start placement (the barrel 25 above the ground at the release frame, 740.46) left CrrPos without the
 * ground and the barrel fell 3 frames (landing sound / smoke 2 frames late), where A's barrel sits on the ground at its
 * release frame. That frame's release is setGrabItemPos's GRAB_PUT ground test (main_04): the ground under the barrel
 * above its placement puts the barrel on the ground (y = ground) and frees it; that release then put it back up. Form: the
 * put-down releases (that test, and procGrabPut's two end branches) mark the release (port latch keyed by
 * &mActorKeepGrab + 2, this frame); freeGrabItem then skips the put-back and leaves the frame's own placement.
 * Negative control WWHD_T60_GRABPUT_NEGCTRL=1. */
static inline bool putNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_GRABPUT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline void putDown(const void* keepGrab) {
    if (putNc() || t60::dt() == 1.0f)
        return;
    wwhd_t60_latch_set(gabi::ea(keepGrab) + 2, frameTag());
}
static inline bool putDownNow(const void* keepGrab) {
    return !putNc() && t60::dt() != 1.0f && wwhd_t60_latch_pending(gabi::ea(keepGrab) + 2, frameTag()) != 0;
}
}  // namespace lkgw
