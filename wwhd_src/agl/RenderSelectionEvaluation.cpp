// Evaluate active record values, retaining post-callback reloads and original fallback indexing.
#include "agl/RenderSelection.h"

namespace agl {
/* 027D0A34 */
u32 agl::RenderSelection_l::evaluateSlot(u32 slot, void* context, u32 outputIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 slotCount = mSlotCount;
    be<u32>* completed = mSlots;
    if (slot < slotCount)
        completed += slot;
    *completed = 0;
    u32 activeCount = mActiveCount;
    if (activeCount == 0)
        return 0;
    u32 capacity = mRecordCount;
    RenderSelectionRecord_l* records = mRecords;
    u32 count = activeCount <= capacity ? activeCount : capacity;
    RenderSelectionRecord_l* end = sead::elementPtr(records, count);  // 32-bit end and steps, as the original's `cmplw p,end`
    gabi::FrameLocal<gptr<RenderSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    if (records == end)
        return 0;
    for (RenderSelectionRecord_l* record = records; record != end; record = sead::nextElement(record)) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(RenderSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (enabled) {
            u32 accepted = gabi::call_ptr<u32>(mMethods->mEvaluateRecord.mFunction,
                                               this, record, slot, context);
            u32 currentCapacity = mRecordCount;
            RenderSelectionRecord_l* currentRecord = mRecords;
            if (index < currentCapacity)
                currentRecord += index;
            u32 valueCount = currentRecord->mValueCount;
            SelectionValue_l* currentValue = currentRecord->mValues;
            if (slot < valueCount)
                currentValue += slot;
            u8 rejected = u8(accepted ^ 1);
            u32 flags = currentValue->mFlags;
            currentValue->mFlags = rejected ? (flags | 1) : (flags & ~1u);
            if (accepted) {
                u32 originalValueCount = record->mValueCount;
                SelectionValue_l* value = record->mValues;
                if (slot < originalValueCount)
                    value += slot;
                u32 stride = value->mBuffer.stride;
                u32 offset = stride * outputIndex;
                void* base = value->mBuffer.gpuData;
                value->mBuffer.cpuData = gabi::at<void>(gabi::ea(base) + offset);
                u32 total = *completed;
                *completed = total + 1;
            }
        }
    }
    return *completed;
}

/* 027D174C */
u32 agl::ExtendedRenderSelection_l::evaluateSlot(u32 slot, void* context, u32 outputIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 slotCount = mSlotCount;
    be<u32>* completed = mSlots;
    if (slot < slotCount)
        completed += slot;
    *completed = 0;
    u32 activeCount = mActiveCount;
    if (activeCount == 0)
        return 0;
    u32 capacity = mRecordCount;
    ExtendedSelectionRecord_l* records = mRecords;
    u32 count = activeCount <= capacity ? activeCount : capacity;
    ExtendedSelectionRecord_l* end = sead::elementPtr(records, count);  // 32-bit end and steps, as the original's `cmplw p,end`
    gabi::FrameLocal<gptr<ExtendedSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    if (records == end)
        return 0;
    for (ExtendedSelectionRecord_l* record = records; record != end; record = sead::nextElement(record)) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(ExtendedSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (enabled) {
            u32 accepted = gabi::call_ptr<u32>(mMethods->mEvaluateRecord.mFunction,
                                               this, record, slot, context);
            u32 currentCapacity = mRecordCount;
            ExtendedSelectionRecord_l* currentRecord = mRecords;
            if (index < currentCapacity)
                currentRecord += index;
            u32 valueCount = currentRecord->mValueCount;
            ExtendedSelectionValue_l* currentValue = currentRecord->mValues;
            if (slot < valueCount)
                currentValue += slot;
            u8 rejected = u8(accepted ^ 1);
            u32 flags = currentValue->mFlags;
            currentValue->mFlags = rejected ? (flags | 1) : (flags & ~1u);
            if (accepted) {
                u32 originalValueCount = record->mValueCount;
                ExtendedSelectionValue_l* value = record->mValues;
                if (slot < originalValueCount)
                    value += slot;
                u32 stride = value->mBuffer.stride;
                u32 offset = stride * outputIndex;
                void* base = value->mBuffer.gpuData;
                value->mBuffer.cpuData = gabi::at<void>(gabi::ea(base) + offset);
                u32 total = *completed;
                *completed = total + 1;
            }
        }
    }
    return *completed;
}

}

/* WWHD glue
*/
