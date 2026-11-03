// Own range return/coalescing caller. List primitives and cache maintenance remain ordinary API boundaries.
#include "agl/RangePoolOrdering.h"
void flushPoolHeapRegion(void* address, u32 bytes);
namespace agl {
static sead::Link* poolRangeLink(cache::RangeRecord* range, u32 offset) {
    return gabi::at<sead::Link>(gabi::ea(range) + offset);
}
static void recyclePoolRange(cache::RangeCache* pool, cache::RangeRecord* range) {
    u32 offset = pool->mAvailableRanges.mOffset;
    poolRangeLink(range, offset)->detach();
    pool->mAvailableRanges.mCount = u32(pool->mAvailableRanges.mCount) - 1;
    u32 recycleOffset = pool->mFreeRecords.mOffset;
    pool->mFreeRecords.insertBefore(poolRangeLink(range, recycleOffset));
    pool->mFreeRecords.mCount = u32(pool->mFreeRecords.mCount) + 1;
}
static void mergePreviousPoolRange(cache::RangeCache* pool, cache::RangeRecord* range) {
    u32 offset = pool->mAvailableRanges.mOffset;
    sead::Link* previous = poolRangeLink(range, offset)->mPrevious;
    if (previous != &pool->mAvailableRanges) {
        cache::RangeRecord* adjacent = gabi::at<cache::RangeRecord>(gabi::ea(previous) - offset);
        if (gabi::ea(adjacent) != 0) {
            u32 start = adjacent->mBegin;
            u32 bytes = adjacent->mSize;
            if (start + bytes == range->mBegin) {
                range->mBegin = start;
                bytes = adjacent->mSize;
                range->mSize = u32(range->mSize) + bytes;
                recyclePoolRange(pool, adjacent);
            }
        }
    }
}
/* 027CB3B8 */
void agl::cache::RangeCache::releaseRange(RangeRecord* range) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 bytes = range->mSize;
    mAllocatedSize = u32(mAllocatedSize) - bytes;
    be<u8>* allocation = range->mAllocatedStorage;
    if (gabi::ea(allocation) != 0) {
        sead::RegistryHeap* heap = mHeap;
        sead::RegistryHeapDispatch* methods = gabi::at<sead::RegistryHeapDispatch>(heap->mHeapInterface);
        gabi::call_ptr<void>(methods->mRelease.mFunction, heap, allocation);
        range->mAllocatedStorage = nullptr;
        heap = mHeap;
        methods = gabi::at<sead::RegistryHeapDispatch>(heap->mHeapInterface);
        u32 start = gabi::call_ptr<u32>(methods->mRegionStart.mFunction, heap);
        heap = mHeap;
        methods = gabi::at<sead::RegistryHeapDispatch>(heap->mHeapInterface);
        u32 size = gabi::call_ptr<u32>(methods->mRegionSize.mFunction, heap);
        flushPoolHeapRegion(gabi::at<void>(start), (size + 0x3Fu) & ~0x3Fu);
    }
    u32 activeOffset = mUsedRanges.mOffset;
    poolRangeLink(range, activeOffset)->detach();
    u32 activeCount = mUsedRanges.mCount;
    s32 availableCount = mAvailableRanges.mCount;
    mUsedRanges.mCount = activeCount - 1;
    sead::Link* next = nullptr;
    if (availableCount > 0)
        next = mAvailableRanges.mNext;
    u32 offset = mAvailableRanges.mOffset;
    if (gabi::ea(next) == 0 || gabi::ea(next) - offset == 0) {
        mAvailableRanges.insertAfter(poolRangeLink(range, offset));
        mAvailableRanges.mCount = u32(mAvailableRanges.mCount) + 1;
        return;
    }
    cache::RangeRecord* current = gabi::at<cache::RangeRecord>(gabi::ea(next) - offset);
    u32 sentinel = gabi::ea(&mAvailableRanges) - offset;
    if (gabi::ea(current) != sentinel) {
        u32 end = u32(range->mBegin) + u32(range->mSize);
        while (true) {
            if (end <= current->mBegin) {
                poolRangeLink(current, offset)->insertBefore(poolRangeLink(range, offset));
                mAvailableRanges.mCount = u32(mAvailableRanges.mCount) + 1;
                u32 rangeBytes = range->mSize;
                u32 rangeStart = range->mBegin;
                if (rangeStart + rangeBytes == current->mBegin) {
                    u32 adjacentBytes = current->mSize;
                    range->mSize = rangeBytes + adjacentBytes;
                    recyclePoolRange(this, current);
                }
                mergePreviousPoolRange(this, range);
                break;
            }
            sead::Link* following = poolRangeLink(current, offset)->mNext;
            current = gabi::at<cache::RangeRecord>(gabi::ea(following) - offset);
            if (gabi::ea(current) == sentinel)
                break;
        }
    }
    sead::Link* availableNext = range->mLink.mNext;
    if (gabi::ea(availableNext) == 0) {
        sead::Link* availablePrevious = range->mLink.mPrevious;
        if (gabi::ea(availablePrevious) == 0) {
            offset = mAvailableRanges.mOffset;
            mAvailableRanges.insertBefore(poolRangeLink(range, offset));
            mAvailableRanges.mCount = u32(mAvailableRanges.mCount) + 1;
            mergePreviousPoolRange(this, range);
        }
    }
    offset = mAvailableRanges.mOffset;
    sead::Link* last = mAvailableRanges.mNext;
    u32 sentinelAddress = gabi::ea(&mAvailableRanges) - offset;
    u32 address = gabi::ea(last) - offset;
    while (address != sentinelAddress) {
        sead::Link* link = gabi::at<sead::Link>(address + offset);
        last = link->mNext;
        address = gabi::ea(last) - offset;
    }
}
}
/* WWHD glue
ext 0273B2D0 sead::Link* sead::Link::insertAfter(sead::Link* node)
ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link* node)
ext 0273B310 sead::Link* sead::Link::detach()
ext 028FCB10 void flushPoolHeapRegion(void* address, u32 bytes)
*/
