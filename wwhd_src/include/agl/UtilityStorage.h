/* Local aggregate storage observed in the AGL utility constructor/destructor.
 * Unwritten spans retain their previous contents; no foreign layout was used. */
#pragma once
#include "sead/HeapRegistry.h"
#include "sead/ElementAt.h"   // sead::elementPtr / nextElement: 32-bit guest pointer walks
namespace agl { namespace utility {
struct StoragePair_l { be<u32> mCount; gptr<void> mStorage; };
WWHD_SIZE(StoragePair_l, 8);
WWHD_OFFSET(StoragePair_l, mStorage, 4);
struct StorageTriple_l { be<u32> mCount; gptr<void> mStorage; be<u32> mThirdWord; };
WWHD_SIZE(StorageTriple_l, 12);
WWHD_OFFSET(StorageTriple_l, mStorage, 4);
WWHD_OFFSET(StorageTriple_l, mThirdWord, 8);
struct StorageGrid_l { be<u32> mRows; be<u32> mColumns; gptr<void> mStorage; };
WWHD_SIZE(StorageGrid_l, 12);
WWHD_OFFSET(StorageGrid_l, mColumns, 4);
WWHD_OFFSET(StorageGrid_l, mStorage, 8);
/* Enclosing-record fragment: original BF734 child is F4 bytes; the pair
 * immediately following it belongs to this aggregate, not the child class. */
struct FirstResourceStorage_l {
    be<u8> mResource[0xF4];
    StoragePair_l mBuffer;
};
WWHD_OFFSET(FirstResourceStorage_l, mBuffer, 0xF4);
WWHD_SIZE(FirstResourceStorage_l, 0xFC);
struct ResourceStorageRecord_l {
    u8 mPrefix[0x14];
    FirstResourceStorage_l mFirstResource;
    be<u8> mSecondResource[0x154];
    StoragePair_l mBuffer;
    be<u8> mThirdResource[0x18];
};
WWHD_OFFSET(ResourceStorageRecord_l, mFirstResource, 0x14);
WWHD_OFFSET(ResourceStorageRecord_l, mBuffer, 0x264);
WWHD_SIZE(ResourceStorageRecord_l, 0x284);
struct SmallStorageRecord_l { u8 mPrefix[12]; StoragePair_l mBuffer; };
WWHD_SIZE(SmallStorageRecord_l, 20);
struct UtilityStorage {
    be<u32> mFirstWord;
    u8 mUnobserved04[0x70];
    StoragePair_l mResourceRecords;
    StoragePair_l mSecondRecords;
    StoragePair_l mThirdRecords;
    StorageTriple_l mFourthRecords;
    StoragePair_l mSmallRecords;
    u8 mUnobservedA0[4];
    StoragePair_l mSixthRecords;
    StoragePair_l mSeventhRecords;
    StoragePair_l mEighthRecords;
    StorageGrid_l mNinthRecords;
    be<u32> mMethods;
};
WWHD_OFFSET(UtilityStorage, mResourceRecords, 0x74);
WWHD_OFFSET(UtilityStorage, mSmallRecords, 0x98);
WWHD_OFFSET(UtilityStorage, mMethods, 0xC8);
WWHD_SIZE(UtilityStorage, 0xCC);
UtilityStorage* constructUtilityStorage(UtilityStorage* storage);
void destroyUtilityStorage(UtilityStorage* storage, u32 flags);
} }
void* aglAllocateUtilityStorage(u32);

sead::RegistryHeap* aglUtilityStorageFindHeap(void*, void*);
void aglUtilityDestroyFirstResource(be<u8>*, u32);
void aglUtilityDestroySecondResource(be<u8>*, u32);
void aglUtilityDestroyThirdResource(be<u8>*, u32);
void aglUtilityStorageDelete(void*);
