// Owned record-buffer cleanup; buffer implementation remains an opaque AGL#15 boundary.
#include "agl/RenderSelection.h"

void destroySelectionBuffer(agl::UniformBuffer* buffer, u32 flags);

namespace agl {
/* 027D081C */
void agl::RenderSelection_l::releaseRecords() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 recordCount = mRecordCount;
    RenderSelectionRecord_l* storage = mRecords;
    RenderSelectionRecord_l* end = sead::elementPtr(storage, recordCount);  // 32-bit end and steps, as the original's `cmplw p,end`
    RenderSelectionRecord_l* record = storage;
    if (record != end) {
        do {
            gabi::call_ptr<void>(mMethods->mReleaseRecord.mFunction, this, record);
            SelectionValue_l* values = record->mValues;
            if (gabi::ea(values) != 0) {
                s32 count = record->mValueCount;
                s32 index = 0;
                while (index < count) {
                    SelectionValue_l* value = values + u32(index);
                    if (gabi::ea(value) != 0) {
                        destroySelectionBuffer(&value->mBuffer, 2);
                        count = record->mValueCount;
                        values = record->mValues;
                    }
                    ++index;
                }
                sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, values);
                u32 release = heap->dispatch()->mRelease.mFunction;
                values = record->mValues;
                gabi::call_ptr<void>(release, heap, values);
                record->mValueCount = 0;
                record->mValues = nullptr;
            }
            record = sead::nextElement(record);
        } while (record != end);
        storage = mRecords;
    }
    if (gabi::ea(storage) != 0) {
        u32 count = mRecordCount;
        (void)count;
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, storage);
        u32 release = heap->dispatch()->mRelease.mFunction;
        storage = mRecords;
        gabi::call_ptr<void>(release, heap, storage);
        mRecordCount = 0;
        mRecords = nullptr;
    }
}

/* 027D1534 */
void agl::ExtendedRenderSelection_l::releaseRecords() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 recordCount = mRecordCount;
    ExtendedSelectionRecord_l* storage = mRecords;
    ExtendedSelectionRecord_l* end = sead::elementPtr(storage, recordCount);  // 32-bit end and steps, as the original's `cmplw p,end`
    ExtendedSelectionRecord_l* record = storage;
    if (record != end) {
        do {
            gabi::call_ptr<void>(mMethods->mReleaseRecord.mFunction, this, record);
            ExtendedSelectionValue_l* values = record->mValues;
            if (gabi::ea(values) != 0) {
                s32 count = record->mValueCount;
                s32 index = 0;
                while (index < count) {
                    ExtendedSelectionValue_l* value = values + u32(index);
                    if (gabi::ea(value) != 0) {
                        destroySelectionBuffer(&value->mBuffer, 2);
                        count = record->mValueCount;
                        values = record->mValues;
                    }
                    ++index;
                }
                sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, values);
                u32 release = heap->dispatch()->mRelease.mFunction;
                values = record->mValues;
                gabi::call_ptr<void>(release, heap, values);
                record->mValueCount = 0;
                record->mValues = nullptr;
            }
            record = sead::nextElement(record);
        } while (record != end);
        storage = mRecords;
    }
    if (gabi::ea(storage) != 0) {
        u32 count = mRecordCount;
        (void)count;
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, storage);
        u32 release = heap->dispatch()->mRelease.mFunction;
        storage = mRecords;
        gabi::call_ptr<void>(release, heap, storage);
        mRecordCount = 0;
        mRecords = nullptr;
    }
}

}

/* WWHD glue
ext 027BEC90 void destroySelectionBuffer(agl::UniformBuffer* buffer, u32 flags)
ext 02755FEC sead::RegistryHeap* findRegistryHeap(void* manager, void* block)
*/
