/** WindowPart text layout and per-character animation scheduling. */
#pragma once
#include "d/hd_parts_button.h"
#include "d/hd_msg_window_base.h"
struct hdWindowPaneVtable_l {
    u8 mPrefix[0xC];
    gabi::gfn mDestroy;
    u8 mToUpdate[0x4C - 0x10];
    gabi::gfn mUpdate;
    u8 mToTransform[0x64 - 0x50];
    gabi::gfn mSetTransform;
};
struct hdWindowPaneTransform_l;
struct hdWindowPane_l {
    u8 mPrefix[0xC];
    gptr<hdWindowPaneTransform_l> mpParentTransform;
    u8 mToVtable[0x30 - 0x10];
    gptr<hdWindowPaneVtable_l> mVtable;
    u8 mToFlags[0x44 - 0x34];
    be<u8> mFlags;
    u8 mStorage[0xA4 - 0x45];
    gptr<be<u16>> mText;
    u8 mToFont[0xC0 - 0xA8];
    be<f32> mFontScale;
    u8 mGap[4];
    gptr<void> mpFont;
    u8 mToCount[2];
    be<u16> mCharacterCount;
    be<u8> mAlignment;
};
struct hdWindowSettings_l {
    be<u8> mFixedWidth;
    u8 mPadding[3];
    be<f32> mWidth, mSpacing, mLeft, mRight;
    be<u32> mAlignment;
    be<u8> mRandomize;
    u8 mGap[3];
    be<f32> mMinimumJitter, mMaximumJitter, mCenter, mFontScale;
};
struct hdWindowAnimatorVtable_l {
    u8 mPrefix[0x24];
    gabi::gfn mSetEnabled;
    u8 mToPlay[0xAC - 0x28];
    gabi::gfn mPlay;
    u8 mToFinish[0xCC - 0xB0];
    gabi::gfn mFinish;
};
struct hdWindowAnimator_l {
    u8 mPrefix[0xC];
    be<f32> mFrame;
    u8 mGapToVtable[4];
    gptr<hdWindowAnimatorVtable_l> mVtable;
    u8 mStorage[0x24 - 0x18];
    be<f32> mProgress;
    u8 mGap[4];
    be<u32> mFlags;
};
struct hdWindowConfigName_l : hdUiSafeString {
    be<u32> mCapacity;
    be<u8> mBuffer[64];
};
struct hdWindowConfig_l {
    hdWindowConfigName_l mName;
    be<s32> mDuration;
    be<u32> mExtraItems, mOrder, mMode;
    be<u8> mResetLayout, mFinishEnabled;
    u8 mGap[2];
    be<u32> mAnimatorCount, mAnimatorCapacity;
    gptr<gptr<hdWindowAnimator_l>> mpAnimators;
    static void destroy(hdWindowConfig_l*, u32);
};
struct hdWindowTree_l;
struct hdWindowTreeNode_l {
    gptr<hdWindowTreeNode_l> mpLeft, mpRight;
    be<u8> mIsRed;
    u8 mColorPadding[3];
    hdUiSafeString mKey;
    be<u32> mValueVtable;
    gptr<hdWindowConfig_l> mpConfig;
    gptr<hdWindowTree_l> mpOwner;
    be<u8> mNameBuffer[65];
    u8 mPadding[3];
};
struct hdWindowTree_l {
    gptr<hdWindowTreeNode_l> mpRoot, mpFreeHead, mpNodes;
    be<u32> mCount, mCapacity;
    hdWindowTreeNode_l mNodes[32];
};
struct hdWindowPaneArray_l {
    be<u32> mCount, mCapacity;
    gptr<gptr<hdWindowPane_l>> mpItems;
};
struct hdWindowMetrics_l {
    be<f32> mLeft, mTop, mRight, mBottom;
};
struct hdWindowVector_l {
    be<f32> mX, mY, mZ;
};
struct hdWindowTransform_l {
    u8 mPrefix[0x40];
    be<u32> mMatrix[12];
    u8 mStorage[0xA8 - 0x70];
    gptr<void> mpHeap;
    u8 mTail[0xC8 - 0xAC];
};
struct hdWindowPart_c {
    gptr<void> mVtable;
    gptr<hdWindowPane_l> mpPane, mpRoot, mpTextPane;
    gptr<void> mpResource;
    be<u32> mLineCount;
    gptr<be<u32>> mpLineLengths;
    hdWindowPaneArray_l mPanes, mCharacters;
    hdWindowSettings_l mSettings;
    hdWindowTree_l mConfigs;
    gptr<hdWindowConfig_l> mpConfig;
    gptr<be<s32>> mpOrder;
    be<f32> mTime;
    be<s32> mItemCount, mDelay, mRemaining, mIntervalDivisor;
    be<u8> mScheduling;
    u8 mPadding[3];
    gptr<hdWindowAnimator_l> mpCurrentAnimator;
    hdWindowTransform_l mTransform;
    be<u32> mTransformFlags, mReserved;
    static void destroy(hdWindowPart_c*, u32);
    void clearConfigs();
    void clear();
    void createCharacters(void*);
    void positionCharacters();
    hdWindowConfig_l* createConfig(hdUiSafeString*, void*);
    void readMetrics(hdWindowPane_l*);
    f32 characterWidth(hdWindowPane_l*);
    f32 lineWidth(s32, s32);
    f32 lineStart(f32);
    f32 randomize(f32);
    void selectConfig(hdUiSafeString*);
    void stop();
    void reset(hdUiSafeString*);
    BOOL isAnimatorFlagged(hdUiSafeString*, u32);
    BOOL isLastAnimatorFlagged(hdUiSafeString*);
    BOOL hasRunningAnimator(hdUiSafeString*);
    void copyConfig(hdUiSafeString*, hdWindowConfig_l*);
    s32 intervalDivisor();
    void setConfig(hdWindowConfig_l*);
    void updateItem(s32);
    void shuffleOrder();
};
struct hdWindowLineCounts_l {
    be<u32> mCapacity;
    gptr<be<u32>> mpCounts;
};
struct hdWindowText_l {
    gptr<be<u16>> mText;
    gptr<hdUiStringTable> mVtable;
};
WWHD_SIZE(hdWindowSettings_l, 0x2C);
WWHD_SIZE(hdWindowTree_l, 0xC94);
WWHD_SIZE(hdWindowPart_c, 0xDE8);
WWHD_OFFSET(hdWindowPart_c, mConfigs, 0x60);
WWHD_OFFSET(hdWindowPart_c, mpConfig, 0xCF4);
WWHD_OFFSET(hdWindowPart_c, mTransform, 0xD18);

WWHD_OFFSET(hdWindowConfigName_l, mCapacity, 8);
WWHD_OFFSET(hdWindowConfigName_l, mBuffer, 0xC);
WWHD_SIZE(hdWindowConfigName_l, 0x4C);
