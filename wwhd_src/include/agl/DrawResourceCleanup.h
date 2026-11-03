// Resource-release views established by the two own record cleanup loops.
#pragma once
#include "sead/FileDevice.h"
#include "sead/ElementAt.h"   // sead::elementPtr / nextElement: 32-bit guest pointer walks
#include "agl/DrawPreparation.h"
#include "sead/Allocation.h"
#include "sead/GraphicsSurface.h"
#include "agl/RenderSelection.h"
#include "agl/TextureData.h"

namespace agl {
struct DrawParameterFrame_l;
struct DrawReleaseResource_l {
    TextureData mTexture;
    u8 mBeforeCachedImage[0x44];
    gptr<void> mCachedImage;
    u8 mCachedPadding[4];
    gptr<void> mCachedMipImage;
    u8 mBeforeSampler[0x7C];
    be<u32> mAddressU, mAddressV, mAddressW;
    sead::ColorChannels mBorderColor;
    u8 mReserved[0x10];
    be<u8> mCompareEnabled;
    u8 mComparePadding[3];
    be<u32> mCompareMode;
    be<u8> mDirtyFlags;
    u8 mDirtyPadding[3];
    gptr<sead::DisposerVtable> mMethods;
};
struct DrawResourceValue_l {
    be<u32> mFlags;
    gptr<TextureData> mSource;
    gptr<DrawReleaseResource_l> mResource;
    sead::RenderMatrix44 mMatrix;
    UniformBuffer mBuffer;
};
struct DrawResourceRecord_l {
    be<u8> mFlags;
    u8 mReserved[7];
    sead::RenderVector3 mPosition;
    sead::RenderVector3 mDirection;
    sead::ColorChannels mPrimaryColor, mAlternateColor;
    be<f32> mAngle;
    be<f32> mRadius;
    be<f32> mScale;
    be<f32> mFalloff;
    be<u32> mSelectorDigit;
    be<f32> mTextureScale;
    be<u32> mValueCount;
    gptr<DrawResourceValue_l> mValues;
};
struct LargeDrawResourceRecord_l {
    be<u8> mFlags;
    u8 mReserved[7];
    sead::RenderVector3 mPosition;
    sead::RenderVector3 mDirection;
    sead::RenderVector3 mAxis;
    sead::ColorChannels mColor;
    sead::ColorChannels mAlternateColor;
    sead::RenderVector3 mRenderDirection, mUp;
    be<f32> mNear, mFar, mVerticalAngle, mAspectRatio, mTop;
    be<f32> mBottom, mLeft, mRight, mPositiveScale;
    be<u8> mEnabled;
    u8 mPrimaryPadding[3];
    DrawReleaseResource_l mPrimaryResource;
    be<f32> mOffsetX, mOffsetY, mScaleX, mScaleY;
    be<u32> mSelectorDigit;
    be<f32> mTextureScale;
    be<u32> mValueCount;
    gptr<DrawResourceValue_l> mValues;
    be<u8> mValuesEnabled;
    u8 mBeforeBounds[3];
    be<f32> mSlope;
    sead::RenderVector3 mCenter;
    be<f32> mHorizontalExtent, mVerticalExtent;
    geometry::Bounds3 mBounds;
    be<u8> mBoundsEnabled;
    u8 mBoundsPadding[3];
    be<u32> mBoundsState;
    u8 mBeforeSecondaryResource[0x90];
    DrawReleaseResource_l mSecondaryResource;
};
struct DrawResourceConfiguration_l {
    u8 mPrefix[4];
    be<u32> mFlags;
    u8 mPayload[0x30];
    be<u32> mFormatIndex;
};
struct DrawResourceDirectionState_l {
    u8 mPrefix[4];
    gptr<DrawResourceConfiguration_l> mConfiguration;
    u8 mPayload[0x14];
    be<f32> mScale;
    void prepareDirection(DrawResourceRecord_l* record);
    void prepareLargeGeometry(LargeDrawResourceRecord_l* record);
    void prepareLargeParameters(LargeDrawResourceRecord_l* record, u32 slot, const DrawParameterFrame_l* context);
    void prepareResourceParameters(DrawResourceRecord_l* record, u32 slot, const DrawParameterFrame_l* context);
};
struct DrawResourceCleanup_l {
    void initializeRecordResources(DrawResourceRecord_l* record, sead::AllocationHeap* heap);
    void initializeLargeRecordResources(LargeDrawResourceRecord_l* record, sead::AllocationHeap* heap);
    void releaseRecordResources(DrawResourceRecord_l* record);
    void releaseLargeRecordResources(LargeDrawResourceRecord_l* record);
};
}
WWHD_OFFSET(agl::DrawReleaseResource_l, mMethods, 0x194);
WWHD_SIZE(agl::DrawReleaseResource_l, 0x198);
WWHD_OFFSET(agl::DrawResourceValue_l, mResource, 8);
WWHD_SIZE(agl::DrawResourceValue_l, 0x68);
WWHD_OFFSET(agl::DrawResourceRecord_l, mValueCount, 0x58);
WWHD_OFFSET(agl::DrawResourceRecord_l, mValues, 0x5C);
WWHD_SIZE(agl::DrawResourceRecord_l, 0x60);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mValueCount, 0x23C);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mValues, 0x240);
WWHD_SIZE(agl::LargeDrawResourceRecord_l, 0x4A8);

