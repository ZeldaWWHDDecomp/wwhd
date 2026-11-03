// Own instruction-derived draw preparation views; these are prefixes, not complete foreign classes.
#pragma once
#include "wwhd.h"
#include "sead/RenderMath.h"
#include "sead/ColorChannels.h"
#include "agl/VertexInput.h"
#include "agl/GeometryMath.h"
#include "sead/Allocation.h"

namespace agl {
struct PositiveDrawValue_l {
    u8 mPrefix[0x38];
    be<f32> mPositiveValue;
};
struct DrawParameterState_l {
    be<u16> mFlags;
    u8 mReserved[2];
    be<u32> mMask;
    sead::RenderVector3 mPosition;
    be<f32> mScale;
    sead::ColorChannels mColor;
    sead::ColorChannels mAlternateColor;
    be<f32> mPositiveValue;
    be<f32> mThreshold;
};
struct RankedDrawItem_l {
    u8 mPrefix[8];
    sead::RenderVector3 mPosition;
    be<f32> mRank;
};
struct ExtendedDrawCullState_l {
    u8 mPrefix[8];
    sead::RenderVector3 mPosition;
    u8 mPayload[0x30];
    be<f32> mRadius;
};
struct DrawPair_l {
    be<u8> mEnabled;
    u8 mReserved[3];
    be<u32> mMask;
    gptr<RankedDrawItem_l> mFirst;
    gptr<RankedDrawItem_l> mSecond;
    gptr<RankedDrawItem_l> mSelected;
};
struct LargeDrawCullState_l { u8 mPrefix[0x260]; geometry::Bounds3 mBounds; };
struct DrawCullVolume_l {
    sead::RenderMatrix34 mTransform;
    be<f32> mNear;
    be<f32> mFar;
    sead::RenderVector3 mSidePlanes[4];
    geometry::Bounds3 mBounds;
    sead::RenderVector3 mWorldPlanes[6];
    be<f32> mPlaneOffsets[6];
};
// The owner prefix is opaque; its vertex input set (agl::VertexInputSet, 0xF4 bytes) starts at +0x1C (+0x20).
struct DrawResourceBindingState_l {
    u8 mPrefix[0x1C];
    VertexInputSet mInputs;
    void preparePrimaryBinding(sead::AllocationHeap* heap);
    void prepareSelectedBinding(sead::AllocationHeap* heap);
    void prepareLargeBinding(sead::AllocationHeap* heap);
};
struct ExtendedDrawResourceBindingState_l {
    u8 mPrefix[0x20];
    VertexInputSet mInputs;
    void prepareExtendedBinding(sead::AllocationHeap* heap);
};
struct DrawPreparation_l {
    void initializeParameters(DrawParameterState_l* value);
    void ensurePositiveValue(PositiveDrawValue_l* value);
    void resetPair(DrawPair_l* pair);
    void selectHigherRank(DrawPair_l* pair);
    u32 intersectsCullVolume(const DrawParameterState_l* value, u32 slot,
                            const DrawCullVolume_l* volume);
    u32 intersectsBoxCullVolume(const LargeDrawCullState_l* value, u32 slot,
                               const DrawCullVolume_l* volume);
    u32 intersectsExtendedCullVolume(const ExtendedDrawCullState_l* value, u32 slot,
                                    const DrawCullVolume_l* volume);
    u32 intersectsSelectedCullVolume(const DrawPair_l* pair, u32 slot,
                                    const DrawCullVolume_l* volume);
};
u32 isZeroDrawVector(const sead::RenderVector3* vector);
}
WWHD_OFFSET(agl::PositiveDrawValue_l, mPositiveValue, 0x38);
WWHD_SIZE(agl::PositiveDrawValue_l, 0x3C);
WWHD_SIZE(agl::DrawParameterState_l, 0x40);
WWHD_OFFSET(agl::DrawParameterState_l, mFlags, 0);
WWHD_OFFSET(agl::DrawParameterState_l, mPosition, 8);
WWHD_OFFSET(agl::DrawParameterState_l, mScale, 0x14);
WWHD_OFFSET(agl::DrawParameterState_l, mColor, 0x18);
WWHD_OFFSET(agl::DrawParameterState_l, mAlternateColor, 0x28);
WWHD_OFFSET(agl::DrawParameterState_l, mPositiveValue, 0x38);
WWHD_OFFSET(agl::DrawParameterState_l, mThreshold, 0x3C);
WWHD_OFFSET(agl::RankedDrawItem_l, mRank, 0x14);
WWHD_SIZE(agl::RankedDrawItem_l, 0x18);
WWHD_OFFSET(agl::DrawPair_l, mEnabled, 0);
WWHD_OFFSET(agl::DrawPair_l, mFirst, 8);
WWHD_OFFSET(agl::DrawPair_l, mSecond, 0xC);
WWHD_OFFSET(agl::DrawPair_l, mSelected, 0x10);
WWHD_SIZE(agl::DrawPair_l, 0x14);
WWHD_SIZE(agl::DrawCullVolume_l, 0xE0);
WWHD_OFFSET(agl::DrawCullVolume_l, mNear, 0x30);
WWHD_OFFSET(agl::DrawCullVolume_l, mFar, 0x34);
WWHD_OFFSET(agl::DrawCullVolume_l, mSidePlanes, 0x38);

WWHD_SIZE(agl::ExtendedDrawCullState_l, 0x48);
WWHD_OFFSET(agl::ExtendedDrawCullState_l, mPosition, 8);
WWHD_OFFSET(agl::ExtendedDrawCullState_l, mRadius, 0x44);

WWHD_OFFSET(agl::LargeDrawCullState_l, mBounds, 0x260);
WWHD_SIZE(agl::LargeDrawCullState_l, 0x278);
WWHD_OFFSET(agl::DrawCullVolume_l, mBounds, 0x68);
WWHD_OFFSET(agl::DrawCullVolume_l, mWorldPlanes, 0x80);
WWHD_OFFSET(agl::DrawCullVolume_l, mPlaneOffsets, 0xC8);

WWHD_OFFSET(agl::DrawResourceBindingState_l, mInputs, 0x1C);
WWHD_OFFSET(agl::ExtendedDrawResourceBindingState_l, mInputs, 0x20);
