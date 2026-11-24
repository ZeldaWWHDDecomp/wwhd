/* d_a_player_seinit_t60.h (WWHD, true 60 build, 2026-11-24): the sound animation's play position
 * when a new or switched animation is set on a frame's first step. Decomp build only.
 * Included by d_a_player_main_01.cpp (initSeAnime).
 *
 * initSeAnime puts the looping sound animation's play position (JAIAnimeSound's last frame) at the previous update's
 * frame, frame - rate; the next setAnimSound fires the keys between it and the frame. On a converted step the position
 * is the previous step's, frame - rate x dt (01: a full rate back replayed the step before's keys for a
 * procedure that re-inits every update). An animation set on a frame's first step and latched (lkT60_markAnmSet: it
 * shows the frame-end phase, setMoveAnime's switch projects the whole frame's play, and does not advance on the frame's
 * later steps) is the frame's one play: A's window is [frame - rate, frame], B's was [frame - rate x dt, frame] and the
 * later steps add nothing (Z strafe zl: ATN_MOVE's switch to ATNWRS at frame 9.9049 rate 1.3528, A position 8.5521, B
 * 9.2285: sound 0x3831 at A 76, missed in B). Form: on the frame's first step, for a latched frame control, the position
 * is the whole rate back; later steps keep the dt form. NC WWHD_T60_SEINIT_NEGCTRL=1. dt 1: the original. */
#pragma once
#include "true60.h"
#include "d/actor/d_a_player_t60.h"
#include <cstdlib>

static inline bool lkT60_seInitNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SEINIT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the rate share initSeAnime steps back from the frame: dt, or the whole rate for a latched first-step set */
static inline f32 lkT60_seInitBack(uint32_t frameCtrlEa) {
    if (t60::dt() == 1.0f)
        return 1.0f;
    if (t60::first_step() && !lkT60_seInitNegctrl() && lkT60_anmSetPending(frameCtrlEa))
        return 1.0f;
    return t60::dt();
}

/* (2026-11-24; zl: B's footstep 0x3831 at frame 140, A's at 144, with equal inits and equal
 * frame-end frames) the anim sound engine (JAIAnimeSound::setAnimSoundActor, JAIAnimation.cpp) is a 30 Hz-update
 * system: a loop wrap plays the keys up to the last call's frame + speed, and every call re-checks / re-starts the
 * playing slots (a start == end key restarts its sound on each call). daPy_lk_c::execute called it on every step with
 * the step's in-between frame; A calls it once a frame with the frame-end frame. Form (as the port's audio frame work,
 * t60_audio.h CTL / PLAYER: once per 30 Hz frame on the tick30 step): Link's setAnimSound runs on the frame's tick30 step
 * only, with that step's (the frame-end) frame and rate. NC WWHD_T60_ANIMSND_NEGCTRL=1: every step, as before. N = 1: every
 * step is the tick30 step. */
static inline bool lkT60_animSoundNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_ANIMSND_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline bool lkT60_animSoundStep() { return t60::steps() <= 1 || t60::tick30() || lkT60_animSoundNegctrl(); }
