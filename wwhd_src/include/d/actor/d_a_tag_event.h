#pragma once
#include "f_op/f_op_actor.h"

struct daTag_Event_c : fopAc_ac_c {
    /* 0x3AC */ be<s16> mEventIdx;
    /* 0x3AE */ be<s16> mDemoTimer;
    /* 0x3B0 */ be<s16> mHuntTimer;
    /* 0x3B2 */ be<u8> mAction;
    /* 0x3B3 */ be<u8> mBkType;
    /* 0x3B4 */ be<u32> mArrivalDelay;
};
WWHD_OFFSET(daTag_Event_c, mEventIdx, 0x3AC);
WWHD_OFFSET(daTag_Event_c, mDemoTimer, 0x3AE);
WWHD_OFFSET(daTag_Event_c, mHuntTimer, 0x3B0);
WWHD_OFFSET(daTag_Event_c, mAction, 0x3B2);
WWHD_OFFSET(daTag_Event_c, mBkType, 0x3B3);
WWHD_OFFSET(daTag_Event_c, mArrivalDelay, 0x3B4);
WWHD_SIZE(daTag_Event_c, 0x3B8);
