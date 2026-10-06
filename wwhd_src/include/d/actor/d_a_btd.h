/* d_a_btd.h (WWHD): Gohma. */
#pragma once
#include "f_op/f_op_actor.h"

// Gohma's HD instance includes a separate death-animation triplet and expanded
// collision, lighting and material state. The native profile allocates 0x70D8.
struct btd_class : fopAc_ac_c {
    u8 _btd_storage[0x3D8 - sizeof(fopAc_ac_c)];
    gptr<void> phase1Morf;
    gptr<void> phase1Btk;
    gptr<void> phase1Brk;
    gptr<void> phase2Morf;
    gptr<void> phase2Btk;
    gptr<void> phase2Brk;
    gptr<void> deathMorf;
    gptr<void> deathBtk;
    gptr<void> deathBrk;
    gptr<void> headMorf;
    gptr<void> headBtk;
    gptr<void> headBrk;
    be<u8> phase;
    u8 _409[3];
    be<s16> state;
    u8 _tail[0x70D8 - 0x40E];
};
WWHD_OFFSET(btd_class, phase1Morf, 0x3D8);
WWHD_OFFSET(btd_class, deathMorf, 0x3F0);
WWHD_OFFSET(btd_class, headMorf, 0x3FC);
WWHD_OFFSET(btd_class, phase, 0x408);
WWHD_OFFSET(btd_class, state, 0x40C);
static_assert(sizeof(btd_class) == 0x70D8);
