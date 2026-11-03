/* agl parameter collections of the light-prepass translation unit: the GPU cycle counter, the light-prepass
 * configuration, the TU's text classes, the curve descriptor constructor and the point (compact), directed (extended)
 * and spatial collections. The collection bodies walk their records through the method table, so derived
 * collections (AGL#18/#19) hook in per record and per view value.
 * References: WWHD's own instructions and data only. */
#include "agl/ParameterCollections.h"
#include "sead/FileDevice.h"
#include "sead/HeapRegistry.h"
void* allocateParameterStorage(u32 size);
void deleteParameterStorage(void* object);
void sampleTopGpuCycle(agl::GpuCycleSample* sample);
void sampleBottomGpuCycle(agl::GpuCycleSample* sample);
void invalidateCycleSample(void* sample, u32 size);
void buildLightResource(be<u32>* result, u32 kind, u32 width, u32 height, u32 depth, u32 format, u32 usage, u32 flags,
                        u32 options, u32 sampled, u32 cleared, u32 linear, u32 transient, u32 readable, u32 writable);
void releaseCollectionFacade_l(void* object);

static u32 sCurveDescriptorMethods;
static sead::SafeTextVtable sParameterTextMethods;
static u32 sCollectionBaseTypeMethods;
// Borrowed from AGL#14 (sCurveCollectionTypeMethods, not exported): every collection base type stores this table.
static u32 sCollectionTypeMethods_l;
// Runtime types of the collection bases, borrowed with no initializer: their owner is sead/RuntimeStatics.cpp once
// the other sharers merge (AGL#18 uses the point pair, AGL#19 the directed and spatial pairs).
static u32 sPointBaseGuard_l;
static sead::RuntimeType sPointBaseType_l;
static u32 sSelectionBaseGuard_l;
static sead::RuntimeType sSelectionBaseType_l;
static u32 sSpatialBaseGuard_l;
static sead::RuntimeType sSpatialBaseType_l;
// The record-collection facade the light prepass embeds (AGL#10's parameter object, borrowed method tables).
static u32 sFacadeDisposerMethods_l;
static u32 sFacadeObjectMethods_l;

/* ---- GPU cycle counter ---- */

/* 027AE698 */
void agl::GpuCycleClock::destroy(GpuCycleClock* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object != NULL && (flags & 1)) deleteParameterStorage(object);
}
/* 027AE6AC */
void agl::GpuCycleClock::synchronize() {}
/* 027AE6B0 */
const sead::SafeText* agl::GpuCycleTimer::name() const { return &sFileDeviceEmptyText; }
/* 027AE6BC */
const sead::SafeText* agl::GpuCycleTimer::description() const { return &sFileDeviceEmptyText; }
/* 027AE6C8 */
u32 agl::GpuCycleTimer::isCpuClock() const { return 0; }
/* 027AE6D0 */
u32 agl::GpuCycleTimer::isGpuClock() const { return 1; }
/* 027AE6D8 */
void agl::GpuCycleSample::sampleTop(GpuCycleSample* sample) {
    gabi::AutoFrame frame{gabi::current_function()};
    sampleTopGpuCycle(sample);
}
/* 027AE6DC */
void agl::GpuCycleSample::sampleBottom(GpuCycleSample* sample) {
    gabi::AutoFrame frame{gabi::current_function()};
    sampleBottomGpuCycle(sample);
}
/* 027AE6E0 */
Pair32 agl::GpuCycleSample::read() {
    gabi::AutoFrame frame{gabi::current_function()};
    // The GPU writes the sample: drop the cached line before reading it.
    invalidateCycleSample(this, sizeof(GpuCycleSample));
    u32 low = mLow;
    u32 high = mHigh;
    return {high, low};
}

/* ---- light-prepass configuration, texts, descriptors ---- */

