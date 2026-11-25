/* d_a_player_athold_t60.h (WWHD, true 60 build, 2026-11-24): the attack window
 * flag of a procedure body held on a frame's later steps. Decomp build only.
 * Separate file: the source keeps thin calls.
 *
 * daPy_lk_c::execute clears mResetFlg0 before the procedure body; the attack procedures set daPyRFlg0_UNK2 (the attack
 * window: procHammerFrontSwing / procWeaponFrontSwing every call, the cuts' lk_cutAt in their window, the inits with
 * UNK1) and setCollision (d_a_player_main_04.cpp) keeps daPyFlg0_CUT_AT_FLG while UNK2 is set, clears it otherwise.
 * changeCutReverseProc (the hammer's ground hit, the cuts' recoil) needs CUT_AT_FLG. A procedure entered on the frame's
 * first step has its body held on the later steps (s_lkT60ProcHold): those steps had no UNK2,
 * and the tick30 step's setCollision cleared CUT_AT_FLG the first step's init had just set. Hammer route
 *(Y at 5.5 s): procHammerFrontSwing entered on frame 192's first step, the flag gone on its tick30 step, the
 * ground hit at 195 never taken (A: hit, the 10-frame hold, procHammerFrontSwingEnd at 206; B at 201 off the animation's
 * end; the hit sound 0x2855 missing).
 * Form: the first step records the UNK2 its body left; a held later step of the same frame and procedure sets it again
 * (the frame's one body's window, as A's frame has it). UNK1 (the attack's start: water splash, the AT start) is not
 * repeated: the first step's setCollision ran it, the tick30 step's AT set takes the start from lkT60_atStartedInFrame.
 * dt 1: nothing. Negative control WWHD_T60_ATHOLD_NEGCTRL=1 (game build): held steps without the flag (the form before). */
#pragma once
#include "true60.h"
#include <cstdint>
#include <cstdlib>

namespace lkT60AtHold {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_ATHOLD_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct State {
    uint32_t tag = 0;
    int32_t proc = -1;
    uint32_t flags = 0;
    bool valid = false;
};
inline State& state() {
    static State s;
    return s;
}
constexpr uint32_t kWindow = 2u; /* daPyRFlg0_UNK2 */
/* after the body on the frame's first step: the window flag it left, for this procedure */
inline void note(uint32_t resetFlg0, uint32_t tag, int32_t proc) {
    if (t60::dt() == 1.0f || !t60::first_step())
        return;
    State& s = state();
    s.tag = tag;
    s.proc = proc;
    s.flags = resetFlg0 & kWindow;
    s.valid = true;
}
/* a held later step (no body): the flags to add to mResetFlg0 */
inline uint32_t held(uint32_t tag, int32_t proc) {
    if (t60::dt() == 1.0f || nc())
        return 0;
    const State& s = state();
    return s.valid && s.tag == tag && s.proc == proc ? s.flags : 0u;
}
}  // namespace lkT60AtHold
