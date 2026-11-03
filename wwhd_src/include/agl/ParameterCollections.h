/* agl parameter collections of the light-prepass translation unit (the TU is shared with AGL#14, #15 and #16).
 *
 * Owner header for the collection classes: the method table every collection shares, the point (compact), directed
 * (extended), spatial and curve record collections with their per-view values, and the TU's runtime-type tables and
 * text classes. Also the light-prepass configuration and the GPU cycle-counter helpers at the start of the TU.
 * Layouts come from WWHD's constructors, initializers and the four collection method tables; slot roles from the
 * calls the collection bodies and the light prepass make through them. AGL#14's accessors (agl/ValueCollections.cpp)
 * use these declarations; its spatial and curve collection methods are declared here. */
#pragma once
#include "wwhd.h"
#include "sead/RuntimeType.h"
#include "sead/FileDevice.h"
#include "sead/HeapRegistry.h"
#include "agl/UniformBuffer.h"
#include "agl/TextureSampler.h"
#include "agl/CurveParameter.h"     // CurveDescriptor
namespace agl {

// Element `index` of a guest array, by 32-bit guest-address arithmetic: a NULL array is walked from address 0 and the
// offset wraps, as on the console.
template <class T> inline T* guestElement(T* array, u32 index) {
    return gabi::at<T>(gabi::ea(array) + index * (u32)sizeof(T));
}
// The clamped element access the collections inline everywhere: an index outside the array selects element 0.
template <class T> inline T* clampedElement(T* array, u32 size, u32 index) {
    return index < size ? guestElement(array, index) : array;
}

/* ---- GPU cycle counter (start of the TU) ---- */
struct GpuCycleSample {
    be<u32> mHigh;
    be<u32> mLow;
    Pair32 read();
    static void sampleTop(GpuCycleSample* sample);
    static void sampleBottom(GpuCycleSample* sample);
};
struct GpuCycleClock {
    static void destroy(GpuCycleClock* object, u32 flags);
    void synchronize();
};
struct GpuCycleTimer {
    const sead::SafeText* name() const;
    const sead::SafeText* description() const;
    u32 isCpuClock() const;
    u32 isGpuClock() const;
};

/* ---- the light-prepass capacities (filled with the defaults by create, then passed to LightPrepass::configure) ---- */
struct LightPrepassConfig {
    be<s32> mPointCount;
    be<s32> mDirectedCount;
    be<s32> mSpatialCount;
    be<s32> mViewCount;
    be<s32> mExtraCount;
    static LightPrepassConfig* create(LightPrepassConfig* object);
};

/* ---- the collection method table (8-byte slots: adjustment, function) ---- */
struct CollectionTable {
    u8 mHeader[0x10];
    sead::VtableSlot mMatchesType;
    u8 mReserved18[8];
    sead::VtableSlot mInitialize;
    sead::VtableSlot mRelease;
    sead::VtableSlot mUpdate;
    sead::VtableSlot mPrepare;
    sead::VtableSlot mRefresh;
    sead::VtableSlot mApply;
    sead::VtableSlot mApplyWithContext;
    u8 mReserved58[0x10];
    sead::VtableSlot mKind;
    u8 mReserved70[8];
    sead::VtableSlot mSetCount;
    sead::VtableSlot mCount;
    sead::VtableSlot mCapacity;
    u8 mReserved90[0x10];
    sead::VtableSlot mIsEnabled;
    u8 mReservedA8[0x10];
    sead::VtableSlot mSetRecordEnabled;
    sead::VtableSlot mCanApply;
    u8 mReservedC8[0x10];
    sead::VtableSlot mBeforeAllocate;
    sead::VtableSlot mInitializeRecord;
    sead::VtableSlot mInitializeValue;
    sead::VtableSlot mReleaseRecord;
    sead::VtableSlot mBeforeUpdate;
    sead::VtableSlot mUpdateRecord;
    sead::VtableSlot mPrepareValue;
    sead::VtableSlot mBeforeRefresh;
    sead::VtableSlot mRefreshRecord;
    sead::VtableSlot mPrepareApply;
    sead::VtableSlot mBeginApply;
    sead::VtableSlot mApplyValue;
    sead::VtableSlot mPrepareContext;
    sead::VtableSlot mApplyContext;
};

/* The common prefix of every collection (the light prepass dispatches through it). */
struct ParameterCollection {
    gptr<CollectionTable> mTable;
    gptr<void> mOwner;
    be<u32> mCapacity;
    gptr<void> mRecords;
    be<u32> mCount;
    be<u32> mValueCapacity;
    gptr<be<u32>> mValueCounts;
};

/* ---- per-view values: a flag word and the uniform buffer the value is uploaded through ---- */
enum ValueFlags { ValueFlag_Skip = 1 };
struct CompactValue {
    be<u32> mFlags;
    UniformBuffer mUniform;
    u8 mColorState[0x10];
    be<f32> mBlendColor[4];
    u8 mResultState[0x1C];
};
struct ExtendedValue {
    be<u32> mFlags;
    be<u32> mViewState;
    u8 mPayload[0x44];
    UniformBuffer mUniform;
};
struct CurveValue {
    be<u32> mFlags;
    UniformBuffer mUniform;
};

/* ---- records ---- */
enum RecordFlags {
    Record_Enabled = 1,
    PointRecord_Falloff = 2,
    PointRecord_Shadow = 4,
    PointRecord_Paired = 0x20,
    PointRecord_Visible = 0x80,
    PointRecord_Dynamic = 0x100,
    DirectedRecord_Visible = 2,
};
struct CompactRecord {
    be<u16> mFlags;
    u8 mFlagsPadding[2];
    be<u32> mMask;
    be<f32> mPosition[3];
    be<f32> mRadius;
    be<f32> mColor[4];
    be<f32> mFalloff[4];
    be<f32> mIntensity;
    be<f32> mAttenuation;
    be<u32> mValueCount;
    gptr<CompactValue> mValues;
};
struct ExtendedRecord {
    be<u8> mFlags;
    u8 mFlagsPadding[3];
    be<u32> mMask;
    be<f32> mPosition[3];
    be<f32> mDirection[3];
    be<f32> mColor[4];
    be<f32> mFalloff[4];
    be<f32> mShape[4];
    be<u32> mMode;
    be<f32> mThreshold;
    be<u32> mValueCount;
    gptr<ExtendedValue> mValues;
};
struct SpatialRecord {
    be<u8> mFlags;
    u8 mFlagsPadding[3];
    be<u32> mMask;
    u8 mPositionState[0x24];
    be<f32> mColor[4];
    be<f32> mFalloff[4];
    u8 mSpatialState[0x40];
    TextureSampler mPrimaryTexture;
    u8 mValuePadding[0x18];
    be<u32> mValueCount;
    gptr<ExtendedValue> mValues;
    u8 mProjectionPadding[0x1C];
    be<f32> mMaximum[3];
    be<f32> mMinimum[3];
    u8 mBoundsPadding[8];
    TextureData mProjectionTexture;
    TextureSampler mFollowingTexture;
};
struct CurveRecord {
    be<u8> mFlags;
    u8 mFlagsPadding[3];
    be<u32> mMask;
    gptr<CompactRecord> mPrimary;
    gptr<CompactRecord> mSecondary;
    u8 mState[4];
    be<u32> mValueCount;
    gptr<CurveValue> mValues;
};

/* ---- the collections (methods in method-table order; the hooks are empty in these base classes) ---- */
struct CompactCollection {
    gptr<CollectionTable> mTable;
    gptr<void> mOwner;
    be<u32> mCapacity;
    gptr<CompactRecord> mRecords;
    be<u32> mCount;
    be<u32> mValueCapacity;
    gptr<be<u32>> mValueCounts;
    u32 matchesType(const sead::RuntimeType* type) const;
    void initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap);
    void release();
    void update();
    u32 prepare(u32 valueIndex, void* context, u32 outputIndex);
    void refresh();
    void apply(u32 valueIndex, void* context, u32 invalidateGpu);
    void applyWithContext(u32 valueIndex, void* context, void* output);
    s32 kind() const;
    void setCount(u32 count);
    u32 count() const;
    u32 isEnabled(u32 index) const;
    void setRecordEnabled(u32 index, u32 enabled);
    u32 canApply(u32 index, u32 valueIndex) const;
    void beforeAllocate();
    void initializeRecord();
    void initializeValue();
    void releaseRecord();
    void beforeUpdate();
    void updateRecord();
    u32 prepareValue() const;
    void beforeRefresh();
    void refreshRecord();
    void prepareApply();
    void beginApply();
    void applyValue();
    void prepareContext();
    void applyContext();
};
struct ExtendedCollection {
    gptr<CollectionTable> mTable;
    gptr<void> mOwner;
    be<u32> mCapacity;
    gptr<ExtendedRecord> mRecords;
    be<u32> mCount;
    be<u32> mValueCapacity;
    gptr<be<u32>> mValueCounts;
    u32 matchesType(const sead::RuntimeType* type) const;
    void initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap);
    void release();
    void update();
    u32 prepare(u32 valueIndex, void* context, u32 outputIndex);
    void refresh();
    void apply(u32 valueIndex, void* context, u32 invalidateGpu);
    void applyWithContext(u32 valueIndex, void* context, void* output);
    s32 kind() const;
    void setCount(u32 count);
    u32 isEnabled(u32 index) const;
    void setRecordEnabled(u32 index, u32 enabled);
    u32 canApply(u32 index, u32 valueIndex) const;
    void beforeAllocate();
    void initializeRecord();
    void initializeValue();
    void releaseRecord();
    void beforeUpdate();
    void updateRecord();
    u32 prepareValue() const;
    void beforeRefresh();
    void refreshRecord();
    void prepareApply();
    void beginApply();
    void applyValue();
    void prepareContext();
    void applyContext();
};
// The methods past apply (findRecord, setCount, the record mask accessors and the hooks) are AGL#14's.
struct SpatialCollection {
    gptr<CollectionTable> mTable;
    gptr<void> mOwner;
    be<u32> mCapacity;
    gptr<SpatialRecord> mRecords;
    be<u32> mCount;
    be<u32> mValueCapacity;
    gptr<be<u32>> mValueCounts;
    u32 matchesType(const sead::RuntimeType* type) const;
    void initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap);
    void release();
    void update();
    u32 prepare(u32 valueIndex, void* context, u32 outputIndex);
    void refresh();
    void apply(u32 valueIndex, void* context, u32 invalidateGpu);
    s32 findRecord();
    void setCount(u32 count);
    u32 isEnabled(u32 index);
    void setMask(u32 index, u32 enabled);
    u32 testMask(u32 index, u32 bit);
    void applyContext(u32 slot, void* context, void* output);
    // Hook role names follow the matching CollectionTable slots.
    void beforeAllocate();
    void initializeRecord();
    void initializeValue();
    void releaseRecord();
    void beforeUpdate();
    void updateRecord();
    void beforeRefresh();
    void refreshRecord();
    void prepareValues();
    void beginApply();
    void applyValue();
    void finishValue();
    void applyWithContext();
    u32 needsRefresh();
};
// Every method is AGL#14's; the light prepass pairs point records in it.
struct CurveCollection {
    gptr<CollectionTable> mTable;
    gptr<void> mOwner;
    be<u32> mCapacity;
    gptr<CurveRecord> mRecords;
    be<u32> mCount;
    be<u32> mValueCapacity;
    gptr<be<u32>> mValueCounts;
    s32 findRecord();
    void setCount(u32 count);
    u32 count();
    u32 capacity();
    u32 isEnabled(u32 index);
    void setMask(u32 index, u32 enabled);
    u32 testMask(u32 index, u32 bit);
    void applyContext(u32 slot, void* context, void* output);
    u32 matchesType(const sead::RuntimeType* type) const;
    void initialize(u32 owner, s32 recordCount, s32 valueCount, sead::RegistryHeap* heap);
    void release();
    void apply(u32 slot, void* context, u32 invalidateGpu);
    void update();
    void refresh();
    u32 prepare(u32 slot, void* context, u32 outputIndex);
    // Hook role names follow the matching CollectionTable slots.
    void beforeAllocate();
    void initializeRecord();
    void initializeValue();
    void releaseRecord();
    void beforeUpdate();
    void updateRecord();
    void beforeRefresh();
    void refreshRecord();
    void prepareValues();
    void beginApply();
    void applyValue();
    void finishValue();
    void applyWithContext();
    u32 needsRefresh();
};

