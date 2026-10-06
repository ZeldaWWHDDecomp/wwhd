#pragma once
#include "f_op/f_op_actor.h"
struct daWarpfout_c : fopAc_ac_c {
    be<s32> mUnused, mTimer, mStaffId;
    be<s16> mInitFlag;
    u8 _3ba[2];
};
WWHD_OFFSET(daWarpfout_c,mTimer,0x3B0);
WWHD_OFFSET(daWarpfout_c,mStaffId,0x3B4);
WWHD_OFFSET(daWarpfout_c,mInitFlag,0x3B8);
WWHD_SIZE(daWarpfout_c,0x3BC);
