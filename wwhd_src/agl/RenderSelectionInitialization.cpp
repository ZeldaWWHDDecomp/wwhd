// Owned selection allocation and initialization; AGL#15 buffer helpers remain opaque.
#include "agl/RenderSelection.h"
void* allocateSelectionScalar(u32 bytes);
void constructSelectionBuffer(agl::UniformBuffer* buffer);
void allocateSelectionBuffer(agl::UniformBuffer* buffer, sead::AllocationHeap* heap, u32 count);
void copySelectionBuffer(agl::UniformBuffer* target, const agl::UniformBuffer* source);
namespace agl {
static void* allocateSelectionStorage(sead::AllocationHeap* heap, u32 bytes) {
    if (gabi::ea(heap) == 0) {
        sead::RegistryManagerState* manager = sRegistryManager;
        heap = currentAllocationHeap(gabi::at<sead::AllocationManager>(gabi::ea(manager)));
    }
    u32 function = heap->mTable->mAllocate;
    return gabi::call_ptr<void*>(function, heap, bytes, s32(4));
}
/* 027D0474 */
void agl::RenderSelection_l::initialize(u32 state, s32 recordCount, s32 slotCount, sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mState = state;
    mActiveCount = 0;
    gabi::call_ptr<void>(mMethods->mPrepareHeap.mFunction, this, heap);
    if (recordCount > 0) {
        RenderSelectionRecord_l* storage = static_cast<RenderSelectionRecord_l*>(
            allocateSelectionStorage(heap, u32(recordCount) * sizeof(RenderSelectionRecord_l)));
        for (s32 index = 0; index < recordCount; ++index) {
            RenderSelectionRecord_l* record = storage + u32(index);
            if (gabi::ea(record) != 0) {
                record->mFlags = 0;
                be<u32>* mask = &record->mSelectionMask;
                if (gabi::ea(mask) == 0)
                    mask = static_cast<be<u32>*>(allocateSelectionScalar(4));
                if (gabi::ea(mask) != 0)
                    *mask = 0;
                record->mSecondaryColor[0] = 0.0f;
                record->mValueCount = 0;
                record->mPrimaryColor[3] = 1.0f;
                record->mPrimaryColor[0] = 0.0f;
                record->mValues = nullptr;
                record->mSecondaryColor[3] = 1.0f;
                record->mPrimaryColor[1] = 0.0f;
                record->mSecondaryColor[1] = 0.0f;
                record->mSecondaryColor[2] = 0.0f;
                record->mPrimaryColor[2] = 0.0f;
            }
        }
        if (gabi::ea(storage) != 0) {
            mRecords = storage;
            mRecordCount = u32(recordCount);
        }
    }
    if (slotCount > 0) {
        be<u32>* slots = static_cast<be<u32>*>(allocateSelectionStorage(heap, u32(slotCount) * 4));
        if (gabi::ea(slots) != 0) {
            mSlots = slots;
            mSlotCount = u32(slotCount);
        }
    }
    u32 count = mRecordCount;
    RenderSelectionRecord_l* record = mRecords;
    RenderSelectionRecord_l* end = record + count;
    RenderSelectionRecord_l* begin = record;
    UniformBuffer* firstBuffer = nullptr;
    if (record != end) {
        do {
            if (slotCount > 0) {
                SelectionValue_l* values = static_cast<SelectionValue_l*>(
                    allocateSelectionStorage(heap, u32(slotCount) * sizeof(SelectionValue_l)));
                for (s32 index = 0; index < slotCount; ++index) {
                    SelectionValue_l* value = values + u32(index);
                    if (gabi::ea(value) != 0) {
                        value->mFlags = 0;
                        constructSelectionBuffer(&value->mBuffer);
                        value->mColor[1] = 0.0f;
                        value->mColor[2] = 0.0f;
                        value->mColor[0] = 0.0f;
                        value->mColor[3] = 1.0f;
                    }
                }
                if (gabi::ea(values) != 0) {
                    record->mValues = values;
                    record->mValueCount = slotCount;
                }
            }
            u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(RenderSelectionRecord_l);
            gabi::call_ptr<void>(mMethods->mSetRecordEnabled.mFunction, this, index, u32(1));
            gabi::call_ptr<void>(mMethods->mInitializeRecord.mFunction, this, record, heap);
            s32 valueCount = record->mValueCount;
            SelectionValue_l* value = record->mValues;
            SelectionValue_l* valueEnd = value + u32(valueCount);
            if (value != valueEnd) {
                do {
                    UniformBuffer* buffer = &value->mBuffer;
                    if (gabi::ea(firstBuffer) == 0) {
                        gabi::call_ptr<void>(mMethods->mInitializeBuffer.mFunction, this, buffer, heap);
                        firstBuffer = buffer;
                    } else {
                        copySelectionBuffer(buffer, firstBuffer);
                    }
                    allocateSelectionBuffer(buffer, heap, 2);
                    ++value;
                } while (value != valueEnd);
            }
            ++record;
        } while (record != end);
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}

/* 027D11D4 */
void agl::ExtendedRenderSelection_l::initialize(u32 state, s32 recordCount, s32 slotCount, sead::AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mState = state;
    mActiveCount = 0;
    gabi::call_ptr<void>(mMethods->mPrepareHeap.mFunction, this, heap);
    if (recordCount > 0) {
        ExtendedSelectionRecord_l* storage = static_cast<ExtendedSelectionRecord_l*>(
            allocateSelectionStorage(heap, u32(recordCount) * sizeof(ExtendedSelectionRecord_l)));
        for (s32 index = 0; index < recordCount; ++index) {
            ExtendedSelectionRecord_l* record = storage + u32(index);
            if (gabi::ea(record) != 0) {
                record->mFlags = 0;
                be<u32>* mask = &record->mSelectionMask;
                if (gabi::ea(mask) == 0)
                    mask = static_cast<be<u32>*>(allocateSelectionScalar(4));
                if (gabi::ea(mask) != 0)
                    *mask = 0;
                record->mSecondaryColor[0] = 0.0f;
                record->mValueCount = 0;
                record->mPrimaryColor[3] = 1.0f;
                record->mPrimaryColor[0] = 0.0f;
                record->mValues = nullptr;
                record->mSecondaryColor[3] = 1.0f;
                record->mPrimaryColor[1] = 0.0f;
                record->mSecondaryColor[1] = 0.0f;
                record->mSecondaryColor[2] = 0.0f;
                record->mPrimaryColor[2] = 0.0f;
            }
        }
        if (gabi::ea(storage) != 0) {
            mRecords = storage;
            mRecordCount = u32(recordCount);
        }
    }
    if (slotCount > 0) {
        be<u32>* slots = static_cast<be<u32>*>(allocateSelectionStorage(heap, u32(slotCount) * 4));
        if (gabi::ea(slots) != 0) {
            mSlots = slots;
            mSlotCount = u32(slotCount);
        }
    }
    u32 count = mRecordCount;
    ExtendedSelectionRecord_l* record = mRecords;
    ExtendedSelectionRecord_l* end = record + count;
    ExtendedSelectionRecord_l* begin = record;
    UniformBuffer* firstBuffer = nullptr;
    if (record != end) {
        do {
            if (slotCount > 0) {
                ExtendedSelectionValue_l* values = static_cast<ExtendedSelectionValue_l*>(
                    allocateSelectionStorage(heap, u32(slotCount) * sizeof(ExtendedSelectionValue_l)));
                for (s32 index = 0; index < slotCount; ++index) {
                    ExtendedSelectionValue_l* value = values + u32(index);
                    if (gabi::ea(value) != 0) {
                        value->mFlags = 0;
                        constructSelectionBuffer(&value->mBuffer);
                    }
                }
                if (gabi::ea(values) != 0) {
                    record->mValues = values;
                    record->mValueCount = slotCount;
                }
            }
            u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(ExtendedSelectionRecord_l);
            gabi::call_ptr<void>(mMethods->mSetRecordEnabled.mFunction, this, index, u32(1));
            gabi::call_ptr<void>(mMethods->mInitializeRecord.mFunction, this, record, heap);
            s32 valueCount = record->mValueCount;
            ExtendedSelectionValue_l* value = record->mValues;
            ExtendedSelectionValue_l* valueEnd = value + u32(valueCount);
            if (value != valueEnd) {
                do {
                    UniformBuffer* buffer = &value->mBuffer;
                    if (gabi::ea(firstBuffer) == 0) {
                        gabi::call_ptr<void>(mMethods->mInitializeBuffer.mFunction, this, buffer, heap);
                        firstBuffer = buffer;
                    } else {
                        copySelectionBuffer(buffer, firstBuffer);
                    }
                    allocateSelectionBuffer(buffer, heap, 2);
                    ++value;
                } while (value != valueEnd);
            }
            ++record;
        } while (record != end);
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}

}
/* WWHD glue
ext 02756140 sead::AllocationHeap* currentAllocationHeap(sead::AllocationManager* manager)
ext 0273AD10 void* allocateSelectionScalar(u32 bytes)
ext 027BEB5C void constructSelectionBuffer(agl::UniformBuffer* buffer)
ext 027BEEE4 void allocateSelectionBuffer(agl::UniformBuffer* buffer, sead::AllocationHeap* heap, u32 count)
ext 027BEE9C void copySelectionBuffer(agl::UniformBuffer* target, const agl::UniformBuffer* source)
*/
