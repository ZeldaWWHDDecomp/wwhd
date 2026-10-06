#pragma once
#include "f_op/f_op_actor.h"
struct daAndsw2_c : fopAc_ac_c {
    be<u8> mAction;
    u8 _3ad;
    be<s16> mTimer, mEventIdx;
    u8 _3b2[2];
};
WWHD_OFFSET(daAndsw2_c, mAction, 0x3AC);
WWHD_OFFSET(daAndsw2_c, mTimer, 0x3AE);
WWHD_OFFSET(daAndsw2_c, mEventIdx, 0x3B0);
WWHD_SIZE(daAndsw2_c, 0x3B4);
