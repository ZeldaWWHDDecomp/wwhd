#pragma once
#include "wwhd.h"
struct hdUiSafeString;
// HD-only text pane and its message conversion workspace; offsets follow the RPX.
struct PaneString_l {
    gptr<void> mpBuffer;
    gptr<gfn> mpVTable;
    be<u32> mCapacity;
};
struct PaneFont_l {
    u8 mReserved[4];
    gptr<gfn> mpVTable;
};
struct PaneType_l {
    gptr<PaneType_l> mpParent;
};
struct PaneProcessor_l {
    gptr<gfn> mpVTable;
};
struct PaneLabelTable_l {
    gptr<void> mpData;
    be<u32> mCount;
};
struct PaneWork_l {
    u8 mReserved[8];
    gptr<PaneLabelTable_l> mpLabels;
    u8 mArguments[0x10C];
    be<u32> mLabel;
    u8 mReserved11C[4];
    PaneString_l mLabelName;
    u8 mReserved12C[0x104];
    PaneString_l mText;
    u8 mReserved23C[0x400];
    be<u32> mLength;
    u8 mReserved640[0x14];
    be<u32> mMessage;
    u8 mReserved658[0x38];
    be<u32> mFont;
    u8 mReserved694[5];
    be<u8> mForce, mShrink;
    u8 mReserved69B[0x8F8 - 0x69B];
    be<u32> mYear, mMonth, mDay, mHour, mMinute; /* 0x8F8: date/time arguments (PICTO-best_shot_mark+btn_pictograph) */
    u8 mReserved90C[0x915 - 0x90C];
    be<u32> mColor;
    u8 mReserved919[3];
};
struct PaneLayout_l {
    u8 mReserved[0x1C];
    gptr<const char> mpName;
};
struct hdTextPane_c {
    u8 mReserved[8];
    gptr<gfn> mpVTable;
    u8 mReservedC[0x30];
    be<f32> mWidth, mHeight;
    be<u8> mPaneFlags;
    u8 mReserved45[0x3B];
    char mName[0x24];
    gptr<be<u16>> mpText;
    u8 mReservedA8[0x10];
    be<f32> mFontWidth, mFontHeight, mLineSpace, mCharSpace;
    gptr<PaneProcessor_l> mpProcessor;
    u8 mReservedCC[0x28];
    be<u8> mTextFlags;
    u8 mReservedF5[0xF];
    be<u32> mDefaultWidthBits, mDefaultHeightBits;
    u8 mReserved10C[8];
    be<u32> mFont;
    be<u32> mCapacity, mColor;
    be<u8> mShrink;
    u8 mReserved121[3];
    gptr<PaneLayout_l> mpLayout;
    u8 mMessageParameters[0x10C];
    gptr<be<u16>> mpFallback;
    be<u16> mFallbackSize;
    u8 mReserved23A[2];
    PaneWork_l mWork;
    u8 mReservedB58[8];
    be<f32> mMinimumWidth, mGlyphScale;
    void setCapacity(u32);
    void setAutomaticCapacity();
    void setFontPreservingSize(u32);
    void fitText();
    void applyText(PaneString_l *, u32, u32, PaneWork_l *, u32);
    void convertText(PaneString_l *, void *, u32, PaneWork_l *);
    void setLabel(PaneWork_l *, u32);
    u32 loadMessage();
    void initializeMessage();
    void prepareMessage(PaneWork_l *, u32);
    void resetMessage(hdUiSafeString* message, hdUiSafeString* label);
    void selectMessage(u32);
    void setString(PaneString_l *, u32, PaneWork_l *);
    void setStringPrepared(PaneString_l *, PaneWork_l *);
    void refresh(PaneWork_l *);
    f32 getTextWidth(be<u16> *);
    f32 getTextWidth();
    f32 getTextHeight(be<u16> *, s32);
    f32 getTextHeight(s32);
    void setCharacterCapacity(u32);
};
WWHD_OFFSET(hdTextPane_c, mpText, 0xA4);
WWHD_OFFSET(hdTextPane_c, mFontWidth, 0xB8);
WWHD_OFFSET(hdTextPane_c, mTextFlags, 0xF4);
WWHD_OFFSET(hdTextPane_c, mCapacity, 0x118);
WWHD_OFFSET(hdTextPane_c, mWork, 0x23C);
WWHD_OFFSET(hdTextPane_c, mMinimumWidth, 0xB60);
WWHD_SIZE(hdTextPane_c, 0xB68);
WWHD_OFFSET(PaneWork_l, mText, 0x230);
WWHD_OFFSET(PaneWork_l, mLength, 0x63C);
WWHD_OFFSET(PaneWork_l, mFont, 0x690);
WWHD_OFFSET(PaneWork_l, mColor, 0x915);
WWHD_SIZE(PaneWork_l, 0x91C);
WWHD_OFFSET(PaneWork_l, mYear, 0x8F8);
WWHD_OFFSET(PaneWork_l, mMinute, 0x908);

/* adapter kept for callers written against bd575401: the member now takes the two contexts itself */
inline void hdTextPane_resetMessage(hdTextPane_c* pane, hdUiSafeString* name, hdUiSafeString* label) {
    pane->resetMessage(name, label);
}
