// format facts: GX2Surface field layout (public wut declarations); no code copied.
#include "agl/RangeCache.h"
#include "agl/UtilityText.h"
namespace agl { namespace cache {
static const u32 sRangeCacheTable;
static void initializeEmbeddedRangeList(sead::RegistryObjectList* list) {
    if (gabi::ea(list) == 0) list = static_cast<sead::RegistryObjectList*>(allocateRangeCacheStorage(16));
    if (!list) return;
    list->mCount = 0;
    list->mPrevious = list;
    list->mOffset = 0xFFFFFFFFu;
    list->mNext = list;
}
/* 027CAB58 */
RangeCache* constructRangeCache(RangeCache* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) {
        object = static_cast<RangeCache*>(allocateRangeCacheStorage(sizeof(RangeCache)));
        if (!object) return nullptr;
    }
    object->mAllocatedSize = 0;
    object->mRegionEnd = nullptr;
    object->mInterfaceTable = &sRangeCacheTable;
    object->mRegionBegin = nullptr;
    RangeRecordBuffer* records = &object->mRecords;
    if (gabi::ea(records) == 0)
        records = static_cast<RangeRecordBuffer*>(allocateRangeCacheStorage(8));
    if (records) {
        records->mRecords = nullptr;
        records->mCount = 0;
    }
    initializeEmbeddedRangeList(&object->mUsedRanges);
    initializeEmbeddedRangeList(&object->mAvailableRanges);
    initializeEmbeddedRangeList(&object->mFreeRecords);
    object->mColorTarget.construct();
    object->mDepthTarget.construct();
    object->mHeap = nullptr;
    object->mAvailableRanges.mOffset = 20;
    object->mFreeRecords.mOffset = 20;
    object->mUsedRanges.mOffset = 20;
    object->mUsedRanges.clear();
    object->mAvailableRanges.clear();
    object->mFreeRecords.clear();
    return object;
}
} }
/* 027CAC9C */
void agl::cache::RangeCache::destroy(RangeCache* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) return;
    RangeRecord* records = object->mRecords.mRecords;
    object->mInterfaceTable = &sRangeCacheTable;
    if (records) {
        sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager, records);
        u32 release = heap->dispatch()->mRelease.mFunction;
        records = object->mRecords.mRecords;
        gabi::call_ptr<void>(release, heap, records);
        object->mRecords.mCount = 0;
        object->mRecords.mRecords = nullptr;
    }
    if (flags & 1) aglUtilityTextStorageDelete(object);
}
/* 027CAD30 */
void agl::cache::RangeCache::initializeRegion(be<u8>* begin, u32 size, s32 recordCount,
                                  sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!size) return;
    RangeRecord* records = nullptr;
    u32 end;
    if (recordCount > 0) {
        if (!heap) heap = registryCurrentHeap(sRegistryManager);
        u32 bytes = static_cast<u32>(recordCount) * sizeof(RangeRecord);
        records = gabi::call_ptr<RangeRecord*>(heap->dispatch()->mAllocate.mFunction,
                                               heap, bytes, 4);
        for (u32 index = 0; index < static_cast<u32>(recordCount); ++index) {
            /* records may be NULL after a failed allocation: the guard and the stores use the guest
             * address records + index * 0x1C, which stays on guest page 0 (a DSI on the console). */
            u32 address = gabi::ea(records) + index * sizeof(RangeRecord);
            if (address) {
                RangeRecord* record = gabi::at<RangeRecord>(address);
                record->mLink.mNext = nullptr;
                record->mLink.mPrevious = nullptr;
            }
        }
    }
    if (records) {
        mRecords.mCount = recordCount;
        end = gabi::ea(records) + static_cast<u32>(recordCount) * sizeof(RangeRecord);
        mRecords.mRecords = records;
    } else {
        s32 oldCount = mRecords.mCount;
        records = mRecords.mRecords;
        end = gabi::ea(records) + static_cast<u32>(oldCount) * sizeof(RangeRecord);
    }
    while (gabi::ea(records) != end) {
        records->mBegin = 0;
        records->mSize = 0;
        records->mAllocatedStorage = nullptr;
        records->mFirstAlignedAddress = 0;
        records->mSecondAlignedAddress = 0;
        sead::Link* link = sead::linkAt(records, mFreeRecords.mOffset);
        mFreeRecords.insertBefore(link);
        u32 count = mFreeRecords.mCount;
        ++records;
        mFreeRecords.mCount = count + 1;
    }
    sead::Link* link = mFreeRecords.popBack();
    RangeRecord* record = link ? sead::objectOf<RangeRecord>(link, mFreeRecords.mOffset) : nullptr;
    record->mBegin = begin ? gabi::ea(begin) : 32;
    record->mSize = size;
    record->mFirstAlignedAddress = 0;
    link = sead::linkAt(record, mAvailableRanges.mOffset);
    mAvailableRanges.insertBefore(link);
    mAvailableRanges.mCount = static_cast<u32>(mAvailableRanges.mCount) + 1;
    mRegionBegin.v = record->mBegin;
    u32 regionBegin = record->mBegin;
    u32 regionSize = record->mSize;
    mRegionEnd.v = regionBegin + regionSize;
    if (begin) aglRangeCacheFlushRegion(begin, size);
}
/* 027CAED8 */
u32 agl::cache::RangeCache::allocateRange(RangeRecord* range,
        const RangeAllocationRequest* request, u32 fromFront) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 required = request->mTotalSize;
    u32 available = range->mSize;
    if (required > available) return 0;
    if (required < available) {
        sead::Link* link = mFreeRecords.popBack();
        available = range->mSize;
        required = request->mTotalSize;
        RangeRecord* leftover = link ? sead::objectOf<RangeRecord>(link, mFreeRecords.mOffset) : nullptr;
        leftover->mSize = available - required;
        u32 begin = range->mBegin;
        if (fromFront) {
            leftover->mBegin = begin + static_cast<u32>(request->mTotalSize);
            u32 offset = mAvailableRanges.mOffset;
            sead::linkAt(range, offset)->insertAfter(sead::linkAt(leftover, offset));
            mAvailableRanges.mCount = static_cast<u32>(mAvailableRanges.mCount) + 1;
        } else {
            leftover->mBegin = begin;
            range->mBegin = static_cast<u32>(range->mBegin) + static_cast<u32>(leftover->mSize);
            u32 offset = mAvailableRanges.mOffset;
            sead::linkAt(range, offset)->insertBefore(sead::linkAt(leftover, offset));
            mAvailableRanges.mCount = static_cast<u32>(mAvailableRanges.mCount) + 1;
        }
        required = request->mTotalSize;
    }
    range->mSize = required;
    sead::RegistryHeap* heap = mHeap;
    u32 firstAddress;
    if (!heap) {
        range->mAllocatedStorage = nullptr;
        u32 alignment = request->mAlignment;
        u32 begin = range->mBegin;
        firstAddress = (begin + alignment - 1) & ~(alignment - 1);
        range->mFirstAlignedAddress = firstAddress;
    } else {
        u32 target = heap->dispatch()->mAllocate.mFunction;
        u32 alignment = request->mAlignment;
        s32 signedAlignment = static_cast<s32>(fromFront ? alignment : 0u - alignment);
        be<u8>* allocation = gabi::call_ptr<be<u8>*>(target, heap, required, signedAlignment);
        range->mAllocatedStorage = allocation;
        heap = mHeap;
        void* regionStart = gabi::call_ptr<void*>(heap->dispatch()->mRegionStart.mFunction, heap);
        heap = mHeap;
        u32 regionSize = gabi::call_ptr<u32>(heap->dispatch()->mRegionSize.mFunction, heap);
        constexpr u32 kFlushBlockSize = 64;
        u32 flushedSize = (regionSize + kFlushBlockSize - 1) & ~(kFlushBlockSize - 1);
        aglRangeCacheFlushRegion(regionStart, flushedSize);
        alignment = request->mAlignment;
        firstAddress = (gabi::ea(allocation) + alignment - 1) & ~(alignment - 1);
        range->mFirstAlignedAddress = firstAddress;
    }
    u32 secondSize = request->mSecondSize;
    u32 firstSize = request->mFirstSize;
    u32 extraAddress = firstAddress + firstSize;
    be<u32>* extra;
    if (secondSize) {
        u32 alignment = request->mAlignment;
        u32 secondAddress = (extraAddress + alignment - 1) & ~(alignment - 1);
        range->mSecondAlignedAddress = secondAddress;
        extra = request->mExtraAddress;
        secondSize = request->mSecondSize;
        extraAddress = secondAddress + secondSize;
    } else {
        range->mSecondAlignedAddress = 0;
        extra = request->mExtraAddress;
    }
    if (extra) {
        u32 alignment = request->mExtraAlignment;
        *extra = (extraAddress + alignment - 1) & ~(alignment - 1);
    }
    sead::linkAt(range, mAvailableRanges.mOffset)->detach();
    u32 availableCount = mAvailableRanges.mCount;
    u32 usedOffset = mUsedRanges.mOffset;
    mAvailableRanges.mCount = availableCount - 1;
    mUsedRanges.insertBefore(sead::linkAt(range, usedOffset));
    u32 usedCount = mUsedRanges.mCount;
    u32 allocatedSize = mAllocatedSize;
    mUsedRanges.mCount = usedCount + 1;
    mAllocatedSize = allocatedSize + static_cast<u32>(range->mSize);
    return 1;
}
/* 027CB1A0 */
agl::cache::RangeRecord* agl::cache::RangeCache::allocateTexture(
        const TextureData* texture, be<u32>* auxiliaryAddress, u32 fromFront) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mRegionBegin) return nullptr;
    gabi::FrameLocal<RangeAllocationRequest> request(8);
    u32 levels = texture->surface.mMipCount;
    u32 firstSize = texture->surface.mImageBytes;
    u32 alignment = texture->surface.mAlignment;
    request->mFirstSize = firstSize;
    u32 format = texture->format;
    request->mAlignment = alignment;
    u32 secondSize = levels > 1 ? static_cast<u32>(texture->surface.mMipBytes) : 0;
    request->mSecondSize = secondSize;
    u32 extraSize = 0;
    u32 extraAlignment = 4;
    request->mExtraSize = extraSize;
    request->mExtraAlignment = extraAlignment;
    request->mExtraAddress = nullptr;
    constexpr u32 kFirstDepthFormat = 0x39;
    constexpr u32 kLastDepthFormat = 0x3B;
    if (format - kFirstDepthFormat <= kLastDepthFormat - kFirstDepthFormat) {
        if (auxiliaryAddress) {
            aglRangeCacheSetDepthTexture(&mDepthTarget, texture);
            extraAlignment = mDepthTarget.mHiZAlignment;
            request->mExtraAddress = auxiliaryAddress;
            request->mExtraAlignment = extraAlignment;
            secondSize = request->mSecondSize;
            extraSize = mDepthTarget.mHiZSize;
            firstSize = request->mFirstSize;
            alignment = request->mAlignment;
            request->mExtraSize = extraSize;
        }
    } else if (texture->surface.mSamples) {
        aglRangeCacheSetColorTexture(&mColorTarget, texture);
        extraAlignment = mColorTarget.mAuxAlignment;
        firstSize = request->mFirstSize;
        request->mExtraAlignment = extraAlignment;
        request->mExtraAddress = auxiliaryAddress;
        secondSize = request->mSecondSize;
        extraSize = mColorTarget.mAuxSize;
        alignment = request->mAlignment;
        request->mExtraSize = extraSize;
    }
    u32 imageEnd = (firstSize + alignment - 1) & ~(alignment - 1);
    u32 mipmapEnd = imageEnd + secondSize;
    u32 alignedEnd = (mipmapEnd + extraAlignment - 1) & ~(extraAlignment - 1);
    u32 largestAlignment = alignment > extraAlignment ? alignment : extraAlignment;
    u32 totalSize = alignedEnd + extraSize;
    s32 count = mAvailableRanges.mCount;
    request->mTotalSize = totalSize + largestAlignment;
    if (count <= 0) return nullptr;
    sead::Link* link = fromFront ? mAvailableRanges.mNext.get() : mAvailableRanges.mPrevious.get();
    if (!link) return nullptr;
    RangeRecord* range = sead::objectOf<RangeRecord>(link, mAvailableRanges.mOffset);
    while (range) {
        if (allocateRange(range, request, fromFront)) return range;
        u32 offset = mAvailableRanges.mOffset;
        link = sead::linkAt(range, offset);
        link = fromFront ? link->mNext.get() : link->mPrevious.get();
        range = link == static_cast<sead::Link*>(&mAvailableRanges)
            ? nullptr : sead::objectOf<RangeRecord>(link, offset);
    }
    return nullptr;
}
/* WWHD glue
data sRangeCacheTable 1016A7C0
ext 0273AD10 void* allocateRangeCacheStorage(u32)
ext 027B6CB8 agl::ColorSurface* agl::ColorSurface::construct()
ext 027B6E64 agl::DepthSurface* agl::DepthSurface::construct()
ext 0273B524 void sead::LinkList::clear()
ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*, void*)
ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
ext 0273B2D0 sead::Link* sead::Link::insertAfter(sead::Link*)
ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
ext 0273B310 sead::Link* sead::Link::detach()
ext 0273B440 sead::Link* sead::LinkList::popBack()
ext C0008898 void aglRangeCacheFlushRegion(const void*, u32)
ext 025C0928 void aglRangeCacheSetColorTexture(agl::ColorSurface*, const agl::TextureData*)
ext 025C0AB8 void aglRangeCacheSetDepthTexture(agl::DepthSurface*, const agl::TextureData*)
*/
