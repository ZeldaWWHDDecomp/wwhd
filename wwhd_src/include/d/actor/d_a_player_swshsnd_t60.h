/* d_a_player_swshsnd_t60.h (WWHD, true 60 build, 2026-11-25): when setItemModel hands the sword /
 * shield place to the audio side. Decomp build only. Included by
 * d_a_player_main_04.cpp (setItemModel).
 *
 * setItemModel (in execute, after the sound animation's setAnimSound) calls setSwordSoundType / setShieldSoundType
 * every update; JAIZelBasic::mLinkSwordShieldOnBack (1: sword and shield both on the back) makes startAnimSound add
 * JA_SE_LK_FT_SW_SHIELD (0x3831) to the walk / jump keys. In A the frame's setAnimSound reads the place of the frame
 * before (setItemModel runs after it); setAnimSound is tick30-only, so on a converted frame the
 * first step's setItemModel set the frame-end place before it (long2 N 2: Link draws the sword on the frame of a walk
 * key, A 11600: A starts 0x3831 with the flag still 1, B's first step had cleared it). Form: on a converted frame only
 * the tick30 step hands the place over (its state is the frame end's); the earlier steps leave the frame before's.
 * NC WWHD_T60_SWSHSND_NEGCTRL=1. dt 1: the original. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool lkT60_swShSndNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SWSHSND_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* true: this update hands the sword / shield place to the audio side */
static inline bool lkT60_swShSndStep() {
    return t60::dt() == 1.0f || t60::steps() <= 1 || t60::tick30() || lkT60_swShSndNegctrl();
}