/* 027B1938 */
u32 agl::createLightResource(u32 kind, u32 width, u32 height, u32 depth, u32 format, u32 usage, u32 flags, u32 options,
                             u8 sampled, u8 cleared, u8 linear, u8 transient, u8 readable, u8 writable) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> handle(0x24);
    *handle = 0;
    buildLightResource(handle, kind, width, height, depth, format, usage, flags, options, sampled, cleared, linear,
                       transient, readable, writable);
    return *handle;
}
/* 027B19CC */
agl::LightPrepassConfig* agl::LightPrepassConfig::create(LightPrepassConfig* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = (LightPrepassConfig*)allocateParameterStorage(sizeof(LightPrepassConfig));
    if (object != NULL) {
        object->mSpatialCount = 256;
        object->mExtraCount = 1;
        object->mPointCount = 256;
        object->mViewCount = 1;
        object->mDirectedCount = 256;
    }
    return object;
}
/* 027B1A1C */
void agl::ParameterText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) deleteParameterStorage(this);
}
/* 027B1A30 */
void agl::ParameterText::assureTermination() {}
/* 027B1A34 */
void agl::ParameterTextBuffer::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
    // The last byte of the buffer, by guest address arithmetic (the capacity is not checked).
    be<u8>* text = mText;
    u32 capacity = mCapacity;
    *gabi::at<be<u8>>(gabi::ea(text) + capacity - 1) = 0;
}
/* 027B1A4C */
void agl::ParameterTextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) deleteParameterStorage(this);
}
/* 027B1A60 */
void agl::ParameterFixedTextA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) deleteParameterStorage(this);
}
/* 027B1A74 */
void agl::ParameterFixedTextB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) deleteParameterStorage(this);
}
/* 027B1A88 */
void agl::ParameterFixedTextC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) deleteParameterStorage(this);
}
/* 027B1A9C */
agl::CurveDescriptor* agl::createCurveDescriptor(CurveDescriptor* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = (CurveDescriptor*)allocateParameterStorage(sizeof(CurveDescriptor));
    if (object != NULL) {
        object->mValues = NULL;
        object->mMetadata = gabi::ea(&sCurveDescriptorMethods);
        object->mCapacity = 0;
        object->mReserved = 4;
        object->mCount = 0;
        object->mKind = 0;
    }
    return object;
}
/* 027B1AF8 */
void agl::CollectionFacade_l::destroy(CollectionFacade_l* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) return;
    object->mDisposer.mVtable = (sead::DisposerVtable*)&sFacadeDisposerMethods_l;
    object->mObjectMethods = &sFacadeObjectMethods_l;
    releaseCollectionFacade_l(object);
    ((sead::RegisteredObject*)object)->destroy(0);
    if (flags & 1) deleteParameterStorage(object);
}

/* ---- point (compact) collection ---- */

