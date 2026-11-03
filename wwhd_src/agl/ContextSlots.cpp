// Context slot storage and core guard flags reconstructed from assigned WWHD instructions.
#include "bindings.h"
#include "agl/ContextSlots.h"
#include "sead/PoolHeap.h"
void* allocateContextSlotsObject(u32);
void constructContextSlot(agl::ContextSlotStorage*);
sead::RegistryHeap* findContextSlotsHeap(sead::RegistryManagerState*, void*);
sead::RegistryHeap* currentContextSlotsHeap(sead::RegistryManagerState*);
u32 contextSlotsCoreId();
void copyContextSlotPointers(void*, const void*, u32, u32);
void applyContextSlot(agl::ContextSlotStorage*);
agl::ContextSlotStorage* acquireCachedContextSlot(void*, u32, u32, u32, u32, u32, u32, u32, u8, u8);
void releaseAllocatorContextStorage(void*, void*);
static u32 sContextSlotsTable;
// Original pointerword borrowed from AGL#10; no initializer or duplicate backing storage.
typedef gptr<void> ContextOwnerPointer;
WWHD_SIZE(ContextOwnerPointer, 4);
static ContextOwnerPointer sBorrowedAllocatorOwner;
// These three field locations are observed boundary facts, not a copied owner type.
static u32 allocatorOwnerCoreFlags(void* owner, u32 core) {
    u32 slot = sCoreReverse[core];
    u32 address = gabi::ea(owner) + slot * 0xBD0 + 0xBE4;
    return *gabi::at<be<u32>>(address);
}
/* 0279A95C */
agl::ContextSlots* agl::ContextSlots::create(ContextSlots* slots) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (slots == NULL) slots = (ContextSlots*)allocateContextSlotsObject(sizeof(ContextSlots));
    if (slots == NULL) return slots;
    slots->mTable = gabi::ea(&sContextSlotsTable);
    be<u8>* flags = gabi::at<be<u8>>(gabi::ea(slots));
    if (flags == NULL) flags = (be<u8>*)allocateContextSlotsObject(sizeof(be<u8>));
    if (flags != NULL) *flags = 0;
    auto* pointers = gabi::at<sead::PointerBuffer>(gabi::ea(slots) + offsetof(ContextSlots, mPointers));
    if (pointers == NULL) pointers = (sead::PointerBuffer*)allocateContextSlotsObject(sizeof(sead::PointerBuffer));
    if (pointers != NULL) { pointers->mCapacity = 0; pointers->mCount = 0; pointers->mStorage = NULL; }
    auto* buffer = gabi::at<ContextSlotBuffer>(gabi::ea(slots) + offsetof(ContextSlots, mSlots));
    if (buffer == NULL) buffer = (ContextSlotBuffer*)allocateContextSlotsObject(sizeof(ContextSlotBuffer));
    if (buffer != NULL) { buffer->mData = NULL; buffer->mCount = 0; }
    return slots;
}
/* 0279AA14 */
void agl::ContextSlots::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mTable = gabi::ea(&sContextSlotsTable);
    mPointers.release();
    ContextSlotStorage* data = mSlots.mData;
    if (data != NULL) {
        s32 index = 0;
        while (index < (s32)mSlots.mCount) {
            auto* slot = gabi::at<ContextSlotStorage>(gabi::ea(data) + (u32)index * sizeof(ContextSlotStorage));
            gabi::call_ptr<void>(slot->mDispatch->mDestroy, slot, 2u);
            ++index;
            data = mSlots.mData;
        }
        sead::RegistryHeap* heap = findContextSlotsHeap(sRegistryManager, data);
        void* storage = mSlots.mData;
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage);
        mSlots.mCount = 0; mSlots.mData = NULL;
    }
    if ((flags & 1) != 0) operator_delete(this);
}
/* 0279AAFC */
void agl::ContextSlots::allocate(s32 count, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mPointers.allocate(count, heap, 4);
    if (count <= 0) return;
    if (heap == NULL) heap = currentContextSlotsHeap(sRegistryManager);
    auto* data = gabi::call_ptr<ContextSlotStorage*>(heap->dispatch()->mAllocate.mFunction,
        heap, (u32)count * sizeof(ContextSlotStorage), 4u);
    u32 remaining = count > 0 ? (u32)count : 1;
    u32 offset = 0;
    do {
        auto* slot = gabi::at<ContextSlotStorage>(gabi::ea(data) + offset);
        if (slot != NULL) constructContextSlot(slot);
        offset += sizeof(ContextSlotStorage);
    } while (--remaining != 0);
    if (data != NULL) { mSlots.mData = data; mSlots.mCount = count; }
}
/* 0279ABB4 */
u32 agl::ContextSlots::resetForCurrentCore() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* owner = sBorrowedAllocatorOwner;
    u32 core = contextSlotsCoreId();
    if ((allocatorOwnerCoreFlags(owner, core) & 1) != 0) {
        u8 flags = mFlags;
        mPointers.mCount = 0;
        mFlags = flags & 0xFE;
    }
    return 1;
}
/* 0279AC28 */
void agl::ContextSlots::copyIntoCurrentSlot(const ContextSlotStorage* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* owner = sBorrowedAllocatorOwner;
    u32 core = contextSlotsCoreId();
    if ((allocatorOwnerCoreFlags(owner, core) & 1) == 0) return;
    u32 index = (u32)(s32)mPointers.mCount;
    ContextSlotStorage* destination = mSlots.mData;
    if (index < (u32)(s32)mSlots.mCount)
        destination = gabi::at<ContextSlotStorage>(gabi::ea(destination) + index * sizeof(ContextSlotStorage));
    for (u32 word = 0; word < 30; ++word) destination->mWords[word] = (u32)source->mWords[word];
    for (u32 byte = 0; byte < 4; ++byte) destination->mByteFields[byte] = (u8)source->mByteFields[byte];
    for (u32 word = 0; word < 4; ++word) destination->mTailWords[word] = (u32)source->mTailWords[word];
    index = (u32)(s32)mPointers.mCount;
    destination = mSlots.mData;
    if (index < (u32)(s32)mSlots.mCount)
        destination = gabi::at<ContextSlotStorage>(gabi::ea(destination) + index * sizeof(ContextSlotStorage));
    if ((s32)index < (s32)mPointers.mCapacity) {
        u32 storage = gabi::ea((void*)mPointers.mStorage);
        *gabi::at<gptr<ContextSlotStorage>>(storage + (index << 2)) = destination;
        mPointers.mCount = (u32)(s32)mPointers.mCount + 1;
    }
}
/* 0279AD54 */
agl::ContextSlotStorage* agl::ContextSlots::popAndApply() {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 count = mPointers.mCount;
    ContextSlotStorage* result = NULL;
    if (count > 0) {
        u32 storage = gabi::ea((void*)mPointers.mStorage);
        result = *gabi::at<gptr<ContextSlotStorage>>(storage);
        copyContextSlotPointers(gabi::at<void>(storage), gabi::at<void>(storage + 4), ((u32)count - 1) << 2, 0);
        mPointers.mCount = (u32)(s32)mPointers.mCount - 1;
    }
    applyContextSlot(result);
    return result;
}
/* 0279ADD4 */
agl::ContextSlotStorage* agl::ContextSlots::acquireOrPop(u32 first, u32 second, u32 third, u32 fourth,
        u32 fifth, u32 sixth, u32 seventh, u8 firstFlag) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (((u8)mFlags & 1) != 0) return popAndApply();
    gabi::FrameLocal<be<u32>> firstStackWord(8), secondStackWord(12);
    *firstStackWord = firstFlag; *secondStackWord = 0;
    ContextSlotStorage* slot = acquireCachedContextSlot(sBorrowedAllocatorOwner,
        first, second, third, fourth, fifth, sixth, seventh, firstFlag, (u8)0);
    copyIntoCurrentSlot(slot);
    return slot;
}
/* 0279AE68 */
void agl::AllocatorCoreGuard::release(void* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (((u8)mFlags & 1) == 0)
        return gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&releaseAllocatorContextStorage)),
            (void*)sBorrowedAllocatorOwner, storage);
}
/* 0279AE88 */
void agl::AllocatorCoreGuard::capture() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* owner = sBorrowedAllocatorOwner;
    u32 core = contextSlotsCoreId();
    u32 flags = allocatorOwnerCoreFlags(owner, core);
    u8 previous = mFlags;
    if ((flags & 1) != 0) mFlags = previous | 1;
    else mFlags = previous & 0xFE;
}
/* WWHD glue
 ext 0279FC6C agl::ContextSlotStorage* acquireCachedContextSlot(void*, u32, u32, u32, u32, u32, u32, u32, u8, u8)
 ext 027A0040 void releaseAllocatorContextStorage(void*, void*) callback
 ext C0009988 void copyContextSlotPointers(void*, const void*, u32, u32)
 ext 027BE794 void applyContextSlot(agl::ContextSlotStorage*)
 data sContextSlotsTable 101674F8
 data sBorrowedAllocatorOwner 101F8DD4
 ext 0273AD10 void* allocateContextSlotsObject(u32)
 ext 027BE6B8 void constructContextSlot(agl::ContextSlotStorage*)
 ext 02755FEC sead::RegistryHeap* findContextSlotsHeap(sead::RegistryManagerState*, void*)
 ext 02756140 sead::RegistryHeap* currentContextSlotsHeap(sead::RegistryManagerState*)
 ext C0009BB0 u32 contextSlotsCoreId()
*/
