/* c_lib_callsig_t60.h (WWHD, true 60 build; 2026-11-24). Decomp build only.
 * Separate file: c_lib.cpp keeps a thin call.
 *
 * cLib_addCalcAngleS on a general step length (c_lib.cpp t60IntEntry) keeps, per value address, the 30 Hz frame's
 * start and continues it while nobody else wrote the value; on the frame's tick30 step it writes the original one step
 * from that start (t60PinFull). When a procedure change on an earlier step hands the same value to another call site
 * with other parameters (Link's procMoveTurn: shape_angle.y toward current.angle.y, scale 3, 0x1770 / 0xBB8, until it
 * reaches it on step 3 of 4 and enters MOVE; procMove's turn on step 4 eases the same word with its own parameters), the
 * later call took the earlier call's frame start: the tick30 pin wrote procMove's one step from the frame start, 1496
 * back from where MoveTurn had already brought it (uncarried mdC at N 4, frame 205: shape -17231 for A's -15713 =
 * current.angle.y; Link's angle 974 off at 207).
 * Form: the entry notes the parameters (scale, maxStep, minStep) of the call that opened it; a call with other
 * parameters on a LATER step than the entry's last use starts a partial frame from the current value (as a value
 * written by someone else: no tick30 pin from the other call's start). Two call sites easing one value within the same
 * step keep the shared entry as before. dt 1: never called (the original). NC WWHD_T60_LIBSIG_NEGCTRL=1. Host data only. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool t60cs_nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] {
        const char* e = getenv("WWHD_T60_LIBSIG_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
#else
    return false;
#endif
}
static inline unsigned t60cs_sig(int scale, int maxStep, int minStep) {
    return ((unsigned)scale << 24 ^ ((unsigned)maxStep & 0xFFFF) << 8 ^ ((unsigned)minStep & 0xFF) << 1) | 1u;
}
/* the entry e (T60IntEntry: addr, frame, start, last, k, partial, sig, step) for this call; restarts it as a partial
 * frame from cur when another call site's parameters meet it on a later step */
template <class E> static inline void t60cs_check(E* e, unsigned sig, int cur) {
    if (t60cs_nc())
        return;
    const unsigned now = t60_step_counter();
    if (e->sig == 0) {
        e->sig = sig;
    } else if (e->sig != sig && e->step != now) {
        const E fresh{e->addr, e->frame, cur, cur, 0, true};
        *e = fresh;
        e->sig = sig;
    }
    e->step = now;
}
