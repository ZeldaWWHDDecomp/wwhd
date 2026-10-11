#pragma once
#include "gabi.h"
struct HdMsgSet {
    gabi::gptr<void> mpData;
    gabi::be<u32> mEntryCount;
};
struct HdMsgEntry {
    u8 mReserved;
    gabi::be<u8> mTag;
};
struct HdMsgFont {
    u8 mReserved[4];
    gabi::gptr<gabi::gfn> mpVTable;
};
struct HdMsgString {
    gabi::gptr<char> mpBuffer;
    gabi::gptr<gabi::gfn> mpVTable;
    gabi::be<s32> mCapacity;
};
struct HdMsgFixedString {
    HdMsgString mString;
    gabi::be<u8> mBuffer[256];
};
struct HdMsgWideText {
    HdMsgString mString;
    gabi::be<u16> mBuffer[512];
};
struct HdMsgWideRuby {
    HdMsgString mString;
    gabi::be<u16> mBuffer[128];
};
// HD: this state contains five-line metrics and packed, unaligned tail words.
struct HdMsgText {
    gabi::be<u32> mState;
    gabi::be<u32> mNextState;
    gabi::gptr<HdMsgSet> mpMessageSet;
    HdMsgFixedString mSetName;
    gabi::be<s32> mLabelIndex;
    gabi::be<u32> mMessageNumber;
    HdMsgFixedString mLabel;
    gabi::gptr<HdMsgEntry> mpEntry;
    HdMsgWideText mText;
    gabi::be<u32> mCharacterIndex;
    gabi::be<u32> mTextStyle;
    gabi::be<u32> mLineCount;
    gabi::be<u8> mLineReady;
    u8 mReserved649[3];
    gabi::be<u32> mLayoutMode;
    gabi::be<u32> mLayoutProgress;
    gabi::be<u32> mLayoutLimit;
    gabi::be<u8> mLayoutComplete;
    gabi::be<u8> mWaitComplete;
    u8 mReserved65A[2];
    gabi::be<u32> mCurrentTag;
    gabi::be<u32> mLineValues[5];
    gabi::be<f32> mLineWidths[5];
    gabi::be<f32> mLayoutWidth;
    gabi::be<u32> mDrawIndex;
    gabi::gptr<HdMsgFont> mpFont;
    gabi::be<u8> mDrawEnabled;
    gabi::be<u8> mFlag695;
    gabi::be<u8> mFlag696;
    gabi::be<u8> mFlag697;
    gabi::be<u8> mFlag698;
    gabi::be<u8> mFlag699;
    gabi::be<u8> mFlag69A;
    gabi::be<u8> mFlag69B;
    gabi::be<u8> mFlag69C;
    u8 mReserved69D[3];
    gabi::be<u32> mSoundPosition[3];
    gabi::be<u32> mSoundState;
    gabi::be<u32> mSoundMode;
    gabi::be<u32> mSoundIndex;
    gabi::be<u32> mCurrentRuby;
    gabi::be<u32> mRubyState;
    HdMsgWideRuby mRubyText;
    HdMsgWideRuby mRubyReading;
    gabi::be<u32> mFontState;
    gabi::be<u8> mPictographFont;
    u8 mReserved8DD[3];
    gabi::be<f32> mScaleX;
    gabi::be<f32> mScaleY;
    gabi::be<f32> mFontScale;
    gabi::be<f32> mCurrentLineWidth;
    gabi::be<f32> mPreviousLineWidth;
    gabi::be<u32> mCurrentLine;
    gabi::be<u32> mLineFlags[5];
    gabi::be<u8> mFlag90C;
    gabi::be<u8> mFlag90D;
    gabi::be<u8> mFlag90E;
    gabi::be<u8> mFlag90F;
    gabi::be<u32> mWaitState;
    gabi::be<u8> mFlag914;
    gabi::be<u32> mPackedValue1;
    gabi::be<u32> mPackedValue2;
    gabi::be<u32> mPackedValue3;
    gabi::be<u8> mFlag921;
    gabi::be<u8> mFlag922;
    gabi::be<u8> mFlag923;
};
static_assert(sizeof(HdMsgText) == 0x924);
static_assert(__builtin_offsetof(HdMsgText, mState) == 0x0);
static_assert(__builtin_offsetof(HdMsgText, mNextState) == 0x4);
static_assert(__builtin_offsetof(HdMsgText, mpMessageSet) == 0x8);
static_assert(__builtin_offsetof(HdMsgText, mSetName) == 0xC);
static_assert(__builtin_offsetof(HdMsgText, mLabelIndex) == 0x118);
static_assert(__builtin_offsetof(HdMsgText, mMessageNumber) == 0x11C);
static_assert(__builtin_offsetof(HdMsgText, mLabel) == 0x120);
static_assert(__builtin_offsetof(HdMsgText, mpEntry) == 0x22C);
static_assert(__builtin_offsetof(HdMsgText, mText) == 0x230);
static_assert(__builtin_offsetof(HdMsgText, mCharacterIndex) == 0x63C);
static_assert(__builtin_offsetof(HdMsgText, mTextStyle) == 0x640);
static_assert(__builtin_offsetof(HdMsgText, mLineCount) == 0x644);
static_assert(__builtin_offsetof(HdMsgText, mLineReady) == 0x648);
static_assert(__builtin_offsetof(HdMsgText, mLayoutMode) == 0x64C);
static_assert(__builtin_offsetof(HdMsgText, mLayoutProgress) == 0x650);
static_assert(__builtin_offsetof(HdMsgText, mLayoutLimit) == 0x654);
static_assert(__builtin_offsetof(HdMsgText, mLayoutComplete) == 0x658);
static_assert(__builtin_offsetof(HdMsgText, mWaitComplete) == 0x659);
static_assert(__builtin_offsetof(HdMsgText, mCurrentTag) == 0x65C);
static_assert(__builtin_offsetof(HdMsgText, mLineValues) == 0x660);
static_assert(__builtin_offsetof(HdMsgText, mLineWidths) == 0x674);
static_assert(__builtin_offsetof(HdMsgText, mLayoutWidth) == 0x688);
static_assert(__builtin_offsetof(HdMsgText, mDrawIndex) == 0x68C);
static_assert(__builtin_offsetof(HdMsgText, mpFont) == 0x690);
static_assert(__builtin_offsetof(HdMsgText, mDrawEnabled) == 0x694);
static_assert(__builtin_offsetof(HdMsgText, mFlag695) == 0x695);
static_assert(__builtin_offsetof(HdMsgText, mFlag696) == 0x696);
static_assert(__builtin_offsetof(HdMsgText, mFlag697) == 0x697);
static_assert(__builtin_offsetof(HdMsgText, mFlag698) == 0x698);
static_assert(__builtin_offsetof(HdMsgText, mFlag699) == 0x699);
static_assert(__builtin_offsetof(HdMsgText, mFlag69A) == 0x69A);
static_assert(__builtin_offsetof(HdMsgText, mFlag69B) == 0x69B);
static_assert(__builtin_offsetof(HdMsgText, mFlag69C) == 0x69C);
static_assert(__builtin_offsetof(HdMsgText, mSoundPosition) == 0x6A0);
static_assert(__builtin_offsetof(HdMsgText, mSoundState) == 0x6AC);
static_assert(__builtin_offsetof(HdMsgText, mSoundMode) == 0x6B0);
static_assert(__builtin_offsetof(HdMsgText, mSoundIndex) == 0x6B4);
static_assert(__builtin_offsetof(HdMsgText, mCurrentRuby) == 0x6B8);
static_assert(__builtin_offsetof(HdMsgText, mRubyState) == 0x6BC);
static_assert(__builtin_offsetof(HdMsgText, mRubyText) == 0x6C0);
static_assert(__builtin_offsetof(HdMsgText, mRubyReading) == 0x7CC);
static_assert(__builtin_offsetof(HdMsgText, mFontState) == 0x8D8);
static_assert(__builtin_offsetof(HdMsgText, mPictographFont) == 0x8DC);
static_assert(__builtin_offsetof(HdMsgText, mScaleX) == 0x8E0);
static_assert(__builtin_offsetof(HdMsgText, mScaleY) == 0x8E4);
static_assert(__builtin_offsetof(HdMsgText, mFontScale) == 0x8E8);
static_assert(__builtin_offsetof(HdMsgText, mCurrentLineWidth) == 0x8EC);
static_assert(__builtin_offsetof(HdMsgText, mPreviousLineWidth) == 0x8F0);
static_assert(__builtin_offsetof(HdMsgText, mCurrentLine) == 0x8F4);
static_assert(__builtin_offsetof(HdMsgText, mLineFlags) == 0x8F8);
static_assert(__builtin_offsetof(HdMsgText, mFlag90C) == 0x90C);
static_assert(__builtin_offsetof(HdMsgText, mFlag90D) == 0x90D);
static_assert(__builtin_offsetof(HdMsgText, mFlag90E) == 0x90E);
static_assert(__builtin_offsetof(HdMsgText, mFlag90F) == 0x90F);
static_assert(__builtin_offsetof(HdMsgText, mWaitState) == 0x910);
static_assert(__builtin_offsetof(HdMsgText, mFlag914) == 0x914);
static_assert(__builtin_offsetof(HdMsgText, mPackedValue1) == 0x915);
static_assert(__builtin_offsetof(HdMsgText, mPackedValue2) == 0x919);
static_assert(__builtin_offsetof(HdMsgText, mPackedValue3) == 0x91D);
static_assert(__builtin_offsetof(HdMsgText, mFlag921) == 0x921);
static_assert(__builtin_offsetof(HdMsgText, mFlag922) == 0x922);
static_assert(__builtin_offsetof(HdMsgText, mFlag923) == 0x923);
struct HdMsgProject {
    u8 mReserved[12];
    gabi::be<u32> mStyleCount;
    gabi::gptr<void> mpStyles;
};
struct HdMsgStyle {
    gabi::be<u32> mReserved;
    gabi::be<u32> mStyle;
};
struct HdMsgSystemFlags {
    u8 mReserved[0x1C0];
    gabi::be<u8> mNoPictographFont;
};
struct HdMsgPlayState {
    u8 mReserved[0x5BB3];
    gabi::be<u8> mMessageState;
};
struct HdMsgStaticWords {
    gabi::be<u32> mWords[4];
};
struct HdMsgRange {
    gabi::be<f32> mLow, mHigh;
};
struct HdMsgRegistration {
    u8 mBytes[12];
};
struct HdMsgLabelFrame {
    HdMsgFixedString mString;
    u8 mReserved[4];
};
struct HdMsgSoundPosition {
    gabi::be<u32> mBits[3];
};
struct HdMsgScale {
    gabi::be<f32> mX, mY;
};
using HdMsgVoidPointer = gabi::gptr<void>;
using HdMsgSystemPointer = gabi::gptr<HdMsgSystemFlags>;

enum HdMsgTextState : u32 { HD_MSG_TEXT_IDLE = 0, HD_MSG_TEXT_WAIT = 6 };
static_assert(sizeof(HdMsgLabelFrame) == 0x110);