/* ---- the TU's text classes (one method table each; only their virtual bodies are out of line) ---- */
struct ParameterText : sead::SafeText {
    void destroy(u32 flags);
    void assureTermination();
};
struct ParameterTextBuffer : sead::SafeTextBuffer {
    void destroy(u32 flags);
    void assureTermination();
};
struct ParameterFixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); };
struct ParameterFixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct ParameterFixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };

// The record-collection facade the light prepass embeds: a disposer-based parameter object of AGL#10 (borrowed
// prefix, limited to the fields its destructor writes).
struct CollectionFacade_l {
    sead::Disposer mDisposer;
    gptr<void> mObjectMethods;
    static void destroy(CollectionFacade_l* object, u32 flags);
};

// The curve descriptor's constructor (the type is AGL#14's agl::CurveDescriptor).
CurveDescriptor* createCurveDescriptor(CurveDescriptor* object);
// Builds a texture resource from its description through a stack handle and returns the handle.
u32 createLightResource(u32 kind, u32 width, u32 height, u32 depth, u32 format, u32 usage, u32 flags, u32 options,
                        u8 sampled, u8 cleared, u8 linear, u8 transient, u8 readable, u8 writable);
}

// The TU's text method table and the collection base runtime-type table, owned by agl/ParameterCollections.cpp
// (AGL#14 borrows both).
extern sead::SafeTextVtable& sParameterTextMethods;
extern be<u32>& sCollectionBaseTypeMethods;

