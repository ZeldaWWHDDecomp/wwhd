/* Release of the spatial and paired light collections (their `release` slot): every record goes through the
 * releaseRecord hook and gives its values' uniform buffers and value array back; then the records' own textures
 * (spatial) and the record array go back to the heap that owns them.
 * References: WWHD's own instructions only. */
#include "agl/LightCollections.h"
using namespace agl;

// The heap-registry heap that owns `storage`, and its release slot.
static void releaseLightStorage(void* storage, void* released) {
    sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, storage);
    gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, released);
}

/* 027D22A8 */
void agl::SpatialLightCollection::release() {
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
        releaseLightStorage(values, record->mValues);
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
        releaseLightStorage(records, mRecords);
        mCapacity = 0;
        mRecords = NULL;
    }
}
/* 027D2F9C */
void agl::PairedLightCollection::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    CurveRecord* begin = mRecords;
    CurveRecord* end = guestElement(begin, mCapacity);
    for (CurveRecord* record = begin; record != end; ++record) {
        gabi::call_ptr<void>(mTable->mReleaseRecord.mFunction, this, record);
        CurveValue* values = record->mValues;
        if (values == NULL) continue;
        s32 count = record->mValueCount;
        for (s32 index = 0; index < count; ++index) {
            CurveValue* value = guestElement(values, (u32)index);
            if (value != NULL) {
                UniformBuffer::destroy(&value->mUniform, 2);
                count = record->mValueCount;
                values = record->mValues;
            }
        }
        releaseLightStorage(values, record->mValues);
        record->mValueCount = 0;
        record->mValues = NULL;
    }
    CurveRecord* records = mRecords;
    if (records != NULL) {
        releaseLightStorage(records, mRecords);
        mCapacity = 0;
        mRecords = NULL;
    }
}
/* WWHD glue
ext 027BEC90 void agl::UniformBuffer::destroy(agl::UniformBuffer*,u32)
ext 027BE2B0 void agl::TextureSampler::destroy(agl::TextureSampler*,u32)
*/
