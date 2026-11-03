/* AGL#17 range-cache layout. The embedded colour/depth targets are the canonical
 * agl::ColorSurface / agl::DepthSurface (constructors 027B6CB8 / 027B6E64). */
#pragma once
#include "sead/HeapRegistry.h"
#include "agl/RenderSurface.h"
#include "agl/TextureData.h"
namespace agl { namespace cache {
struct RangeRecord {
    be<u32> mBegin, mSize;
    gptr<be<u8>> mAllocatedStorage;
    be<u32> mFirstAlignedAddress, mSecondAlignedAddress;
    sead::Link mLink;
};
struct RangeRecordBuffer { be<s32> mCount; gptr<RangeRecord> mRecords; };
struct RangeAllocationRequest {
    be<u32> mTotalSize;
    be<u32> mFirstSize;
    be<u32> mSecondSize;
    be<u32> mAlignment;
    gptr<be<u32>> mExtraAddress;
    be<u32> mExtraSize;
    be<u32> mExtraAlignment;
};
WWHD_OFFSET(RangeAllocationRequest, mAlignment, 0xC);
WWHD_OFFSET(RangeAllocationRequest, mExtraAddress, 0x10);
WWHD_OFFSET(RangeAllocationRequest, mExtraAlignment, 0x18);
WWHD_SIZE(RangeAllocationRequest, 28);
struct RangeCache {
    gptr<be<u8>> mRegionBegin, mRegionEnd;
    be<u32> mAllocatedSize;
    RangeRecordBuffer mRecords;
    sead::RegistryObjectList mUsedRanges, mAvailableRanges, mFreeRecords;
    ColorSurface mColorTarget;
    DepthSurface mDepthTarget;
    gptr<sead::RegistryHeap> mHeap;
    gptr<const void> mInterfaceTable;
    static void destroy(RangeCache* object, u32 flags);
    void initializeRegion(be<u8>* begin, u32 size, s32 recordCount, sead::RegistryHeap* heap);
    u32 allocateRange(RangeRecord* range, const RangeAllocationRequest* request, u32 fromFront);
    RangeRecord* allocateTexture(const TextureData* texture, be<u32>* auxiliaryAddress, u32 fromFront);
    // 027CB3B8 (AGL#18): return a record to the available list, coalescing adjacent ranges.
    void releaseRange(RangeRecord* range);
};
WWHD_SIZE(RangeRecord, 28);
WWHD_OFFSET(RangeRecord, mLink, 20);
WWHD_SIZE(RangeRecordBuffer, 8);
WWHD_OFFSET(RangeCache, mRecords, 0xC);
WWHD_OFFSET(RangeCache, mUsedRanges, 0x14);
WWHD_OFFSET(RangeCache, mAvailableRanges, 0x24);
WWHD_OFFSET(RangeCache, mFreeRecords, 0x34);
WWHD_OFFSET(RangeCache, mColorTarget, 0x44);
WWHD_OFFSET(RangeCache, mDepthTarget, 0x18C);
WWHD_OFFSET(RangeCache, mHeap, 0x2E0);
WWHD_OFFSET(RangeCache, mInterfaceTable, 0x2E4);
WWHD_SIZE(RangeCache, 0x2E8);
RangeCache* constructRangeCache(RangeCache* object);
} }
void* allocateRangeCacheStorage(u32);
void aglRangeCacheFlushRegion(const void* address, u32 size);
void aglRangeCacheSetColorTexture(agl::ColorSurface*, const agl::TextureData*);
void aglRangeCacheSetDepthTexture(agl::DepthSurface*, const agl::TextureData*);
