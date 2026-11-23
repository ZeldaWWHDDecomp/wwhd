/* f_pc_t60_pystat.h (WWHD, true 60 build, 2026-11-22, rule on the process-order read rule for the
 * player status words). Decomp build only. Separate file:
 * f_pc_executor.cpp keeps thin calls.
 *
 * the process-order read rule: an actor EARLIER in the process line than Link reads, in A, Link's frame f - 1 end. The play-wide
 * player status words (dComIfGp_getPlayerStatus0 / 1: play + 0x5CD8 / 0x5CDC, mPlayerStatus[0][0..1]) are written by
 * Link in his execute (and polled by others through dComIfGp_checkPlayerStatus0 / 1). At N steps a converted reader
 * earlier in the line saw, on the frame's later steps, a bit Link set on an earlier step of the same frame: the melody
 * stone (obj_mknjd 0x4D, STATE_WAIT) saw WIND_WAKER_CONDUCT (status1 bit 0x1, set by Link's tact init) one frame early
 * (575 / 576).
 *
 * Form: at the start of every step's process line (the step's first fpcEx_Execute) the two words are put back to their
 * values at the frame start (the previous frame's end: taken at the frame's first step's line start), so every reader
 * before Link sees the frame-start value on every step. Link's execute (0xA8) first re-applies the live value (the
 * value the previous step's line left, with the bits the readers before him set / cleared in this step merged in), so
 * Link and the readers after him see the live word as before. A step whose line has no Link execute (Link held, absent)
 * re-applies the live value at the next step's line start. Writes of the earlier actors are kept (merged), never lost.
 * N = 1: never (the original). Negative control: env WWHD_T60_PYSTAT_NEGCTRL=1 (no swap: the per-step live words). */
#pragma once
#include <cstdlib>
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
namespace t60pystat {
enum { kWords = 2 };
static const uint32_t kOff = 0x5CD8; /* PLAY_PLAYER_STATUS0: mPlayerStatus[0][0], [0][1] follows */
static uint32_t s_step = ~0u;        /* step counter of the step whose line start ran */
static uint32_t s_fs[kWords];        /* the frame-start values */
static uint32_t s_live[kWords];      /* the live values while the words show the frame start */
static bool s_swapped = false;       /* the words show s_fs (+ the earlier readers' writes) */
static uint32_t s_play = 0;          /* the play the words belong to */
static inline bool negctrl() {
    static const int v = [] {
        const char* e = getenv("WWHD_T60_PYSTAT_NEGCTRL");
        return e && e[0] && e[0] != '0' ? 1 : 0;
    }();
    return v != 0;
}
/* the words back to live: live with the bits set / cleared since the swap merged in */
static inline void unswap() {
    if (!s_swapped)
        return;
    s_swapped = false;
    for (int i = 0; i < kWords; i++) {
        const uint32_t ea = s_play + kOff + 4u * i;
        const uint32_t now = gabi::load<uint32_t>(ea);
        const uint32_t set = now & ~s_fs[i], clr = s_fs[i] & ~now;
        gabi::store<uint32_t>(ea, (s_live[i] | set) & ~clr);
    }
}
/* fpcEx_Execute's head (N > 1): the step's first execute starts its line */
static inline void lineStart() {
    if (negctrl())
        return;
    const uint32_t now = t60_step_counter();
    if (now == s_step)
        return;
    s_step = now;
    unswap();
    const uint32_t play = dComIfGp_ea();
    if (play != s_play) {
        s_play = play;
        if (!t60::first_step())
            return; /* (a new play mid-frame: no frame start of it) */
    }
    if (t60::first_step()) {
        for (int i = 0; i < kWords; i++)
            s_fs[i] = gabi::load<uint32_t>(play + kOff + 4u * i);
        return;
    }
    bool differs = false;
    for (int i = 0; i < kWords; i++) {
        s_live[i] = gabi::load<uint32_t>(play + kOff + 4u * i);
        differs |= s_live[i] != s_fs[i];
    }
    if (!differs)
        return;
    s_swapped = true;
    for (int i = 0; i < kWords; i++)
        gabi::store<uint32_t>(play + kOff + 4u * i, s_fs[i]);
}
/* just before Link's execute: his live words back */
static inline void linkExec() { unswap(); }
}  // namespace t60pystat
static inline void t60_pystat_line(int steps) {
    if (steps > 1)
        t60pystat::lineStart();
}
static inline void t60_pystat_link() { t60pystat::linkExec(); }
#else
static inline void t60_pystat_line(int) {}
static inline void t60_pystat_link() {}
#endif
