/* Heap entry adapters: array allocation, delete, current-heap lookup and switching.
 * Verified WWHD behavior. */
#include "JSystem/JKRHeap_adapters.h"

using namespace heap_adapters;
static Heap* rootHeap;
static HeapManager* heapManager;
static FreeCallback fallbackFree;

/* 0273AEC4 */
static u32 operator_new_array(u32 size, s32 alignment) {
    Heap* heap = getCurrentHeapFromManager(rootHeap);
    if (heap == nullptr) {
        return 0;
    }
    return heap->alloc(size, alignment);
}

/* inlined into JKRHeap_operator_delete, JKRHeap_operator_delete_array */
static void freeFromContainingHeap(u32 memory) {
    if (memory == 0) {
        return;
    }
    Heap* root = rootHeap;
    if (root != nullptr) {
        Heap* heap = findContainingHeap(root, memory);
        if (heap != nullptr) {
            heap->free(memory);
        }
    } else {
        // HD: a missing manager root selects the SDK fallback callback.
        gabi::call_ptr(fallbackFree.free.get(), memory);
    }
}

/* 0273AF40 */
static void JKRHeap_operator_delete(u32 memory) {
    freeFromContainingHeap(memory);
}

/* 0273AFC8 */
static void JKRHeap_operator_delete_array(u32 memory) {
    freeFromContainingHeap(memory);
}

/* 02756140 */
static u32 getCurrentHeap() {
    HeapManager* manager = heapManager;
    ThreadHeapContext* context = getThreadSpecificHeapContext(manager->mThreadSpecificKey);
    // HD: retain the full guest register, including a possible null current heap.
    return context->mCurrentHeap;
}

/* 027EC0B0 */
static void JKRFree(u32 memory) {
    freeAllocation(memory);
}

/* 027EC220 */
static u32 JKRSetCurrentHeap(u32 heap) {
    return setCurrentHeap(rootHeap, heap);
}

/* WWHD glue
 data rootHeap 101F8B4C
 data heapManager 101F8B94
 data fallbackFree C1002000
 ext 02756140 heap_adapters::Heap* getCurrentHeapFromManager(heap_adapters::Heap*)
 ext 02755FEC heap_adapters::Heap* findContainingHeap(heap_adapters::Heap*, u32)
 ext C0009CC8 heap_adapters::ThreadHeapContext* getThreadSpecificHeapContext(u32)
 ext 02756170 u32 setCurrentHeap(heap_adapters::Heap*, u32)
 ext 0273AF40 void freeAllocation(u32)
*/
