// Cascade-shadow receiver and parameter-storage envelopes from WWHD's own constructor.
#pragma once
#include "agl/ParameterRoot.h"
#include "sead/MemberDispatch.h"
#include "sead/HeapRegistry.h"
namespace agl {
struct CascadeScalarPrefix { be<u32> mLinks[2]; be<u32> mTable; void emptyHook(); };
struct CascadeWordParameter : CascadeScalarPrefix { be<u32> mValue; u32 typeCode(); u32 byteSize(); be<u32>* valueAddress(); be<u32>* alternateValueAddress(); };
struct CascadeFloatParameter : CascadeScalarPrefix { be<f32> mValue; u32 typeCode(); u32 byteSize(); be<f32>* valueAddress(); be<f32>* alternateValueAddress(); };
struct CascadeBoolParameter : CascadeScalarPrefix { be<u8> mValue; u8 mPadding[3]; u32 typeCode(); u32 byteSize(); be<u8>* valueAddress(); be<u8>* alternateValueAddress(); };
struct CascadeArrayParameter : CascadeScalarPrefix { gptr<void> mData; be<u32> mCount; u32 typeCode(); u32 byteSize(); void* data(); void* alternateData(); };
struct CascadeWordView : CascadeScalarPrefix { be<u32> mValue; u32 typeCode(); u32 byteSize(); be<u32>* valueAddress(); be<u32>* alternateValueAddress(); };
struct CascadeBounds { be<f32> mMinimum[3]; be<f32> mMaximum[3]; };
struct ShadowUnitDispatch_l { u8 mPrefix[0xC]; be<u32> mBounds; u8 mMiddle[0x34]; be<u32> mRelease; };
struct ShadowUnit_l {
    u8 mPrefix[0x58]; gptr<ShadowUnitDispatch_l> mVtable; be<u32> mReserved;
    sead::MemberSlot mFirstBounds; sead::MemberSlot mSecondBounds;
    u8 mBeforeInputBounds[0x304]; CascadeBounds mInputBounds[3];
    CascadeBounds mBounds[3]; u8 mBeforeSurfaces[0x1A0];
    be<u32> mFirstSurfaceWords[2]; be<u32> mSecondSurfaceWords[2];
};
struct ShadowUnitBuffer { be<u32> mCount; gptr<ShadowUnit_l> mData; };
// These envelopes reserve the member storage; their owning external interfaces stay opaque.
struct CascadeMapStorage_l {
    u8 mPrefix[0x6C]; be<u32> mRenderMode;
    u8 mBeforeWidth[12]; be<s32> mWidth;
    u8 mBeforeHeight[12]; be<s32> mHeight;
    u8 mTail[0xB54];
};
struct CascadeShadow;
struct CascadeIndexDispatch { u8 mPrefix[12]; be<u32> mInvoke; };
struct CascadeIndexCallback { gptr<CascadeIndexDispatch> mTable; };
struct CascadeIndexRecord { be<u32> mIndex; gptr<CascadeShadow> mOwner; gptr<gptr<void>> mContextSlot; };
struct CascadeSupportStorage_l { u8 mPrefix[0x74]; gptr<CascadeIndexCallback> mIndexCallback; u8 mTail[0x20]; };
struct CascadeShadow {
    ParameterRoot mRoot;
    be<u32> mBoundsTable;
    be<u16> mFirstCount; be<s16> mFirstIndex; be<u32> mFirstBoundsCallback;
    be<u16> mSecondCount; be<s16> mSecondIndex; be<u32> mSecondBoundsCallback;
    parameter_tree::Value mParameters;
    CascadeWordParameter mBoundingCalculation;
    CascadeWordParameter mCascadeCount;
    CascadeArrayParameter mCascadeNear;
    CascadeBoolParameter mDepthClamp;
    CascadeFloatParameter mFarOffset;
    CascadeFloatParameter mNearOffset;
    CascadeWordParameter mStableTexelWidth;
    CascadeFloatParameter mPointEpsilon;
    ShadowUnitBuffer mUnits;
    CascadeMapStorage_l mMap;
    be<u8> mMapFlag; u8 mMapPadding[3];
    CascadeSupportStorage_l mSupport;
    be<u32> mResetWord;
    static CascadeShadow* create(CascadeShadow* shadow);
    void releaseMap(); void destroy(u32 flags);
    u32 cascadeMask(); u32 alternateCascadeMask();
    void initializeUnits(const be<s32>* requestedCount, sead::RegistryHeap* heap);
    void updateCascadeRanges(void* firstContext, void* secondContext);
    void resetUnits(); void adjustUnitMargins(); void limitMapLayers();
    void* renderLayers(void* context);
    u32 mergeInputBounds(const CascadeBounds* bounds, u32 lane);
    u32 gatherAlternateInputBounds(void* context, u32 lane);
    u32 matchingCascadeMask(void* context); u32 matchingAlternateCascadeMask(void* context);
};
}
WWHD_SIZE(agl::ParameterRoot, 0x1C8);
WWHD_SIZE(agl::CascadeScalarPrefix, 0xC);
WWHD_SIZE(agl::CascadeWordParameter, 0x10);
WWHD_SIZE(agl::CascadeFloatParameter, 0x10);
WWHD_SIZE(agl::CascadeBoolParameter, 0x10);
WWHD_SIZE(agl::CascadeArrayParameter, 0x14);
WWHD_OFFSET(agl::CascadeShadow, mBoundsTable, 0x1C8);
WWHD_OFFSET(agl::CascadeShadow, mParameters, 0x1DC);
WWHD_OFFSET(agl::CascadeShadow, mBoundingCalculation, 0x238);
WWHD_OFFSET(agl::CascadeShadow, mCascadeCount, 0x248);
WWHD_OFFSET(agl::CascadeShadow, mCascadeNear, 0x258);
WWHD_OFFSET(agl::CascadeShadow, mDepthClamp, 0x26C);
WWHD_OFFSET(agl::CascadeShadow, mFarOffset, 0x27C);
WWHD_OFFSET(agl::CascadeShadow, mNearOffset, 0x28C);
WWHD_OFFSET(agl::CascadeShadow, mStableTexelWidth, 0x29C);
WWHD_OFFSET(agl::CascadeShadow, mPointEpsilon, 0x2AC);
WWHD_OFFSET(agl::CascadeShadow, mUnits, 0x2BC);
WWHD_OFFSET(agl::CascadeShadow, mMap, 0x2C4);
WWHD_OFFSET(agl::CascadeShadow, mMapFlag, 0xEA8);
WWHD_OFFSET(agl::CascadeShadow, mSupport, 0xEAC);
WWHD_OFFSET(agl::CascadeShadow, mResetWord, 0xF44);
WWHD_SIZE(agl::CascadeShadow, 0xF48);

