// Library work-record pool and label construction reconstructed from assigned WWHD bodies.
#include "bindings.h"
#include "agl/LibraryWorkPool.h"
void* allocateLibraryWorkObject(u32);
void* allocateLibraryWorkPool(u32, sead::RegistryHeap*, u32);
void initializeLibraryWorkDisposer(sead::Disposer*, sead::RegistryHeap*, u32);
void finalizeLibraryWorkDisposer(sead::Disposer*, u32);
void constructLibraryWorkRecord(agl::LibraryWorkRecord*);
sead::RegistryHeap* findLibraryWorkHeap(sead::RegistryManagerState*, void*);
void constructLibraryWorkLabelArray(void*, u32, u32, u32);
void destroyLibraryWorkLabelArray(void*, u32, u32, u32, u32, u32);
void destroyLibraryWorkLabel(void*, u32);
static u32 sLibraryWorkPoolTable;
static u32 sLibraryWorkDisposerTable;
static u32 sLibraryWorkLabelBufferBaseTable;
static u32 sLibraryWorkLabelBaseTable;
static u32 sLibraryWorkLabelTable;
// AGL#9-owned pointerwords, exported by reference in LibraryWorkPool.h.
typedef gptr<agl::LibraryWorkPool> LibraryWorkPoolPointer;
typedef gptr<sead::Disposer> LibraryWorkDisposerPointer;
WWHD_SIZE(LibraryWorkPoolPointer, 4);
WWHD_SIZE(LibraryWorkDisposerPointer, 4);
static LibraryWorkPoolPointer sLibraryWorkPoolSingleton;
static LibraryWorkDisposerPointer sLibraryWorkPoolRegisteredDisposer;
/* 02799AA0 */
agl::LibraryWorkPool* agl::LibraryWorkPool::create(LibraryWorkPool* pool) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (pool == NULL) pool = (LibraryWorkPool*)allocateLibraryWorkObject(sizeof(LibraryWorkPool));
    if (pool == NULL) return pool;
    pool->mTable = reinterpret_cast<LibraryWorkPoolDispatch*>(&sLibraryWorkPoolTable);
    auto* records = gabi::at<LibraryWorkBuffer>(gabi::ea(pool) + offsetof(LibraryWorkPool, mRecords));
    if (records == NULL) records = (LibraryWorkBuffer*)allocateLibraryWorkObject(sizeof(LibraryWorkBuffer));
    if (records != NULL) { records->mData = NULL; records->mCount = 0; }
    auto* freeList = gabi::at<sead::LinkList>(gabi::ea(pool) + offsetof(LibraryWorkPool, mFree));
    if (freeList == NULL) freeList = (sead::LinkList*)allocateLibraryWorkObject(sizeof(sead::LinkList));
    if (freeList != NULL) {
        freeList->mNext = freeList; freeList->mCount = 0; freeList->mPrevious = freeList;
    }
    auto* active = gabi::at<sead::LinkList>(gabi::ea(pool) + offsetof(LibraryWorkPool, mActive));
    if (active == NULL) active = (sead::LinkList*)allocateLibraryWorkObject(sizeof(sead::LinkList));
    if (active != NULL) { active->mNext = active; active->mCount = 0; active->mPrevious = active; }
    auto* pointers = gabi::at<LibraryWorkInlinePointers>(gabi::ea(pool) + offsetof(LibraryWorkPool, mPointers));
    if (pointers == NULL) pointers = (LibraryWorkInlinePointers*)allocateLibraryWorkObject(sizeof(LibraryWorkInlinePointers));
    if (pointers != NULL) {
        pointers->mBuffer.mCount = 0; pointers->mBuffer.mCapacity = 0; pointers->mBuffer.mStorage = NULL;
        pointers->mBuffer.setStorage(16, &pointers->mStorage[0]);
    }
    auto* labels = gabi::at<LibraryWorkLabel>(gabi::ea(pool) + offsetof(LibraryWorkPool, mLabels));
    if (labels == NULL) labels = (LibraryWorkLabel*)allocateLibraryWorkObject(sizeof(pool->mLabels));
    if (labels != NULL) constructLibraryWorkLabelArray(labels, 128, sizeof(LibraryWorkLabel),
        gabi::guest_fn(reinterpret_cast<const void*>(&LibraryWorkLabel::create)));
    pool->mSecondFlag = 1; pool->mFirstFlag = 1;
    return pool;
}
/* 02799BC8 */
agl::LibraryWorkPool* agl::LibraryWorkPool::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    LibraryWorkPool* pool = sLibraryWorkPoolSingleton;
    if (pool != NULL) return pool;
    pool = (LibraryWorkPool*)allocateLibraryWorkPool(sizeof(LibraryWorkPool), heap, 4);
    // GHS forms the embedded address before testing the allocation result.
    auto* disposer = gabi::at<sead::Disposer>(gabi::ea(pool) + offsetof(LibraryWorkPool, mDisposer));
    if (disposer != NULL) {
        initializeLibraryWorkDisposer(disposer, heap, 3);
        disposer->mVtable = reinterpret_cast<sead::DisposerVtable*>(&sLibraryWorkDisposerTable);
    }
    sLibraryWorkPoolRegisteredDisposer = disposer;
    if (pool != NULL) pool = create(pool);
    sLibraryWorkPoolSingleton = pool;
    return pool;
}
/* 02799C74 */
void agl::LibraryWorkPool::releaseRecords() {
    gabi::AutoFrame frame{gabi::current_function()};
    LibraryWorkRecord* data = mRecords.mData;
    if (data == NULL) return;
    s32 index = 0;
    while (index < (s32)(u32)mRecords.mCount) {
        auto* record = gabi::at<LibraryWorkRecord>(gabi::ea(data) + (u32)index * sizeof(LibraryWorkRecord));
        gabi::call_ptr<void>(record->mDispatch->mDestroy, record, 2u);
        ++index;
        data = mRecords.mData;
    }
    sead::RegistryHeap* heap = findLibraryWorkHeap(sRegistryManager, data);
    void* storage = mRecords.mData;
    gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage);
    mRecords.mCount = 0; mRecords.mData = NULL;
}
/* 02799D28 */
void agl::LibraryWorkPool::initializeRecords(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseRecords();
    if (heap == NULL) return;
    auto* data = gabi::call_ptr<LibraryWorkRecord*>(heap->dispatch()->mAllocate.mFunction,
        heap, 128u * sizeof(LibraryWorkRecord), 4u);
    for (u32 index = 0; index < 128; ++index) {
        auto* record = gabi::at<LibraryWorkRecord>(gabi::ea(data) + index * sizeof(LibraryWorkRecord));
        if (record != NULL) constructLibraryWorkRecord(record);
    }
    if (data != NULL) { mRecords.mCount = 128; mRecords.mData = data; }
    for (u32 index = 0; index < 128; ++index) {
        LibraryWorkRecord* record = mRecords.mData;
        if (index < (u32)mRecords.mCount)
            record = gabi::at<LibraryWorkRecord>(gabi::ea(record) + index * sizeof(LibraryWorkRecord));
        sead::LinkList* previous = record->mList;
        if (previous != NULL) {
            record->mList = NULL;
            record->mLink.detach();
            previous->mCount = (u32)(s32)previous->mCount - 1;
        }
        record->mList = &mFree;
        mFree.insertBefore(&record->mLink);
        mFree.mCount = (u32)(s32)mFree.mCount + 1;
    }
}
/* 02799E48 */
void agl::LibraryWorkPool::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mTable = reinterpret_cast<LibraryWorkPoolDispatch*>(&sLibraryWorkPoolTable);
    releaseRecords();
    destroyLibraryWorkLabelArray(&mLabels[0], 128, sizeof(LibraryWorkLabel),
        gabi::guest_fn(reinterpret_cast<const void*>(&destroyLibraryWorkLabel)), 0, 0);
    if ((flags & 1) != 0) operator_delete(this);
}
/* 02799EC4 */
void agl::destroyLibraryWorkPoolDisposer(sead::Disposer* disposer, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (disposer == NULL) return;
    disposer->mVtable = reinterpret_cast<sead::DisposerVtable*>(&sLibraryWorkDisposerTable);
    if (disposer == (sead::Disposer*)sLibraryWorkPoolRegisteredDisposer) {
        LibraryWorkPool* pool = sLibraryWorkPoolSingleton;
        sLibraryWorkPoolRegisteredDisposer = NULL;
        gabi::call_ptr<void>(pool->mTable->mDestroy, pool, 2u);
        sLibraryWorkPoolSingleton = NULL;
    }
    finalizeLibraryWorkDisposer(disposer, 0);
    if ((flags & 1) != 0) operator_delete(disposer);
}
/* 02799FC4 */
agl::LibraryWorkLabel* agl::LibraryWorkLabel::create(LibraryWorkLabel* label) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (label == NULL) label = (LibraryWorkLabel*)allocateLibraryWorkObject(sizeof(LibraryWorkLabel));
    if (label == NULL) return label;
    sead::SafeTextBuffer* base = label;
    if (base == NULL) base = (sead::SafeTextBuffer*)allocateLibraryWorkObject(sizeof(sead::SafeTextBuffer));
    if (base != NULL) {
        base->mText = gabi::at<be<u8>>(gabi::ea(label) + offsetof(sead::FixedSafeText<128>, mStorage));
        base->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sLibraryWorkLabelBufferBaseTable);
        base->mCapacity = 128;
        label->mStorage[127] = 0;
    }
    be<u8>* storage = label->mText;
    label->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sLibraryWorkLabelBaseTable);
    *storage = 0;
    label->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sLibraryWorkLabelTable);
    return label;
}
/* WWHD glue
 data sLibraryWorkPoolTable 10167360
 data sLibraryWorkDisposerTable 10167350
 data sLibraryWorkLabelBufferBaseTable 101672E8
 data sLibraryWorkLabelBaseTable 10167320
 data sLibraryWorkLabelTable 10167338
 data sLibraryWorkPoolSingleton 101F8DC0
 data sLibraryWorkPoolRegisteredDisposer 101F8DC4
 ext 0273AD10 void* allocateLibraryWorkObject(u32)
 ext 0273B0D4 void* allocateLibraryWorkPool(u32, sead::RegistryHeap*, u32)
 ext 02752B0C void initializeLibraryWorkDisposer(sead::Disposer*, sead::RegistryHeap*, u32)
 ext 02752BEC void finalizeLibraryWorkDisposer(sead::Disposer*, u32)
 ext 027A0698 void constructLibraryWorkRecord(agl::LibraryWorkRecord*)
 ext 02755FEC sead::RegistryHeap* findLibraryWorkHeap(sead::RegistryManagerState*, void*)
 ext 0273B560 void sead::PointerBuffer::setStorage(s32, void*)
 ext 0273B310 sead::Link* sead::Link::detach()
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
 ext 028EFFD0 void constructLibraryWorkLabelArray(void*, u32, u32, u32)
 ext 028F0164 void destroyLibraryWorkLabelArray(void*, u32, u32, u32, u32, u32)
 ext 0279A068 void destroyLibraryWorkLabel(void*, u32) callback
*/
