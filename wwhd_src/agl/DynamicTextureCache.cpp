// agl dynamic texture cache: construction of the singleton and its per-core pools, acquiring a texture of a
// given description (reusing a matching or the oldest free entry and allocating its memory from a range pool),
// releasing it, and teardown; plus this translation unit's text copies. Written from WWHD's instructions;
// GX2 surface values: wut facts.
#include "agl/DynamicTextureCache.h"
#include "sead/PoolHeap.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void destroyRegistered(sead::RegisteredObject* object, u32 flags);
static void* allocateInHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
static sead::RegistryHeap* currentHeap(sead::RegistryManagerState* manager);
static sead::RegistryHeap* findHeap(sead::RegistryManagerState* manager, void* allocation);
static void registerInHeap(sead::RegisteredObject* object, sead::RegistryHeap* heap, u32 selection);
static void constructArray(void* objects, u32 count, u32 stride, u32 constructor);
static void destroyArray(void* objects, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved);
static void destroyPool(TextureCachePool* pool, u32 flags);
static void destroyRangePool(RangePool_l* range, u32 flags);
static void setDisplayName(DynamicTextureCache* cache, sead::SafeText* name);
static u32 currentCore();
static void setSurfaceSwizzle(sead::GraphicsSurface* surface, u32 swizzle);
static DynamicTextureCacheMethods sDynamicTextureCacheMethods;
static sead::DisposerVtable sDisposerMethods;
static sead::SafeTextVtable sTextMethods;

/* inlined: the pool of the calling core */
static u32 currentCorePool() {
    return sCoreReverse[currentCore()];
}

/* 0279F218 */
DynamicTextureCache* DynamicTextureCache::create(DynamicTextureCache* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    DynamicTextureCache* cache = storage;
    if (cache == nullptr) cache = static_cast<DynamicTextureCache*>(allocateObject(sizeof(DynamicTextureCache)));
    if (cache == nullptr) return cache;
    cache->mMethods = &sDynamicTextureCacheMethods;
    be<u32>* flags = &cache->mFlags;
    if (flags == nullptr) flags = static_cast<be<u32>*>(allocateObject(sizeof(be<u32>)));
    if (flags != nullptr) *flags = 0;
    TextureCachePool* pools = cache->mPools;
    if (pools == nullptr) pools = static_cast<TextureCachePool*>(allocateObject(sizeof(cache->mPools)));
    if (pools != nullptr) constructArray(pools, kCorePoolCount + 1, sizeof(TextureCachePool),
        gabi::guest_fn(reinterpret_cast<const void*>(&TextureCachePool::create)));
    sead::CriticalSection::construct(&cache->mSharedLock);
    cache->mReservedA = 0;
    cache->mReservedB = 0;
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mVtable = &sTextMethods;
    name->mText.v = gabi::guest_str("Icon=TEXTURE");
    setDisplayName(cache, name);
    return cache;
}

/* 0279F2EC */
DynamicTextureCache* DynamicTextureCache::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    DynamicTextureCache* cache = sDynamicTextureCache;
    if (cache != nullptr) return cache;
    cache = static_cast<DynamicTextureCache*>(allocateInHeap(sizeof(DynamicTextureCache), heap, 4));
    // GHS tests the member address itself (also for a failed allocation).
    DynamicTextureCacheDisposer* disposer =
        gabi::at<DynamicTextureCacheDisposer>(gabi::ea(cache) + offsetof(DynamicTextureCache, mDisposer));
    if (disposer != nullptr) {
        registerInHeap(disposer, heap, 3);
        disposer->mVtable = &sDisposerMethods;
    }
    sDynamicTextureCacheDisposer = disposer;
    if (cache != nullptr) cache = create(cache);
    sDynamicTextureCache = cache;
    return cache;
}

/* 0279F38C */
void DynamicTextureCache::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mMethods = &sDynamicTextureCacheMethods;
    for (TextureCachePool* pool = mPools; pool != mPools + kCorePoolCount + 1; ++pool) {
        TextureCacheEntry* entries = pool->mEntries;
        if (entries != nullptr) {
            sead::RegistryHeap* heap = findHeap(sRegistryManager, entries);
            gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, pool->mEntries.get());
            pool->mEntryCount = 0;
            pool->mEntries = nullptr;
        }
        pool->mDisplayList.release();
        pool->mDisplayListOffset = 0;
        pool->mDisplayListUsed = 0;
    }
    mSharedLock.destroy(2);
    destroyArray(mPools, kCorePoolCount + 1, sizeof(TextureCachePool), gabi::guest_fn((const void*)destroyPool), 0, 0);
    if (flags & 1) releaseObject(this);
}

