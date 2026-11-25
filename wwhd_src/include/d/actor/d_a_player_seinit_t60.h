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

/* (2026-11-25; long2 at N 2 on intgc13: Link's footstep 0x3831 started in A only, seg 9
 * BOW_SUBJECT 0x94 A step 10001 and seg 16 MOVE A step 11602): setSeAnime re-inits the sound animation when the move
 * blend switches its SE animation (a continuation set, not latched). A's init comes after the frame's play and puts the
 * play position at the frame-end frame minus the whole rate (19.4759 - 0.5134 = 18.9625); the setAnimSound after it
 * fires the keys from there. B's init on the frame's first step took the first step's frame minus rate x dt (19.2865 -
 * 0.2567 = 19.0298): a key between was never fired. With the tick30-only setAnimSound (above) the
 * init's position only matters at the tick30 step's setAnimSound: an init on an earlier step of the frame is noted, and
 * the tick30 step sets A's position from the frame-end frame and rate before its setAnimSound (the same wrap as
 * initSeAnime's). Host memory (inline: initSeAnime in main_01 notes, the execute in main_04 finishes). dt 1 / N 1:
 * nothing. NC WWHD_T60_SEINITEND_NEGCTRL=1 (and with WWHD_T60_ANIMSND_NEGCTRL=1): the step form as before. */
static inline bool lkT60_seInitEndNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SEINITEND_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
struct LkT60SeInitPend {
    uint32_t tag, fc;
    bool valid;
};
inline LkT60SeInitPend g_lkT60SeInitPend;
/* initSeAnime (a looping sound animation's position set) on an earlier step of the frame */
static inline void lkT60_seInitNote(uint32_t fc) {
    if (t60::dt() == 1.0f || t60::steps() <= 1 || t60::tick30() || lkT60_seInitEndNegctrl() || lkT60_animSoundNegctrl())
        return;
    g_lkT60SeInitPend = LkT60SeInitPend{lkT60_frameTag(), fc, true};
}
/* the tick30 step, before setAnimSound: an earlier step's init of this frame takes A's position (frame - rate, wrapped
 * as initSeAnime does) */
template <class FC>
static inline void lkT60_seInitFinish(uint32_t sound, uint32_t fcEa, const FC* fc) {
    LkT60SeInitPend& p = g_lkT60SeInitPend;
    if (!p.valid || t60::dt() == 1.0f || !t60::tick30() || lkT60_seInitEndNegctrl())
        return;
    const bool mine = p.tag == lkT60_frameTag() && p.fc == fcEa;
    p.valid = false;
    if (!mine || (u8)fc->mAttribute != 2 /* EMode_LOOP */)
        return;
    f32 position = gabi::fsubs_ppc((f32)fc->mFrame, (f32)fc->mRate);
    if (!(position > (f32)(s16)fc->mStart))
        position = gabi::fsubs_ppc((f32)(s16)fc->mEnd, position);
    else if (!(position < (f32)(s16)fc->mEnd))
        position = gabi::fsubs_ppc(position, (f32)(s16)fc->mEnd);
    gabi::call(0x0201BFD8 /* JAIZelAnime::setPlayPosition */, sound, position);
}