/* 027B1B6C */
u32 agl::CompactCollection::matchesType(const sead::RuntimeType* type) const {
    if (sPointBaseGuard_l == 0) {
        sPointBaseGuard_l = 1;
        sPointBaseType_l.mTable = &sCollectionTypeMethods_l;
    }
    if (type == &sPointBaseType_l) return 1;
    if (sAglCommonBaseGuard == 0) {
        sAglCommonBaseGuard = 1;
        sAglCommonBaseType.mTable = &sCollectionBaseTypeMethods;
    }
    return type == &sAglCommonBaseType;
}
// inlined into the collections' initialize: allocation from the given heap, or the current one
static void* allocateCollectionStorage(sead::RegistryHeap* heap, u32 size) {
    if (heap == NULL) heap = registryCurrentHeap(sRegistryManager);
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, size, 4);
}
/* 027B1BE4 */
void agl::CompactCollection::initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mOwner = owner;
    mCount = 0;
    gabi::call_ptr<void>(mTable->mBeforeAllocate.mFunction, this, heap);
    UniformBuffer* firstUniform = NULL;
    if (capacity > 0) {
        CompactRecord* records = (CompactRecord*)allocateCollectionStorage(heap, (u32)capacity * sizeof(CompactRecord));
        for (u32 index = 0; index != (u32)capacity; ++index) {
            CompactRecord* record = guestElement(records, index);
            if (record == NULL) continue;
            record->mFlags = 0;
            record->mMask = 0;
            record->mFalloff[0] = 0.0f;
            record->mValueCount = 0;
            record->mColor[3] = 1.0f;
            record->mColor[0] = 0.0f;
            record->mValues = NULL;
            record->mFalloff[3] = 1.0f;
            record->mColor[1] = 0.0f;
            record->mFalloff[1] = 0.0f;
            record->mFalloff[2] = 0.0f;
            record->mColor[2] = 0.0f;
        }
        if (records != NULL) {
            mRecords = records;
            mCapacity = capacity;
        }
    }
    if (valueCapacity > 0) {
        be<u32>* counts = (be<u32>*)allocateCollectionStorage(heap, (u32)valueCapacity * 4);
        if (counts != NULL) {
            mValueCounts = counts;
            mValueCapacity = valueCapacity;
        }
    }
    CompactRecord* begin = mRecords;
    CompactRecord* end = guestElement(begin, mCapacity);
    for (CompactRecord* record = begin; record != end; ++record) {
        if (valueCapacity > 0) {
            CompactValue* values = (CompactValue*)allocateCollectionStorage(heap, (u32)valueCapacity * sizeof(CompactValue));
            for (u32 index = 0; index != (u32)valueCapacity; ++index) {
                CompactValue* value = guestElement(values, index);
                if (value == NULL) continue;
                value->mFlags = 0;
                UniformBuffer::create(&value->mUniform);
                value->mBlendColor[1] = 0.0f;
                value->mBlendColor[2] = 0.0f;
                value->mBlendColor[0] = 0.0f;
                value->mBlendColor[3] = 1.0f;
            }
            if (values != NULL) {
                record->mValues = values;
                record->mValueCount = valueCapacity;
            }
        }
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CompactRecord);
        gabi::call_ptr<void>(mTable->mSetRecordEnabled.mFunction, this, index, 1);
        gabi::call_ptr<void>(mTable->mInitializeRecord.mFunction, this, record, heap);
        // The first value's uniform buffer gets its format from the derived collection; the others copy it.
        CompactValue* values = record->mValues;
        CompactValue* valuesEnd = guestElement(values, record->mValueCount);
        for (CompactValue* value = values; value != valuesEnd; ++value) {
            UniformBuffer* uniform = &value->mUniform;
            if (firstUniform == NULL) {
                gabi::call_ptr<void>(mTable->mInitializeValue.mFunction, this, uniform, heap);
                firstUniform = uniform;
            } else {
                uniform->adoptFormat(firstUniform);
            }
            uniform->allocateStorage(heap, 2);
        }
    }
}
/* 027B1F8C */
void agl::CompactCollection::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    CompactRecord* begin = mRecords;
    CompactRecord* end = guestElement(begin, mCapacity);
    for (CompactRecord* record = begin; record != end; ++record) {
        gabi::call_ptr<void>(mTable->mReleaseRecord.mFunction, this, record);
        CompactValue* values = record->mValues;
        if (values == NULL) continue;
        s32 count = record->mValueCount;
        for (s32 index = 0; index < count; ++index) {
            CompactValue* value = guestElement(values, (u32)index);
            if (value != NULL) {
                UniformBuffer::destroy(&value->mUniform, 2);
                count = record->mValueCount;
                values = record->mValues;
            }
        }
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, values);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, (CompactValue*)record->mValues);
        record->mValueCount = 0;
        record->mValues = NULL;
    }
    CompactRecord* records = begin != end ? (CompactRecord*)mRecords : begin;
    if (records != NULL) {
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, records);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, (CompactRecord*)mRecords);
        mCapacity = 0;
        mRecords = NULL;
    }
}
/* 027B20AC */
void agl::CompactCollection::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeUpdate.mFunction, this);
    CompactRecord* begin = mRecords;
    u32 count = mCount;
    u32 capacity = mCapacity;
    CompactRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (CompactRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CompactRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mUpdateRecord.mFunction, this, record);
    }
}
// inlined into prepare: the view's slot of the uniform buffer ring becomes the upload target
static void selectUniformSlot(agl::UniformBuffer* uniform, u32 slot) {
    u32 stride = uniform->stride;
    uniform->cpuData.v = uniform->gpuData.v + stride * slot;
}
/* 027B21A4 */
u32 agl::CompactCollection::prepare(u32 valueIndex, void* context, u32 outputIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* prepared = clampedElement((be<u32>*)mValueCounts, mValueCapacity, valueIndex);
    *prepared = 0;
    u32 count = mCount;
    if (count == 0) return 0;
    u32 capacity = mCapacity;
    CompactRecord* begin = mRecords;
    CompactRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (CompactRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CompactRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        u32 ready = gabi::call_ptr<u32>(mTable->mPrepareValue.mFunction, this, record, valueIndex, context);
        CompactRecord* current = clampedElement((CompactRecord*)mRecords, mCapacity, index);
        CompactValue* value = clampedElement((CompactValue*)current->mValues, current->mValueCount, valueIndex);
        if ((u8)ready != 1) value->mFlags = value->mFlags | ValueFlag_Skip;
        else value->mFlags = value->mFlags & ~ValueFlag_Skip;
        if (ready != 0) {
            value = clampedElement((CompactValue*)record->mValues, record->mValueCount, valueIndex);
            selectUniformSlot(&value->mUniform, outputIndex);
            *prepared = *prepared + 1;
        }
    }
    return begin != end ? (u32)*prepared : 0;
}
/* 027B2360 */
void agl::CompactCollection::refresh() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeRefresh.mFunction, this);
    CompactRecord* begin = mRecords;
    u32 count = mCount;
    u32 capacity = mCapacity;
    CompactRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (CompactRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CompactRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mRefreshRecord.mFunction, this, record);
    }
}
/* 027B245C */
void agl::CompactCollection::apply(u32 valueIndex, void* context, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement((be<u32>*)mValueCounts, mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareApply.mFunction, this, valueIndex, context);
    u32 count = mCount;
    u32 capacity = mCapacity;
    CompactRecord* begin = mRecords;
    CompactRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (CompactRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CompactRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        CompactRecord* current = clampedElement((CompactRecord*)mRecords, mCapacity, index);
        CompactValue* value = clampedElement((CompactValue*)current->mValues, current->mValueCount, valueIndex);
        if (value->mFlags & ValueFlag_Skip) continue;
        gabi::call_ptr<void>(mTable->mBeginApply.mFunction, this, record, valueIndex, context);
        gabi::call_ptr<void>(mTable->mApplyValue.mFunction, this, record, valueIndex, context);
        value = clampedElement((CompactValue*)record->mValues, record->mValueCount, valueIndex);
        value->mUniform.flush(value->mUniform.cpuData, invalidateGpu);
    }
}
/* 027B2628 */
void agl::CompactCollection::applyWithContext(u32 valueIndex, void* context, void* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement((be<u32>*)mValueCounts, mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareContext.mFunction, this, valueIndex, context, output);
    u32 count = mCount;
    u32 capacity = mCapacity;
    CompactRecord* begin = mRecords;
    CompactRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (CompactRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CompactRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        CompactRecord* current = clampedElement((CompactRecord*)mRecords, mCapacity, index);
        CompactValue* value = clampedElement((CompactValue*)current->mValues, current->mValueCount, valueIndex);
        if ((value->mFlags & ValueFlag_Skip) == 0)
            gabi::call_ptr<void>(mTable->mApplyContext.mFunction, this, record, valueIndex, context, output);
    }
}
/* 027B27B8 */
s32 agl::CompactCollection::kind() const { return -1; }
/* 027B27C0 */
void agl::CompactCollection::setCount(u32 count) { mCount = count; }
/* 027B27C8 */
u32 agl::CompactCollection::count() const { return mCount; }
/* 027B27D0 */
u32 agl::CompactCollection::isEnabled(u32 index) const {
    return clampedElement((CompactRecord*)mRecords, mCapacity, index)->mFlags & Record_Enabled;
}
/* 027B27F4 */
void agl::CompactCollection::setRecordEnabled(u32 index, u32 enabled) {
    CompactRecord* record = clampedElement((CompactRecord*)mRecords, mCapacity, index);
    record->mMask = enabled != 0 ? 0xFFFFFFFF : 0;
}
// The PowerPC shift: a count of 32 or more shifts every bit out.
static u32 valueBit(u32 index) { return (index & 32) ? 0 : 1u << (index & 31); }
/* 027B2838 */
u32 agl::CompactCollection::canApply(u32 index, u32 valueIndex) const {
    CompactRecord* record = clampedElement((CompactRecord*)mRecords, mCapacity, index);
    return (record->mMask & valueBit(valueIndex)) != 0;
}
/* 027B286C */
void agl::CompactCollection::beforeAllocate() {}
/* 027B2870 */
void agl::CompactCollection::initializeRecord() {}
/* 027B2874 */
void agl::CompactCollection::initializeValue() {}
/* 027B2878 */
void agl::CompactCollection::releaseRecord() {}
/* 027B287C */
void agl::CompactCollection::beforeUpdate() {}
/* 027B2880 */
void agl::CompactCollection::updateRecord() {}
/* 027B2884 */
u32 agl::CompactCollection::prepareValue() const { return 0; }
/* 027B288C */
void agl::CompactCollection::beforeRefresh() {}
/* 027B2890 */
void agl::CompactCollection::refreshRecord() {}
/* 027B2894 */
void agl::CompactCollection::prepareApply() {}
/* 027B2898 */
void agl::CompactCollection::beginApply() {}
/* 027B289C */
void agl::CompactCollection::applyValue() {}
/* 027B28A0 */
void agl::CompactCollection::prepareContext() {}
/* 027B28A4 */
void agl::CompactCollection::applyContext() {}

