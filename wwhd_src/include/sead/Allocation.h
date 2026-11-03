// WWHD's allocation entry points and heap virtual-call layout, from local instructions.
#pragma once
#include "wwhd.h"
namespace sead {
struct AllocationTable {
    u8 mTypeAndDestroy[52];
    be<u32> mAllocate;
};
struct AllocationHeap {
    u8 mObjectLinks[12];
    gptr<AllocationTable> mTable;
};
struct AllocationManager;
}
WWHD_OFFSET(sead::AllocationHeap,mTable,12);
WWHD_SIZE(sead::AllocationHeap,16);
WWHD_OFFSET(sead::AllocationTable,mAllocate,52);
WWHD_SIZE(sead::AllocationTable,56);
sead::AllocationHeap* currentAllocationHeap(sead::AllocationManager* manager);
