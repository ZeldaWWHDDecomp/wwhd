/* Initialization of the spatial and paired light collections (their `initialize` slot, called once by
 * agl::LightPrepass::configure): the record array and the per-view prepared counts come from the given heap (or the
 * current one); every record gets its values, is enabled and handed to the initializeRecord hook. The first value's
 * uniform buffer gets its format from the initializeValue hook; every later one copies that format.
 * References: WWHD's own instructions only. */
#include "agl/LightCollections.h"
using namespace agl;

// Inlined into both initializers: allocation from the given heap, or the current one.
static void* allocateLightStorage(sead::RegistryHeap* heap, u32 size) {
    if (heap == NULL) heap = registryCurrentHeap(sRegistryManager);
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, size, 4);
}

/* 027D1EE0 */
void agl::SpatialLightCollection::initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mOwner = owner;
    mCount = 0;
    gabi::call_ptr<void>(mTable->mBeforeAllocate.mFunction, this, heap);
    UniformBuffer* firstUniform = NULL;
    if (capacity > 0) {
        SpatialRecord* records = (SpatialRecord*)allocateLightStorage(heap, (u32)capacity * sizeof(SpatialRecord));
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
        be<u32>* counts = (be<u32>*)allocateLightStorage(heap, (u32)valueCapacity * 4);
        if (counts != NULL) {
            mValueCounts = counts;
            mValueCapacity = valueCapacity;
        }
    }
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = guestElement(begin, mCapacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        if (valueCapacity > 0) {
            ExtendedValue* values = (ExtendedValue*)allocateLightStorage(heap, (u32)valueCapacity * sizeof(ExtendedValue));
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
/* 027D2C98 */
void agl::PairedLightCollection::initialize(void* owner, s32 capacity, s32 valueCapacity, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mOwner = owner;
    mCount = 0;
    gabi::call_ptr<void>(mTable->mBeforeAllocate.mFunction, this, heap);
    UniformBuffer* firstUniform = NULL;
    if (capacity > 0) {
        CurveRecord* records = (CurveRecord*)allocateLightStorage(heap, (u32)capacity * sizeof(CurveRecord));
        for (u32 index = 0; index != (u32)capacity; ++index) {
            CurveRecord* record = guestElement(records, index);
            if (record == NULL) continue;
            record->mFlags = 0;
            record->mMask = 0;
            record->mValues = NULL;
            record->mValueCount = 0;
        }
        if (records != NULL) {
            mRecords = records;
            mCapacity = capacity;
        }
    }
    if (valueCapacity > 0) {
        be<u32>* counts = (be<u32>*)allocateLightStorage(heap, (u32)valueCapacity * 4);
        if (counts != NULL) {
            mValueCounts = counts;
            mValueCapacity = valueCapacity;
        }
    }
    CurveRecord* begin = mRecords;
    CurveRecord* end = guestElement(begin, mCapacity);
    for (CurveRecord* record = begin; record != end; ++record) {
        if (valueCapacity > 0) {
            CurveValue* values = (CurveValue*)allocateLightStorage(heap, (u32)valueCapacity * sizeof(CurveValue));
            for (u32 index = 0; index != (u32)valueCapacity; ++index) {
                CurveValue* value = guestElement(values, index);
                if (value == NULL) continue;
                value->mFlags = 0;
                UniformBuffer::create(&value->mUniform);
            }
            if (values != NULL) {
                record->mValues = values;
                record->mValueCount = valueCapacity;
            }
        }
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CurveRecord);
        gabi::call_ptr<void>(mTable->mSetRecordEnabled.mFunction, this, index, 1);
        gabi::call_ptr<void>(mTable->mInitializeRecord.mFunction, this, record, heap);
        CurveValue* values = record->mValues;
        CurveValue* valuesEnd = guestElement(values, record->mValueCount);
        for (CurveValue* value = values; value != valuesEnd; ++value) {
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
/* WWHD glue
ext 027BEB5C agl::UniformBuffer* agl::UniformBuffer::create(agl::UniformBuffer*)
ext 027BEE9C void agl::UniformBuffer::adoptFormat(const agl::UniformBuffer*)
ext 027BEEE4 void agl::UniformBuffer::allocateStorage(void*,u32)
ext 027BDF7C agl::TextureSampler* agl::TextureSampler::create(agl::TextureSampler*)
ext 027BE6B8 agl::TextureData* agl::TextureData::create(agl::TextureData*)
*/
