/* The record walks of the spatial and paired light collections: update, refresh, prepare (per view value),
 * apply (upload) and applyWithContext (draw). Each walks the first min(count, capacity) records and skips the ones
 * whose isEnabled hook says no; prepare marks each value skipped or not and points the value's uniform buffer at the
 * output slot, apply and applyWithContext only visit values prepare did not skip.
 * References: WWHD's own instructions only. */
#include "agl/LightCollections.h"
using namespace agl;

// The records a walk visits: the first min(count, capacity) records.
template <class Record> static Record* walkEnd(Record* begin, u32 count, u32 capacity) {
    return guestElement(begin, count <= capacity ? count : capacity);
}
// Inlined into prepare: the output slot of the value's uniform-buffer ring becomes the upload target.
static void selectUniformSlot(UniformBuffer* uniform, u32 slot) {
    u32 stride = uniform->stride;
    uniform->cpuData.v = uniform->gpuData.v + stride * slot;
}

/* ---- spatial lights ---- */

/* 027D240C */
void agl::SpatialLightCollection::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeUpdate.mFunction, this);
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = walkEnd(begin, mCount, mCapacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mUpdateRecord.mFunction, this, record);
    }
}
/* 027D26C0 */
void agl::SpatialLightCollection::refresh() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeRefresh.mFunction, this);
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = walkEnd(begin, mCount, mCapacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mRefreshRecord.mFunction, this, record);
    }
}
/* 027D2504 */
u32 agl::SpatialLightCollection::prepare(u32 valueIndex, void* context, u32 outputIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* prepared = clampedElement(mValueCounts.get(), mValueCapacity, valueIndex);
    *prepared = 0;
    u32 count = mCount;
    if (count == 0) return 0;
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = walkEnd(begin, count, mCapacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        u32 ready = gabi::call_ptr<u32>(mTable->mPrepareValue.mFunction, this, record, valueIndex, context);
        SpatialRecord* current = clampedElement(mRecords.get(), mCapacity, index);
        ExtendedValue* value = clampedElement(current->mValues.get(), current->mValueCount, valueIndex);
        if ((u8)ready != 1) value->mFlags = value->mFlags | ValueFlag_Skip;
        else value->mFlags = value->mFlags & ~ValueFlag_Skip;
        if (ready != 0) {
            value = clampedElement(record->mValues.get(), record->mValueCount, valueIndex);
            selectUniformSlot(&value->mUniform, outputIndex);
            *prepared = *prepared + 1;
        }
    }
    return begin != end ? (u32)*prepared : 0;
}
/* 027D27BC */
void agl::SpatialLightCollection::apply(u32 valueIndex, void* context, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement(mValueCounts.get(), mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareApply.mFunction, this, valueIndex, context);
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = walkEnd(begin, mCount, mCapacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        SpatialRecord* current = clampedElement(mRecords.get(), mCapacity, index);
        ExtendedValue* value = clampedElement(current->mValues.get(), current->mValueCount, valueIndex);
        if (value->mFlags & ValueFlag_Skip) continue;
        gabi::call_ptr<void>(mTable->mBeginApply.mFunction, this, record, valueIndex, context);
        gabi::call_ptr<void>(mTable->mApplyValue.mFunction, this, record, valueIndex, context);
        value = clampedElement(record->mValues.get(), record->mValueCount, valueIndex);
        value->mUniform.flush(value->mUniform.cpuData, invalidateGpu);
    }
}
/* 027D2988 */
void agl::SpatialLightCollection::applyWithContext(u32 valueIndex, void* context, void* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement(mValueCounts.get(), mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareContext.mFunction, this, valueIndex, context, output);
    SpatialRecord* begin = mRecords;
    SpatialRecord* end = walkEnd(begin, mCount, mCapacity);
    for (SpatialRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        SpatialRecord* current = clampedElement(mRecords.get(), mCapacity, index);
        ExtendedValue* value = clampedElement(current->mValues.get(), current->mValueCount, valueIndex);
        if ((value->mFlags & ValueFlag_Skip) == 0)
            gabi::call_ptr<void>(mTable->mApplyContext.mFunction, this, record, valueIndex, context, output);
    }
}