/* 0279F474 */
void DynamicTextureCache::initialize(sead::RegistryHeap* heap, u32 baseA, u32 sizeA, u32 baseB, u32 sizeB, u32 baseC, u32 sizeC) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (TextureCachePool* pool = mPools; pool != mPools + kCorePoolCount + 1; ++pool) {
        sead::RegistryHeap* entryHeap = heap;
        if (entryHeap == nullptr) entryHeap = currentHeap(sRegistryManager);
        TextureCacheEntry* entries = gabi::call_ptr<TextureCacheEntry*>(entryHeap->dispatch()->mAllocate.mFunction,
            entryHeap, static_cast<u32>(TextureCachePool::kEntryCount * sizeof(TextureCacheEntry)), 4);
        for (s32 index = 0; index < TextureCachePool::kEntryCount; ++index) {
            TextureCacheEntry* entry = &entries[index];
            if (entry == nullptr) continue;
            TextureData::create(entry);
            entry->mName.mText.v = gabi::guest_str("");
            entry->mName.mVtable = &sTextMethods;
            be<u8>* entryFlags = &entry->mFlags;
            if (entryFlags == nullptr) entryFlags = static_cast<be<u8>*>(allocateObject(1));
            if (entryFlags != nullptr) *entryFlags = 0;
        }
        if (entries != nullptr) {
            pool->mEntries = entries;
            pool->mEntryCount = TextureCachePool::kEntryCount;
        }
        pool->mRangePools[0].configure(baseA, sizeA, pool->mEntryCount, heap);
        pool->mRangePools[1].configure(baseB, sizeB, pool->mEntryCount, heap);
        pool->mRangePools[2].configure(baseC, sizeC, pool->mEntryCount, heap);
        pool->mRangePools[3].configure(0, 0x40000000, pool->mEntryCount, heap);
        TextureCacheEntry* entry = pool->mEntries;
        TextureCacheEntry* end = entry + static_cast<s32>(pool->mEntryCount);
        for (; entry != end; ++entry) {
            entry->mRangeCapacity = 0;
            entry->mRangePool = nullptr;
            entry->mFlags = 0;
            entry->mSwizzle = 0;
            entry->mPool = nullptr;
            entry->mMipCount = 0;
            entry->mTileMode = 0;
            entry->mMarker = 0xEE;
            entry->mAllocation = nullptr;
            entry->mRangePoolIndex = 0;
            entry->mAge = 0;
        }
        void* storage = allocateInHeap(0x4000, heap, 0x20);
        pool->mDisplayList.setStorage(storage, 0x4000);
    }
}

/* 0279F6B0 */
u32 DynamicTextureCache::rangePoolsMatchShared(TextureCachePool* pool) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureCachePool* shared = &mPools[kSharedPool];
    if (RangePool_l::compare(&pool->mRangePools[0], &shared->mRangePools[0]) != 0) return 0;
    if (RangePool_l::compare(&pool->mRangePools[1], &shared->mRangePools[1]) != 0) return 0;
    return RangePool_l::compare(&pool->mRangePools[2], &shared->mRangePools[2]) == 0;
}

/* 0279F738 */
u32 DynamicTextureCache::rangePoolsMatch(TextureCachePool* pool) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (pool != nullptr) return rangePoolsMatchShared(pool);
    for (u32 index = 0; index < kCorePoolCount; ++index) {
        if (rangePoolsMatchShared(&mPools[index]) == 0) return 0;
    }
    return 1;
}

/* inlined into acquire: an allocation's image and mip pointers. A failed or released allocation is NULL
   and the original reads them through it all the same (guest 0xC / 0x10); a plain member read of a NULL
   pointer would let the host compiler drop the NULL checks above, hence the explicit guest loads. */
static void* allocationImage(RangeAllocation_l* allocation) {
    return gabi::at<void>(gabi::load<u32>(gabi::ea(allocation) + offsetof(RangeAllocation_l, mImage)));
}
static void* allocationMipImage(RangeAllocation_l* allocation) {
    return gabi::at<void>(gabi::load<u32>(gabi::ea(allocation) + offsetof(RangeAllocation_l, mMipImage)));
}

/* inlined into acquire: a size below the entry's minimum counts as the minimum */
static u32 effectiveSize(s32 size, u8 minimum) {
    return minimum > size ? minimum : static_cast<u32>(size);
}

