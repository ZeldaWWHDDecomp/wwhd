/* Archive file loading. Recovered from WWHD; own names and control flow. */
#include "nw/snd/ArchiveLoading.h"
using namespace nw::snd;
static u32 sWaveArchiveAddressMarker;

/* 02891214 */
void* ArchiveFileLoader::loadWholeFile(u32 id, ArchiveAllocator_l* allocator, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveFileSummary> info(8);
    info->size = 0xFFFFFFFFu;
    info->offset = 0xFFFFFFFFu;
    info->externalPath = nullptr;
    if (!readArchiveFileInfo(archive, id, info.get())) return nullptr;
    u32 bytes = info->size;
    if (!bytes || bytes == 0xFFFFFFFFu) return nullptr;
    void* buffer = gabi::call_ptr<void*>(allocator->vtable->allocate.function, allocator, bytes);
    if (!buffer) return nullptr;
    if ((u32)readArchiveFilePart(this, id, buffer, (s32)bytes, 0, chunkBytes) != bytes) return nullptr;
    flushArchiveBuffer(buffer, bytes);
    return buffer;
}

/* 028912F0 */
void* ArchiveFileLoader::loadCachedFile(u32 id, ArchiveAllocator_l* allocator, s32 chunkBytes, u32 unused) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* buffer = const_cast<void*>(fileAddress(id));
    if (!buffer) {
        buffer = loadWholeFile(id, allocator, chunkBytes);
        if (buffer) storeFileAddress(id, buffer);
    }
    return buffer;
}

/* 02891380 */
void* ArchiveFileLoader::loadWaveArchive(u32 id, ArchiveAllocator_l* allocator, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveWaveArchiveSummary> info(8);
    gabi::FrameLocal<WaveArchiveReader> reader(0x14);
    gabi::FrameLocal<ArchiveWaveHeaderScratch_l> scratch(0x24);
    u32 fileId = archiveWaveFileId(archive, id);
    void* buffer = const_cast<void*>(cachedFileAddress(fileId));
    if (buffer) return buffer;
    info->fileId = 0xFFFFFFFFu;
    info->loadingFlags = 0;
    if (!readArchiveWaveInfo(archive, id, info.get())) return nullptr;
    u32 waveCount = info->option;
    if (!waveCount) return nullptr;
    WaveArchiveHeader* header = gabi::at<WaveArchiveHeader>((gabi::ea(scratch.get()) - 0x24u + 0x63u) & ~63u);
    if (readArchiveFilePart(this, fileId, header, 0x2C, 0, chunkBytes) != 0x2C) return nullptr;
    u32 dataOffset = header->dataOffset();
    if (header->tableOffset() > dataOffset) return nullptr;
    buffer = gabi::call_ptr<void*>(allocator->vtable->allocate.function, allocator,
                                   dataOffset + waveCount * 4u + 4u);
    if (!buffer) return nullptr;
    if ((u32)readArchiveFilePart(this, fileId, buffer, dataOffset, 0, chunkBytes) != dataOffset) return nullptr;
    copyArchiveLoadingMemory(gabi::at<void>(gabi::ea(buffer) + dataOffset), &sWaveArchiveAddressMarker, 4);
    WaveArchiveReader::construct(reader.get(), static_cast<const WaveArchiveHeader*>(buffer), 1);
    reader->clearCachedAddresses();
    storeFileAddress(fileId, buffer);
    return buffer;
}

