/* HD multi-filter layer and its television/DRC surfaces. Private decomp source. */
#pragma once
#include "bindings.h"
struct hdFilterTexture_c {
    be<u32> mImageWords[30];
    be<u8> mChannels[4];
    be<u32> mImageInfo[4];
    be<u32> mTextureVtable;
    be<u8> mEnabled, mHasMipmaps;
    u8 _92[2];
    be<u32> mSamplerWords[6];
    be<u32> mMipWords[40];
    be<u8> mSamplerState;
    u8 _14D[3];
    be<u32> mSamplerParameters[16];
    be<u8> mFlags;
    u8 _191[3];
    be<u32> mVtable;
};
WWHD_OFFSET(hdFilterTexture_c,mTextureVtable,0x8C);
WWHD_OFFSET(hdFilterTexture_c,mEnabled,0x90);
WWHD_OFFSET(hdFilterTexture_c,mSamplerWords,0x94);
WWHD_OFFSET(hdFilterTexture_c,mMipWords,0xAC);
WWHD_OFFSET(hdFilterTexture_c,mSamplerState,0x14C);
WWHD_OFFSET(hdFilterTexture_c,mSamplerParameters,0x150);
WWHD_OFFSET(hdFilterTexture_c,mFlags,0x190);
WWHD_OFFSET(hdFilterTexture_c,mVtable,0x194);
WWHD_SIZE(hdFilterTexture_c,0x198);
struct hdFilterBinding_l { u8 mData[0x558]; };
struct hdFilterSurface_c {
    u8 _00[4];
    hdFilterBinding_l mBinding;
    be<u32> mHasTexture;
    u8 _560[4];
    hdFilterTexture_c mTexture;
    u8 _6FC[0x8BC-0x6FC];
    be<u8> mEnabled;
    u8 _8BD[0xA70-0x8BD];
};
WWHD_OFFSET(hdFilterSurface_c,mBinding,4);
WWHD_OFFSET(hdFilterSurface_c,mHasTexture,0x55C);
WWHD_OFFSET(hdFilterSurface_c,mTexture,0x564);
WWHD_OFFSET(hdFilterSurface_c,mEnabled,0x8BC);
WWHD_SIZE(hdFilterSurface_c,0xA70);
struct hdMultiFilter_c {
    u8 _00[0xC];
    be<u32> mVtable;
    be<u32> mDrawVtable;
    u8 _14[0x58-0x14];
    be<u32> mTarget;
    u8 _5C[0x610-0x5C];
    hdFilterSurface_c mTvSurface;
    hdFilterSurface_c mDrcSurface;
    gptr<hdFilterSurface_c> mpActiveSurface;
    void setRenderTarget(void* target);
    void initialize(void* heap);
    void prepare(struct hdFilterRenderInfo_c* info);
    void draw(struct hdFilterRenderInfo_c* info);
    void execute(struct hdFilterRenderInfo_c* info);
};
WWHD_OFFSET(hdMultiFilter_c,mVtable,0xC);
WWHD_OFFSET(hdMultiFilter_c,mDrawVtable,0x10);
WWHD_OFFSET(hdMultiFilter_c,mTarget,0x58);
WWHD_OFFSET(hdMultiFilter_c,mTvSurface,0x610);
WWHD_OFFSET(hdMultiFilter_c,mDrcSurface,0x1080);
WWHD_OFFSET(hdMultiFilter_c,mpActiveSurface,0x1AF0);
WWHD_SIZE(hdMultiFilter_c,0x1AF4);
struct hdFilterRenderInfo_c {
    be<u32> mPass;
    u8 _04[4];
    gptr<struct hdFilterViewport_l> mpViewport;
    be<u32> mTarget;
    u8 _10[0x1C-0x10];
    gptr<void> mpViewportBounds;
};
WWHD_OFFSET(hdFilterRenderInfo_c,mpViewport,8);
WWHD_OFFSET(hdFilterRenderInfo_c,mTarget,0xC);
WWHD_OFFSET(hdFilterRenderInfo_c,mpViewportBounds,0x1C);
struct hdFilterViewport_l { u8 _00[0x1C]; gptr<void> mpTarget; };
WWHD_OFFSET(hdFilterViewport_l,mpTarget,0x1C);
struct hdFilterTargetMgr_l { u8 _00[0x2C]; gptr<void> mpDrcTarget; };
struct hdFilterTargetArg_c { be<u32> mTarget; };
WWHD_SIZE(hdFilterTargetArg_c,4);
struct hdFilterStep_c {
    u8 _00[0xC]; be<u32> mVtable; SafeString mName;
    be<u32> mTarget, mFlags, mDelegateVtable;
    gptr<hdMultiFilter_c> mpLayer;
    be<u32> mDelegateAdjustment, mDrawFunction;
    u8 _30[0x10];
};
WWHD_SIZE(hdFilterStep_c,0x40);
struct hdFilterDebugRenderer_l { be<u32> mTarget,mFlags; };
struct hdFilterBounds_c { u8 mData[0x18]; };
struct hdFilterState_c { be<u8> mEnabled,mBlend; u8 _02[0xC-2]; be<u8> mAlphaEnabled; u8 _0D[0x74-0xD]; };
WWHD_SIZE(hdFilterState_c,0x74);
struct hdFilterMatrix_c { u8 mData[0x30]; };
struct hdFilterProjection_c { u8 mData[0xAC]; };

struct hdFilterGlobalDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdFilterGlobalDescriptor_c,12);

struct hdFilterHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdFilterHeaderZero_c,16);
