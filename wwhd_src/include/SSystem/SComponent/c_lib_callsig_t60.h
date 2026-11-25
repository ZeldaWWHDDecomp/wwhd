/* c_lib_callsig_t60.h (WWHD, true 60 build; 2026-11-24). Decomp
 * build only. Separate file: c_lib.cpp keeps a thin call.
 *
 * The integer eases on a general step length (c_lib.cpp t60IntEntry) keep, per value address, the 30 Hz frame's
 * start and continue it while nobody else wrote the value; on the frame's tick30 step they write the original one step
 * from that start (t60PinFull). When a procedure change on an earlier step hands the same value to another call site
 * with other parameters (Link's procMoveTurn: shape_angle.y toward current.angle.y, scale 3, 0x1770 / 0xBB8, until it
 * reaches it on step 3 of 4 and enters MOVE; procMove's turn on step 4 eases the same word with its own parameters), the
 * later call took the earlier call's frame start: the tick30 pin wrote procMove's one step from the frame start, 1496
 * back from where MoveTurn had already brought it (uncarried mdC at N 4, frame 205: shape -17231 for A's -15713 =
 * current.angle.y; Link's angle 974 off at 207).
 * Form (site-keyed, 2026-11-24): the entry notes the call site (t60cs_site: the guest LR of the call, as the guest
 * or native entry leaves it, with the host return address of the ease) and the parameters (t60cs_sig) of the call that
 * opened it, and of another call site met on the same step (site2 / sig2). A call from a site the entry has not met this
 * frame, with parameters it has not met, on a LATER step than the entry's last use, on an entry that still has its tick30
 * pin, starts a partial frame from the current value (as a value written by someone else: no tick30 pin from the other
 * call's start). Kept as before: the same site with other limits on a later step (the rope swing's maxStep from
 * cos(swingZ), an enemy's speed-scaled limit: the pin stays); two sites easing one value on every step (Link's neck yaw,
 * procDamage and the neck update: met on the first step); a partial entry (it never writes another call's full step).
 * dt 1: never called (the original). NC WWHD_T60_LIBSIG_NEGCTRL=1 (no check). Host data only. */
#pragma once
#include "true60.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

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
/* addCalcAngleS's parameters */
static inline unsigned t60cs_sig(int scale, int maxStep, int minStep) {
    return ((unsigned)scale << 24 ^ ((unsigned)maxStep & 0xFFFF) << 8 ^ ((unsigned)minStep & 0xFF) << 1) | 1u;
}
/* the other eases: the function (fn: 1 addCalcAngleS2, 2 addCalcAngleL, 3 + kind chaseUC / chaseS /
 * chaseAngleS) and its full parameters */
static inline unsigned t60cs_sig(unsigned fn, int a, int b, int c) {
    unsigned h = fn * 0x9E3779B1u;
    h = (h ^ (unsigned)a) * 0x85EBCA6Bu;
    h = (h ^ (h >> 13) ^ (unsigned)b) * 0xC2B2AE35u;
    h = (h ^ (h >> 16) ^ (unsigned)c) * 0x27D4EB2Fu;
    return (h ^ (h >> 15)) | 1u;
}
/* the call site: the guest LR of the call (the guest entry's bl, or the native entry's answer for its caller) and the
 * ease's host return address (ra: __builtin_return_address(0) in the ease's own body, T60CS_SITE) */
static inline unsigned t60cs_site(const void* ra) {
    Cpu* c = gabi::cpu;
    const unsigned lr = c ? (unsigned)c->lr : 0u;
    const uint64_t h = (uint64_t)(uintptr_t)ra;
    return ((unsigned)(h ^ (h >> 32)) * 0x9E3779B1u ^ lr * 0x85EBCA6Bu) | 1u;
}
#define T60CS_SITE() t60cs_site(__builtin_return_address(0))

/* diagnostic: WWHD_T60_LIBSIG_LOG=1 prints every partial-frame restart on stderr (`t60cs restart ea=VALUE frame=F step=S
 * first=0|1 tick30=0|1 old=SIG new=SIG site=OLD>NEW lr=GUESTLR cur=V ra0=+OFF`; ra0: the image-relative host return
 * address of the check's caller, for atos). Off: no effect. */
static inline bool t60cs_log() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_LIBSIG_LOG");
        return e && e[0] == '1';
    }();
    return on;
#else
    return false;
#endif
}
static inline unsigned long t60cs_off(const void* p) {
    Dl_info i;
    return p && dladdr(p, &i) && i.dli_fbase ? (unsigned long)((const char*)p - (const char*)i.dli_fbase) : 0ul;
}

/* the entry e (T60IntEntry: addr, frame, start, last, k, partial; sig, step, sig2, site, site2) for this call (sig, site);
 * restarts it as a partial frame from cur when a call site it has not met meets it with other parameters on a later step */
template <class E> __attribute__((always_inline)) static inline void t60cs_check(E* e, unsigned sig, int cur, unsigned site) {
    if (t60cs_nc())
        return;
    const unsigned now = t60_step_counter();
    if (e->sig == 0) {
        e->sig = sig;
        e->site = site;
    } else if (e->site != site && e->site2 != site && e->sig != sig && e->sig2 != sig && e->step != now && !e->partial) {
        if (t60cs_log()) {
            Cpu* c = gabi::cpu;
            fprintf(stderr, "t60cs restart ea=%08x frame=%u step=%u first=%d tick30=%d old=%08x new=%08x site=%08x>%08x lr=%08x cur=%d ra0=+%lx\n",
                    (unsigned)e->addr, (unsigned)e->frame, now, (int)t60::first_step(), (int)t60::tick30(), (unsigned)e->sig,
                    sig, (unsigned)e->site, site, c ? (unsigned)c->lr : 0u, cur, t60cs_off(__builtin_return_address(0)));
        }
        const E fresh{e->addr, e->frame, cur, cur, 0, true};
        *e = fresh;
        e->sig = sig;
        e->site = site;
    } else if (e->site != site) {
        e->site2 = site; /* another call site of this frame's entry (same step, or kept): met */
        if (e->sig != sig)
            e->sig2 = sig;
    }
    e->step = now;
}