WWHD_OFFSET(agl::DrawReleaseResource_l, mAddressU, 0x15C);
WWHD_OFFSET(agl::DrawReleaseResource_l, mBorderColor, 0x168);
WWHD_OFFSET(agl::DrawReleaseResource_l, mCompareEnabled, 0x188);
WWHD_OFFSET(agl::DrawReleaseResource_l, mCompareMode, 0x18C);
WWHD_OFFSET(agl::DrawReleaseResource_l, mDirtyFlags, 0x190);
WWHD_OFFSET(agl::DrawResourceRecord_l, mDirection, 0x14);
WWHD_OFFSET(agl::DrawResourceRecord_l, mAngle, 0x40);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mColor, 0x2C);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mNear, 0x64);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mOffsetX, 0x224);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mValuesEnabled, 0x244);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mBoundsEnabled, 0x278);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mBoundsState, 0x27C);

WWHD_OFFSET(agl::DrawResourceConfiguration_l, mFormatIndex, 0x38);
WWHD_OFFSET(agl::DrawResourceDirectionState_l, mConfiguration, 4);
WWHD_OFFSET(agl::DrawResourceDirectionState_l, mScale, 0x1C);

WWHD_OFFSET(agl::DrawReleaseResource_l, mCachedImage, 0xD4);
WWHD_OFFSET(agl::DrawReleaseResource_l, mCachedMipImage, 0xDC);
WWHD_OFFSET(agl::DrawResourceRecord_l, mPrimaryColor, 0x20);
WWHD_OFFSET(agl::DrawResourceRecord_l, mAlternateColor, 0x30);
WWHD_OFFSET(agl::DrawResourceRecord_l, mSelectorDigit, 0x50);

WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mPrimaryResource, 0x8C);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mSelectorDigit, 0x234);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mBounds, 0x260);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mSecondaryResource, 0x310);

WWHD_OFFSET(agl::DrawResourceValue_l, mMatrix, 0xC);
WWHD_OFFSET(agl::DrawResourceValue_l, mBuffer, 0x4C);
WWHD_OFFSET(agl::DrawResourceRecord_l, mTextureScale, 0x54);


WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mRenderDirection, 0x4C);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mUp, 0x58);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mTextureScale, 0x238);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mSlope, 0x248);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mCenter, 0x24C);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mHorizontalExtent, 0x258);
WWHD_OFFSET(agl::LargeDrawResourceRecord_l, mVerticalExtent, 0x25C);