/* 0279F7F0 */
TextureCacheEntry* DynamicTextureCache::acquire(TextureCachePool* pool, sead::SafeText* name, u32 format, u32 dimension,
    u32 width, u32 height, u32 mipCount, u32 antiAlias, u32 depth, u32 allocationFlags,
    u8 sameCoreOnly, u32 rangePoolIndex, u8 updateSurface, u8 tileMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (static_cast<u32>(mFlags) & DynamicTextureCache_ForceLinearTiling) tileMode = GX2TileMode_LinearAligned;
    // Look for a free entry with the same description; age the others and remember the oldest.
    TextureCacheEntry* oldest = nullptr;
    TextureCacheEntry* entry = pool->mEntries;
    u32 oldestAge = 0xFFFF;
    s32 count = pool->mEntryCount;
    bool matched = false;
    for (s32 index = 0; index < count; ++index, ++entry) {
        if (entry->mAllocation != nullptr) continue;
        const sead::GraphicsSurface& surface = entry->surface;
        if (static_cast<u32>(entry->format) == format && static_cast<u32>(surface.mDimension) == dimension &&
            effectiveSize(surface.mWidth, entry->minimumWidth) == width &&
            effectiveSize(surface.mHeight, entry->minimumHeight) == height &&
            static_cast<u32>(surface.mSamples) == antiAlias &&
            effectiveSize(surface.mDepth, entry->minimumDepth) == depth &&
            static_cast<u32>(entry->mMipCount) == mipCount && static_cast<u32>(entry->mTileMode) == tileMode) {
            matched = true;
            break;
        }
        u32 age = entry->mAge;
        if (age != 0) { age = static_cast<u8>(age - 1); entry->mAge = age; }
        if (age < oldestAge) { oldest = entry; oldestAge = age; }
    }
    if (!matched) {
        entry = oldest;
        entry->mMipCount = mipCount;
        entry->mTileMode = tileMode;
        switch (dimension) {
        case GX2SurfaceDim_2D: entry->configure(GX2SurfaceDim_2D, format, width, height, 1, mipCount, tileMode, 0); break;
        case GX2SurfaceDim_3D: entry->configure(GX2SurfaceDim_3D, format, width, height, depth, mipCount, tileMode, 0); break;
        case GX2SurfaceDim_Cube:
            entry->configure(GX2SurfaceDim_Cube, format, width, height, depth == 1 ? 6 : depth * 6, mipCount, tileMode, 0);
            break;
        case GX2SurfaceDim_2DArray: entry->configure(GX2SurfaceDim_2DArray, format, width, height, depth, mipCount, tileMode, 0); break;
        case GX2SurfaceDim_2DMSAA: entry->configure(GX2SurfaceDim_2DMSAA, format, width, height, 1, 1, tileMode, antiAlias); break;
        }
    }
    u32 entryFlags = entry->mFlags;
    entry->mRangePoolIndex = rangePoolIndex;
    u32 first = 0;
    bool allocate = true;
    if ((entryFlags & TextureCacheEntry_UseRangePool0) == 0) {
        if (entryFlags & TextureCacheEntry_UseRangePool1) rangePoolIndex = 1;
        first = rangePoolIndex;
        // A one-pool range loop [first, first + 1): empty only when first + 1 overflows.
        allocate = static_cast<s32>(rangePoolIndex) < static_cast<s32>(rangePoolIndex + 1);
    }
    RangeAllocation_l* allocation;
    if (allocate) {
        RangePool_l* range = &pool->mRangePools[first];
        entry->mRangePool = range;
        allocation = range->allocate(entry, allocationFlags, sameCoreOnly);
        entry->mAllocation = allocation;
        if (allocation != nullptr) {
            if (rangePoolsMatch(sameCoreOnly != 0 ? pool : nullptr) != 0) {
                allocation = entry->mAllocation;
            } else {
                allocation = entry->mAllocation;
                if (allocation != nullptr) {
                    entry->mRangePool->release(allocation);
                    entry->mAllocation = nullptr;
                }
                allocation = nullptr;
            }
        }
    } else {
        allocation = entry->mAllocation;
    }
    entry->surface.mImage = allocationImage(allocation);
    allocation = entry->mAllocation;
    void* mipImage = allocationMipImage(allocation);
    RangePool_l* range = entry->mRangePool;
    if (mipImage != nullptr) entry->surface.mMipImage = mipImage;
    entry->mRangeCapacity = range->mCapacity;
    entry->mAge = pool->mEntryCount;
    entry->mName.mText = name->mText.get();
    if (updateSurface != 0 && (static_cast<u32>(mPools[currentCorePool()].mCoreFlags) & TextureCachePool_SkipSurfaceUpdate) == 0) {
        entry->updateSurface();
    }
    setSurfaceSwizzle(&entry->surface, entry->mSwizzle);
    entry->mPool = pool;
    return entry;
}

