// WWHD allocation dispatch. Implementation reconstructed from the game binary.
#include "sead/Allocation.h"

static sead::AllocationManager* sAllocationManager;
static u32 sFallbackAllocation;

/* 0273AD10 */
static void* allocateObject(u32 size) {
    sead::AllocationManager* manager = sAllocationManager;
    if (manager == NULL) return gabi::call_ptr<void*>(sFallbackAllocation, size);
    sead::AllocationHeap* heap = currentAllocationHeap(manager);
    if (heap == NULL) return NULL;
    return gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, size, 4);
}

/* 0273ADAC */
static void* allocateArray(u32 size) {
    sead::AllocationManager* manager = sAllocationManager;
    if (manager == NULL) return gabi::call_ptr<void*>(sFallbackAllocation, size);
    sead::AllocationHeap* heap = currentAllocationHeap(manager);
    if (heap == NULL) return NULL;
    return gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, size, 4);
}

/* 0273AE48 */
static void* allocateAligned(u32 size, s32 alignment) {
    sead::AllocationHeap* heap = currentAllocationHeap(sAllocationManager);
    if (heap == NULL) return NULL;
    return gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, size, alignment);
}

/* 0273B050 */
static void* allocateFromHeap(u32 size, sead::AllocationHeap* heap, s32 alignment) {
    if (heap == NULL) {
        heap = currentAllocationHeap(sAllocationManager);
        if (heap == NULL) return NULL;
    }
    return gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, size, alignment);
}

/* 0273B0D4 */
static void* allocateArrayFromHeap(u32 size, sead::AllocationHeap* heap, s32 alignment) {
    if (heap == NULL) {
        heap = currentAllocationHeap(sAllocationManager);
        if (heap == NULL) return NULL;
    }
    return gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, size, alignment);
}

/* WWHD glue
 data sAllocationManager 101F8B4C
 data sFallbackAllocation C1000000
 ext 02756140 sead::AllocationHeap* currentAllocationHeap(sead::AllocationManager*)
*/
