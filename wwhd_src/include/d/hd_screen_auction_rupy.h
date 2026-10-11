#pragma once
#include "wwhd.h"
// HD: auction meter/counter screen, safe strings and state delegates.
struct RupyString {
    gptr<be<u8>> mpBuffer;
    gptr<gfn> mpVTable;
};
struct RupyAnimation {
    u8 mReserved[4];
    be<u32> mCount;
    gptr<gptr<void>> mpFrames;
};
struct RupyLayout {
    u8 mReserved[0xD4];
    gptr<RupyAnimation> mpAnimation;
    u8 mReservedD8[8];
    gptr<gfn> mpVTable;
};
struct RupyState {
    be<u32> mId;
    gptr<be<u8>> mpName;
    gptr<gfn> mpVTable;
    be<u16> mThisInit, mInitIndex;
    be<u32> mInitKind;
    be<u16> mThisMove, mMoveIndex;
    be<u32> mMoveKind;
    be<u16> mThisExit, mExitIndex;
    be<u32> mExitKind;
    be<u16> mThisFinal, mFinalIndex;
    be<u32> mFinalKind;
    gptr<void> mpOwner;
};
struct RupyStateMachine {
    gptr<gfn> mpVTable;
};
struct RupyStateAction {
    gptr<gfn> mpVTable;
};
struct RupyScreen {
    be<u32> mReserved;
    gptr<gfn> mpVTable;
    u8 mReserved8[0x10];
    gptr<RupyStateMachine> mpStateMachine;
    be<u32> mReserved1C;
    gptr<RupyStateAction> mpCurrentAction;
    u8 mReserved24[8];
    gptr<void> mpDelegateVTableA, mpDelegateVTableB;
    gptr<void> mpArchive;
    u8 mReserved38[12];
    gptr<RupyLayout> mpLayout;
    be<u32> mChildCount;
    gptr<gptr<void>> mpChildren;
    be<f32> mMeterFrames, mShownRupees;
    be<u32> mCounterDelay;
    be<s16> mCounterValue, mDigits[3];
    be<u8> mOpen, mCounting, mCounterStarted, mCounterEnabled, mCounterShown, mCounterIdle,
        mMeterEnabled, mMeterShown;
};
struct RupyPlay {
    u8 mReserved[0x5BA6];
    be<s16> mBid, mMeter;
};
struct RupyUI {
    u8 mReserved[0x178];
    gptr<void> mpAuction;
};
struct RupyHeap {
    u8 mReserved[0x10];
    gptr<void> mpHeap;
};
struct RupyPart {
    u8 mReserved[8];
    RupyString mType;
    u8 mReserved10[4];
    RupyString mName;
};
struct RupySharedStrings {
    RupyString mNames[3];
};
struct RupyAnimationNames {
    RupyString mNames[6];
};
struct RupyLayoutNames {
    RupyString mNames[2];
};
struct RupyWords {
    be<u32> mValues[4];
};
struct RupyRange {
    be<f32> mLow, mHigh;
};
struct RupyScales {
    be<f32> mValues[4];
};
struct RupyRegistration {
    u8 mBytes[12];
};
struct RupyAnimationMap {
    be<u32> mGroups[6];
};
static_assert(sizeof(RupyScreen) == 0x6C);
static_assert(offsetof(RupyScreen, mpLayout) == 0x44);
static_assert(offsetof(RupyScreen, mCounterValue) == 0x5C);
static_assert(offsetof(RupyScreen, mMeterEnabled) == 0x6A);
static_assert(sizeof(RupyState) == 0x30);
struct RupyFrame {
    u8 mReserved[0x34];
    gptr<void> mpResource;
};
using RupyUIPointer = gptr<RupyUI>;
using RupyHeapPointer = gptr<RupyHeap>;
enum RupyAnimationId : u32 {
    RUPY_COUNTER_APPEAR = 0,
    RUPY_COUNTER_COUNT = 1,
    RUPY_COUNTER_HIDE = 2,
    RUPY_METER_APPEAR = 3,
    RUPY_METER_FILL = 4,
    RUPY_METER_HIDE = 5
};
enum RupySoundId : u32 { RUPY_SOUND_COUNT = 0x8DF, RUPY_SOUND_FINISHED = 0x8E0 };
