/* disappear_class (enemy death effects), WWHD layout.
 * GameCube -> WWHD: actor members move by +0x11C. The unused phase, item bit and timer keep
 * their GameCube names and sizes. HD adds the actor's virtual destructor; total size 0x3BC. */
#pragma once
#include "gc_api/gc_api.h"

enum {
    daDisItem_IBALL_e = 0,
    daDisItem_NONE1_e = 1,
    daDisItem_HEART_CONTAINER_e = 2,
    daDisItem_NONE3_e = 3,
    daDisItem_UNK4_e = 4,
    daDisItem_HEART_e = 10,
    daDisItem_MAGIC_e = 11,
    daDisItem_ARROW_e = 12,
    daDisItem_NONE13_e = 13,
};
enum { JA_SE_CM_MONS_EXPLODE = 0x5801 };

class disappear_class : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1000DCAC };
    disappear_class();
    /* 0x3AC */ request_of_phase_process_class mPhase; // Unused
    /* 0x3B4 */ be<s32> mItemBitNo;
    /* 0x3B8 */ be<s16> mTimer;
    /* 0x3BA */ u8 _3BA[2];
};
WWHD_OFFSET(disappear_class, mPhase, 0x3AC);
WWHD_OFFSET(disappear_class, mItemBitNo, 0x3B4);
WWHD_OFFSET(disappear_class, mTimer, 0x3B8);
WWHD_SIZE(disappear_class, 0x3BC);