/* 0289150C */
u32 ArchiveFileLoader::loadIndividualWave(u32 id, u32 waveId, ArchiveAllocator_l* allocator, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<WaveArchiveReader> reader(8);
    gabi::FrameLocal<ArchiveLoadedWaveHeader_l> header(0x18);
    u32 fileId = archiveWaveFileId(archive, id);
    void* archiveData = const_cast<void*>(cachedFileAddress(fileId));
    if (!archiveData) {
        archiveData = loadWaveArchive(id, allocator, chunkBytes);
        if (!archiveData) return 0;
    }
    WaveArchiveReader::construct(reader.get(), static_cast<const WaveArchiveHeader*>(archiveData), 1);
    if (reader->mValid && reader->loadedAddress(waveId)) return 1;
    u32 bytes = reader->fileSize(waveId);
    void* storage = gabi::call_ptr<void*>(allocator->vtable->allocate.function, allocator, bytes + 0x80u);
    if (!storage) return 0;
    u32 aligned = (gabi::ea(storage) + 0x3Fu) & ~63u;
    header->tag = 0x46495756u;
    header->fileId = fileId;
    header->waveId = waveId;
    copyArchiveLoadingMemory(gabi::at<void>(aligned), header.get(), 0x40);
    u32 offset = reader->fileDataOffset(waveId);
    void* buffer = gabi::at<void>(aligned + 0x40u);
    if ((u32)readArchiveFilePart(this, fileId, buffer, bytes, offset, chunkBytes) != bytes) return 0;
    reader->exchangeAddress(waveId, buffer);
    flushArchiveBuffer(buffer, bytes);
    return 1;
}

/* 02891698 */
u32 ArchiveFileLoader::loadWaveGroup(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((flags & 8) && !loadCachedFile(archiveWaveFileId(archive, id), allocator, chunkBytes, 1)) return 0;
    return 1;
}

/* 0289172C */
u32 ArchiveFileLoader::loadWaveEntry(u32 id, u32 waveId, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveWaveArchiveSummary> info(8);
    info->fileId = 0xFFFFFFFFu;
    info->loadingFlags = 0;
    if (!readArchiveWaveInfo(archive, id, info.get())) return 0;
    if (info->loadingFlags) return loadIndividualWave(id, waveId, allocator, chunkBytes) != 0;
    return loadWaveGroup(id, allocator, flags, chunkBytes) != 0;
}

/* 028917E8 */
u32 ArchiveFileLoader::loadBank(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<BankReader> reader(8);
    u32 fileId = archiveWaveFileId(archive, id);
    if ((flags & 4) && !loadCachedFile(fileId, allocator, chunkBytes, 0)) return 0;
    if (!(flags & 8)) return 1;
    void* image = const_cast<void*>(fileAddress(fileId));
    if (image) {
        BankReader::constructFrom(reader.get(), static_cast<const BankFileHeader*>(image));
        const BankWaveReferences* table = static_cast<const BankWaveReferences*>(reader->waveReferences());
        if (!table) return 0;
        u32 index = 0;
        u32 count = table->count;
        u32 address = gabi::ea(table) + 4u;
        while (index < count) {
            if (!address) return 0;
            const BankWaveReference* entry = gabi::at<BankWaveReference>(address);
            if (!loadWaveEntry(entry->archiveIndex, entry->waveIndex, allocator, flags, chunkBytes)) return 0;
            count = table->count;
            ++index;
            address += 8u;
        }
    } else {
        const ArchiveIndexTable* dependencies = archiveDependencyIds(archive, id);
        u32 index = 0;
        u32 count = dependencies->count;
        while (index < count) {
            if (!loadWaveGroup(dependencies->entries[index], allocator, flags, chunkBytes)) return 0;
            count = dependencies->count;
            ++index;
        }
    }
    return 1;
}

/* 02891960 */
u32 ArchiveFileLoader::loadSequence(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveSequenceSummary> info(8);
    if ((flags & 1) && !loadCachedFile(archiveWaveFileId(archive, id), allocator, chunkBytes, 0)) return 0;
    if (!(flags & 12)) return 1;
    info->priority = 0;
    info->startOffset = 0;
    info->priorityFix = 0;
    info->trackFlags = 0;
    for (u32 index = 0; index != 4; ++index) info->bankIds[index] = 0xFFFFFFFFu;
    if (!readArchiveSoundBanks(archive, id, info.get())) return 0;
    for (u32 index = 0; index != 4; ++index) {
        u32 bank = info->bankIds[index];
        if (bank != 0xFFFFFFFFu && !loadBank(bank, allocator, flags, chunkBytes)) return 0;
    }
    return 1;
}

