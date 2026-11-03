/* Sequence file cache views reconstructed from WWHD accesses. */
#pragma once
#include "nw/snd/SoundArchive.h"
namespace nw { namespace snd {
struct SequenceFileCacheEntry { be<u32> mId; gptr<void> mAddress; };
struct SequenceFileCacheVtable { VtableEntry header,destroy,clear,exchange,find,resolve; };
struct SequenceFileCache {
    be<u32> mReserved[2];
    gptr<SequenceFileCacheVtable> mVtable;
    ArchiveLoader mLoader;
    SequenceFileCacheEntry mEntries[9];
    be<u8> mActive;
    be<u8> mReleased;
    u8 mPadding[2];
    static SequenceFileCache* construct(SequenceFileCache* cache);
    void destroy(u32 flags);
    void finalize();
    void initialize(SoundArchive* archive);
    void clear();
    void* exchange(u32 id,void* address);
    void* find(u32 id) const;
    void* findVirtual(u32 id);
    static void* findVirtualAdjusted(ArchiveLoader* loader,u32 id);
    static void* exchangeAdjusted(ArchiveLoader* loader,u32 id,void* address);
    static void* findAdjusted(const ArchiveLoader* loader,u32 id);
};
static_assert(sizeof(SequenceFileCache)==0x264);
static_assert(offsetof(SequenceFileCache,mLoader)==0xC);
static_assert(offsetof(SequenceFileCache,mLoader)+offsetof(ArchiveLoader,vtable)==0x214);
static_assert(offsetof(SequenceFileCache,mEntries)==0x218);
static_assert(offsetof(SequenceFileCache,mActive)==0x260);
static_assert(offsetof(SequenceFileCache,mReleased)==0x261);
}}
