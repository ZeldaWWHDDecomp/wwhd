// agl dynamic texture cache ("Icon=TEXTURE"): a singleton (pointer 101F8DD4) with one pool per CPU core plus
// one shared pool. Each pool holds 128 texture entries and four range pools for their memory; acquire()
// reuses a free entry with the same description or the oldest free one, and allocates its image memory.
// GX2 surface dimension and tile-mode values: wut facts.
#pragma once
#include "agl/TextureData.h"
#include "agl/RenderRecording.h"
#include "sead/Thread.h"
namespace agl {

// AGL#17's range pool (0x2E8): only the members and calls this unit uses.
struct RangeAllocation_l { u8 mLink[0xC]; gptr<void> mImage; gptr<void> mMipImage; };
struct RangePool_l {
    u8 mHeader[8];
    be<u32> mCapacity;
    u8 mState[0x2DC];
    static RangePool_l* create(RangePool_l* storage);
    void destroy(u32 flags);
    void configure(u32 base, u32 size, u32 count, sead::RegistryHeap* heap);
    RangeAllocation_l* allocate(struct TextureCacheEntry* texture, u32 flags, u8 sameCoreOnly);
    void release(RangeAllocation_l* allocation);
    static u32 compare(RangePool_l* left, RangePool_l* right);
};

struct TextureCachePool;

// A cached texture: its description, where its memory came from and its reuse state.
struct TextureCacheEntry : TextureData {
    gptr<RangePool_l> mRangePool;
    gptr<RangeAllocation_l> mAllocation;
    sead::SafeText mName;
    be<u32> mRangeCapacity;
    be<u8> mRangePoolIndex;
    be<u8> mAge;
    be<u8> mMipCount;
    be<u8> mFlags;
    be<u8> mSwizzle;
    be<u8> mMarker;
    be<u8> mTileMode;
    u8 mPadding;
    gptr<TextureCachePool> mPool;
};
// entry flags: allocate from range pool 0, or from range pool 1, whatever the request says
enum : u8 { TextureCacheEntry_UseRangePool0 = 1, TextureCacheEntry_UseRangePool1 = 2 };

struct TextureCachePool {
    static constexpr s32 kEntryCount = 128;
    static constexpr u32 kRangePoolCount = 4;
    be<s32> mEntryCount;
    gptr<TextureCacheEntry> mEntries;
    RangePool_l mRangePools[kRangePoolCount];
    DisplayListRecording mDisplayList;
    be<u32> mDisplayListOffset;
    be<u32> mDisplayListUsed;
    be<u32> mCoreFlags;
    static TextureCachePool* create(TextureCachePool* storage);
    void destroy(u32 flags);
};
enum : u32 { TextureCachePool_SkipSurfaceUpdate = 1 };

struct DynamicTextureCacheMethods { sead::VtableSlot mHeader, mReserved, mDestroy; };
struct DynamicTextureCacheDisposer : sead::RegisteredObject { void destroy(u32 flags); };

struct DynamicTextureCache {
    static constexpr u32 kCorePoolCount = 3;
    static constexpr u32 kSharedPool = 3;
    gptr<DynamicTextureCacheMethods> mMethods;
    DynamicTextureCacheDisposer mDisposer;
    be<u32> mFlags;
    TextureCachePool mPools[kCorePoolCount + 1];
    sead::CriticalSection mSharedLock;
    be<u32> mReservedA;
    be<u32> mReservedB;
    static DynamicTextureCache* create(DynamicTextureCache* storage);
    static DynamicTextureCache* instance(sead::RegistryHeap* heap);
    void destroy(u32 flags);
    void initialize(sead::RegistryHeap* heap, u32 baseA, u32 sizeA, u32 baseB, u32 sizeB, u32 baseC, u32 sizeC);
    u32 rangePoolsMatchShared(TextureCachePool* pool);
    u32 rangePoolsMatch(TextureCachePool* pool);
    TextureCacheEntry* acquire(TextureCachePool* pool, sead::SafeText* name, u32 format, u32 dimension,
        u32 width, u32 height, u32 mipCount, u32 antiAlias, u32 depth, u32 allocationFlags,
        u8 sameCoreOnly, u32 rangePoolIndex, u8 updateSurface, u8 tileMode);
    TextureCacheEntry* acquire2D(sead::SafeText* name, u32 format, u32 width, u32 height, u32 mipCount,
        u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode);
    TextureCacheEntry* acquire2DArray(sead::SafeText* name, u32 format, u32 width, u32 height, u32 depth, u32 mipCount,
        u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode);
    TextureCacheEntry* acquireShared2D(sead::SafeText* name, u32 format, u32 width, u32 height, u32 mipCount,
        u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode);
    TextureCacheEntry* acquireShared2DArray(sead::SafeText* name, u32 format, u32 width, u32 height, u32 depth, u32 mipCount,
        u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode);
    TextureCacheEntry* acquireSharedMultisample(sead::SafeText* name, u32 format, u32 width, u32 height, u32 antiAlias,
        u32 allocationFlags, u32 rangePoolIndex, u8 updateSurface, u8 tileMode);
    u32 releaseFromPool(TextureCachePool* pool, TextureCacheEntry* texture);
    void release(TextureCacheEntry* texture);
    void releaseShared(TextureCacheEntry* texture);
};
enum : u32 { DynamicTextureCache_ForceLinearTiling = 8 };
// GX2 values used here (wut facts)
enum : u32 {
    GX2SurfaceDim_2D = 1, GX2SurfaceDim_3D = 2, GX2SurfaceDim_Cube = 3, GX2SurfaceDim_2DArray = 5, GX2SurfaceDim_2DMSAA = 6,
    GX2TileMode_LinearAligned = 1,
};

// text copies emitted by this translation unit (only their out-of-line virtuals)
namespace texture_cache_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };
}
}
// The singleton and its registered disposer, owned by agl/DynamicTextureCacheStatics.cpp.
extern gptr<agl::DynamicTextureCache>& sDynamicTextureCache;
extern gptr<agl::DynamicTextureCacheDisposer>& sDynamicTextureCacheDisposer;
WWHD_OFFSET(agl::RangeAllocation_l, mImage, 0xC);
WWHD_OFFSET(agl::RangePool_l, mCapacity, 8);
WWHD_SIZE(agl::RangePool_l, 0x2E8);
WWHD_OFFSET(agl::TextureCacheEntry, mRangePool, 0x90);
WWHD_OFFSET(agl::TextureCacheEntry, mAllocation, 0x94);
WWHD_OFFSET(agl::TextureCacheEntry, mName, 0x98);
WWHD_OFFSET(agl::TextureCacheEntry, mRangeCapacity, 0xA0);
WWHD_OFFSET(agl::TextureCacheEntry, mFlags, 0xA7);
WWHD_OFFSET(agl::TextureCacheEntry, mTileMode, 0xAA);
WWHD_OFFSET(agl::TextureCacheEntry, mPool, 0xAC);
WWHD_SIZE(agl::TextureCacheEntry, 0xB0);
WWHD_OFFSET(agl::TextureCachePool, mRangePools, 8);
WWHD_OFFSET(agl::TextureCachePool, mDisplayList, 0xBA8);
WWHD_OFFSET(agl::TextureCachePool, mDisplayListOffset, 0xBC4);
WWHD_OFFSET(agl::TextureCachePool, mCoreFlags, 0xBCC);
WWHD_SIZE(agl::TextureCachePool, 0xBD0);
WWHD_OFFSET(agl::DynamicTextureCacheMethods, mDestroy, 0x10);
WWHD_OFFSET(agl::DynamicTextureCache, mDisposer, 4);
WWHD_OFFSET(agl::DynamicTextureCache, mFlags, 0x14);
WWHD_OFFSET(agl::DynamicTextureCache, mPools, 0x18);
WWHD_OFFSET(agl::DynamicTextureCache, mSharedLock, 0x2F58);
WWHD_OFFSET(agl::DynamicTextureCache, mReservedA, 0x2F94);
WWHD_SIZE(agl::DynamicTextureCache, 0x2F9C);
