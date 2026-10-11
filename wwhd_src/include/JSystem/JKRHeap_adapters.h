/* JKR heap entry adapters, HD replacement layouts.
 * HD: allocation uses a heap-manager root and thread-local current heap, rather than
 * the GameCube JKRHeap statics. These private views describe only verified fields. */
#pragma once
#include "bindings.h"

namespace heap_adapters {
struct HeapVtable {
    u8 _00[0x34];
    gabi::gfn alloc;
    u8 _38[4];
    gabi::gfn free;
};
struct Heap {
    u8 _00[0xC];
    gptr<HeapVtable> __vtbl;
    u32 alloc(u32 size, s32 alignment) {
        return gabi::call_ptr<u32>(__vtbl->alloc.get(), this, size, alignment);
    }
    void free(u32 memory) {
        gabi::call_ptr(__vtbl->free.get(), this, memory);
    }
};
struct HeapManager {
    u8 _00[0x24];
    be<u32> mThreadSpecificKey;
};
struct ThreadHeapContext {
    u8 _00[0x70];
    be<u32> mCurrentHeap;
};
struct FreeCallback {
    gabi::gfn free;
};
WWHD_OFFSET(HeapVtable, alloc, 0x34);
WWHD_OFFSET(HeapVtable, free, 0x3C);
WWHD_SIZE(HeapVtable, 0x40);
WWHD_OFFSET(Heap, __vtbl, 0xC);
WWHD_SIZE(Heap, 0x10);
WWHD_OFFSET(HeapManager, mThreadSpecificKey, 0x24);
WWHD_SIZE(HeapManager, 0x28);
WWHD_OFFSET(ThreadHeapContext, mCurrentHeap, 0x70);
WWHD_SIZE(ThreadHeapContext, 0x74);
WWHD_SIZE(FreeCallback, 4);
} // namespace heap_adapters
heap_adapters::Heap* getCurrentHeapFromManager(heap_adapters::Heap* root);
heap_adapters::Heap* findContainingHeap(heap_adapters::Heap* root, u32 memory);
heap_adapters::ThreadHeapContext* getThreadSpecificHeapContext(u32 key);
u32 setCurrentHeap(heap_adapters::Heap* root, u32 heap);
void freeAllocation(u32 memory);
