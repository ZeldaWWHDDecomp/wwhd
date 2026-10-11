#pragma once
#include "wwhd.h"
// HD: scoped message typing/wait states and item scheduling have private layouts.
struct WindowMsgInput {
    u8 mReserved0[2];
    be<u8> mNoSkip;
};
static_assert(offsetof(WindowMsgInput, mNoSkip) == 0x2);
struct WindowMessage {
    be<u32> mState;
    u8 mReserved4[280];
    be<u32> mMessageNumber;
    u8 mReserved120[268];
    gptr<WindowMsgInput> mpInput;
    u8 mReserved230[1056];
    be<u32> mHasNext;
    u8 mReserved654[4];
    be<u8> mLayoutDone;
    u8 mReserved659[62];
    be<u8> mTimedWait;
    be<u8> mInputWait;
    u8 mReserved699[19];
    be<s32> mTypingPause;
    u8 mReserved6B0[4];
    be<s32> mWaitTimer;
    be<u32> mNextMode;
    u8 mReserved6BC[593];
    be<u8> mConfirmInput;
    be<u8> mCloseRequested;
    be<u8> mCloseBlocked;
    be<u32> mNextMessageNumber;
    u8 mReserved914[9];
    be<u32> mTextColor;
};
static_assert(offsetof(WindowMessage, mState) == 0x0);
static_assert(offsetof(WindowMessage, mMessageNumber) == 0x11C);
static_assert(offsetof(WindowMessage, mpInput) == 0x22C);
static_assert(offsetof(WindowMessage, mHasNext) == 0x650);
static_assert(offsetof(WindowMessage, mLayoutDone) == 0x658);
static_assert(offsetof(WindowMessage, mTimedWait) == 0x697);
static_assert(offsetof(WindowMessage, mInputWait) == 0x698);
static_assert(offsetof(WindowMessage, mTypingPause) == 0x6AC);
static_assert(offsetof(WindowMessage, mWaitTimer) == 0x6B4);
static_assert(offsetof(WindowMessage, mNextMode) == 0x6B8);
static_assert(offsetof(WindowMessage, mConfirmInput) == 0x90D);
static_assert(offsetof(WindowMessage, mCloseRequested) == 0x90E);
static_assert(offsetof(WindowMessage, mCloseBlocked) == 0x90F);
static_assert(offsetof(WindowMessage, mNextMessageNumber) == 0x910);
static_assert(offsetof(WindowMessage, mTextColor) == 0x91D);
struct WindowPane {
    u8 mReserved0[68];
    be<u8> mFlags;
    u8 mReserved45[103];
    be<u32> mTopColor;
    be<u32> mBottomColor;
};
static_assert(offsetof(WindowPane, mFlags) == 0x44);
static_assert(offsetof(WindowPane, mTopColor) == 0xAC);
static_assert(offsetof(WindowPane, mBottomColor) == 0xB0);
struct WindowCore {
    u8 mReserved0[2376];
    be<u32> mSelection;
    u8 mReserved94C[24];
    gptr<void> mpSelector;
    u8 mReserved968[24];
    be<f32> mTypingSpeed;
};
static_assert(offsetof(WindowCore, mSelection) == 0x948);
static_assert(offsetof(WindowCore, mpSelector) == 0x964);
static_assert(offsetof(WindowCore, mTypingSpeed) == 0x980);
struct WindowPlay {
    u8 mReserved0[23344];
    be<s8> mControllerIndex;
    u8 mReserved5B31[49];
    be<s16> mProgress;
    u8 mReserved5B64[82];
    be<u8> mMessageMode;
    u8 mReserved5BB7[3];
    be<u8> mMessageSoundMode;
    u8 mReserved5BBB[23];
    be<u8> mAdvanceCount;
    be<u8> mHandInput;
    u8 mReserved5BD4[76];
    be<u8> mInputBlocked;
    u8 mReserved5C21[15];
    be<u32> mWaitValue;
};
static_assert(offsetof(WindowPlay, mControllerIndex) == 0x5B30);
static_assert(offsetof(WindowPlay, mProgress) == 0x5B62);
static_assert(offsetof(WindowPlay, mMessageMode) == 0x5BB6);
static_assert(offsetof(WindowPlay, mMessageSoundMode) == 0x5BBA);
static_assert(offsetof(WindowPlay, mAdvanceCount) == 0x5BD2);
static_assert(offsetof(WindowPlay, mHandInput) == 0x5BD3);
static_assert(offsetof(WindowPlay, mInputBlocked) == 0x5C20);
static_assert(offsetof(WindowPlay, mWaitValue) == 0x5C30);
struct WindowBase {
    u8 mReserved0[4];
    gptr<gfn> mpVTable;
    u8 mReserved8[16];
    be<u32> mStateMachine;
    u8 mReserved1C[52];
    be<u32> mTypingFlags;
    be<u32> mInputFlags;
    u8 mReserved58[4];
    be<u32> mMessageId;
    u8 mReserved60[8];
    gptr<WindowPane> mpTextPane;
    u8 mReserved6C[16];
    gptr<WindowPane> mpNumberPane;
    u8 mReserved80[4];
    be<u32> mAdvanceCount;
    be<u32> mWaitValue;
    be<u32> mWaitTimer;
    be<s32> mCharacterCount;
    union {
        be<f32> mTypingAccumulator;
        be<u32> mTypingAccumulatorBits;
    };
    be<s32> mStopOffsets[10];
    be<s32> mStopCounts[10];
    be<u32> mStopIndex;
    u8 mReservedEC[1];
    be<u8> mStopReady;
    u8 mReservedEE[6];
    be<u32> mSelection;
    u8 mReservedF8[24];
    be<u8> mOpenEnabled;
    be<u8> mOpenBlocked;
    u8 mReserved112[6];
    gptr<WindowMessage> mpMessage;
};
static_assert(offsetof(WindowBase, mpVTable) == 0x4);
static_assert(offsetof(WindowBase, mStateMachine) == 0x18);
static_assert(offsetof(WindowBase, mTypingFlags) == 0x50);
static_assert(offsetof(WindowBase, mInputFlags) == 0x54);
static_assert(offsetof(WindowBase, mMessageId) == 0x5C);
static_assert(offsetof(WindowBase, mpTextPane) == 0x68);
static_assert(offsetof(WindowBase, mpNumberPane) == 0x7C);
static_assert(offsetof(WindowBase, mAdvanceCount) == 0x84);
static_assert(offsetof(WindowBase, mWaitValue) == 0x88);
static_assert(offsetof(WindowBase, mWaitTimer) == 0x8C);
static_assert(offsetof(WindowBase, mCharacterCount) == 0x90);
static_assert(offsetof(WindowBase, mTypingAccumulator) == 0x94);
static_assert(offsetof(WindowBase, mStopOffsets) == 0x98);
static_assert(offsetof(WindowBase, mStopCounts) == 0xC0);
static_assert(offsetof(WindowBase, mStopIndex) == 0xE8);
static_assert(offsetof(WindowBase, mStopReady) == 0xED);
static_assert(offsetof(WindowBase, mSelection) == 0xF4);
static_assert(offsetof(WindowBase, mOpenEnabled) == 0x110);
static_assert(offsetof(WindowBase, mOpenBlocked) == 0x111);
static_assert(offsetof(WindowBase, mpMessage) == 0x118);
struct WindowAnimator {
    u8 mReserved0[20];
    gptr<gfn> mpVTable;
    u8 mReserved18[12];
    be<f32> mProgress;
    u8 mReserved28[4];
    be<u32> mFlags;
};
static_assert(offsetof(WindowAnimator, mpVTable) == 0x14);
static_assert(offsetof(WindowAnimator, mProgress) == 0x24);
static_assert(offsetof(WindowAnimator, mFlags) == 0x2C);
struct WindowPartConfig {
    u8 mReserved0[76];
    be<s32> mDuration;
    be<u32> mExtraItems;
    be<u32> mOrder;
    be<u32> mMode;
    u8 mReserved5C[1];
    be<u8> mFinishEnabled;
    u8 mReserved5E[2];
    be<u32> mAnimatorCount;
    u8 mReserved64[4];
    gptr<gptr<WindowAnimator>> mpAnimators;
};
static_assert(offsetof(WindowPartConfig, mDuration) == 0x4C);
static_assert(offsetof(WindowPartConfig, mExtraItems) == 0x50);
static_assert(offsetof(WindowPartConfig, mOrder) == 0x54);
static_assert(offsetof(WindowPartConfig, mMode) == 0x58);
static_assert(offsetof(WindowPartConfig, mFinishEnabled) == 0x5D);
static_assert(offsetof(WindowPartConfig, mAnimatorCount) == 0x60);
static_assert(offsetof(WindowPartConfig, mpAnimators) == 0x68);
struct WindowPart {
    u8 mReserved0[3316];
    gptr<WindowPartConfig> mpConfig;
    gptr<be<s32>> mpOrder;
    be<f32> mTime;
    be<s32> mItemCount;
    be<s32> mDelay;
    be<s32> mRemaining;
    be<s32> mIntervalDivisor;
    be<u8> mScheduling;
    u8 mReservedD11[3];
    gptr<WindowAnimator> mpCurrentAnimator;
};
static_assert(offsetof(WindowPart, mpConfig) == 0xCF4);
static_assert(offsetof(WindowPart, mpOrder) == 0xCF8);
static_assert(offsetof(WindowPart, mTime) == 0xCFC);
static_assert(offsetof(WindowPart, mItemCount) == 0xD00);
static_assert(offsetof(WindowPart, mDelay) == 0xD04);
static_assert(offsetof(WindowPart, mRemaining) == 0xD08);
static_assert(offsetof(WindowPart, mIntervalDivisor) == 0xD0C);
static_assert(offsetof(WindowPart, mScheduling) == 0xD10);
static_assert(offsetof(WindowPart, mpCurrentAnimator) == 0xD14);
struct WindowSave {
    u8 mReserved0[51];
    be<u8> mProgressTarget;
};
static_assert(offsetof(WindowSave, mProgressTarget) == 0x33);
struct WindowProgress {
    u8 mReserved0[80];
    be<u32> mTarget;
    be<u32> mShown;
};
static_assert(offsetof(WindowProgress, mTarget) == 0x50);
static_assert(offsetof(WindowProgress, mShown) == 0x54);
struct WindowController {
    be<u32> mFlags;
};
static_assert(offsetof(WindowController, mFlags) == 0x0);
struct WindowState {
    u8 mReserved[0x30];
};
struct WindowNumberLimits {
    be<s16> mValues[4];
};
using WindowCorePointer = gptr<WindowCore>;
using WindowSavePointer = gptr<WindowSave>;

