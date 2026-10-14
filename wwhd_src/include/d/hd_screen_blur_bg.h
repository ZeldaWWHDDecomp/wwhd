/** HD menu blur background and capture texture. */
#pragma once
#include "d/hd_parts.h"
struct hdBlurSurface_l {
    be<u32> mDimension, mFormat, mBufferMode, mWidth, mHeight;
    u8 mRemainingDescriptor[0x74-0x14];
};
WWHD_SIZE(hdBlurSurface_l, 0x74);
struct hdBlurTexture_l {
    be<u32> mInitialized;
    be<u16> mWidth, mHeight;
    be<u8> mFormat;
    u8 mFormatPadding[3];
    hdBlurSurface_l mSurface;
    be<u32> mFirstMip, mMipHeight, mFirstSlice, mSliceWidth, mComponentSelect;
    u8 mRegisters[0x14];
};
WWHD_SIZE(hdBlurTexture_l, 0xA8);
struct hdBlurBackground_c : hdParts_c {
    hdBlurTexture_l mTexture;
    be<u32> mCaptureKind;
    static void destroy(hdBlurBackground_c*, s32);
    static hdBlurBackground_c* construct(hdBlurBackground_c*);
    void waitExecute();
    void captureEnter();
    void inEnter();
    void inExecute();
    void outEnter();
    void outExecute();
    u32 load(u32,u32,u32);
    void unload();
    void execute();
    void requestOut();
    u32 loadLayout(void*,void*,u32,u32,void*);
};
WWHD_OFFSET(hdBlurBackground_c, mTexture, 0x50);
WWHD_OFFSET(hdBlurBackground_c, mCaptureKind, 0xF8);
WWHD_SIZE(hdBlurBackground_c, 0xFC);
