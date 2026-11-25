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
 * run at dt 1 while the procedure is DAMAGE; the tick30 step then lands on A's frame value. dt 1 / other procedures:
 * nothing (mCurProc is read only on a general step length). NC WWHD_T60_NECKDMG_NEGCTRL=1. Host state only. */
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
/* before the neck's x / y eases: the step's dt to put back (1.0f: unchanged) */
template <class P> static inline float lkT60_neckDmgOpen(const P* curProc) {
    if (t60::dt() == 1.0f || lkT60_neckDmgNc() || (int32_t)*curProc != 0x66 /* daPyProc_DAMAGE_e */)
        return 1.0f;
    return t60_set_dt(1.0f);
}
static inline void lkT60_neckDmgClose(float prev) {
    if (prev != 1.0f)
        t60_set_dt(prev);
}