/* 02891A6C */
u32 ArchiveFileLoader::loadWaveSound(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes, u32 fallbackId) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveWaveSummary> info(8);
    gabi::FrameLocal<WaveSoundNoteParameters> entry(0x14);
    gabi::FrameLocal<WaveSoundDataReader> reader(0x2C);
    u32 fileId = archiveWaveFileId(archive, id);
    if ((flags & 2) && !loadCachedFile(fileId, allocator, chunkBytes, 0)) return 0;
    if (!(flags & 8)) return 1;
    void* image = const_cast<void*>(fileAddress(fileId));
    if (image) {
        info->priority = 0;
        info->trackFlags = 0;
        info->priorityFix = 0;
        if (!readArchiveWaveSoundSummary(archive, id, info.get())) return 0;
        u32 waveIndex = info->waveIndex;
        WaveSoundDataReader::construct(reader.get(), static_cast<const WaveSoundArchiveDirectory*>(image));
        entry->mEnvelope.values[3] = 0;
        entry->mEnvelope.values[1] = 0;
        entry->mEnvelope.values[4] = 0;
        entry->mEnvelope.values[0] = 0;
        entry->mEnvelope.values[2] = 0;
        if (!reader->readNote(entry.get(), waveIndex, 0)) return 0;
        return loadWaveEntry(entry->mArchiveId, entry->mWaveIndex, allocator, flags, chunkBytes) != 0;
    }
    const ArchiveIndexTable* dependencies = archiveDependencyIds(archive, fallbackId == 0xFFFFFFFFu ? id : fallbackId);
    u32 count = dependencies->count;
    u32 index = 0;
    while (index < count) {
        if (!loadWaveGroup(dependencies->entries[index], allocator, flags, chunkBytes)) return 0;
        count = dependencies->count;
        ++index;
    }
    return 1;
}

/* 02891C0C */
void ArchiveFileLoader::ensureWaveAddressTable(u32 id, ArchiveAllocator_l* allocator) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveWaveArchiveSummary> info(8);
    gabi::FrameLocal<WaveArchiveReader> oldReader(0x14);
    gabi::FrameLocal<WaveArchiveReader> newReader(0x24);
    info->loadingFlags = 0;
    info->fileId = 0xFFFFFFFFu;
    if (!readArchiveWaveInfo(archive, id, info.get()) || !info->loadingFlags) return;
    void* oldImage = const_cast<void*>(cachedFileAddress((u32)info->fileId));
    WaveArchiveReader::construct(oldReader.get(), static_cast<const WaveArchiveHeader*>(oldImage), 0);
    if (oldReader->hasAddressCache()) return;
    u32 dataOffset = static_cast<const WaveArchiveHeader*>(oldImage)->dataOffset();
    void* newImage = gabi::call_ptr<void*>(allocator->vtable->allocate.function, allocator,
                                          dataOffset + (u32)info->option * 4u + 4u);
    if (!newImage) return;
    copyArchiveLoadingMemory(newImage, oldImage, dataOffset);
    copyArchiveLoadingMemory(gabi::at<void>(gabi::ea(newImage) + dataOffset), &sWaveArchiveAddressMarker, 4);
    WaveArchiveReader::construct(newReader.get(), static_cast<const WaveArchiveHeader*>(newImage), 1);
    newReader->clearCachedAddresses();
    u32 count = info->option;
    for (u32 index = 0; index < count; ++index) {
        const void* wave = oldReader->loadedAddress(index);
        newReader->exchangeAddress(index, wave);
        count = info->option;
    }
    storeFileAddress((u32)info->fileId, newImage);
}

