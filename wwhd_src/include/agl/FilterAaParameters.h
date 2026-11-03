// Parameter and text prefixes reconstructed from this WWHD translation unit.

#pragma once
#include "wwhd.h"
#include "agl/ParameterBase.h"
#include "sead/Viewport.h"
#include "sead/RenderState.h"
struct SurfaceImagePrefix_l;
namespace agl::filter_aa {
enum ParameterKind : u32 { kByteValue=0, kIntegerValue=1, kScalarValue=2, kColorValue=6 };
struct ParameterBase_l : agl::ParameterBase {
    void setMethods(const void* methods) {
        mMethods=reinterpret_cast<sead::VtableSlot*>(const_cast<void*>(methods));
    }
    void emptyHook();
};
struct ByteParameter : ParameterBase_l {
    be<u8> mValue; u8 mPadding[3];
    u32 kind() const;
    const void* readData() const;
    void* writeData();
    u32 byteSize() const;
};
struct ScalarParameter : ParameterBase_l {
    be<f32> mValue;
    u32 kind() const;
    const void* readData() const;
    void* writeData();
    u32 byteSize() const;
};
struct IntegerParameter : ParameterBase_l {
    be<s32> mValue;
    u32 kind() const;
    const void* readData() const;
    void* writeData();
    u32 byteSize() const;
};
struct Color4 {
    be<f32> mRed, mGreen, mBlue, mAlpha;
    static Color4* construct(Color4* storage);
};
struct ColorParameter : ParameterBase_l {
    Color4 mValue;
    static ColorParameter* construct(ColorParameter* storage);
    u32 kind() const;
    const void* readData() const;
    void* writeData();
    u32 byteSize() const;
};
// Parameter groups follow their construction order.
struct ParameterGroupA {
    IntegerParameter mFirst, mSecond, mThird;
    ColorParameter mColor;
    IntegerParameter mLast;
    static ParameterGroupA* construct(ParameterGroupA* storage);
};
struct ParameterGroupB {
    ByteParameter mFirst;
    IntegerParameter mSecond, mThird, mFourth, mFifth;
    static ParameterGroupB* construct(ParameterGroupB* storage);
};
// These extents are established by the owning constructor's subobject offsets.
struct SurfacePair {
    gptr<::SurfaceImagePrefix_l> mHeader;
    u8 mSurface[0x198];
    u8 mSecondSurface[0x148];
    static SurfacePair* construct(SurfacePair* storage);
};
struct SurfaceBatchEntry_l {
    u8 mSurfaces[4][0x198];
    u8 mSurfaceState[0x40];
    SurfacePair mPairs[2];
    u8 mUnknownTail[8];
    be<f32> mScaleX, mScaleY;
    u8 mUnknownAfterScale[4];
    u8 mReleaseContext[0x1C];
};
struct ParameterSlice_l {
    u8 mPrefix[12];
    be<u8> mEnabled;
    u8 mBeforeFirst[15];
    be<f32> mFirst;
    u8 mBetweenValues[12];
    be<f32> mSecond;
    u8 mTail[32];
};
struct ParameterBlock_l {
    u8 mPrefix[0xB8];
    ParameterSlice_l mSlices[6];
    u8 mTail[0x14];
};
struct FilterParameterContainer_l;
struct EffectRuntimePrefix_l {
    be<u32> mBaseWord;
    be<s32> mRequestedSurfaceCount, mParameterCount;
    u8 mUnknownBase[4];
    be<u32> mParameterIndexMask;
    be<u32> mValueRecordCount;
    gptr<FilterParameterContainer_l> mValueRecords;
    gptr<const void> mRuntimeMethods;
    u8 mParameters[0x2AC];
    be<u32> mSurfaceCount;
    gptr<SurfaceBatchEntry_l> mSurfaces;
    be<u32> mSecondCount;
    gptr<gptr<ParameterBlock_l>> mSecondStorage;
    be<u32> mThirdCount;
    gptr<ParameterBlock_l> mThirdStorage;
    static EffectRuntimePrefix_l* construct(EffectRuntimePrefix_l* storage);
    void releasePairHandles(u32 index);
    void initializeArrays(s32 surfaceCount,u32 sharedParameters,void* heap);
    u32 renderDefault(u32 surfaceIndex,void* context,u32 fallback);
    u32 drawColor(void* context,const Color4* uniformColor,const Color4* blendColor,u32 previousMode);
    u32 drawMipBlur(u32 surfaceIndex,u32 level,u32 previousMode,f64 strength);
    u32 drawBloomLevels(u32 surfaceIndex,u32 previousMode);
    u32 applySliceColor(u32 surfaceIndex,u32 sliceIndex,void* receiver,u32 fallback);
    static void destroy(EffectRuntimePrefix_l* storage,u32 flags);
};
struct ControlValueRecord { be<u32> mTag, mValueToken; };
struct ControlValueReference { gptr<ControlValueRecord> mRecord; };
struct ControlPrefix_l {
    u8 mPrefix[12];
    gptr<const void> mPrimaryMethods;
    gptr<const void> mSecondaryMethods;
    u8 mBody[64];
    be<u32> mValueToken;
    static void destroy(ControlPrefix_l* control,u32 flags);
    void emptyHook();
    u32 constantOneHook() const;
    u32 matchesValue(const ControlValueReference* value) const;
    void alternateEmptyHook();
};
struct FilterSurfaceEntry_l {
    be<u32> mHeader;
    u8 mSurfaceState[0x40];
    u8 mSecondSurfaceState[0x148];
    u8 mSurfaces[2][0x198];
    u8 mThirdSurface[0x198];
    u8 mTextureData[0x90];
    gptr<void> mStorage;
    u8 mBeforeAttributes[8];
    u8 mFirstAttribute[0x40], mFirstIndices[0x30], mSecondAttribute[0x40], mSecondIndices[0x30];
};
struct ShaderVariant_l { u8 mStorage[0x84]; };
// Read-only shader query records.
struct ShaderQueryChoice_l { gptr<const char> mText; gptr<const void> mMethods; };
struct ShaderQueryRecord_l {
    gptr<const char> mName;
    gptr<const void> mMethods;
    u8 mOpaque[8];
    be<u32> mChoiceCount;
    gptr<const ShaderQueryChoice_l> mChoices;
    be<u16> mRadixWeight;
    u8 mTail[2];
};
struct ShaderVariantList_l {
    gptr<ShaderVariant_l> mDefault;
    be<u32> mCount;
    gptr<ShaderVariant_l> mVariants;
    be<u32> mQueryRecordCount;
    gptr<const ShaderQueryRecord_l> mQueryRecords;
};
struct ShaderArchiveResource_l { u8 mPrefix[8]; ShaderVariantList_l mVariants; };
struct ShaderArchive_l {
    u8 mPrefix[2];
    be<u16> mQuerySelection;
    u8 mBody[0x78];
    gptr<ShaderArchiveResource_l> mResource;
};
struct ShaderArchiveRegistry_l {
    u8 mPrefix[0x14];
    be<u32> mCount;
    u8 mBody[4];
    gptr<gptr<ShaderArchive_l>> mArchives;
};
struct FilterSetupArguments_l { be<s32> mEntryCount; be<u32> mStorageBytes; };
using FilterRenderState_l=sead::CompactRenderState;
struct FilterParameterContainer_l { u8 mPrefix[0x58]; gptr<const void> mMethods; };
struct FilterAaObject_l {
    u8 mPrefix[0x60];
    gptr<const void> mPrimaryMethods;
    u8 mBeforeSecondary[0x164];
    gptr<const void> mSecondaryMethods;
    FilterRenderState_l mRenderState;
    gptr<ShaderVariant_l> mLumaPrograms[2];
    gptr<ShaderVariant_l> mPrograms[2][7][3];
    gptr<ShaderVariant_l> mDepthPrograms[2][2][7][3];
    FilterParameterContainer_l mParameterContainer;
    IntegerParameter mAntialiasType;
    ByteParameter mEnabled;
    ScalarParameter mAlphaOutput;
    IntegerParameter mEdgeQuality, mFetchQuality;
    ScalarParameter mEdgeThresholdScale;
    ColorParameter mLumaCoefficients;
    ScalarParameter mSubpixel, mMaximumSpan, mSpanMultiply, mMinimumSpan;
    ByteParameter mReprojection;
    ScalarParameter mReprojectionMoveLimit;
    be<u32> mRuntimeWord, mRuntimeCounter;
    be<u8> mRuntimeFlag;
    u8 mBeforeEntries[3];
    be<u32> mEntryCount;
    gptr<FilterSurfaceEntry_l> mEntries;
    ControlPrefix_l mControl;
    u8 mTail[0x18C];
    static void destroy(FilterAaObject_l* storage,u32 flags);
    void initialize(const FilterSetupArguments_l* arguments,void* heap);
    static FilterAaObject_l* construct(FilterAaObject_l* storage);
    u32 render(u32 surfaceIndex,sead::RenderSurface* renderSurface,sead::Viewport* viewContext,void* output,void* depth,u32 depthKind,void* previousDepth,u32 previousMode);
};
struct ControlOwnerPrefix_l { u32 constantOneHook() const; void emptyHook(); };
// Only the surface address is passed here; its owning unit supplies the full type.
struct SurfacePrefix_l { u8 mOpaque[1]; };
struct SurfaceOwnerPrefix_l {
    be<u32> mHeader;
    SurfacePrefix_l mSurface;
    static void destroy(SurfaceOwnerPrefix_l* owner,u32 flags);
};
}
namespace agl::filter_aa_text {
struct Text {
    gptr<char> mText; gptr<const void> mMethods;
    static void destroy(Text* text,u32 flags);
    void assureTermination();
};
struct TextBufferPrefix_l {
    gptr<char> mText; gptr<const void> mMethods; be<u32> mCapacity;
    void assureTermination();
};
// Per-table prefix views in table-address order. Fixed buffer extents are unknown
// except where an owning constructor explicitly establishes the capacity.
struct TextBufferA : TextBufferPrefix_l { static void destroy(TextBufferA* text,u32 flags); };
struct TextBufferB : TextBufferPrefix_l { static void destroy(TextBufferB* text,u32 flags); };
struct TextBufferC : TextBufferPrefix_l { static void destroy(TextBufferC* text,u32 flags); };
struct TextBufferD : TextBufferPrefix_l { static void destroy(TextBufferD* text,u32 flags); };
struct FormattedText128 : TextBufferD {
    be<u8> mBuffer[128];
    static FormattedText128* construct(FormattedText128* storage,const char* format,
        u32 first,u32 second,u32 third,u32 fourth,u32 fifth,u32 sixth,
        f64 float1,f64 float2,f64 float3,f64 float4,f64 float5,f64 float6,f64 float7,f64 float8);
};
struct TextBufferE : TextBufferPrefix_l { static void destroy(TextBufferE* text,u32 flags); };
struct TextBufferF : TextBufferPrefix_l { static void destroy(TextBufferF* text,u32 flags); };
struct TextBufferG : TextBufferPrefix_l { static void destroy(TextBufferG* text,u32 flags); };
}
WWHD_OFFSET(agl::filter_aa::ParameterBase_l,mNameHash,0);
WWHD_OFFSET(agl::filter_aa::ParameterBase_l,mNext,4);
WWHD_OFFSET(agl::filter_aa::ParameterBase_l,mMethods,8);
WWHD_SIZE(agl::filter_aa::ParameterBase_l,12);
WWHD_OFFSET(agl::filter_aa::ByteParameter,mValue,12);
WWHD_SIZE(agl::filter_aa::ByteParameter,16);
WWHD_OFFSET(agl::filter_aa::ScalarParameter,mValue,12);
WWHD_SIZE(agl::filter_aa::ScalarParameter,16);
WWHD_OFFSET(agl::filter_aa::IntegerParameter,mValue,12);
WWHD_SIZE(agl::filter_aa::IntegerParameter,16);
WWHD_OFFSET(agl::filter_aa::ColorParameter,mValue,12);
WWHD_SIZE(agl::filter_aa::ColorParameter,28);
WWHD_OFFSET(agl::filter_aa::Color4,mRed,0);
WWHD_OFFSET(agl::filter_aa::Color4,mGreen,4);
WWHD_OFFSET(agl::filter_aa::Color4,mBlue,8);
WWHD_OFFSET(agl::filter_aa::Color4,mAlpha,12);
WWHD_SIZE(agl::filter_aa::Color4,16);
WWHD_OFFSET(agl::filter_aa::ControlValueReference,mRecord,0);
WWHD_OFFSET(agl::filter_aa::ControlValueRecord,mValueToken,4);
WWHD_OFFSET(agl::filter_aa::ControlPrefix_l,mPrimaryMethods,12);
WWHD_OFFSET(agl::filter_aa::ControlPrefix_l,mSecondaryMethods,16);
WWHD_OFFSET(agl::filter_aa::ControlPrefix_l,mValueToken,0x54);
WWHD_OFFSET(agl::filter_aa::SurfaceOwnerPrefix_l,mSurface,4);
WWHD_OFFSET(agl::filter_aa_text::Text,mText,0);
WWHD_OFFSET(agl::filter_aa_text::Text,mMethods,4);
WWHD_SIZE(agl::filter_aa_text::Text,8);
WWHD_OFFSET(agl::filter_aa_text::TextBufferPrefix_l,mText,0);
WWHD_OFFSET(agl::filter_aa_text::TextBufferPrefix_l,mMethods,4);
WWHD_OFFSET(agl::filter_aa_text::TextBufferPrefix_l,mCapacity,8);
WWHD_SIZE(agl::filter_aa_text::TextBufferPrefix_l,12);

