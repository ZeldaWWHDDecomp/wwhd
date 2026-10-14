/** HD Heart UI part. Reuses the canonical parts framework. */
#pragma once
#include "d/hd_parts.h"
struct hdHeartPane_l : hdPartsPane_c {
    u8 mToFlags[0x44 - sizeof(hdPartsPane_c)];
    be<u8> mFlags;
};
struct hdHeartPart_c : hdParts_c {
    gptr<hdHeartPane_l> mpPane;
    be<u32> mValue, mRequestedState;
    be<u8> mReserved, mLowLifeMode, mSkipTransition;
    u8 mTail;
    static void destroy(hdHeartPart_c*, u32);
    void waitMove();
    void waitLeave();
    void toMaxEnter();
    void toMaxMove();
    void toMaxLeave();
    void toNormalEnter();
    void toNormalMove();
    void toNormalLeave();
    BOOL load(void*, void*, void*, void*);
    void update();
    void tick();
    void setValue(u32);
    void setVisible(u32);
    BOOL loadLayout(void*, void*, void*, void*, void*);
};
struct hdHeartSave_l {
    u8 mPrefix[0x22];
    be<u16> mLife;
};
struct hdHeartHud_l {
    u8 mPrefix[0x3C];
    be<u16> mMode;
};
WWHD_SIZE(hdHeartPart_c, 0x60);
WWHD_OFFSET(hdHeartPane_l, mFlags, 0x44);
WWHD_OFFSET(hdHeartPart_c, mpPane, 0x50);

WWHD_OFFSET(hdHeartPart_c, mpLayout, 0x44);
WWHD_OFFSET(hdHeartPart_c, mRequestedState, 0x58);
WWHD_OFFSET(hdHeartPart_c, mSkipTransition, 0x5E);

struct hdHeartInitLists_l {
    be<u32> mFirst, mSecond, mThird, mFourth;
};
struct hdHeartInitAngles_l {
    be<f32> mMinimum, mMaximum;
    be<u8> mFlag, mByte;
};
struct hdHeartInitRecord_l {
    u8 mStorage[12];
};
struct hdHeartAngleLimits_l {
    be<f32> mMinimum, mMaximum;
};