/* 02891D70 */
void ArchiveFileLoader::ensureBankWaveAddressTable(u32 id, ArchiveAllocator_l* allocator) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> fileId(8);
    gabi::FrameLocal<BankReader> reader(0xC);
    if (id == 0xFFFFFFFFu) return;
    *fileId = 0xFFFFFFFFu;
    if (!readArchiveBankFileId(archive, id, fileId.get())) return;
    void* image = const_cast<void*>(cachedFileAddress((u32)*fileId));
    if (!image) return;
    BankReader::constructFrom(reader.get(), static_cast<const BankFileHeader*>(image));
    const BankWaveReferences* table = static_cast<const BankWaveReferences*>(reader->waveReferences());
    if (!table || !(u32)table->count) return;
    u32 address = gabi::ea(table) + 4u;
    if (address) ensureWaveAddressTable(gabi::at<BankWaveReference>(address)->archiveIndex, allocator);
}

/* 02891E28 */
void ArchiveFileLoader::ensureSequenceWaveAddressTables(u32 id, ArchiveAllocator_l* allocator) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveSequenceSummary> info(8);
    info->priority = 0;
    info->startOffset = 0;
    info->priorityFix = 0;
    info->trackFlags = 0;
    for (u32 index = 0; index != 4; ++index) info->bankIds[index] = 0xFFFFFFFFu;
    if (!readArchiveSoundBanks(archive, id, info.get())) return;
    for (u32 index = 0; index != 4; ++index) ensureBankWaveAddressTable(info->bankIds[index], allocator);
}

/* 02891ECC */
void ArchiveFileLoader::ensureSoundWaveAddressTable(u32 id, ArchiveAllocator_l* allocator) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveWaveSummary> info(8);
    gabi::FrameLocal<WaveSoundNoteParameters> entry(0x14);
    gabi::FrameLocal<WaveSoundDataReader> reader(0x2C);
    gabi::FrameLocal<ArchiveSoundSummary> soundInfo(0x34);
    if (id == 0xFFFFFFFFu || !readArchiveSoundSummary(archive, id, soundInfo.get())) return;
    info->priorityFix = 0;
    info->priority = 0;
    info->trackFlags = 0;
    if (!readArchiveWaveSoundSummary(archive, id, info.get())) return;
    void* image = const_cast<void*>(cachedFileAddress((u32)soundInfo->fileId));
    if (!image) return;
    WaveSoundDataReader::construct(reader.get(), static_cast<const WaveSoundArchiveDirectory*>(image));
    entry->mEnvelope.values[4] = 0;
    entry->mEnvelope.values[1] = 0;
    entry->mEnvelope.values[0] = 0;
    entry->mEnvelope.values[3] = 0;
    entry->mEnvelope.values[2] = 0;
    if (reader->readNote(entry.get(), info->waveIndex, 0))
        ensureWaveAddressTable(entry->mArchiveId, allocator);
}

/* 02891FC4 */
u32 ArchiveFileLoader::loadGroupDependencies(void* image, ArchiveAllocator_l* allocator, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveSoundGroupSummary> range(8);
    gabi::FrameLocal<GroupExtraEntry> dependency(0x14);
    gabi::FrameLocal<GroupFileLocation> file(0x1C);
    gabi::FrameLocal<GroupExtraEntry> fallbackDependency(0x24);
    gabi::FrameLocal<GroupFileReader> reader(0x2C);
    constructArchiveGroupReader(reader.get(), image);
    u32 fileCount = reader->mInfo->mCount;
    for (u32 index = 0; index != fileCount; ++index) {
        if (!readArchiveGroupFile(reader.get(), file.get(), index)) return 0;
        if (file->mAddress) {
            storeFileAddress((u32)file->mFileId, file->mAddress.get());
        } else if (index == 0) {
            u32 count = archiveGroupDependencyCount(reader.get());
            for (u32 item = 0; item < count; ++item) {
                if (readArchiveGroupDependency(reader.get(), fallbackDependency.get(), item) &&
                    !loadNamedGroup(fallbackDependency->mFileId, allocator, fallbackDependency->mFlags, chunkBytes)) return 0;
            }
            return 1;
        }
    }
    u32 count = archiveGroupDependencyCount(reader.get());
    for (u32 index = 0; index != count; ++index) {
        if (!readArchiveGroupDependency(reader.get(), dependency.get(), index) || !((u32)dependency->mFlags & 8)) continue;
        u32 id = dependency->mFileId;
        switch (id >> 24) {
        case 1:
            if (archiveSoundKind(archive, id) == 1) ensureSequenceWaveAddressTables(dependency->mFileId, allocator);
            break;
        case 3:
            ensureBankWaveAddressTable(id, allocator);
            break;
        case 2:
            range->files = nullptr;
            range->firstSoundId = 0xFFFFFFFFu;
            range->lastSoundId = 0xFFFFFFFFu;
            if (readArchiveSoundGroupSummary(archive, id, range.get())) {
                u32 first = range->firstSoundId;
                if (first == 0xFFFFFFFFu) break;
                u32 kind = archiveSoundKind(archive, first);
                if (kind == 1) {
                    u32 last = range->lastSoundId;
                    for (u32 current = range->firstSoundId; current <= last; ++current) {
                        ensureSequenceWaveAddressTables(current, allocator);
                        last = range->lastSoundId;
                    }
                } else if (kind == 3) ensureSoundWaveAddressTable(range->firstSoundId, allocator);
            }
            break;
        }
    }
    return 1;
}

