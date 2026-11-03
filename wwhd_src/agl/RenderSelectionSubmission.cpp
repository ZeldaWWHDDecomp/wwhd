// Upload and submit owned collection slots with the original post-callback lookups.
#include "agl/RenderSelection.h"

void flushSelectionBuffer(agl::UniformBuffer* buffer, void* data, u32 invalidate);

namespace agl {
/* 027D0CEC */
void agl::RenderSelection_l::uploadSlot(u32 slot, void* context, u32 invalidate) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mActiveCount) == 0)
        return;
    u32 slotCount = mSlotCount;
    be<u32>* completed = mSlots;
    if (slot < slotCount)
        completed += slot;
    if (u32(*completed) == 0)
        return;
    gabi::call_ptr<void>(mMethods->mBeginUpload.mFunction, this, slot, context);
    u32 activeCount = mActiveCount;
    u32 capacity = mRecordCount;
    RenderSelectionRecord_l* records = mRecords;
    u32 count = activeCount <= capacity ? activeCount : u32(mRecordCount);
    RenderSelectionRecord_l* end = sead::elementPtr(records, count);  // 32-bit end and steps, as the original's `cmplw p,end`
    gabi::FrameLocal<gptr<RenderSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    for (RenderSelectionRecord_l* record = records; record != end; record = sead::nextElement(record)) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(RenderSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (!enabled)
            continue;
        u32 selected = gabi::call_ptr<u32>(mMethods->mTestRecordMask.mFunction, this, index, slot);
        if (!selected)
            continue;
        u32 currentCapacity = mRecordCount;
        RenderSelectionRecord_l* currentRecord = mRecords;
        if (index < currentCapacity)
            currentRecord += index;
        u32 valueCount = currentRecord->mValueCount;
        SelectionValue_l* currentValue = currentRecord->mValues;
        if (slot < valueCount)
            currentValue += slot;
        if (u32(currentValue->mFlags) & 1)
            continue;
        gabi::call_ptr<void>(mMethods->mPrepareUpload.mFunction, this, record, slot, context);
        gabi::call_ptr<void>(mMethods->mWriteUpload.mFunction, this, record, slot, context);
        u32 originalValueCount = record->mValueCount;
        SelectionValue_l* value = record->mValues;
        if (slot < originalValueCount)
            value += slot;
        void* data = value->mBuffer.cpuData;
        flushSelectionBuffer(&value->mBuffer, data, invalidate);
    }
}

/* 027D0EB8 */
void agl::RenderSelection_l::submitSlot(u32 slot, void* context, void* argument) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mActiveCount) == 0)
        return;
    u32 slotCount = mSlotCount;
    be<u32>* completed = mSlots;
    if (slot < slotCount)
        completed += slot;
    if (u32(*completed) == 0)
        return;
    gabi::call_ptr<void>(mMethods->mBeginSubmit.mFunction, this, slot, context, argument);
    u32 activeCount = mActiveCount;
    u32 capacity = mRecordCount;
    RenderSelectionRecord_l* records = mRecords;
    u32 count = activeCount <= capacity ? activeCount : u32(mRecordCount);
    RenderSelectionRecord_l* end = sead::elementPtr(records, count);  // 32-bit end and steps, as the original's `cmplw p,end`
    gabi::FrameLocal<gptr<RenderSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    for (RenderSelectionRecord_l* record = records; record != end; record = sead::nextElement(record)) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(RenderSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (!enabled)
            continue;
        u32 selected = gabi::call_ptr<u32>(mMethods->mTestRecordMask.mFunction, this, index, slot);
        if (!selected)
            continue;
        u32 currentCapacity = mRecordCount;
        RenderSelectionRecord_l* currentRecord = mRecords;
        if (index < currentCapacity)
            currentRecord += index;
        u32 valueCount = currentRecord->mValueCount;
        SelectionValue_l* currentValue = currentRecord->mValues;
        if (slot < valueCount)
            currentValue += slot;
        if (u32(currentValue->mFlags) & 1)
            continue;
        gabi::call_ptr<void>(mMethods->mSubmitRecord.mFunction, this, record, slot, context, argument);
    }
}

