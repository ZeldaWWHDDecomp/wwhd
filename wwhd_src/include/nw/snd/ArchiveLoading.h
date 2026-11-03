/* Archive file loading methods on the canonical archive/loader layouts. Own names; binary-derived. */
#pragma once
#include "nw/snd/WaveArchiveReader.h"
#include "nw/snd/WaveFileReader.h"
#include "nw/snd/SoundArchive.h"
#include "nw/snd/GroupFileReader.h"
#include "nw/snd/BankReader.h"
namespace nw { namespace snd {
WWHD_SIZE(ArchiveFileSummary, 12);
WWHD_SIZE(ArchiveWaveArchiveSummary, 9);
WWHD_SIZE(ArchiveSequenceSummary, 26);
WWHD_SIZE(ArchiveWaveSummary, 10);
WWHD_SIZE(ArchiveSoundSummary, 21);
WWHD_SIZE(ArchiveSoundGroupSummary, 12);
WWHD_SIZE(ArchiveGroupSummary, 8);

struct ArchiveAllocatorVtable_l { VtableEntry header, destroy, allocate; };
WWHD_OFFSET(ArchiveAllocatorVtable_l, allocate, 0x10);
struct ArchiveAllocator_l { gptr<ArchiveAllocatorVtable_l> vtable; };

struct ArchiveLoadedWaveHeader_l { be<u32> tag, fileId, waveId; be<u8> reserved[52]; };
WWHD_SIZE(ArchiveLoadedWaveHeader_l, 64);
struct ArchiveWaveHeaderScratch_l { be<u8> bytes[0x80]; };

/* The slots of the archive's vtable (SoundArchive + 0x28C) before ArchiveBackendVtable's named
 * open entries at +0x50: SoundArchive.h keeps them as otherEntries. */
struct ArchiveBackendTable_l {
    VtableEntry header, destroy, fileAddress, reserved, beginLoad, endLoad;   // fileAddress: MemorySoundArchive::itemData 0288A410
    VtableEntry defaultStreamDescriptorValue, defaultStreamDescriptorExtraValue, unknownOption, streamDescriptorConstant;
};
WWHD_OFFSET(ArchiveBackendTable_l, beginLoad, 0x20);
WWHD_OFFSET(ArchiveBackendTable_l, streamDescriptorConstant, 0x48);
static_assert(sizeof(ArchiveBackendTable_l) == offsetof(ArchiveBackendVtable, openEmbedded), "slots before the open entries");

struct ArchiveFileLoader : ArchiveLoader {
    void* loadWholeFile(u32 id, ArchiveAllocator_l* allocator, s32 chunkBytes);
    void* loadCachedFile(u32 id, ArchiveAllocator_l* allocator, s32 chunkBytes, u32 unused);
    void* loadWaveArchive(u32 id, ArchiveAllocator_l* allocator, s32 chunkBytes);
    u32 loadWaveGroup(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes);
    u32 loadWaveEntry(u32 id, u32 waveId, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes);
    u32 loadIndividualWave(u32 id, u32 waveId, ArchiveAllocator_l* allocator, s32 chunkBytes);
    u32 loadGroup(u32 id, ArchiveAllocator_l* allocator, s32 chunkBytes);
    u32 loadByLabel(const char* label, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes);
    u32 loadNamedGroup(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes);
    u32 loadGroupDependencies(void* image, ArchiveAllocator_l* allocator, s32 chunkBytes);
    u32 loadBankWaves(void* image, ArchiveAllocator_l* allocator);
    u32 loadSoundWave(void* image, u32 waveId, ArchiveAllocator_l* allocator);
    u32 loadBank(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes);
    u32 loadSequence(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes);
    void ensureWaveAddressTable(u32 id, ArchiveAllocator_l* allocator);
    void ensureBankWaveAddressTable(u32 id, ArchiveAllocator_l* allocator);
    void ensureSequenceWaveAddressTables(u32 id, ArchiveAllocator_l* allocator);
    void ensureSoundWaveAddressTable(u32 id, ArchiveAllocator_l* allocator);
    u32 loadWaveSound(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes, u32 fallbackId);
    u32 loadSoundGroup(u32 id, ArchiveAllocator_l* allocator, u32 flags, s32 chunkBytes);
    const void* loadedFileAddress(u32 id);   // the archive's file data for the id, or NULL
    void* fileAddressForItem(u32 id);
    void setArchive(SoundArchive* image);
};
WWHD_OFFSET(ArchiveFileLoader, vtable, 0x208);
WWHD_SIZE(ArchiveFileLoader, 0x20C);
}}
u32 readArchiveFileInfo(nw::snd::SoundArchive*, u32, nw::snd::ArchiveFileSummary*);
s32 readArchiveFilePart(nw::snd::ArchiveFileLoader*, u32, void*, s32, s32, s32);
void flushArchiveBuffer(void*, u32);

u32 archiveWaveFileId(nw::snd::SoundArchive*, u32);
u32 readArchiveWaveInfo(nw::snd::SoundArchive*, u32, nw::snd::ArchiveWaveArchiveSummary*);

void copyArchiveLoadingMemory(void*, const void*, u32);

u32 archiveFindItem(nw::snd::SoundArchive*, const char*);

const nw::snd::ArchiveIndexTable* archiveDependencyIds(nw::snd::SoundArchive*, u32);
u32 readArchiveSoundBanks(nw::snd::SoundArchive*, u32, nw::snd::ArchiveSequenceSummary*);

u32 readArchiveBankFileId(nw::snd::SoundArchive*, u32, be<u32>*);
u32 readArchiveSoundSummary(nw::snd::SoundArchive*, u32, nw::snd::ArchiveSoundSummary*);
u32 readArchiveWaveSoundSummary(nw::snd::SoundArchive*, u32, nw::snd::ArchiveWaveSummary*);

u32 readArchiveSoundGroupSummary(nw::snd::SoundArchive*, u32, nw::snd::ArchiveSoundGroupSummary*);
u32 archiveSoundKind(nw::snd::SoundArchive*, u32);
u32 archiveLoaderReady(nw::snd::ArchiveFileLoader*);

nw::snd::GroupFileReader* constructArchiveGroupReader(nw::snd::GroupFileReader*, void*);
u32 readArchiveGroupFile(nw::snd::GroupFileReader*, nw::snd::GroupFileLocation*, u32);
u32 archiveGroupDependencyCount(nw::snd::GroupFileReader*);
u32 readArchiveGroupDependency(nw::snd::GroupFileReader*, nw::snd::GroupExtraEntry*, u32);
u32 readArchiveGroupSummary(nw::snd::SoundArchive*, u32, nw::snd::ArchiveGroupSummary*);