WWHD_SIZE(agl::ShadowUnit_l, 0x5B4);
WWHD_OFFSET(agl::ShadowUnit_l, mVtable, 0x58);
WWHD_OFFSET(agl::ShadowUnitDispatch_l, mRelease, 0x44);

WWHD_OFFSET(agl::ShadowUnit_l, mFirstBounds, 0x60);
WWHD_OFFSET(agl::ShadowUnit_l, mSecondBounds, 0x68);
WWHD_OFFSET(agl::ShadowUnit_l, mBounds, 0x3BC);
WWHD_SIZE(agl::CascadeBounds, 0x18);

WWHD_OFFSET(agl::ShadowUnit_l, mInputBounds, 0x374);
WWHD_OFFSET(agl::ShadowUnit_l, mFirstSurfaceWords, 0x5A4);
WWHD_OFFSET(agl::ShadowUnit_l, mSecondSurfaceWords, 0x5AC);
WWHD_OFFSET(agl::CascadeMapStorage_l, mRenderMode, 0x6C);
WWHD_OFFSET(agl::CascadeMapStorage_l, mWidth, 0x7C);
WWHD_OFFSET(agl::CascadeMapStorage_l, mHeight, 0x8C);
WWHD_SIZE(agl::CascadeIndexRecord, 12);
