/* d_a_player_upperplay_t60.h (WWHD, true 60 build; 2026-11-24, the carry placement item):
 * an upper frame control whose animation stops being driven mid-frame keeps the frame's play. Decomp build only.
 * Separate file (no new functions in glued units): the
 * actor source (d_a_player_main_01.cpp animeUpdate, its only includer) keeps thin calls.
 *
 * Pig route (pigX, hd3): the carried pig 0.31 off at frame 80 with Link exact; its place is Link's hands, and his hands
 * followed his upper body. The original plays the animations (animeUpdate) BEFORE the procedure dispatch, so an upper
 * frame control gets its frame's whole play before a dispatch that leaves it (the new move set has no upper animation of
 * its own: setMoveAnime points mAnmRatioUpper[0] at the under animation, and animeUpdate plays an upper control only
 * while its pack is its own). At N steps a set that leaves it on an earlier step stops its play after that step: frame
 * 78 (GRAB_WAIT -> MOVE, MOVE's blend switch on the first step) mFrameCtrlUpper[0] 1.248 -> 1.947 (A 2.646) and kept;
 * frame 31 (WAIT -> GRAB_READY) the wait's upper 37.950 (A 38.500); the worn barrel (obr) on its GRAB_WAIT -> MOVE frame
 * the same; mFrameCtrlUnder[1] (the move blend's second animation) likewise when a single animation replaces the blend
 * on an earlier step (setSingleMoveAnime: the stone throw's entry, stt N 4 frame 256, A 13.57 -> 15.87, B one step,
 * 14.14). Form: the frame's first step notes which of those controls it played; on a later step of that frame a noted
 * control its animation no longer drives is still played (its frame / rate words end the frame as A's; a control set
 * again this frame is latched and is not concerned). Host memory, this TU. dt 1: never. Negative control
 * WWHD_T60_UPPERPLAY_NEGCTRL=1. */
#pragma once
#include <cstdint>
#include <cstdlib>
#include "true60.h"

namespace lkup60 {
static uint32_t s_stamp = ~0u; /* t60_step_counter() of the frame's first step */
static uint8_t s_played = 0;   /* the controls the first step played (bit: upper 0..2, 3 = mFrameCtrlUnder[1]) */
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_UPPERPLAY_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
}  // namespace lkup60
/* slot `i` was played on this step */
static inline void lkT60_upperPlayed(int i) {
    if (t60::dt() == 1.0f || !t60::first_step())
        return;
    if (lkup60::s_stamp != t60_step_counter()) {
        lkup60::s_stamp = t60_step_counter();
        lkup60::s_played = 0;
    }
    lkup60::s_played |= (uint8_t)(1u << i);
}
/* slot `i` is not driven on this step: true when it still has the frame's play (the first step played it and it was not
 * set again this frame) */
static inline bool lkT60_upperContinue(int i, bool pending) {
    if (t60::dt() == 1.0f || t60::first_step() || pending || lkup60::nc())
        return false;
    if (t60_step_counter() - lkup60::s_stamp >= (uint32_t)t60::steps())
        return false;
    return (lkup60::s_played >> i) & 1u;
}
