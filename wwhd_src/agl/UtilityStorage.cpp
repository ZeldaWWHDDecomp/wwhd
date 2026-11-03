#include "agl/UtilityStorage.h"
namespace agl { namespace utility {
static const sead::VtableSlot sUtilityStorageMethodTable;
/* 027C6C54 */
UtilityStorage* constructUtilityStorage(UtilityStorage* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!storage) {
        storage = static_cast<UtilityStorage*>(aglAllocateUtilityStorage(sizeof(UtilityStorage)));
        if (!storage) return nullptr;
    }
    storage->mMethods = gabi::ea(&sUtilityStorageMethodTable);
    storage->mFirstWord = 0;
    storage->mResourceRecords.mStorage = nullptr;
    storage->mResourceRecords.mCount = 0;
    storage->mSecondRecords.mStorage = nullptr;
    storage->mSecondRecords.mCount = 0;
    storage->mThirdRecords.mStorage = nullptr;
    storage->mThirdRecords.mCount = 0;
    storage->mFourthRecords.mStorage = nullptr;
    storage->mFourthRecords.mCount = 0;
    storage->mFourthRecords.mThirdWord = 0;
    storage->mSmallRecords.mStorage = nullptr;
    storage->mSmallRecords.mCount = 0;
    storage->mSixthRecords.mStorage = nullptr;
    storage->mSixthRecords.mCount = 0;
    storage->mSeventhRecords.mStorage = nullptr;
    storage->mSeventhRecords.mCount = 0;
    storage->mEighthRecords.mStorage = nullptr;
    storage->mEighthRecords.mCount = 0;
    storage->mNinthRecords.mColumns = 0;
    storage->mNinthRecords.mRows = 0;
    storage->mNinthRecords.mStorage = nullptr;
    return storage;
}
} }
namespace agl { namespace utility {
static void releaseStoragePair(StoragePair_l* buffer) {
    void* allocation = buffer->mStorage;
    if (!allocation) return;
    sead::RegistryHeap* heap = aglUtilityStorageFindHeap(sRegistryManager, allocation);
    u32 release = heap->dispatch()->mRelease.mFunction;
    allocation = buffer->mStorage;
    gabi::call_ptr<void>(release, heap, allocation);
    buffer->mCount = 0;
    buffer->mStorage = nullptr;
}
/* 027C6E10 */
void destroyUtilityStorage(UtilityStorage* storage, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!storage) return;
    auto* entries = static_cast<ResourceStorageRecord_l*>(static_cast<void*>(storage->mResourceRecords.mStorage));
    ResourceStorageRecord_l* end = sead::elementPtr(entries, static_cast<u32>(storage->mResourceRecords.mCount));  // 32-bit end and steps, as the original's `cmplw p,end`
    storage->mMethods = gabi::ea(&sUtilityStorageMethodTable);
    for (ResourceStorageRecord_l* record = entries; record != end; record = sead::nextElement(record)) {
        releaseStoragePair(&record->mBuffer);
        releaseStoragePair(&record->mFirstResource.mBuffer);
    }
    if (storage->mResourceRecords.mStorage) {
        for (s32 index = 0; index < static_cast<s32>(storage->mResourceRecords.mCount); ++index) {
            entries = static_cast<ResourceStorageRecord_l*>(static_cast<void*>(storage->mResourceRecords.mStorage));
            ResourceStorageRecord_l* record = entries + index;
            if (!record) continue;
            aglUtilityDestroyThirdResource(record->mThirdResource, 2);
            aglUtilityDestroySecondResource(record->mSecondResource, 2);
            aglUtilityDestroyFirstResource(record->mFirstResource.mResource, 2);
        }
        releaseStoragePair(&storage->mResourceRecords);
    }
    auto* small = static_cast<SmallStorageRecord_l*>(static_cast<void*>(storage->mSmallRecords.mStorage));
    SmallStorageRecord_l* smallEnd = sead::elementPtr(small, static_cast<u32>(storage->mSmallRecords.mCount));
    for (SmallStorageRecord_l* record = small; record != smallEnd; record = sead::nextElement(record))
        releaseStoragePair(&record->mBuffer);
    releaseStoragePair(&storage->mSmallRecords);
    releaseStoragePair(&storage->mSixthRecords);
    void* grid = storage->mNinthRecords.mStorage;
    if (grid) {
        sead::RegistryHeap* heap = aglUtilityStorageFindHeap(sRegistryManager, grid);
        u32 release = heap->dispatch()->mRelease.mFunction;
        grid = storage->mNinthRecords.mStorage;
        gabi::call_ptr<void>(release, heap, grid);
        storage->mNinthRecords.mRows = 0;
        storage->mNinthRecords.mStorage = nullptr;
        storage->mNinthRecords.mColumns = 0;
    }
    if (flags & 1) aglUtilityStorageDelete(storage);
}
} }
/* WWHD glue
data sUtilityStorageMethodTable 1016A538
ext 0273AD10 void* aglAllocateUtilityStorage(u32)
ext 02755FEC sead::RegistryHeap* aglUtilityStorageFindHeap(void*, void*)
ext 027BF880 void aglUtilityDestroyFirstResource(be<u8>*, u32)
ext 027B5CBC void aglUtilityDestroySecondResource(be<u8>*, u32)
ext 027B54A0 void aglUtilityDestroyThirdResource(be<u8>*, u32)
ext 0273AF40 void aglUtilityStorageDelete(void*)
*/