/* ---- paired point lights ---- */

/* 027D30BC */
void agl::PairedLightCollection::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeUpdate.mFunction, this);
    CurveRecord* begin = mRecords;
    CurveRecord* end = walkEnd(begin, mCount, mCapacity);
    for (CurveRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CurveRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mUpdateRecord.mFunction, this, record);
    }
}
/* 027D3370 */
void agl::PairedLightCollection::refresh() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    gabi::call_ptr<void>(mTable->mBeforeRefresh.mFunction, this);
    CurveRecord* begin = mRecords;
    CurveRecord* end = walkEnd(begin, mCount, mCapacity);
    for (CurveRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CurveRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) != 0)
            gabi::call_ptr<void>(mTable->mRefreshRecord.mFunction, this, record);
    }
}
/* 027D31B4 */
u32 agl::PairedLightCollection::prepare(u32 valueIndex, void* context, u32 outputIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u32>* prepared = clampedElement(mValueCounts.get(), mValueCapacity, valueIndex);
    *prepared = 0;
    u32 count = mCount;
    if (count == 0) return 0;
    CurveRecord* begin = mRecords;
    CurveRecord* end = walkEnd(begin, count, mCapacity);
    for (CurveRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CurveRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        u32 ready = gabi::call_ptr<u32>(mTable->mPrepareValue.mFunction, this, record, valueIndex, context);
        CurveRecord* current = clampedElement(mRecords.get(), mCapacity, index);
        CurveValue* value = clampedElement(current->mValues.get(), current->mValueCount, valueIndex);
        if ((u8)ready != 1) value->mFlags = value->mFlags | ValueFlag_Skip;
        else value->mFlags = value->mFlags & ~ValueFlag_Skip;
        if (ready != 0) {
            value = clampedElement(record->mValues.get(), record->mValueCount, valueIndex);
            selectUniformSlot(&value->mUniform, outputIndex);
            *prepared = *prepared + 1;
        }
    }
    return begin != end ? (u32)*prepared : 0;
}
/* 027D346C */
void agl::PairedLightCollection::apply(u32 valueIndex, void* context, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement(mValueCounts.get(), mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareApply.mFunction, this, valueIndex, context);
    CurveRecord* begin = mRecords;
    CurveRecord* end = walkEnd(begin, mCount, mCapacity);
    for (CurveRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CurveRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        CurveRecord* current = clampedElement(mRecords.get(), mCapacity, index);
        CurveValue* value = clampedElement(current->mValues.get(), current->mValueCount, valueIndex);
        if (value->mFlags & ValueFlag_Skip) continue;
        gabi::call_ptr<void>(mTable->mBeginApply.mFunction, this, record, valueIndex, context);
        gabi::call_ptr<void>(mTable->mApplyValue.mFunction, this, record, valueIndex, context);
        value = clampedElement(record->mValues.get(), record->mValueCount, valueIndex);
        value->mUniform.flush(value->mUniform.cpuData, invalidateGpu);
    }
}
/* 027D3638 */
void agl::PairedLightCollection::applyWithContext(u32 valueIndex, void* context, void* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0) return;
    if (*clampedElement(mValueCounts.get(), mValueCapacity, valueIndex) == 0) return;
    gabi::call_ptr<void>(mTable->mPrepareContext.mFunction, this, valueIndex, context, output);
    CurveRecord* begin = mRecords;
    CurveRecord* end = walkEnd(begin, mCount, mCapacity);
    for (CurveRecord* record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(CurveRecord);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index) == 0) continue;
        if (gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, valueIndex) == 0) continue;
        CurveRecord* current = clampedElement(mRecords.get(), mCapacity, index);
        CurveValue* value = clampedElement(current->mValues.get(), current->mValueCount, valueIndex);
        if ((value->mFlags & ValueFlag_Skip) == 0)
            gabi::call_ptr<void>(mTable->mApplyContext.mFunction, this, record, valueIndex, context, output);
    }
}
/* WWHD glue
ext 027BEF80 void agl::UniformBuffer::flush(void*,u32)
*/
