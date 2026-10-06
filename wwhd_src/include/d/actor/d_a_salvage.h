#pragma once
#include "f_op/f_op_actor.h"
// WWHD actor layout. The salvage registry is shared static data, not an actor member.
struct daSalvage_c : fopAc_ac_c {
    u8 mPhase[8];
    gptr<void> mModelData;
    gptr<void> mModels[16];
    gptr<void> mpBrk, mpBtk, mEmitter;
    be<s8> mPreviousRoom;
    be<u8> mOrder, mInitialized;
    u8 pad407;
    be<u32> mChestId;
    be<s16> mGetItemEvent, mHazureEvent, mGetItemLeftEvent, mHazureLeftEvent;
    be<s8> mStayNo;
    u8 pad415[3];
    be<s32> mFadeDelay;
    be<u8> mRndDepthIdx, mHasBox, mTimeout;
    u8 pad41f;
    be<u32> mBoxId;
    be<s16> mProcAdjustment, mProcIndex;
    be<u32> mProc;
};
WWHD_OFFSET(daSalvage_c, mEmitter, 0x400);
WWHD_OFFSET(daSalvage_c, mProc, 0x428);
WWHD_SIZE(daSalvage_c, 0x42C);