/* 027D1A04 */
void agl::ExtendedRenderSelection_l::uploadSlot(u32 slot, void* context, u32 invalidate) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mActiveCount) == 0)
        return;
    u32 slotCount = mSlotCount;
    be<u32>* completed = mSlots;
    if (slot < slotCount)
        completed += slot;
    if (u32(*completed) == 0)
        return;
    gabi::call_ptr<void>(mMethods->mBeginUpload.mFunction, this, slot, context);
    u32 activeCount = mActiveCount;
    u32 capacity = mRecordCount;
    ExtendedSelectionRecord_l* records = mRecords;
    u32 count = activeCount <= capacity ? activeCount : u32(mRecordCount);
    ExtendedSelectionRecord_l* end = records + count;
    gabi::FrameLocal<gptr<ExtendedSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    for (ExtendedSelectionRecord_l* record = records; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(ExtendedSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (!enabled)
            continue;
        u32 selected = gabi::call_ptr<u32>(mMethods->mTestRecordMask.mFunction, this, index, slot);
        if (!selected)
            continue;
        u32 currentCapacity = mRecordCount;
        ExtendedSelectionRecord_l* currentRecord = mRecords;
        if (index < currentCapacity)
            currentRecord += index;
        u32 valueCount = currentRecord->mValueCount;
        ExtendedSelectionValue_l* currentValue = currentRecord->mValues;
        if (slot < valueCount)
            currentValue += slot;
        if (u32(currentValue->mFlags) & 1)
            continue;
        gabi::call_ptr<void>(mMethods->mPrepareUpload.mFunction, this, record, slot, context);
        gabi::call_ptr<void>(mMethods->mWriteUpload.mFunction, this, record, slot, context);
        u32 originalValueCount = record->mValueCount;
        ExtendedSelectionValue_l* value = record->mValues;
        if (slot < originalValueCount)
            value += slot;
        void* data = value->mBuffer.cpuData;
        flushSelectionBuffer(&value->mBuffer, data, invalidate);
    }
}

/* 027D1BD0 */
void agl::ExtendedRenderSelection_l::submitSlot(u32 slot, void* context, void* argument) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mActiveCount) == 0)
        return;
    u32 slotCount = mSlotCount;
    be<u32>* completed = mSlots;
    if (slot < slotCount)
        completed += slot;
    if (u32(*completed) == 0)
        return;
    gabi::call_ptr<void>(mMethods->mBeginSubmit.mFunction, this, slot, context, argument);
    u32 activeCount = mActiveCount;
    u32 capacity = mRecordCount;
    ExtendedSelectionRecord_l* records = mRecords;
    u32 count = activeCount <= capacity ? activeCount : u32(mRecordCount);
    ExtendedSelectionRecord_l* end = records + count;
    gabi::FrameLocal<gptr<ExtendedSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    for (ExtendedSelectionRecord_l* record = records; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(ExtendedSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (!enabled)
            continue;
        u32 selected = gabi::call_ptr<u32>(mMethods->mTestRecordMask.mFunction, this, index, slot);
        if (!selected)
            continue;
        u32 currentCapacity = mRecordCount;
        ExtendedSelectionRecord_l* currentRecord = mRecords;
        if (index < currentCapacity)
            currentRecord += index;
        u32 valueCount = currentRecord->mValueCount;
        ExtendedSelectionValue_l* currentValue = currentRecord->mValues;
        if (slot < valueCount)
            currentValue += slot;
        if (u32(currentValue->mFlags) & 1)
            continue;
        gabi::call_ptr<void>(mMethods->mSubmitRecord.mFunction, this, record, slot, context, argument);
    }
}

}

/* WWHD glue
ext 027BEF80 void flushSelectionBuffer(agl::UniformBuffer* buffer, void* data, u32 invalidate)
*/