enum WindowScheduleMode : u32 {
    WINDOW_SCHEDULE_TIMELINE = 0,
    WINDOW_SCHEDULE_REMAINING_SQUARED = 1,
    WINDOW_SCHEDULE_ELAPSED_SQUARED = 2,
    WINDOW_SCHEDULE_RANDOM_DELAY = 3,
    WINDOW_SCHEDULE_EXTENDED_TIMELINE = 4
};
enum WindowItemOrder : u32 {
    WINDOW_ITEMS_FORWARD = 0,
    WINDOW_ITEMS_REVERSE = 1,
    WINDOW_ITEMS_CENTER_OUT = 2,
    WINDOW_ITEMS_EDGES_IN = 3,
    WINDOW_ITEMS_CONFIGURED = 4
};
enum WindowInputFlags : u32 {
    WINDOW_INPUT_CONFIRM = 1,
    WINDOW_INPUT_SKIP = 2,
    WINDOW_INPUT_PREVIOUS_KEY = 0x10000,
    WINDOW_INPUT_NEXT_KEY = 0x20000,
    WINDOW_INPUT_PREVIOUS_STICK = 0x100000,
    WINDOW_INPUT_NEXT_STICK = 0x200000
};
enum WindowTypingFlags : u32 { WINDOW_TYPING_SPEEDUP = 1, WINDOW_TYPING_SKIP = 2 };
enum WindowSoundId : u32 {
    WINDOW_SOUND_CLOSE = 0x80B,
    WINDOW_SOUND_ACCEPT = 0x80D,
    WINDOW_SOUND_SELECTION = 0x83D,
    WINDOW_SOUND_CANCEL = 0x880,
    WINDOW_SOUND_NUMBER = 0x8E1
};
