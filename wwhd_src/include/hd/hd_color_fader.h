/** HD color-fade callbacks. HD has two listener entries, one for each render pass. */
#pragma once
#include "bindings.h"
struct hdFadeColor_l { be<f32> mRed, mGreen, mBlue, mAlpha; };
WWHD_SIZE(hdFadeColor_l, 16);
struct hdFadeColorBytes_l { be<u8> mRed, mGreen, mBlue, mAlpha; };
WWHD_SIZE(hdFadeColorBytes_l, 4);
struct hdFadeDelegate_l {
    be<u32> mVtable;
    gptr<void> mpOwner;
    be<u16> mAdjustment;
    be<s16> mVirtualIndex;
    be<u32> mCallback;
};
WWHD_SIZE(hdFadeDelegate_l, 0x10);
struct hdFadeListener_l {
    u8 mDisposer[12];
    be<u32> mVtable;
    be<u32> mListenerVtable;
    be<u32> mInterfaceVtable;
    be<u32> mEnabled;
    be<u32> mUserData;
    hdFadeDelegate_l mDelegate;
    u8 mReserved[16];
};
WWHD_OFFSET(hdFadeListener_l, mVtable, 0x0C);
WWHD_OFFSET(hdFadeListener_l, mDelegate, 0x20);
WWHD_SIZE(hdFadeListener_l, 0x40);
struct hdColorFader_c {
    hdFadeListener_l mSceneListener;
    hdFadeListener_l mForegroundListener;
    be<u8> mActive;
    u8 mPadding[3];
    void destroy(u32 flags);
    void initialize();
    static void calc();
};
WWHD_OFFSET(hdColorFader_c, mForegroundListener, 0x40);
WWHD_OFFSET(hdColorFader_c, mActive, 0x80);
WWHD_SIZE(hdColorFader_c, 0x84);