/* 02892280 */
u32 ArchiveFileLoader::loadGroup(u32 id, ArchiveAllocator_l* allocator, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* image = loadCachedFile(archiveWaveFileId(archive, id), allocator, chunkBytes, 1);
    if (!image) return 0;
    return loadGroupDependencies(image, allocator, chunkBytes);
}

/* 028922FC */
u32 ArchiveFileLoader::loadSoundGroup(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveSoundGroupSummary> info(8);
    info->files = nullptr;
    info->firstSoundId = 0xFFFFFFFFu;
    info->lastSoundId = 0xFFFFFFFFu;
    if (!readArchiveSoundGroupSummary(archive, id, info.get())) return 0;
    u32 first = info->firstSoundId;
    if (first == 0xFFFFFFFFu) return 1;
    u32 kind = archiveSoundKind(archive, first);
    if (kind != 1 && kind != 3) return 1;
    u32 last = info->lastSoundId;
    u32 current = info->firstSoundId;
    while (current <= last) {
        u32 result = kind == 1 ? loadSequence(current, allocator, flags, chunkBytes)
                              : loadWaveSound(current, allocator, flags, chunkBytes, id);
        if (!result) return 0;
        last = info->lastSoundId;
        ++current;
    }
    return 1;
}

/* 02892430 */
u32 ArchiveFileLoader::loadNamedGroup(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!archiveLoaderReady(this) || id == 0xFFFFFFFFu || !allocator) return 0;
    SoundArchive* backend = archive;
    ArchiveBackendTable_l* table = reinterpret_cast<ArchiveBackendTable_l*>(backend->vtable.get());
    gabi::call_ptr<void>(table->beginLoad.function, backend);
    u32 result = 0;
    switch (id >> 24) {
    case 1:
        switch (archiveSoundKind(archive, id)) {
        case ArchiveSound_Sequence: result = loadSequence(id, allocator, flags, chunkBytes); break;
        case ArchiveSound_Wave: result = loadWaveSound(id, allocator, flags, chunkBytes, 0xFFFFFFFFu); break;
        }
        break;
    case 2: result = loadSoundGroup(id, allocator, flags, chunkBytes); break;
    case 3: result = loadBank(id, allocator, flags, chunkBytes); break;
    case 5: result = loadWaveGroup(id, allocator, flags, chunkBytes); break;
    case 6: result = loadGroup(id, allocator, chunkBytes); break;
    }
    backend = archive;
    table = reinterpret_cast<ArchiveBackendTable_l*>(backend->vtable.get());
    gabi::call_ptr<void>(table->endLoad.function, backend);
    return result;
}

/* 02892650 */
u32 ArchiveFileLoader::loadByLabel(const char* label, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    return loadNamedGroup(archiveFindItem(archive, label), allocator, flags, chunkBytes);
}