/* 0279FC6C */
TextureCacheEntry* DynamicTextureCache::acquire2D(sead::SafeText* name, u32 format, u32 width, u32 height, u32 mipCount,
    u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    return acquire(&mPools[currentCorePool()], name, format, GX2SurfaceDim_2D, width, height, mipCount, 0, 1,
        allocationFlags, 1, rangePoolIndex, updateSurface, tileMode);
}

/* 0279FD18 */
TextureCacheEntry* DynamicTextureCache::acquire2DArray(sead::SafeText* name, u32 format, u32 width, u32 height, u32 depth,
    u32 mipCount, u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    return acquire(&mPools[currentCorePool()], name, format, GX2SurfaceDim_2DArray, width, height, mipCount, 0, depth,
        allocationFlags, 1, rangePoolIndex, updateSurface, tileMode);
}

/* 0279FDC8 */
TextureCacheEntry* DynamicTextureCache::acquireShared2D(sead::SafeText* name, u32 format, u32 width, u32 height, u32 mipCount,
    u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    mSharedLock.lock();
    TextureCacheEntry* entry = acquire(&mPools[kSharedPool], name, format, GX2SurfaceDim_2D, width, height, mipCount, 0, 1,
        allocationFlags, 0, rangePoolIndex, updateSurface, tileMode);
    mSharedLock.unlock();
    return entry;
}

/* 0279FE74 */
TextureCacheEntry* DynamicTextureCache::acquireShared2DArray(sead::SafeText* name, u32 format, u32 width, u32 height, u32 depth,
    u32 mipCount, u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    mSharedLock.lock();
    TextureCacheEntry* entry = acquire(&mPools[kSharedPool], name, format, GX2SurfaceDim_2DArray, width, height, mipCount, 0,
        depth, allocationFlags, 0, rangePoolIndex, updateSurface, tileMode);
    mSharedLock.unlock();
    return entry;
}

/* 0279FF24 */
TextureCacheEntry* DynamicTextureCache::acquireSharedMultisample(sead::SafeText* name, u32 format, u32 width, u32 height,
    u32 antiAlias, u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    mSharedLock.lock();
    u32 dimension = antiAlias != 0 ? GX2SurfaceDim_2DMSAA : GX2SurfaceDim_2D;
    TextureCacheEntry* entry = acquire(&mPools[kSharedPool], name, format, dimension, width, height, 1, antiAlias, 1,
        allocationFlags, 0, rangePoolIndex, updateSurface, tileMode);
    mSharedLock.unlock();
    return entry;
}

/* 0279FFDC */
u32 DynamicTextureCache::releaseFromPool(TextureCachePool* pool, TextureCacheEntry* texture) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (texture->mPool != pool) return 0;
    RangeAllocation_l* allocation = texture->mAllocation;
    texture->mRangePool->release(allocation);
    texture->mAllocation = nullptr;
    return 1;
}

/* 027A0040 */
void DynamicTextureCache::release(TextureCacheEntry* texture) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (releaseFromPool(&mPools[currentCorePool()], texture) != 0) return;
    // Not from this core's pool: search the other cores' pools under the shared lock.
    mSharedLock.lock();
    for (u32 index = 0; index < kCorePoolCount; ++index) {
        if (index != currentCorePool() && releaseFromPool(&mPools[index], texture) != 0) break;
    }
    mSharedLock.unlock();
}

/* 027A0114 */
void DynamicTextureCache::releaseShared(TextureCacheEntry* texture) {
    gabi::AutoFrame frame{gabi::current_function()};
    mSharedLock.lock();
    releaseFromPool(&mPools[kSharedPool], texture);
    mSharedLock.unlock();
}

/* 027A0174 */
void DynamicTextureCacheDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sDisposerMethods;
    if (this == sDynamicTextureCacheDisposer.get()) {
        DynamicTextureCache* cache = sDynamicTextureCache;
        sDynamicTextureCacheDisposer = nullptr;
        gabi::call_ptr<void>(cache->mMethods->mDestroy.mFunction, cache, 2);
        sDynamicTextureCache = nullptr;
    }
    destroyRegistered(this, 0);
    if (flags & 1) releaseObject(this);
}

/* HD: compiler-generated and template code of this translation unit */

/* 027A021C */
void texture_cache_text::Text::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0230 */
void texture_cache_text::Text::assureTerminationImpl() {}

