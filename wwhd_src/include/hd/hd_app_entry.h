/* HD layout child, UI graphics singleton and application entry views. Private reconstruction. */
#pragma once
#include "wwhd.h"
struct HdAppHeap {
    u8 mPad[0xC];
    gptr<gfn> mVTable;
};
struct HdAppSafeString {
    gptr<void> mText;
    gptr<void> mVTable;
};
struct HdAppRect {
    be<u32> mWords[4];
};
struct HdAppOwner {
    u8 mPad0[0xC];
    gptr<void> mText;
    u8 mPad10[0xC];
    gptr<void> mName;
};
struct HdAppChild {
    HdAppSafeString mName;
    HdAppSafeString mOwnerName;
    be<u8> mEnabled;
    u8 mPad11[3];
    HdAppSafeString mOwnerText;
    gptr<HdAppOwner> mOwner;
    be<u32> mValue;
    HdAppRect mRect;
    be<u32> mReserved34;
    be<u32> mReserved38;
};
struct HdAppDisposer {
    u8 mPad[0xC];
    gptr<gfn> mVTable;
};
struct HdAppGraphics {
    HdAppDisposer mDisposer;
    gptr<HdAppHeap> mHeap;
    gptr<void> mResource;
    gptr<void> mDrawInfo;
    gptr<void> mFont;
    gptr<gfn> mVTable;
};
struct HdAppGraphicsPointer {
    gptr<HdAppGraphics> mInstance;
};
struct HdAppDisposerPointer {
    gptr<HdAppDisposer> mDisposer;
};
struct HdAppThreadPointer {
    gptr<void> mThread;
};
struct HdAppFramework {
    u8 mPad[0x24];
    gptr<gfn> mVTable;
};
struct HdAppFrameworkParams {
    be<u32> mHeapMode;
    be<u32> mHeapSize0;
    be<u32> mHeapSize1;
    be<u32> mHeapSize2;
    be<u32> mHeapSize3;
    be<u32> mWidth;
    be<u32> mHeight;
    be<u32> mCommandBufferSize;
    be<u8> mFlag;
    u8 mPad21[3];
    HdAppSafeString mName;
};
struct HdAppArena {
    be<u32> mSize;
    be<u32> mReserved;
};
struct HdAppDimensions {
    be<f32> mWidth;
    be<f32> mHeight;
};
struct HdAppConversion {
    be<u32> mMagic;
    be<u32> mValue;
};
struct HdAppTaskFactory {
    be<u32> mKind;
    gptr<void> mEntry;
};
struct HdAppTaskArgs {
    be<u32> mWords[32];
};
struct HdAppRunArg {
    be<u32> mWords[2];
};
struct HdAppRootArray {
    gptr<HdAppHeap> mRoot;
};
struct HdAppRootPointer {
    gptr<HdAppRootArray> mArray;
};
struct HdAppGraphicsSystem {
    gptr<void> mSystem;
};
struct HdAppHeapSizes {
    be<u32> mSizes[4];
};
struct HdAppStatics {
    be<u32> mWords[4];
};
struct HdAppRange {
    be<f32> mLower;
    be<f32> mUpper;
    be<u8> mSigned;
    be<u8> mUnsigned;
    u8 mPad[2];
};
struct HdAppRegistration {
    be<u32> mWords[3];
};
struct HdAppBaseHeap {
    be<u32> mSignature;
};
WWHD_SIZE(HdAppChild, 0x3C);
WWHD_SIZE(HdAppGraphics, 0x24);
WWHD_SIZE(HdAppFrameworkParams, 0x2C);
WWHD_OFFSET(HdAppChild, mOwnerName, 8);
WWHD_OFFSET(HdAppChild, mOwnerText, 0x14);
WWHD_OFFSET(HdAppChild, mRect, 0x24);
WWHD_OFFSET(HdAppFrameworkParams, mName, 0x24);
enum HdAppHeapSignature { HD_APP_EXP_HEAP = 0x45585048 };
enum HdAppFrameworkMode { HD_APP_FRAMEWORK_SINGLE_HEAP = 1, HD_APP_FRAMEWORK_MULTI_HEAP = 2 };
enum HdAppInitialSize { HD_APP_INITIAL_WIDTH = 1280, HD_APP_INITIAL_HEIGHT = 720 };
