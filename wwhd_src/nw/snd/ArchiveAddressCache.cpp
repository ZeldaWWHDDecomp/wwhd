/* NW-SND#6: the archive file-address cache and the sound handle's attach/detach.
 * Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/ArchiveStartContext.h"
using namespace nw::snd;

// Code-defined GHS vtables (checked against the image by the glue). The loader table's entries
// carry the -0xC this adjustment of the second base.
static u32 sAddressProviderVtable[6] = {0, 0, 0, 0, 0, 0x028F02E4};
static u32 sAddressCacheVtable[12] = {
    0, 0, 0, 0x02896740, 0, 0x02896B6C, 0, 0x028967A0, 0, 0x028967D0, 0, 0x02896808
};
static u32 sAddressCacheLoaderVtable[10] = {
    0x000C0000, 0, 0xFFF40000, 0x028F036C, 0xFFF40000, 0x028967C8,
    0xFFF40000, 0x02896800, 0xFFF40000, 0x02896890
};

/* 02896898 */
ArchiveAddressCache* ArchiveAddressCache::create(ArchiveAddressCache* cache) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (cache == NULL) cache = (ArchiveAddressCache*)utAllocate(sizeof(ArchiveAddressCache));
    if (cache != NULL) {
        cache->reserved[1] = 0;
        cache->reserved[0] = 0;
        cache->vtable = (ArchiveAddressCacheVtable*)sAddressProviderVtable;
        ArchiveLoader::create(&cache->loader);
        cache->table = NULL;
        cache->source = NULL;
        cache->loader.vtable = (ArchiveLoaderVtable*)sAddressCacheLoaderVtable;
        cache->vtable = (ArchiveAddressCacheVtable*)sAddressCacheVtable;
    }
    return cache;
}

/* 02896740 */
void ArchiveAddressCache::destroy(ArchiveAddressCache* cache, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (cache != NULL) {
        cache->loader.vtable = (ArchiveLoaderVtable*)sAddressCacheLoaderVtable;
        ArchiveLoader::destroy(&cache->loader, 0);
        if (flags & 1) utDelete(cache);
    }
}

/* 02896920 */
u32 ArchiveAddressCache::requiredTableBytes(SoundArchive* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    return ((archive->fileCount() << 2) + 7) & ~3u;
}

/* 02896950 */
u32 ArchiveAddressCache::initializeTable(SoundArchive* archive, be<u32>* cursor, u32 end) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = archive->fileCount();
    u32 begin = *cursor;
    u32 limit = (begin + (count << 2) + 7) & ~3u;
    if (limit > end) return 0;
    table = gabi::at<FileAddressTable>(begin);
    *cursor = limit;
    FileAddressTable* addresses = table;
    u32 files = archive->fileCount();
    addresses->count = files;
    addresses = table;
    u32 total = addresses->count;
    for (u32 i = 0; i < total; ++i) {
        addresses->addresses[i] = NULL;
        addresses = table;
        total = addresses->count;
    }
    return 1;
}

/* 02896A40 */
u32 ArchiveAddressCache::initialize(SoundArchive* archive, u32 buffer, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> cursor(8);
    *cursor = buffer;
    if (!initializeTable(archive, cursor.get(), buffer + size)) return 0;
    loader.setArchive(archive);
    DriverCommandManager* manager = DriverCommandManager::instance();
    ArchiveCacheCommand* command = (ArchiveCacheCommand*)manager->allocCommand(sizeof(ArchiveCacheCommand));
    command->cache = this;
    command->id = ArchiveCacheCommand_Attach;
    manager->pushCommand(command);
    return 1;
}

/* 02896AE0 */
void ArchiveAddressCache::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager = DriverCommandManager::instance();
    ArchiveCacheCommand* command = (ArchiveCacheCommand*)manager->allocCommand(sizeof(ArchiveCacheCommand));
    command->cache = this;
    command->id = ArchiveCacheCommand_Detach;
    manager->pushCommand(command);
    u32 serial = manager->submit(1);
    manager->waitFor(serial);
    source = NULL;
    table = NULL;
}

/* 02896B5C */
const void* ArchiveAddressCache::fileAddress(u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    return gabi::tail_ptr<const void*>(vtable->resolveAddress.function, this, id);
}

/* 028967A0 */
const void* ArchiveAddressCache::exchangeAddress(u32 id, const void* address) {
    gabi::AutoFrame frame{gabi::current_function()};
    FileAddressTable* addresses = table;
    if (addresses == NULL) return NULL;
    const void* previous = addresses->addresses[id];
    addresses->addresses[id] = address;
    return previous;
}

/* The loader-side entries of the cache: GHS this-adjusting thunks into the cache's own methods. */
static inline ArchiveAddressCache* cacheOfLoader(ArchiveFileLoader* loader) {
    return nodeOwner<ArchiveAddressCache>(loader, offsetof(ArchiveAddressCache, loader));
}

/* 028967C8 */
static const void* loaderExchangeAddress(ArchiveFileLoader* loader, u32 id, const void* address) {
    gabi::AutoFrame frame{gabi::current_function()};
    return cacheOfLoader(loader)->exchangeAddress(id, address);
}

/* 028967D0 */
const void* ArchiveAddressCache::cachedAddress(u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    FileAddressTable* addresses = table;
    if (addresses == NULL || id >= (u32)addresses->count) return NULL;
    return addresses->addresses[id];
}

/* 02896800 */
static const void* loaderCachedAddress(ArchiveFileLoader* loader, u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    return cacheOfLoader(loader)->cachedAddress(id);
}