/* 028926BC */
void* ArchiveFileLoader::fileAddressForItem(u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> bankFile(8);
    gabi::FrameLocal<ArchiveGroupSummary> groupInfo(0xC);
    gabi::FrameLocal<ArchiveWaveArchiveSummary> waveInfo(0x14);
    gabi::FrameLocal<ArchiveSoundSummary> soundInfo(0x20);
    if (!archive) return nullptr;
    u32 fileId;
    switch (id >> 24) {
    case 1:
        readArchiveSoundSummary(archive, id, soundInfo.get());
        fileId = soundInfo->fileId;
        break;
    case 3:
        *bankFile = 0xFFFFFFFFu;
        readArchiveBankFileId(archive, id, bankFile.get());
        fileId = *bankFile;
        break;
    case 5:
        waveInfo->loadingFlags = 0;
        waveInfo->fileId = 0xFFFFFFFFu;
        readArchiveWaveInfo(archive, id, waveInfo.get());
        fileId = waveInfo->fileId;
        break;
    case 6:
        groupInfo->size = 0;
        groupInfo->fileId = 0xFFFFFFFFu;
        readArchiveGroupSummary(archive, id, groupInfo.get());
        fileId = groupInfo->fileId;
        break;
    default:
        return nullptr;
    }
    if (fileId == 0xFFFFFFFFu) return nullptr;
    return const_cast<void*>(fileAddress(fileId));
}

/* 028927FC */
u32 ArchiveFileLoader::loadBankWaves(void* image, ArchiveAllocator_l* allocator) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<BankReader> reader(8);
    if (!image) return 0;
    BankReader::constructFrom(reader.get(), static_cast<const BankFileHeader*>(image));
    const BankWaveReferences* table = static_cast<const BankWaveReferences*>(reader->waveReferences());
    if (!table) return 0;
    u32 index = 0;
    u32 count = table->count;
    u32 address = gabi::ea(table) + 4u;
    while (index < count) {
        if (!address) return 0;
        const BankWaveReference* entry = gabi::at<BankWaveReference>(address);
        if (!loadWaveEntry(entry->archiveIndex, entry->waveIndex, allocator, 8, 0)) return 0;
        count = table->count;
        ++index;
        address += 8u;
    }
    return 1;
}

/* 028928B8 */
u32 ArchiveFileLoader::loadSoundWave(void* image, u32 waveId, ArchiveAllocator_l* allocator) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<WaveSoundNoteParameters> entry(8);
    gabi::FrameLocal<WaveSoundDataReader> reader(0x20);
    if (!image) return 0;
    WaveSoundDataReader::construct(reader.get(), static_cast<const WaveSoundArchiveDirectory*>(image));
    entry->mEnvelope.values[0] = 0;
    entry->mEnvelope.values[2] = 0;
    entry->mEnvelope.values[4] = 0;
    entry->mEnvelope.values[1] = 0;
    entry->mEnvelope.values[3] = 0;
    if (!reader->readNote(entry.get(), waveId, 0)) return 0;
    return loadWaveEntry(entry->mArchiveId, entry->mWaveIndex, allocator, 8, 0) != 0;
}

/* 02892988 */
const void* ArchiveFileLoader::loadedFileAddress(u32 id) {
    SoundArchive* backend = archive;
    if (!backend) return nullptr;
    ArchiveBackendTable_l* table = reinterpret_cast<ArchiveBackendTable_l*>(backend->vtable.get());
    return gabi::tail_ptr<const void*>(table->fileAddress.function, backend, id);
}

/* 02892980 */
void ArchiveFileLoader::setArchive(SoundArchive* image) {
    archive = image;
}