WWHD_OFFSET(agl::CollectionFacade_l, mObjectMethods, 0x10);
WWHD_SIZE(agl::CollectionFacade_l, 0x14);
WWHD_OFFSET(agl::GpuCycleSample, mLow, 4);
WWHD_SIZE(agl::GpuCycleSample, 8);
WWHD_OFFSET(agl::LightPrepassConfig, mDirectedCount, 4);
WWHD_OFFSET(agl::LightPrepassConfig, mSpatialCount, 8);
WWHD_OFFSET(agl::LightPrepassConfig, mViewCount, 0xC);
WWHD_OFFSET(agl::LightPrepassConfig, mExtraCount, 0x10);
WWHD_SIZE(agl::LightPrepassConfig, 0x14);
WWHD_OFFSET(agl::CollectionTable, mMatchesType, 0x10);
WWHD_OFFSET(agl::CollectionTable, mInitialize, 0x20);
WWHD_OFFSET(agl::CollectionTable, mRelease, 0x28);
WWHD_OFFSET(agl::CollectionTable, mUpdate, 0x30);
WWHD_OFFSET(agl::CollectionTable, mPrepare, 0x38);
WWHD_OFFSET(agl::CollectionTable, mRefresh, 0x40);
WWHD_OFFSET(agl::CollectionTable, mApply, 0x48);
WWHD_OFFSET(agl::CollectionTable, mApplyWithContext, 0x50);
WWHD_OFFSET(agl::CollectionTable, mKind, 0x68);
WWHD_OFFSET(agl::CollectionTable, mSetCount, 0x78);
WWHD_OFFSET(agl::CollectionTable, mCount, 0x80);
WWHD_OFFSET(agl::CollectionTable, mCapacity, 0x88);
WWHD_OFFSET(agl::CollectionTable, mIsEnabled, 0xA0);
WWHD_OFFSET(agl::CollectionTable, mSetRecordEnabled, 0xB8);
WWHD_OFFSET(agl::CollectionTable, mCanApply, 0xC0);
WWHD_OFFSET(agl::CollectionTable, mBeforeAllocate, 0xD8);
WWHD_OFFSET(agl::CollectionTable, mReleaseRecord, 0xF0);
WWHD_OFFSET(agl::CollectionTable, mBeforeUpdate, 0xF8);
WWHD_OFFSET(agl::CollectionTable, mPrepareValue, 0x108);
WWHD_OFFSET(agl::CollectionTable, mBeforeRefresh, 0x110);
WWHD_OFFSET(agl::CollectionTable, mRefreshRecord, 0x118);
WWHD_OFFSET(agl::CollectionTable, mPrepareApply, 0x120);
WWHD_OFFSET(agl::CollectionTable, mPrepareContext, 0x138);
WWHD_OFFSET(agl::CollectionTable, mApplyContext, 0x140);
WWHD_SIZE(agl::CollectionTable, 0x148);
WWHD_OFFSET(agl::ParameterCollection, mCapacity, 8);
WWHD_OFFSET(agl::ParameterCollection, mRecords, 0xC);
WWHD_OFFSET(agl::ParameterCollection, mCount, 0x10);
WWHD_OFFSET(agl::ParameterCollection, mValueCapacity, 0x14);
WWHD_OFFSET(agl::ParameterCollection, mValueCounts, 0x18);
WWHD_SIZE(agl::ParameterCollection, 0x1C);
WWHD_OFFSET(agl::CompactValue, mUniform, 4);
WWHD_OFFSET(agl::CompactValue, mBlendColor, 0x30);
WWHD_SIZE(agl::CompactValue, 0x5C);
WWHD_OFFSET(agl::ExtendedValue, mViewState, 4);
WWHD_OFFSET(agl::ExtendedValue, mUniform, 0x4C);
WWHD_SIZE(agl::ExtendedValue, 0x68);
WWHD_OFFSET(agl::CurveValue, mUniform, 4);
WWHD_SIZE(agl::CurveValue, 0x20);
WWHD_OFFSET(agl::CompactRecord, mMask, 4);
WWHD_OFFSET(agl::CompactRecord, mPosition, 8);
WWHD_OFFSET(agl::CompactRecord, mRadius, 0x14);
WWHD_OFFSET(agl::CompactRecord, mColor, 0x18);
WWHD_OFFSET(agl::CompactRecord, mFalloff, 0x28);
WWHD_OFFSET(agl::CompactRecord, mIntensity, 0x38);
WWHD_OFFSET(agl::CompactRecord, mAttenuation, 0x3C);
WWHD_OFFSET(agl::CompactRecord, mValueCount, 0x40);
WWHD_OFFSET(agl::CompactRecord, mValues, 0x44);
WWHD_SIZE(agl::CompactRecord, 0x48);
WWHD_OFFSET(agl::ExtendedRecord, mMask, 4);
WWHD_OFFSET(agl::ExtendedRecord, mPosition, 8);
WWHD_OFFSET(agl::ExtendedRecord, mDirection, 0x14);
WWHD_OFFSET(agl::ExtendedRecord, mColor, 0x20);
WWHD_OFFSET(agl::ExtendedRecord, mFalloff, 0x30);
WWHD_OFFSET(agl::ExtendedRecord, mShape, 0x40);
WWHD_OFFSET(agl::ExtendedRecord, mMode, 0x50);
WWHD_OFFSET(agl::ExtendedRecord, mThreshold, 0x54);
WWHD_OFFSET(agl::ExtendedRecord, mValueCount, 0x58);
WWHD_OFFSET(agl::ExtendedRecord, mValues, 0x5C);
WWHD_SIZE(agl::ExtendedRecord, 0x60);
WWHD_OFFSET(agl::SpatialRecord, mMask, 4);
WWHD_OFFSET(agl::SpatialRecord, mColor, 0x2C);
WWHD_OFFSET(agl::SpatialRecord, mFalloff, 0x3C);
WWHD_OFFSET(agl::SpatialRecord, mPrimaryTexture, 0x8C);
WWHD_OFFSET(agl::SpatialRecord, mValueCount, 0x23C);
WWHD_OFFSET(agl::SpatialRecord, mValues, 0x240);
WWHD_OFFSET(agl::SpatialRecord, mMaximum, 0x260);
WWHD_OFFSET(agl::SpatialRecord, mMinimum, 0x26C);
WWHD_OFFSET(agl::SpatialRecord, mProjectionTexture, 0x280);
WWHD_OFFSET(agl::SpatialRecord, mFollowingTexture, 0x310);
WWHD_SIZE(agl::SpatialRecord, 0x4A8);
WWHD_OFFSET(agl::CurveRecord, mMask, 4);
WWHD_OFFSET(agl::CurveRecord, mPrimary, 8);
WWHD_OFFSET(agl::CurveRecord, mSecondary, 0xC);
WWHD_OFFSET(agl::CurveRecord, mValueCount, 0x14);
WWHD_OFFSET(agl::CurveRecord, mValues, 0x18);
WWHD_SIZE(agl::CurveRecord, 0x1C);
WWHD_OFFSET(agl::CompactCollection, mRecords, 0xC);
WWHD_OFFSET(agl::CompactCollection, mValueCounts, 0x18);
WWHD_SIZE(agl::CompactCollection, 0x1C);
WWHD_OFFSET(agl::ExtendedCollection, mRecords, 0xC);
WWHD_OFFSET(agl::ExtendedCollection, mValueCounts, 0x18);
WWHD_SIZE(agl::ExtendedCollection, 0x1C);
WWHD_OFFSET(agl::SpatialCollection, mCapacity, 8);
WWHD_OFFSET(agl::SpatialCollection, mRecords, 0xC);
WWHD_OFFSET(agl::SpatialCollection, mCount, 0x10);
WWHD_OFFSET(agl::SpatialCollection, mValueCapacity, 0x14);
WWHD_OFFSET(agl::SpatialCollection, mValueCounts, 0x18);
WWHD_SIZE(agl::SpatialCollection, 0x1C);
WWHD_OFFSET(agl::CurveCollection, mCapacity, 8);
WWHD_OFFSET(agl::CurveCollection, mRecords, 0xC);
WWHD_OFFSET(agl::CurveCollection, mCount, 0x10);
WWHD_OFFSET(agl::CurveCollection, mValueCapacity, 0x14);
WWHD_OFFSET(agl::CurveCollection, mValueCounts, 0x18);
WWHD_SIZE(agl::CurveCollection, 0x1C);
