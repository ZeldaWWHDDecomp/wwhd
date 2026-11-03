// Pool and fixed label layouts reconstructed from assigned WWHD allocation and traversal.
#pragma once
#include "sead/HeapRegistry.h"
namespace agl {
struct LibraryWorkPool;
struct LibraryWorkDispatch { u8 mPrefix[12]; be<u32> mDestroy; };
struct LibraryWorkRecord {
    sead::Link mLink; be<u32> mValue; gptr<sead::LinkList> mList;
    u8 mOpaque[0x40]; gptr<LibraryWorkDispatch> mDispatch;
};
struct LibraryWorkBuffer { be<u32> mCount; gptr<LibraryWorkRecord> mData; };
struct LibraryWorkLabel : sead::FixedSafeText<128> { static LibraryWorkLabel* create(LibraryWorkLabel* label); };
struct LibraryWorkInlinePointers { sead::PointerBuffer mBuffer; gptr<void> mStorage[16]; };
struct LibraryWorkPoolDispatch { u8 mPrefix[0x14]; be<u32> mDestroy; };
struct LibraryWorkPool {
    gptr<LibraryWorkPoolDispatch> mTable;
    sead::Disposer mDisposer;
    LibraryWorkBuffer mRecords;
    sead::LinkList mFree;
    sead::LinkList mActive;
    LibraryWorkInlinePointers mPointers;
    LibraryWorkLabel mLabels[128];
    be<u8> mFirstFlag; be<u8> mSecondFlag; u8 mPadding[2];
    static LibraryWorkPool* create(LibraryWorkPool* pool);
    static LibraryWorkPool* instance(sead::RegistryHeap* heap);
    void releaseRecords(); void initializeRecords(sead::RegistryHeap* heap);
    void destroy(u32 flags);
};
void destroyLibraryWorkPoolDisposer(sead::Disposer* disposer, u32 flags);
}
WWHD_OFFSET(agl::LibraryWorkRecord, mList, 0xC);
WWHD_OFFSET(agl::LibraryWorkRecord, mDispatch, 0x50);
WWHD_SIZE(agl::LibraryWorkRecord, 0x54);
WWHD_SIZE(agl::LibraryWorkBuffer, 8);
WWHD_SIZE(agl::LibraryWorkLabel, 0x8C);
WWHD_SIZE(agl::LibraryWorkInlinePointers, 0x4C);
WWHD_OFFSET(agl::LibraryWorkPool, mDisposer, 4);
WWHD_OFFSET(agl::LibraryWorkPool, mRecords, 0x14);
WWHD_OFFSET(agl::LibraryWorkPool, mFree, 0x1C);
WWHD_OFFSET(agl::LibraryWorkPool, mActive, 0x28);
WWHD_OFFSET(agl::LibraryWorkPool, mPointers, 0x34);
WWHD_OFFSET(agl::LibraryWorkPool, mLabels, 0x80);
WWHD_OFFSET(agl::LibraryWorkPool, mFirstFlag, 0x4680);
WWHD_SIZE(agl::LibraryWorkPool, 0x4684);

// AGL#9 owns the pool pointerword and its registered embedded disposer.
extern gptr<agl::LibraryWorkPool>& sLibraryWorkPoolSingleton;
extern gptr<sead::Disposer>& sLibraryWorkPoolRegisteredDisposer;