/* WWHD glue
ext 028AA7F4 nw::snd::GroupFileReader* constructArchiveGroupReader(nw::snd::GroupFileReader*,void*)
ext 028AA8FC u32 readArchiveGroupFile(nw::snd::GroupFileReader*,nw::snd::GroupFileLocation*,u32)
ext 028AA95C u32 archiveGroupDependencyCount(nw::snd::GroupFileReader*)
ext 028AA978 u32 readArchiveGroupDependency(nw::snd::GroupFileReader*,nw::snd::GroupExtraEntry*,u32)
ext 0288EE74 u32 readArchiveGroupSummary(nw::snd::SoundArchive*,u32,nw::snd::ArchiveGroupSummary*)
ext 0288EDEC u32 readArchiveSoundGroupSummary(nw::snd::SoundArchive*,u32,nw::snd::ArchiveSoundGroupSummary*)
ext 0288E92C u32 archiveSoundKind(nw::snd::SoundArchive*,u32)
ext 02890FF8 u32 archiveLoaderReady(nw::snd::ArchiveFileLoader*)
ext 0289FA10 u32 nw::snd::WaveArchiveReader::hasAddressCache() const
ext 0288EA44 u32 readArchiveBankFileId(nw::snd::SoundArchive*,u32,be<u32>*)
ext 0288E934 u32 readArchiveSoundSummary(nw::snd::SoundArchive*,u32,nw::snd::ArchiveSoundSummary*)
ext 0288ECDC u32 readArchiveWaveSoundSummary(nw::snd::SoundArchive*,u32,nw::snd::ArchiveWaveSummary*)
ext 0288EF8C const nw::snd::ArchiveIndexTable* archiveDependencyIds(nw::snd::SoundArchive*,u32)
ext 0288E9BC u32 readArchiveSoundBanks(nw::snd::SoundArchive*,u32,nw::snd::ArchiveSequenceSummary*)
ext 0288E91C u32 archiveFindItem(nw::snd::SoundArchive*,const char*)
ext 028A32C8 nw::snd::BankReader* nw::snd::BankReader::constructFrom(nw::snd::BankReader*,const nw::snd::BankFileHeader*)
ext 028A3518 const void* nw::snd::BankReader::waveReferences()
ext 028A02F0 nw::snd::WaveSoundDataReader* nw::snd::WaveSoundDataReader::construct(nw::snd::WaveSoundDataReader*,const nw::snd::WaveSoundArchiveDirectory*)
ext 028A0498 u32 nw::snd::WaveSoundDataReader::readNote(nw::snd::WaveSoundNoteParameters*,u32,u32)
ext 0289FD24 const void* nw::snd::WaveArchiveReader::loadedAddress(u32)
ext 0289FC80 u32 nw::snd::WaveArchiveReader::fileSize(u32) const
ext 0289FCA8 u32 nw::snd::WaveArchiveReader::fileDataOffset(u32) const
ext 0289FDC8 const void* nw::snd::WaveArchiveReader::exchangeAddress(u32,const void*)
data sWaveArchiveAddressMarker 101854E4
ext 0289F988 u32 nw::snd::WaveArchiveHeader::dataOffset() const
ext 0289F938 u32 nw::snd::WaveArchiveHeader::tableOffset() const
ext C000A848 void copyArchiveLoadingMemory(void*,const void*,u32)
ext 0289FB20 nw::snd::WaveArchiveReader* nw::snd::WaveArchiveReader::construct(nw::snd::WaveArchiveReader*,const nw::snd::WaveArchiveHeader*,u32)
ext 0289FC28 void nw::snd::WaveArchiveReader::clearCachedAddresses()
ext 0288E924 u32 archiveWaveFileId(nw::snd::SoundArchive*,u32)
ext 0288ED64 u32 readArchiveWaveInfo(nw::snd::SoundArchive*,u32,nw::snd::ArchiveWaveArchiveSummary*)
ext 0288EEFC u32 readArchiveFileInfo(nw::snd::SoundArchive*,u32,nw::snd::ArchiveFileSummary*)
ext 02891044 s32 readArchiveFilePart(nw::snd::ArchiveFileLoader*,u32,void*,s32,s32,s32)
ext 028898F0 void flushArchiveBuffer(void*,u32)
*/
