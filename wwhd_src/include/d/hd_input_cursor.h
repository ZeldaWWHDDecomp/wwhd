/* HD-only swipe/stick cursor detector. Five touch samples, inertia and four-way direction. */
#pragma once
#include "bindings.h"
struct HdCursorPoint { be<f32> x, y; };
union HdCursorSample { HdCursorPoint mPoint; be<u32> mWords[2]; };
struct HdInputCursor {
    gptr<void> mVtable;
    be<u32> mDirection;
    HdCursorPoint mHistory[5];
    be<u8> mActive;
    u8 padding[3];
    HdCursorPoint mCursor;
    be<f32> mInertiaFactor;
    be<u32> mInertiaFrames, mFramesLeft;
    HdCursorPoint mScale;
    be<f32> mReturnRate, mUnusedRate, mDirectionThreshold;
    void update(HdCursorPoint*);
    void updateThunk(HdCursorPoint*);
};
struct HdCursorGlobals { be<u32> mFirst, mSecond, mThird, mFourth; };
struct HdCursorRange { be<f32> mLower, mUpper; be<u8> mSignedInitialized, mUnsignedInitialized; };
struct HdCursorRegistration { u8 bytes[12]; };
struct HdCursorIconName { gptr<const char> mpName; gptr<void> mVtable; };
WWHD_SIZE(HdCursorPoint, 8);
WWHD_SIZE(HdCursorSample, 8);
WWHD_OFFSET(HdInputCursor, mHistory, 8);
WWHD_OFFSET(HdInputCursor, mActive, 0x30);
WWHD_OFFSET(HdInputCursor, mCursor, 0x34);
WWHD_OFFSET(HdInputCursor, mDirectionThreshold, 0x58);
WWHD_SIZE(HdInputCursor, 0x5C);
WWHD_SIZE(HdCursorGlobals, 16);
WWHD_SIZE(HdCursorRange, 10);
WWHD_SIZE(HdCursorRegistration, 12);
WWHD_SIZE(HdCursorIconName, 8);
void* hdCursorAllocate(u32);
void hdCursorNormalize(HdCursorPoint*);
u32 hdCursorChase(HdCursorPoint*, HdCursorPoint*, f32);
u32 hdCursorAtan(f32);
void hdCursorRegister(void*);
void hdCursorInitSigned(be<u8>*);
void hdCursorInitUnsigned(be<u8>*);
