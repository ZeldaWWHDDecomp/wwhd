/* m_Do_ext_morf_snd_t60.h (WWHD, true 60 build): the
 * morfs' animation sound once per 30 Hz frame. Included by m_Do_ext_morf.cpp and m_Do_ext_morf2.cpp
 * (mDoExt_McaMorf::play / mDoExt_McaMorf2::play).
 *
 * play advances the frame control and hands the frame to the animation sound (JAIZelAnime::setAnimSound 0201BF08 ->
 * JAIAnimeSound::setAnimSoundActor). That engine is a once-per-30-Hz-frame system (a loop wrap plays the keys up to the
 * last call's frame + speed; every call re-checks / re-starts its playing slots), not a pure crossing test: a converted
 * actor playing on every step fed it the in-between frames twice a frame (Link's case, zl pair: a footstep 4
 * frames early). Form (as Link's execute, d_a_player_seinit_t60.h): a converted step (dt != 1) hands the sound its frame
 * on the frame's tick30 step only (the frame-end frame); dt 1 (unconverted processes, N = 1, the whole form) every call,
 * the original. NC WWHD_T60_ANIMSND_NEGCTRL=1: every step, as before. */
#pragma once
#include "true60.h"
#include <cstdlib>

static inline bool morf_t60_animSoundStep() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_ANIMSND_NEGCTRL"); return e && e[0] == '1'; }();
#else
    const bool nc = false;
#endif
    return t60::dt() == 1.0f || t60::tick30() || nc;
}
