/* HD JPEG worker and GX2 surface views. Private reconstruction. */
#pragma once
#include "wwhd.h"
struct HdJpegHeap { u8 mPad[0xC]; gptr<gfn> mVTable; };
struct HdJpegThread { u8 mPad[0xC]; gptr<gfn> mVTable; };
struct HdJpegCriticalSection { u8 mOpaque[0x3C]; };
struct HdJpegDisposer { u8 mPad[0xC]; gptr<gfn> mVTable; };
struct HdJpegSaveJob { gptr<void> mBuffer; be<u32> mWidth; be<u32> mHeight; be<u32> mParameter; be<u32> mSlot; };
struct HdJpegDecodeJob { gptr<void> mJpeg; gptr<struct HdJpegTextureMap> mTexture; gptr<void> mImage; be<u32> mSize; };
struct HdJpegMgr {
 HdJpegDisposer mDisposer; gptr<HdJpegThread> mThread; be<u32> mState; be<u8> mDone; be<u8> mOk;
 u8 mPad1A[2]; HdJpegSaveJob mSave; HdJpegDecodeJob mDecode; HdJpegCriticalSection mCriticalSection;
};
struct HdJpegSafeString { gptr<const char> mText; gptr<void> mVTable; };
struct HdJpegDelegate { gptr<gfn> mVTable; gptr<HdJpegMgr> mObject; be<u16> mThisAdjustment; be<u16> mVirtualOffset; gptr<void> mFunction; };
struct HdJpegSaveObject { be<u32> mWord; be<u8> mOk; u8 mPad[3]; };
struct HdJpegSaveParameters { be<u32> mWidth; be<u32> mHeight; be<u32> mFormat; };
struct HdJpegSaveFrame { HdJpegSaveObject mObject; HdJpegSaveParameters mParameters; };
struct HdJpegSurface {
 be<u32> mDimension; be<u32> mWidth; be<u32> mHeight; be<u32> mDepth; be<u32> mMipLevels;
 be<u32> mFormat; be<u32> mAa; be<u32> mUse; be<u32> mImageSize; gptr<void> mImage;
 be<u32> mMipSize; gptr<void> mMip; be<u32> mTileMode; be<u32> mSwizzle; be<u32> mAlignment;
 be<u32> mPitch; be<u32> mMipOffsets[13];
};
struct HdJpegTexture { HdJpegSurface mSurface; be<u32> mFirstMip; be<u32> mMipCount; be<u32> mFirstSlice; be<u32> mSliceCount; be<u32> mComponentSelect; be<u32> mRegisters[5]; };
struct HdJpegTextureMap { be<u32> mType; be<u16> mWidth; be<u16> mHeight; be<u8> mFormat; u8 mPad[3]; HdJpegTexture mTexture; };
struct HdJpegDecodeFrame {
 u8 mPad0[8]; be<u32> mWorkSize; be<u32> mFormat;
 union { be<u32> mPackedSize; be<u16> mSizeHalves[2]; };
 be<u32> mWidth; be<u32> mHeight; u8 mPad1C[4]; HdJpegSurface mSource;
 HdJpegTexture mDestination; be<u32> mConfig[0x36];
};
struct HdJpegConfiguration { be<u32> mWords[0x36]; };
struct HdJpegSingleton { gptr<HdJpegMgr> mInstance; };
struct HdJpegDisposerHolder { gptr<HdJpegDisposer> mDisposer; };
struct HdJpegStatics { be<u32> mWords[4]; };
struct HdJpegRange { be<f32> mLower; be<f32> mUpper; be<u8> mSigned; be<u8> mUnsigned; u8 mPad[2]; };
struct HdJpegRegistration { be<u32> mWords[3]; };
enum HdJpegJob { HD_JPEG_IDLE, HD_JPEG_SAVE, HD_JPEG_DECODE, HD_JPEG_NOT_STARTED };
enum HdJpegSurfaceKind { HD_JPEG_SURFACE_2D=1, HD_JPEG_FORMAT_RGBA8=0x1A, HD_JPEG_TILE_LINEAR=0x10 };
WWHD_SIZE(HdJpegMgr,0x7C);
WWHD_SIZE(HdJpegSurface,0x74);
WWHD_SIZE(HdJpegTexture,0x9C);
WWHD_SIZE(HdJpegDecodeFrame,0x208);
WWHD_OFFSET(HdJpegDecodeFrame,mSource,0x20);
WWHD_OFFSET(HdJpegDecodeFrame,mDestination,0x94);
WWHD_OFFSET(HdJpegDecodeFrame,mConfig,0x130);