WWHD_OFFSET(agl::filter_aa::ParameterGroupA,mColor,0x30);
WWHD_OFFSET(agl::filter_aa::ParameterGroupA,mLast,0x4C);
WWHD_SIZE(agl::filter_aa::ParameterGroupA,0x5C);
WWHD_SIZE(agl::filter_aa::ParameterGroupB,0x50);
WWHD_OFFSET(agl::filter_aa::SurfacePair,mSurface,4);
WWHD_OFFSET(agl::filter_aa::SurfacePair,mSecondSurface,0x19C);
WWHD_SIZE(agl::filter_aa::SurfacePair,0x2E4);
WWHD_OFFSET(agl::filter_aa::EffectRuntimePrefix_l,mParameters,0x20);
WWHD_OFFSET(agl::filter_aa::EffectRuntimePrefix_l,mSurfaceCount,0x2CC);
WWHD_OFFSET(agl::filter_aa::EffectRuntimePrefix_l,mThirdStorage,0x2E0);
WWHD_SIZE(agl::filter_aa::EffectRuntimePrefix_l,0x2E4);

WWHD_OFFSET(agl::filter_aa::SurfaceBatchEntry_l,mPairs,0x6A0);
WWHD_OFFSET(agl::filter_aa::SurfaceBatchEntry_l,mReleaseContext,0xC7C);
WWHD_SIZE(agl::filter_aa::SurfaceBatchEntry_l,0xC98);

