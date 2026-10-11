/* HD-only UI event receiver and GamePad orientation layouts. */
#pragma once
#include "bindings.h"
struct HdInputPane {
    u8 padding[0x48];
    Mtx34 mGlobalMtx;
};
struct HdInputReceiver {
    u8 padding[0x14];
    gptr<gfn> mVtable;
    gptr<HdInputPane> mpPane;
    u8 padding2[0x3C];
    be<u8> mFlags[3];
    u8 padding3;
    be<u32> mUserValue;
    u8 hitsCorner(HdInputReceiver*);
    f32 paneDistance(HdInputReceiver*);
    u8 getPrimaryFlag();
    u8 getSecondaryFlag();
    u8 getTertiaryFlag();
};
struct HdInputRect { be<f32> mLeft, mTop, mRight, mBottom; };
struct HdInputCornerWorkspace { HdInputRect mRect; Mtx34 mMtx; };
struct HdInputDistanceWorkspace { Mtx34 mFirst, mSecond; };
struct HdInputController {
    u8 padding[0x10];
    gptr<gfn> mVtable;
    u8 padding2[0x6C];
    cXyz mOrientation[3];
    u8 padding3[0xA30];
    be<s32> mSampleCount;
};
struct HdInputAxes { cXyz mAxes[3]; };
struct HdInputOrientation { cXyz mAxes[3]; Mtx34 mMatrix; };
struct HdInputPointer { gptr<void> mpValue; };
struct HdInputCallback { gptr<void> mVtable; };
struct HdInputInitGlobals { be<u32> mFirst, mSecond, mThird, mFourth; };
struct HdInputInitRange { be<f32> mLower, mUpper; be<u8> mSignedInitialized, mUnsignedInitialized; };
struct HdInputRegistration { u8 bytes[12]; };
WWHD_OFFSET(HdInputReceiver, mpPane, 0x18);
WWHD_OFFSET(HdInputReceiver, mFlags, 0x58);
WWHD_SIZE(HdInputReceiver, 0x60);
WWHD_OFFSET(HdInputPane, mGlobalMtx, 0x48);
WWHD_SIZE(HdInputPane, 0x78);
WWHD_SIZE(HdInputRect, 16);
WWHD_SIZE(HdInputCornerWorkspace, 0x40);
WWHD_SIZE(HdInputDistanceWorkspace, 0x60);
WWHD_OFFSET(HdInputController, mOrientation, 0x80);
WWHD_OFFSET(HdInputController, mSampleCount, 0xAD4);
WWHD_SIZE(HdInputController, 0xAD8);
WWHD_SIZE(HdInputAxes, 36);
WWHD_SIZE(HdInputOrientation, 0x54);
WWHD_SIZE(HdInputPointer, 4);
WWHD_SIZE(HdInputCallback, 4);
WWHD_SIZE(HdInputInitGlobals, 16);
WWHD_SIZE(HdInputInitRange, 10);
WWHD_SIZE(HdInputRegistration, 12);
void* hdInputAllocate(u32);
void hdInputReceiverConstruct(HdInputReceiver*, void*);
void hdInputReceiverDestroy(HdInputReceiver*, u32);
void hdInputDelete(void*);
void hdInputGetPaneRect(HdInputPane*, HdInputRect*);
void hdInputRegister(void*);
void hdInputInitSigned(be<u8>*);
void hdInputInitUnsigned(be<u8>*);
HdInputController* hdInputGetController(void*, u32);
void hdInputMakeOrientationMatrix(Mtx34*, cXyz*, cXyz*, cXyz*);
