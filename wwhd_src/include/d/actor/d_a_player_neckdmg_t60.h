/* d_a_player_neckdmg_t60.h (WWHD, true 60 build; 2026-11-24). Decomp build only.
 * Separate file: d_a_player_main_04.cpp keeps a thin call.
 *
 * Link's neck during the damage reaction: procDamage 0241E150 writes mNeckAngle.x (the hit direction x the damage
 * animation's curve) and mNeckAngle.y = 0 on every call; setNeckAngle 02400C4C then eases both toward the look target
 * (cLib_addCalcAngleS 3 / 0x1000 / 0x100). The original per frame: the reset value plus one full ease step. On a
 * general step length the reset makes the ease's frame entry new on every step (c_lib.cpp t60IntEntry: written by
 * someone else, a partial frame), so each step moved only its 1/N share: mo N 4, neck yaw B 1024 for A 4096 (0x1000,
 * the step's limit / 4) through the damage frames, x thousands off. The value does not carry from step to step there
 * (rewritten before the ease), so each step takes the original's full step from its own reset value: the two eases
 * run at dt 1 on a step whose procDamage rewrote them (lkT60_neckDmgMark); the tick30 step then lands on A's frame
 * value. dt 1 / any other step: nothing (host state only, no guest read). NC WWHD_T60_NECKDMG_NEGCTRL=1. Host state only. */
#pragma once
#include "true60.h"
#include <cstdint>
#include <cstdlib>

static inline bool lkT60_neckDmgNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] {
        const char* e = getenv("WWHD_T60_NECKDMG_NEGCTRL");
        return e && e[0] == '1';
    }();
    return nc;
#else
    return false;
#endif
}
/* the step on which procDamage last rewrote the neck (t60_step_counter; one copy for the unit's sources: C++17 inline
 * variable, host only; 0: none) */
inline uint32_t g_lkT60NeckDmgStep = 0;
/* procDamage, after its writes of mNeckAngle */
static inline void lkT60_neckDmgMark() {
    if (t60::dt() != 1.0f)
        g_lkT60NeckDmgStep = t60_step_counter();
}
/* setNeckAngle's look-at reference on a step whose procDamage rewrote the neck: the live (just written) angles, as
 * the original reads them after procDamage; the frame-start reference (g_lkT60NeckRef.nx / ny, captured on the
 * frame's first step after only that step's animation play) put the target off by the damage curve's change over
 * the frame's later steps (mo N 2: neck pitch up to 811 off in the damage frames) */
static inline bool lkT60_neckDmgLive() {
    return t60::dt() != 1.0f && !lkT60_neckDmgNc() && g_lkT60NeckDmgStep == t60_step_counter();
}
/* the step of the frame's last full-step neck ease (lkT60_neckDmgOpen at dt 1; host only) */
inline uint32_t g_lkT60NeckFullStep = 0;
inline bool g_lkT60NeckFullValid = false; /* (a full step taken since the start) */
/* before the neck's x / y eases: the step's dt to put back (1.0f: unchanged); dt 1 only on a step whose procDamage
 * rewrote the neck (a value carried from the step before keeps the frame split) */
static inline float lkT60_neckDmgOpen() {
    if (t60::dt() == 1.0f || lkT60_neckDmgNc() || g_lkT60NeckDmgStep != t60_step_counter())
        return 1.0f;
    g_lkT60NeckFullStep = t60_step_counter();
    g_lkT60NeckFullValid = true;
    return t60_set_dt(1.0f);
}
/* a later step of a frame whose earlier step already took the neck's full step (the damage reaction ended on that
 * step: mo N 2 frame 214, the step after it eased again from a new entry, y 584 ahead): the x / y eases hold, as the
 * original eases once in that frame; any other step (and dt 1): false */
static inline bool lkT60_neckDmgHold() {
    if (t60::dt() == 1.0f || lkT60_neckDmgNc() || !g_lkT60NeckFullValid || t60::first_step() ||
        g_lkT60NeckDmgStep == t60_step_counter())
        return false;
    return t60_step_counter() - g_lkT60NeckFullStep < (uint32_t)t60::steps();
}
static inline void lkT60_neckDmgClose(float prev) {
    if (prev != 1.0f)
        t60_set_dt(prev);
}