WWHD_OFFSET(agl::filter_aa::EffectRuntimePrefix_l,mRuntimeMethods,0x1C);
WWHD_OFFSET(agl::filter_aa::SurfaceBatchEntry_l,mSurfaceState,0x660);

WWHD_OFFSET(agl::filter_aa_text::FormattedText128,mBuffer,12);
WWHD_SIZE(agl::filter_aa_text::FormattedText128,0x8C);

WWHD_SIZE(agl::filter_aa::ParameterSlice_l,0x50);
WWHD_OFFSET(agl::filter_aa::ParameterSlice_l,mEnabled,12);
WWHD_OFFSET(agl::filter_aa::ParameterSlice_l,mFirst,0x1C);
WWHD_OFFSET(agl::filter_aa::ParameterSlice_l,mSecond,0x2C);
WWHD_SIZE(agl::filter_aa::ParameterBlock_l,0x2AC);
WWHD_OFFSET(agl::filter_aa::SurfaceBatchEntry_l,mScaleX,0xC70);
WWHD_OFFSET(agl::filter_aa::SurfaceBatchEntry_l,mScaleY,0xC74);

WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mSurfaces,0x18C);
WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mThirdSurface,0x4BC);
WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mStorage,0x6E4);
WWHD_SIZE(agl::filter_aa::FilterSurfaceEntry_l,0x7D0);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mPrimaryMethods,0x60);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mSecondaryMethods,0x1C8);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mParameterContainer,0x440);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mEntryCount,0x584);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mEntries,0x588);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mControl,0x58C);
WWHD_SIZE(agl::filter_aa::FilterAaObject_l,0x770);

WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mSecondSurfaceState,0x44);
WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mTextureData,0x654);
WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mFirstAttribute,0x6F0);
WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mFirstIndices,0x730);
WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mSecondAttribute,0x760);
WWHD_OFFSET(agl::filter_aa::FilterSurfaceEntry_l,mSecondIndices,0x7A0);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mLumaPrograms,0x240);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mPrograms,0x248);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mDepthPrograms,0x2F0);
WWHD_OFFSET(agl::filter_aa::ShaderArchive_l,mQuerySelection,2);
WWHD_OFFSET(agl::filter_aa::ShaderArchive_l,mResource,0x7C);
WWHD_OFFSET(agl::filter_aa::ShaderArchiveRegistry_l,mArchives,0x1C);
WWHD_SIZE(agl::filter_aa::ShaderVariant_l,0x84);

WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mRenderState,0x1CC);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mAntialiasType,0x49C);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mLumaCoefficients,0x4FC);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mReprojectionMoveLimit,0x568);
WWHD_OFFSET(agl::filter_aa::FilterAaObject_l,mRuntimeWord,0x578);
WWHD_OFFSET(agl::filter_aa::FilterRenderState_l,mCullMode,0x8);
WWHD_OFFSET(agl::filter_aa::FilterRenderState_l,mBlendEnable,0xC);
WWHD_OFFSET(agl::filter_aa::FilterRenderState_l,mAlphaTest,0x38);
WWHD_OFFSET(agl::filter_aa::FilterRenderState_l,mWriteRed,0x44);

WWHD_SIZE(agl::filter_aa::ShaderQueryChoice_l,8);
WWHD_OFFSET(agl::filter_aa::ShaderQueryRecord_l,mChoiceCount,0x10);
WWHD_OFFSET(agl::filter_aa::ShaderQueryRecord_l,mChoices,0x14);
WWHD_OFFSET(agl::filter_aa::ShaderQueryRecord_l,mRadixWeight,0x18);
WWHD_SIZE(agl::filter_aa::ShaderQueryRecord_l,0x1C);
WWHD_OFFSET(agl::filter_aa::ShaderVariantList_l,mQueryRecordCount,0xC);
WWHD_OFFSET(agl::filter_aa::ShaderVariantList_l,mQueryRecords,0x10);
WWHD_SIZE(agl::filter_aa::ShaderVariantList_l,0x14);
WWHD_SIZE(agl::filter_aa::ShaderArchiveResource_l,0x1C);

// FormattedText128::construct is a GHS register-bank ABI adapter. Its six u32
// and eight f64 trailing parameters transport r5-r10 and f1-f8; they do not
// establish the original source-language variadic argument types.