/* 02896808 */
const void* ArchiveAddressCache::resolveAddress(u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    FileAddressSource* user = source;
    if (user != NULL) {
        const void* address = user->fileAddress(id);
        if (address != NULL) return address;
    }
    const void* address = loader.loadedFileAddress(id);
    if (address != NULL) return address;
    return cachedAddressVirtual(id);
}

/* 02896890 */
static const void* loaderResolveAddress(ArchiveFileLoader* loader, u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    return cacheOfLoader(loader)->resolveAddress(id);
}

/* 02896B6C */
void ArchiveAddressCache::invalidateRange(u32 first, u32 last) {
    gabi::AutoFrame frame{gabi::current_function()};
    FileAddressTable* addresses = table;
    if (addresses != NULL) {
        u32 count = addresses->count;
        for (u32 i = 0; i < count; ++i) {
            u32 address = gabi::ea((const void*)addresses->addresses[i]);
            if (first <= address && address <= last) {
                addresses->addresses[i] = NULL;
                addresses = table;
                count = addresses->count;
            }
        }
    }
    SoundArchive* archive = loader.archive;
    if (archive == NULL) return;
    u32 remaining = archive->waveArchiveCount();
    u32 index = 0;
    gabi::FrameLocal<ArchiveWaveArchiveSummary> summary(8);
    gabi::FrameLocal<WaveArchiveReader> reader(0x14);
    while (remaining != 0) {
        summary->fileId = 0xFFFFFFFF;
        summary->loadingFlags = 0;
        if (archive->readWaveArchiveInfo(((u32)ArchiveItem_WaveArchive << 24) | index, summary.get()) && summary->loadingFlags != 0) {
            const void* data = cachedAddressVirtual(summary->fileId);
            if (data != NULL) {
                WaveArchiveReader::construct(reader.get(), static_cast<const WaveArchiveHeader*>(data), 1);
                u32 count = summary->option;
                for (u32 wave = 0; wave < count; ++wave) {
                    const void* address = reader->loadedAddress(wave);
                    if (address != NULL && first <= gabi::ea(address) && gabi::ea(address) <= last) {
                        reader->exchangeAddress(wave, NULL);
                    }
                    count = summary->option;
                }
            }
        }
        --remaining;
        ++index;
    }
}

/* 02896CD8 */
void SoundHandle::detachSound() {
    gabi::AutoFrame frame{gabi::current_function()};
    BasicSound* attached = sound;
    if (attached != NULL) {
        if (attached->generalHandle == this) {
            attached->generalHandle = NULL;
            attached = sound;
        }
        if (attached->tempGeneralHandle == this) attached->tempGeneralHandle = NULL;
        sound = NULL;
    }
}

/* 02896D24 */
void SoundHandle::attachSound(BasicSound* value) {
    gabi::AutoFrame frame{gabi::current_function()};
    sound = value;
    if (value->isAttachedGeneralHandle()) sound->detachGeneralHandle();
    sound->generalHandle = this;
}

/* 02896D70 */
void SoundHandle::attachTempSound(BasicSound* value) {
    gabi::AutoFrame frame{gabi::current_function()};
    sound = value;
    if (value->isAttachedTempGeneralHandle()) sound->detachTempGeneralHandle();
    sound->tempGeneralHandle = this;
}

/* WWHD glue
 data sAddressProviderVtable 10182200
 data sAddressCacheVtable 10182218
 data sAddressCacheLoaderVtable 10182248
 ext 0273AD10 void* utAllocate(u32)
 ext 0273AF40 void utDelete(void*)
 ext 02890F88 nw::snd::ArchiveLoader* nw::snd::ArchiveLoader::create(nw::snd::ArchiveLoader*)
 ext 02890FD0 void nw::snd::ArchiveLoader::destroy(nw::snd::ArchiveLoader*,u32)
 ext 02892980 void nw::snd::ArchiveFileLoader::setArchive(nw::snd::SoundArchive*)
 ext 02892988 const void* nw::snd::ArchiveFileLoader::loadedFileAddress(u32)
 ext 02885F10 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::instance()
 ext 02887248 nw::snd::DriverCommand* nw::snd::DriverCommandManager::allocCommand(u32)
 ext 028870F0 u32 nw::snd::DriverCommandManager::pushCommand(nw::snd::DriverCommand*)
 ext 02887314 u32 nw::snd::DriverCommandManager::submit(u32)
 ext 028871BC void nw::snd::DriverCommandManager::waitFor(u32)
 ext 0288E90C u32 nw::snd::SoundArchive::fileCount()
 ext 0288E904 u32 nw::snd::SoundArchive::waveArchiveCount()
 ext 0288ED64 u32 nw::snd::SoundArchive::readWaveArchiveInfo(u32,nw::snd::ArchiveWaveArchiveSummary*)
 ext 0289FB20 nw::snd::WaveArchiveReader* nw::snd::WaveArchiveReader::construct(nw::snd::WaveArchiveReader*,const nw::snd::WaveArchiveHeader*,u32)
 ext 0289FD24 const void* nw::snd::WaveArchiveReader::loadedAddress(u32)
 ext 0289FDC8 const void* nw::snd::WaveArchiveReader::exchangeAddress(u32,const void*)
 ext 028856F4 u32 nw::snd::BasicSound::isAttachedGeneralHandle()
 ext 02885704 void nw::snd::BasicSound::detachGeneralHandle()
 ext 0288570C u32 nw::snd::BasicSound::isAttachedTempGeneralHandle()
 ext 0288571C void nw::snd::BasicSound::detachTempGeneralHandle()
*/
