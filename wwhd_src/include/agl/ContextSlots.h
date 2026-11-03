// Slot collection layout and core-flag accesses derived from assigned WWHD routines.
#pragma once
#include "sead/HeapRegistry.h"
namespace agl {
// Original pointerword 101F8DD4 is borrowed from AGL#10. ContextSlots.cpp keeps
// a named view without initializer until that owner exports its canonical declaration.
// Its only interpreted owner fields here are the assigned slot*0xBD0+0xBE4 core flags.
// Array allocation and traversal establish this 0x90-byte storage stride;
// only its final dispatch word is interpreted here.
struct ContextSlotDispatch { u8 mPrefix[12]; be<u32> mDestroy; };
struct ContextSlotStorage {
    be<u32> mWords[30];
    be<u8> mByteFields[4];
    be<u32> mTailWords[4];
    gptr<ContextSlotDispatch> mDispatch;
};
struct ContextSlotBuffer { be<s32> mCount; gptr<ContextSlotStorage> mData; };
struct ContextSlots {
    be<u8> mFlags; u8 mPadding[3];
    sead::PointerBuffer mPointers;
    ContextSlotBuffer mSlots;
    be<u32> mTable;
    static ContextSlots* create(ContextSlots* slots);
    void destroy(u32 flags);
    void allocate(s32 count, sead::RegistryHeap* heap);
    u32 resetForCurrentCore();
    void copyIntoCurrentSlot(const ContextSlotStorage* source);
    ContextSlotStorage* popAndApply();
    ContextSlotStorage* acquireOrPop(u32 first, u32 second, u32 third, u32 fourth,
        u32 fifth, u32 sixth, u32 seventh, u8 firstFlag);
};
// This byte is the complete local guard state used by the assigned capture routine.
struct AllocatorCoreGuard { be<u8> mFlags; void capture(); void release(void* storage); };
}
WWHD_SIZE(agl::ContextSlotStorage, 0x90);
WWHD_OFFSET(agl::ContextSlotStorage, mDispatch, 0x8C);
WWHD_SIZE(agl::ContextSlotBuffer, 8);
WWHD_OFFSET(agl::ContextSlots, mPointers, 4);
WWHD_OFFSET(agl::ContextSlots, mSlots, 0x10);
WWHD_OFFSET(agl::ContextSlots, mTable, 0x18);
WWHD_SIZE(agl::ContextSlots, 0x1C);
WWHD_SIZE(agl::AllocatorCoreGuard, 1);

WWHD_OFFSET(agl::ContextSlotStorage, mByteFields, 0x78);
WWHD_OFFSET(agl::ContextSlotStorage, mTailWords, 0x7C);
