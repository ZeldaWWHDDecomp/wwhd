/* NW-SND#6: the archive's file-address cache (a file loader that remembers where each archive file
 * sits in memory; sead::AudioArchiveLoader derives from it), and the sound handle's attach/detach.
 * Layout from the constructor 02896898 and the users in this unit. */
#pragma once
#include "nw/snd/ArchiveLoading.h"
namespace nw { namespace snd {
struct FileAddressTable {
    be<u32> count;
    gptr<const void> addresses[1];   // count entries
};
/* An optional user object asked first for a file's address. */
struct FileAddressSourceVtable { VtableEntry header, destroy, fileAddress; };
struct FileAddressSource {
    gptr<FileAddressSourceVtable> vtable;
    const void* fileAddress(u32 id) { return gabi::call_ptr<const void*>(vtable->fileAddress.function, this, id); }
};
struct ArchiveAddressCacheVtable {
    VtableEntry header, destroy, invalidateRange, exchangeAddress, cachedAddress, resolveAddress;
};
struct ArchiveAddressCache {
    /* 0x000 */ be<u32> reserved[2];
    /* 0x008 */ gptr<ArchiveAddressCacheVtable> vtable;
    /* 0x00C */ ArchiveFileLoader loader;            // second base: its vtable slots forward here
    /* 0x218 */ gptr<FileAddressTable> table;
    /* 0x21C */ gptr<FileAddressSource> source;

    static ArchiveAddressCache* create(ArchiveAddressCache* cache);
    static void destroy(ArchiveAddressCache* cache, u32 flags);
    u32 requiredTableBytes(SoundArchive* archive);
    u32 initializeTable(SoundArchive* archive, be<u32>* cursor, u32 end);
    u32 initialize(SoundArchive* archive, u32 buffer, u32 size);
    void finalize();
    const void* fileAddress(u32 id);
    void invalidateRange(u32 first, u32 last);
    const void* exchangeAddress(u32 id, const void* address);
    const void* cachedAddress(u32 id);
    const void* resolveAddress(u32 id);
    /* virtual dispatch */
    const void* cachedAddressVirtual(u32 id) {
        return gabi::call_ptr<const void*>(vtable->cachedAddress.function, this, id);
    }
};
WWHD_OFFSET(ArchiveAddressCache, loader, 0xC);
WWHD_OFFSET(ArchiveAddressCache, table, 0x218);
WWHD_OFFSET(ArchiveAddressCache, source, 0x21C);
WWHD_SIZE(ArchiveAddressCache, 0x220);
WWHD_OFFSET(ArchiveAddressCacheVtable, resolveAddress, 0x28);
WWHD_OFFSET(FileAddressSourceVtable, fileAddress, 0x10);

/* The command the cache sends to the sound thread when it attaches or detaches its table. */
enum ArchiveCacheCommandId { ArchiveCacheCommand_Attach = 0x47, ArchiveCacheCommand_Detach = 0x48 };
struct ArchiveCacheCommand : DriverCommand { gptr<ArchiveAddressCache> cache; };
WWHD_SIZE(ArchiveCacheCommand, 0x14);
} }