/* ---- directed (extended) collection ---- */

/* 027B28A8 */
u32 agl::ExtendedCollection::matchesType(const sead::RuntimeType* type) const {
    if (sSelectionBaseGuard_l == 0) {
        sSelectionBaseGuard_l = 1;
        sSelectionBaseType_l.mTable = &sCollectionTypeMethods_l;
    }
    if (type == &sSelectionBaseType_l) return 1;
    if (sAglCommonBaseGuard == 0) {
        sAglCommonBaseGuard = 1;
        sAglCommonBaseType.mTable = &sCollectionBaseTypeMethods;
    }
    return type == &sAglCommonBaseType;
}
/* 027B2920 */
void agl::ExtendedCollection::initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mOwner = owner;
    mCount = 0;
    gabi::call_ptr<void>(mTable->mBeforeAllocate.mFunction, this, heap);
    UniformBuffer* firstUniform = NULL;
    if (capacity > 0) {
        ExtendedRecord* records = (ExtendedRecord*)allocateCollectionStorage(heap, (u32)capacity * sizeof(ExtendedRecord));
        for (u32 index = 0; index != (u32)capacity; ++index) {
            ExtendedRecord* record = guestElement(records, index);
            if (record == NULL) continue;
            record->mFlags = 0;
            record->mMask = 0;
            record->mFalloff[0] = 0.0f;
            record->mValueCount = 0;
            record->mColor[3] = 1.0f;
            record->mColor[0] = 0.0f;
            record->mValues = NULL;
            record->mFalloff[3] = 1.0f;
            record->mColor[1] = 0.0f;
            record->mFalloff[1] = 0.0f;
            record->mFalloff[2] = 0.0f;
            record->mColor[2] = 0.0f;
        }
        if (records != NULL) {
            mRecords = records;
            mCapacity = capacity;
        }
    }
    if (valueCapacity > 0) {
        be<u32>* counts = (be<u32>*)allocateCollectionStorage(heap, (u32)valueCapacity * 4);
        if (counts != NULL) {
            mValueCounts = counts;
            mValueCapacity = valueCapacity;
        }
    }
    ExtendedRecord* begin = mRecords;
    ExtendedRecord* end = guestElement(begin, mCapacity);
    for (ExtendedRecord* record = begin; record != end; ++record) {
        if (valueCapacity > 0) {
            ExtendedValue* values = (ExtendedValue*)allocateCollectionStorage(heap, (u32)valueCapacity * sizeof(ExtendedValue));
            for (u32 index = 0; index != (u32)valueCapacity; ++index) {
                ExtendedValue* value = guestElement(values, index);
                if (value == NULL) continue;
                value->mFlags = 0;
                UniformBuffer::create(&value->mUniform);
            }
            if (values != NULL) {
                record->mValues = values;
                record->mValueCount = valueCapacity;
            }
        }
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(ExtendedRecord);
        gabi::call_ptr<void>(mTable->mSetRecordEnabled.mFunction, this, index, 1);
        gabi::call_ptr<void>(mTable->mInitializeRecord.mFunction, this, record, heap);
        ExtendedValue* values = record->mValues;
        ExtendedValue* valuesEnd = guestElement(values, record->mValueCount);
        for (ExtendedValue* value = values; value != valuesEnd; ++value) {
            UniformBuffer* uniform = &value->mUniform;
            if (firstUniform == NULL) {
                gabi::call_ptr<void>(mTable->mInitializeValue.mFunction, this, uniform, heap);
                firstUniform = uniform;
            } else {
                uniform->adoptFormat(firstUniform);
            }
            uniform->allocateStorage(heap, 2);
        }
    }
}
/* 027B2C80 */
void agl::ExtendedCollection::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    ExtendedRecord* begin = mRecords;
    ExtendedRecord* end = guestElement(begin, mCapacity);
    for (ExtendedRecord* record = begin; record != end; ++record) {
        gabi::call_ptr<void>(mTable->mReleaseRecord.mFunction, this, record);
        ExtendedValue* values = record->mValues;
        if (values == NULL) continue;
        s32 count = record->mValueCount;
        for (s32 index = 0; index < count; ++index) {
            ExtendedValue* value = guestElement(values, (u32)index);
            if (value != NULL) {
                UniformBuffer::destroy(&value->mUniform, 2);
                count = record->mValueCount;
                values = record->mValues;
            }
        }
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, values);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, (ExtendedValue*)record->mValues);
        record->mValueCount = 0;
        record->mValues = NULL;
    }
    ExtendedRecord* records = begin != end ? (ExtendedRecord*)mRecords : begin;
    if (records != NULL) {
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, records);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, (ExtendedRecord*)mRecords);
        mCapacity = 0;
        mRecords = NULL;
    }
}
/* 027B2DA0 */
void agl::ExtendedCollection::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeUpdate.mFunction, this);
    ExtendedRecord* begin = mRecords;
    u32 count = mCount;
    u32 capacity = mCapacity;
    ExtendedRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (ExtendedRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(ExtendedRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mUpdateRecord.mFunction, this, record);
    }
}
/* 027B2E98 */
u32 agl::ExtendedCollection::prepare(u32 valueIndex, void* context, u32 outputIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* prepared = clampedElement((be<u32>*)mValueCounts, mValueCapacity, valueIndex);
    *prepared = 0;
    u32 count = mCount;
    if (count == 0) return 0;
    u32 capacity = mCapacity;
    ExtendedRecord* begin = mRecords;
    ExtendedRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (ExtendedRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(ExtendedRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        u32 ready = gabi::call_ptr<u32>(mTable->mPrepareValue.mFunction, this, record, valueIndex, context);
        ExtendedRecord* current = clampedElement((ExtendedRecord*)mRecords, mCapacity, index);
        ExtendedValue* value = clampedElement((ExtendedValue*)current->mValues, current->mValueCount, valueIndex);
        if ((u8)ready != 1) value->mFlags = value->mFlags | ValueFlag_Skip;
        else value->mFlags = value->mFlags & ~ValueFlag_Skip;
        if (ready != 0) {
            value = clampedElement((ExtendedValue*)record->mValues, record->mValueCount, valueIndex);
            selectUniformSlot(&value->mUniform, outputIndex);
            *prepared = *prepared + 1;
        }
    }
    return begin != end ? (u32)*prepared : 0;
}
/* 027B3054 */
void agl::ExtendedCollection::refresh() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeRefresh.mFunction, this);
    ExtendedRecord* begin = mRecords;
    u32 count = mCount;
    u32 capacity = mCapacity;
    ExtendedRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (ExtendedRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(ExtendedRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mRefreshRecord.mFunction, this, record);
    }
}
/* 027B3150 */
void agl::ExtendedCollection::apply(u32 valueIndex, void* context, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement((be<u32>*)mValueCounts, mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareApply.mFunction, this, valueIndex, context);
    u32 count = mCount;
    u32 capacity = mCapacity;
    ExtendedRecord* begin = mRecords;
    ExtendedRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (ExtendedRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(ExtendedRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        ExtendedRecord* current = clampedElement((ExtendedRecord*)mRecords, mCapacity, index);
        ExtendedValue* value = clampedElement((ExtendedValue*)current->mValues, current->mValueCount, valueIndex);
        if (value->mFlags & ValueFlag_Skip) continue;
        gabi::call_ptr<void>(mTable->mBeginApply.mFunction, this, record, valueIndex, context);
        gabi::call_ptr<void>(mTable->mApplyValue.mFunction, this, record, valueIndex, context);
        value = clampedElement((ExtendedValue*)record->mValues, record->mValueCount, valueIndex);
        value->mUniform.flush(value->mUniform.cpuData, invalidateGpu);
    }
}
/* 027B331C */
void agl::ExtendedCollection::applyWithContext(u32 valueIndex, void* context, void* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement((be<u32>*)mValueCounts, mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareContext.mFunction, this, valueIndex, context, output);
    u32 count = mCount;
    u32 capacity = mCapacity;
    ExtendedRecord* begin = mRecords;
    ExtendedRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (ExtendedRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(ExtendedRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        ExtendedRecord* current = clampedElement((ExtendedRecord*)mRecords, mCapacity, index);
        ExtendedValue* value = clampedElement((ExtendedValue*)current->mValues, current->mValueCount, valueIndex);
        if ((value->mFlags & ValueFlag_Skip) == 0)
            gabi::call_ptr<void>(mTable->mApplyContext.mFunction, this, record, valueIndex, context, output);
    }
}
/* 027B34AC */
s32 agl::ExtendedCollection::kind() const { return -1; }
/* 027B34B4 */
void agl::ExtendedCollection::setCount(u32 count) { mCount = count; }
/* 027B34BC */
u32 agl::ExtendedCollection::isEnabled(u32 index) const {
    return clampedElement((ExtendedRecord*)mRecords, mCapacity, index)->mFlags & Record_Enabled;
}
/* 027B34E0 */
void agl::ExtendedCollection::setRecordEnabled(u32 index, u32 enabled) {
    ExtendedRecord* record = clampedElement((ExtendedRecord*)mRecords, mCapacity, index);
    record->mMask = enabled != 0 ? 0xFFFFFFFF : 0;
}
/* 027B3524 */
u32 agl::ExtendedCollection::canApply(u32 index, u32 valueIndex) const {
    ExtendedRecord* record = clampedElement((ExtendedRecord*)mRecords, mCapacity, index);
    return (record->mMask & valueBit(valueIndex)) != 0;
}
/* 027B3558 */
void agl::ExtendedCollection::beforeAllocate() {}
/* 027B355C */
void agl::ExtendedCollection::initializeRecord() {}
/* 027B3560 */
void agl::ExtendedCollection::initializeValue() {}
/* 027B3564 */
void agl::ExtendedCollection::releaseRecord() {}
/* 027B3568 */
void agl::ExtendedCollection::beforeUpdate() {}
/* 027B356C */
void agl::ExtendedCollection::updateRecord() {}
/* 027B3570 */
u32 agl::ExtendedCollection::prepareValue() const { return 0; }
/* 027B3578 */
void agl::ExtendedCollection::beforeRefresh() {}
/* 027B357C */
void agl::ExtendedCollection::refreshRecord() {}
/* 027B3580 */
void agl::ExtendedCollection::prepareApply() {}
/* 027B3584 */
void agl::ExtendedCollection::beginApply() {}
/* 027B3588 */
void agl::ExtendedCollection::applyValue() {}
/* 027B358C */
void agl::ExtendedCollection::prepareContext() {}
/* 027B3590 */
void agl::ExtendedCollection::applyContext() {}

/* ---- spatial collection ---- */

/* 027B3594 */
u32 agl::SpatialCollection::matchesType(const sead::RuntimeType* type) const {
    if (sSpatialBaseGuard_l == 0) {
        sSpatialBaseGuard_l = 1;
        sSpatialBaseType_l.mTable = &sCollectionTypeMethods_l;
    }
    if (type == &sSpatialBaseType_l) return 1;
    if (sAglCommonBaseGuard == 0) {
        sAglCommonBaseGuard = 1;
        sAglCommonBaseType.mTable = &sCollectionBaseTypeMethods;
    }
    return type == &sAglCommonBaseType;
}
/* 027B360C */
void agl::SpatialCollection::initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mOwner = owner;
    mCount = 0;
    gabi::call_ptr<void>(mTable->mBeforeAllocate.mFunction, this, heap);
    UniformBuffer* firstUniform = NULL;
    if (capacity > 0) {
        SpatialRecord* records = (SpatialRecord*)allocateCollectionStorage(heap, (u32)capacity * sizeof(SpatialRecord));
        for (u32 index = 0; index != (u32)capacity; ++index) {
            SpatialRecord* record = guestElement(records, index);
            if (record == NULL) continue;
            record->mFlags = 0;
            record->mMask = 0;
            record->mFalloff[3] = 1.0f;
            record->mColor[3] = 1.0f;
            record->mFalloff[1] = 0.0f;
            record->mColor[0] = 0.0f;
            record->mColor[1] = 0.0f;
            record->mColor[2] = 0.0f;
            record->mFalloff[2] = 0.0f;
            record->mFalloff[0] = 0.0f;
            TextureSampler::create(&record->mPrimaryTexture);
            record->mMaximum[0] = 3.4028234663852886e38f;
            record->mValues = NULL;
            record->mMinimum[0] = -3.4028234663852886e38f;
            record->mMinimum[1] = -3.4028234663852886e38f;
            record->mMinimum[2] = -3.4028234663852886e38f;
            record->mValueCount = 0;
            record->mMaximum[1] = 3.4028234663852886e38f;
            record->mMaximum[2] = 3.4028234663852886e38f;
            TextureData::create(&record->mProjectionTexture);
            TextureSampler::create(&record->mFollowingTexture);
        }
        if (records != NULL) {
            mRecords = records;
            mCapacity = capacity;
        }
    }
    if (valueCapacity > 0) {
        be<u32>* counts = (be<u32>*)allocateCollectionStorage(heap, (u32)valueCapacity * 4);
        if (counts != NULL) {
            mValueCounts = counts;
            mValueCapacity = valueCapacity;
        }
    }
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = guestElement(begin, mCapacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        if (valueCapacity > 0) {
            ExtendedValue* values = (ExtendedValue*)allocateCollectionStorage(heap, (u32)valueCapacity * sizeof(ExtendedValue));
            for (u32 index = 0; index != (u32)valueCapacity; ++index) {
                ExtendedValue* value = guestElement(values, index);
                if (value == NULL) continue;
                value->mFlags = 0;
                UniformBuffer::create(&value->mUniform);
            }
            if (values != NULL) {
                record->mValues = values;
                record->mValueCount = valueCapacity;
            }
        }
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        gabi::call_ptr<void>(mTable->mSetRecordEnabled.mFunction, this, index, 1);
        gabi::call_ptr<void>(mTable->mInitializeRecord.mFunction, this, record, heap);
        ExtendedValue* values = record->mValues;
        ExtendedValue* valuesEnd = guestElement(values, record->mValueCount);
        for (ExtendedValue* value = values; value != valuesEnd; ++value) {
            UniformBuffer* uniform = &value->mUniform;
            if (firstUniform == NULL) {
                gabi::call_ptr<void>(mTable->mInitializeValue.mFunction, this, uniform, heap);
                firstUniform = uniform;
            } else {
                uniform->adoptFormat(firstUniform);
            }
            uniform->allocateStorage(heap, 2);
        }
    }
}
/* 027B39D4 */
void agl::SpatialCollection::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = guestElement(begin, mCapacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        gabi::call_ptr<void>(mTable->mReleaseRecord.mFunction, this, record);
        ExtendedValue* values = record->mValues;
        if (values == NULL) continue;
        s32 count = record->mValueCount;
        for (s32 index = 0; index < count; ++index) {
            ExtendedValue* value = guestElement(values, (u32)index);
            if (value != NULL) {
                UniformBuffer::destroy(&value->mUniform, 2);
                count = record->mValueCount;
                values = record->mValues;
            }
        }
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, values);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, (ExtendedValue*)record->mValues);
        record->mValueCount = 0;
        record->mValues = NULL;
    }
    SpatialRecord* records = begin != end ? (SpatialRecord*)mRecords : begin;
    if (records != NULL) {
        // The records' own textures are destroyed in reverse member order before the array goes.
        s32 count = mCapacity;
        for (s32 index = 0; index < count; ++index) {
            SpatialRecord* record = guestElement(records, (u32)index);
            if (record != NULL) {
                TextureSampler::destroy(&record->mFollowingTexture, 2);
                TextureSampler::destroy(&record->mPrimaryTexture, 2);
                count = mCapacity;
                records = mRecords;
            }
        }
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, records);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, (SpatialRecord*)mRecords);
        mCapacity = 0;
        mRecords = NULL;
    }
}
/* 027B3B38 */
void agl::SpatialCollection::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeUpdate.mFunction, this);
    SpatialRecord* begin = mRecords;
    u32 count = mCount;
    u32 capacity = mCapacity;
    SpatialRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mUpdateRecord.mFunction, this, record);
    }
}
/* 027B3C30 */
u32 agl::SpatialCollection::prepare(u32 valueIndex, void* context, u32 outputIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* prepared = clampedElement((be<u32>*)mValueCounts, mValueCapacity, valueIndex);
    *prepared = 0;
    u32 count = mCount;
    if (count == 0) return 0;
    u32 capacity = mCapacity;
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        u32 ready = gabi::call_ptr<u32>(mTable->mPrepareValue.mFunction, this, record, valueIndex, context);
        SpatialRecord* current = clampedElement((SpatialRecord*)mRecords, mCapacity, index);
        ExtendedValue* value = clampedElement((ExtendedValue*)current->mValues, current->mValueCount, valueIndex);
        if ((u8)ready != 1) value->mFlags = value->mFlags | ValueFlag_Skip;
        else value->mFlags = value->mFlags & ~ValueFlag_Skip;
        if (ready != 0) {
            value = clampedElement((ExtendedValue*)record->mValues, record->mValueCount, valueIndex);
            selectUniformSlot(&value->mUniform, outputIndex);
            *prepared = *prepared + 1;
        }
    }
    return begin != end ? (u32)*prepared : 0;
}
/* 027B3DEC */
void agl::SpatialCollection::refresh() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeRefresh.mFunction, this);
    SpatialRecord* begin = mRecords;
    u32 count = mCount;
    u32 capacity = mCapacity;
    SpatialRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mRefreshRecord.mFunction, this, record);
    }
}
/* 027B3EE8 */
void agl::SpatialCollection::apply(u32 valueIndex, void* context, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement((be<u32>*)mValueCounts, mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareApply.mFunction, this, valueIndex, context);
    u32 count = mCount;
    u32 capacity = mCapacity;
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = guestElement(begin, count <= capacity ? count : capacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        SpatialRecord* current = clampedElement((SpatialRecord*)mRecords, mCapacity, index);
        ExtendedValue* value = clampedElement((ExtendedValue*)current->mValues, current->mValueCount, valueIndex);
        if (value->mFlags & ValueFlag_Skip) continue;
        gabi::call_ptr<void>(mTable->mBeginApply.mFunction, this, record, valueIndex, context);
        gabi::call_ptr<void>(mTable->mApplyValue.mFunction, this, record, valueIndex, context);
        value = clampedElement((ExtendedValue*)record->mValues, record->mValueCount, valueIndex);
        value->mUniform.flush(value->mUniform.cpuData, invalidateGpu);
    }
}
/* WWHD glue
ext 0273AD10 void* allocateParameterStorage(u32)
ext 0273AF40 void deleteParameterStorage(void*)
ext C0006760 void sampleTopGpuCycle(agl::GpuCycleSample*)
ext C0006748 void sampleBottomGpuCycle(agl::GpuCycleSample*)
ext C00088A8 void invalidateCycleSample(void*,u32)
ext 027CBDB0 void buildLightResource(be<u32>*,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32)
ext 027A0998 void releaseCollectionFacade_l(void*)
ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*,void*)
ext 027BEB5C agl::UniformBuffer* agl::UniformBuffer::create(agl::UniformBuffer*)
ext 027BEC90 void agl::UniformBuffer::destroy(agl::UniformBuffer*,u32)
ext 027BEE9C void agl::UniformBuffer::adoptFormat(const agl::UniformBuffer*)
ext 027BEEE4 void agl::UniformBuffer::allocateStorage(void*,u32)
ext 027BEF80 void agl::UniformBuffer::flush(void*,u32)
ext 027BDF7C agl::TextureSampler* agl::TextureSampler::create(agl::TextureSampler*)
ext 027BE2B0 void agl::TextureSampler::destroy(agl::TextureSampler*,u32)
ext 027BE6B8 agl::TextureData* agl::TextureData::create(agl::TextureData*)
data sCurveDescriptorMethods 10168EAC
data sParameterTextMethods 10168EC4
data sCollectionBaseTypeMethods 10168F14
data sCollectionTypeMethods_l 10168FCC
data sFacadeDisposerMethods_l 10168F54
data sFacadeObjectMethods_l 10168F64
data sPointBaseGuard_l 101FD8D4
data sPointBaseType_l 101FDFF0
data sSelectionBaseGuard_l 101FD8D8
data sSelectionBaseType_l 101FDFF4
data sSpatialBaseGuard_l 101FD8DC
data sSpatialBaseType_l 101FDFF8
*/