/* 027A0234 */
void texture_cache_text::TextBuffer::assureTerminationImpl() {
    mText[static_cast<s32>(mCapacity) - 1] = 0;
}

/* 027A024C */
void texture_cache_text::TextBuffer::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0260 */
void texture_cache_text::FixedTextA::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0274 */
void texture_cache_text::FixedTextB::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0288 */
void texture_cache_text::FixedTextC::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A029C */
TextureCachePool* TextureCachePool::create(TextureCachePool* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureCachePool* pool = storage;
    if (pool == nullptr) pool = static_cast<TextureCachePool*>(allocateObject(sizeof(TextureCachePool)));
    if (pool == nullptr) return pool;
    pool->mEntryCount = 0;
    pool->mEntries = nullptr;
    constructArray(pool->mRangePools, kRangePoolCount, sizeof(RangePool_l),
        gabi::guest_fn(reinterpret_cast<const void*>(&RangePool_l::create)));
    DisplayListRecording::create(&pool->mDisplayList);
    be<u32>* coreFlags = &pool->mCoreFlags;
    if (coreFlags == nullptr) coreFlags = static_cast<be<u32>*>(allocateObject(sizeof(be<u32>)));
    if (coreFlags != nullptr) *coreFlags = 0;
    return pool;
}

/* 027A032C */
void TextureCachePool::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mDisplayList.destroy(2);
    destroyArray(mRangePools, kRangePoolCount, sizeof(RangePool_l), gabi::guest_fn((const void*)destroyRangePool), 0, 0);
    if (flags & 1) releaseObject(this);
}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object) callback
ext 0273B0D4 void* allocateInHeap(u32 size, sead::RegistryHeap* heap, s32 alignment)
ext 02756140 sead::RegistryHeap* currentHeap(sead::RegistryManagerState* manager)
ext 02755FEC sead::RegistryHeap* findHeap(sead::RegistryManagerState* manager, void* allocation)
ext 02752B0C void registerInHeap(sead::RegisteredObject* object, sead::RegistryHeap* heap, u32 selection)
ext 02752BEC void destroyRegistered(sead::RegisteredObject* object, u32 flags)
ext 028EFFD0 void constructArray(void* objects, u32 count, u32 stride, u32 constructor)
ext 028F0164 void destroyArray(void* objects, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved)
ext 027A032C void destroyPool(TextureCachePool* pool, u32 flags) callback
ext 027CAC9C void destroyRangePool(RangePool_l* range, u32 flags) callback
ext 02785CB4 void setDisplayName(DynamicTextureCache* cache, sead::SafeText* name)
ext C0009BB0 u32 currentCore()
ext C00069B8 void setSurfaceSwizzle(sead::GraphicsSurface* surface, u32 swizzle)
ext 02760084 sead::CriticalSection* sead::CriticalSection::construct(sead::CriticalSection* object)
ext 02760168 void sead::CriticalSection::destroy(u32 flags)
ext 027601BC void sead::CriticalSection::lock()
ext 027601F0 void sead::CriticalSection::unlock()
ext 027BE6B8 agl::TextureData* agl::TextureData::create(TextureData*)
ext 027BE930 void agl::TextureData::configure(u32 dimension, u32 format, u32 width, u32 height, u32 depth, s32 mipCount, u32 tileMode, u32 antiAlias)
ext 027BE794 void agl::TextureData::updateSurface()
ext 027C05A0 void agl::DisplayListRecording::setStorage(void* data, u32 capacity)
ext 027C05F4 agl::DisplayListRecording* agl::DisplayListRecording::create(DisplayListRecording* storage)
ext 027C0680 void agl::DisplayListRecording::release()
ext 027C06C4 void agl::DisplayListRecording::destroy(u32 flags)
ext 027CAB58 agl::RangePool_l* agl::RangePool_l::create(RangePool_l* storage) callback
ext 027CAD30 void agl::RangePool_l::configure(u32 base, u32 size, u32 count, sead::RegistryHeap* heap)
ext 027CB1A0 agl::RangeAllocation_l* agl::RangePool_l::allocate(TextureCacheEntry* texture, u32 flags, u8 sameCoreOnly)
ext 027CB3B8 void agl::RangePool_l::release(RangeAllocation_l* allocation)
ext 027CB744 u32 agl::RangePool_l::compare(RangePool_l* left, RangePool_l* right)
data sDynamicTextureCacheMethods 10167E9C
data sDisposerMethods 10167E8C
data sTextMethods 10167DD8
str 10167E70 "Icon=TEXTURE" in 0279F218
str 10000160 "" in 0279F474
*/
