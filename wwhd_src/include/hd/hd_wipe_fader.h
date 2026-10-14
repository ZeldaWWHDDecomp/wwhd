/* HD texture wipe fader with two output render callbacks. Private decomp source. */
#pragma once
#include "hd/hd_multi_filter.h"
#include "hd/hd_layer_mask.h"
#include "hd/hd_render_resolution.h"
struct hdWipeFader_c;
struct hdWipeDelegate_c { be<u32> mVtable; gptr<hdWipeFader_c> mpOwner; be<u16> mAdjustment,mVirtualSlot; be<u32> mFunction; };
WWHD_SIZE(hdWipeDelegate_c,0x10);
struct hdWipeStep_c {
    u8 _00[0xC]; be<u32> mVtable; SafeString mName;
    be<u32> mTarget,mFlags;
    hdWipeDelegate_c mDelegate;
    u8 _30[0x10];
};
WWHD_OFFSET(hdWipeStep_c,mDelegate,0x20);
WWHD_SIZE(hdWipeStep_c,0x40);
struct hdWipeTextureDescriptor_c { be<u32> mWords[36]; };
WWHD_SIZE(hdWipeTextureDescriptor_c,0x90);
struct hdWipeFader_c {
    hdWipeStep_c mTvStep;
    hdWipeStep_c mDrcStep;
    be<u8> mTvEnabled,mDrcEnabled;
    u8 _82[2];
    hdFilterTexture_c mTexture;
    hdWipeTextureDescriptor_c mTextureDescriptor;
    void loadTexture();
    void draw(hdMaskRenderInfo_c* info);
    void drawIfEnabled(hdMaskRenderInfo_c* info);
    void initialize();
};
WWHD_OFFSET(hdWipeFader_c,mTvStep,0);
WWHD_OFFSET(hdWipeFader_c,mDrcStep,0x40);
WWHD_OFFSET(hdWipeFader_c,mTvEnabled,0x80);
WWHD_OFFSET(hdWipeFader_c,mDrcEnabled,0x81);
WWHD_OFFSET(hdWipeFader_c,mTexture,0x84);
WWHD_OFFSET(hdWipeFader_c,mTextureDescriptor,0x21C);
WWHD_SIZE(hdWipeFader_c,0x2AC);
struct hdWipeDrawState_c {
    be<u8> mEnabled,mBlend;
    u8 _02[2];
    be<u32> mDepthMode;
    u8 _08[4];
    be<u8> mAlphaEnabled;
    u8 _0D[0x38-0xD];
    be<u8> mAlphaTestEnabled;
    u8 _39[3];
    be<u32> mAlphaFunction;
    be<f32> mAlphaReference;
    be<u8> mWriteRed,mWriteGreen,mWriteBlue,mWriteAlpha;
    u8 _48[0x74-0x48];
};
WWHD_OFFSET(hdWipeDrawState_c,mAlphaTestEnabled,0x38);
WWHD_OFFSET(hdWipeDrawState_c,mAlphaFunction,0x3C);
WWHD_OFFSET(hdWipeDrawState_c,mAlphaReference,0x40);
WWHD_OFFSET(hdWipeDrawState_c,mWriteRed,0x44);
WWHD_SIZE(hdWipeDrawState_c,0x74);
struct hdWipeTextureScale_c { be<f32> mX,mY; };
WWHD_SIZE(hdWipeTextureScale_c,8);
struct hdWipeLayerList_l { u8 _00[0x1020]; be<u32> mLayerCount; gptr<gptr<void>> mpLayers; };

struct hdWipeFaderHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdWipeFaderHeaderZero_c,16);
struct hdWipeFaderHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdWipeFaderHeaderDescriptor_c,12);

enum hdWipeImageWord_e : u32 {
 hdWipeImageWidth=1, hdWipeImageHeight=2, hdWipeImageDepth=3,
 hdWipeImageMipCount=4, hdWipeImageFormat=5, hdWipeImageSampleMode=6,
 hdWipeImageUsage=7, hdWipeImageSwizzle=13, hdWipeImageAlignment=14
};
