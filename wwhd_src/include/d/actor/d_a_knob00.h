#pragma once
#include "f_op/f_op_actor.h"

// WWHD door layout, confirmed from actor field accesses.
struct daKnob00_c : fopAc_ac_c {
 u8 _3AC[0x3BC-0x3AC];
 be<u8> mFrontCheck; u8 _3BD;
 be<s16> mEventIdx[12];
 be<u8> mToolId[12];
 be<u8> mEventNo; u8 _3E3;
 be<s32> mStaffId; u8 _3E8[0x500-0x3E8];
 gptr<J3DModel> mpModel; u8 _504[0x594-0x504];
 be<u32> mpBgW; u8 _598[12];
 be<u8> mAction; u8 _5A5;
 be<u8> mFlags; u8 _5A7;
 be<s16> mAdjustTimer;
 be<u8> mPasswordState;
 be<u8> mDoorType;
};
WWHD_OFFSET(daKnob00_c,mEventNo,0x3E2);
WWHD_OFFSET(daKnob00_c,mpModel,0x500);
WWHD_OFFSET(daKnob00_c,mAction,0x5A4);
WWHD_SIZE(daKnob00_c,0x5AC);
