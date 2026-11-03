// WWHD heap with allocation metadata in a separately supplied record array.
#pragma once
#include "sead/HeapRegistry.h"
namespace sead {
struct TrackedAllocationRecord {
    gptr<void> mAddress;
    be<u32> mSize;
    Link mLink;
};
struct TrackedAllocationPool {
    LinkList mActive;
    gptr<TrackedAllocationRecord> mUnused;
    gptr<TrackedAllocationRecord> mStorage;
    be<s32> mCapacity;
};
struct TrackedRegionHeap : RegistryHeap {
    TrackedAllocationPool mRecords;
    static TrackedRegionHeap* create(TrackedRegionHeap* object,const SafeText* name,
        RegistryHeap* parent,TrackedAllocationRecord* records,u32 recordBytes,
        void* region,u32 regionSize,u32 locking);
    static TrackedRegionHeap* createWithBuffer(const SafeText* name,void* buffer,
        u32 bufferBytes,void* region,u32 regionSize,u32 locking);
    static u32 metadataSize(u32 recordCount);
    void destroyHeap();
    void* allocate(u32 size,s32 alignment);
    TrackedAllocationRecord* findAllocation(void* address);
    void release(void* address);
    void* resizeFront(void* address,u32 size);
    void* resizeBack(void* address,u32 size);
    void releaseAll();
    u32 freeSize() const;
    u32 maxAllocation(u32 alignment) const;
    u32 contains(u32 address) const;
    void inspectRecords();
    void synchronize();
    void destroy(u32 flags);
    u32 matchesType(const RuntimeType* type) const;
    u32 trimToParent();
    u32 regionStart() const;
    u32 regionSize() const;
    u32 supportsRelease() const;
    u32 queryStorageTrait() const;
};
namespace tracked_heap_text {
// Capacities are unknown for fixed text copies that are only destroyed here.
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedText : SafeTextBuffer { void destroy(u32 flags); };
}
}
WWHD_OFFSET(sead::TrackedAllocationRecord,mSize,4);
WWHD_OFFSET(sead::TrackedAllocationRecord,mLink,8);
WWHD_SIZE(sead::TrackedAllocationRecord,16);
WWHD_OFFSET(sead::TrackedAllocationPool,mUnused,12);
WWHD_OFFSET(sead::TrackedAllocationPool,mStorage,16);
WWHD_OFFSET(sead::TrackedAllocationPool,mCapacity,20);
WWHD_SIZE(sead::TrackedAllocationPool,24);
WWHD_OFFSET(sead::TrackedRegionHeap,mRecords,0x94);
WWHD_SIZE(sead::TrackedRegionHeap,0xAC);
