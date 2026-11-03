/* nw::snd sound archive (NW-SND#4): the archive reader (header blocks, string trie, item tables and
 * record summaries), the archive facade with its memory and file backends, the archive loader, and
 * the sequence track's note, voice and fade handling of this translation unit.
 * Written from WWHD's own disassembly and verified against the recompiled original (spec
 * tools/verify/units/lib2_nw_snd_4.txt). Names are our own; layouts come from the binary
 * (constructor sizes, vtables, field accesses).
 * Classes of other units that this TU only touches keep an _l suffix (voice, player, streams). */
#include "nw/snd/PlaybackVoice.h"
#include "nw/snd/SoundArchive.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;

struct ArchiveVtableHeader { be<u32> reserved[2]; };
static ArchiveVtableHeader sArchiveVtableHeader;
static ArchiveVtableHeader sArchiveLoaderVtableHeader;
static ArchiveVtableHeader sSequenceTrackVtableHeader;
static SequenceVoiceRequest_l sSequenceVoiceRequestDefaults;
/* 0288F3B0 */
ArchiveBlockReference* SoundArchiveHeader::findBlock(u32 kind) {
    for (u32 i = 0; i < 3; ++i) {
        if (blocks[i].kind == kind) return &blocks[i];
    }
    return NULL;
}
/* 0288F3E0 */
u32 SoundArchiveHeader::stringBlockSize() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return findBlock(ArchiveBlock_Strings)->size;
}
/* 0288F408 */
u32 SoundArchiveHeader::infoBlockSize() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return findBlock(ArchiveBlock_Info)->size;
}
/* 0288F430 */
u32 SoundArchiveHeader::stringBlockOffset() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return findBlock(ArchiveBlock_Strings)->offset;
}
/* 0288F458 */
u32 SoundArchiveHeader::infoBlockOffset() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return findBlock(ArchiveBlock_Info)->offset;
}
/* 0288F480 */
u32 SoundArchiveHeader::fileBlockOffset() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return findBlock(ArchiveBlock_Files)->offset;
}
/* 0288F4A8 */
void* ArchiveStringBlock::table(s32 index) {
    if (index > 1) return NULL;
    const ArchiveReference* entry = gabi::at<ArchiveReference>(gabi::ea(this) + (u32)index * sizeof(ArchiveReference));
    return entry->resolve<void>(this);
}
/* 0288F4CC */
const char* ArchiveStringBlock::stringAt(s32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (index == -1) return NULL;
    ArchiveStringTable* strings = (ArchiveStringTable*)table(0);
    if (gabi::ea(strings) == 0) return NULL;
    const ArchiveStringEntry* entry = gabi::at<ArchiveStringEntry>(gabi::ea(strings) + 4 + (u32)index * sizeof(ArchiveStringEntry));
    return entry->string.resolve<const char>(strings);
}
/* 0288F670 */
ArchiveReferenceTable* SoundArchiveInfo::soundTable() {
    return sounds.resolve<ArchiveReferenceTable>(this);
}
/* 0288F6F0 */
ArchiveReferenceTable* SoundArchiveInfo::bankTable() {
    return banks.resolve<ArchiveReferenceTable>(this);
}
/* 0288F768 */
ArchiveReferenceTable* SoundArchiveInfo::playerTable() {
    return players.resolve<ArchiveReferenceTable>(this);
}
/* 0288F7E0 */
ArchiveReferenceTable* SoundArchiveInfo::soundGroupTable() {
    return soundGroups.resolve<ArchiveReferenceTable>(this);
}
/* 0288F858 */
ArchiveReferenceTable* SoundArchiveInfo::groupTable() {
    return groups.resolve<ArchiveReferenceTable>(this);
}
/* 0288F8D0 */
ArchiveReferenceTable* SoundArchiveInfo::waveArchiveTable() {
    return waveArchives.resolve<ArchiveReferenceTable>(this);
}
/* 0288F948 */
ArchiveReferenceTable* SoundArchiveInfo::fileTable() {
    return files.resolve<ArchiveReferenceTable>(this);
}
/* 0288F9A8 */
void* SoundArchiveInfo::projectInfo() {
    return project.resolve<void>(this);
}
/* 0288F67C */
u32 SoundArchiveInfo::itemKind(u32 id) {
    return id >> 24;
}
/* 0288F684 */
void* SoundArchiveInfo::soundInfo(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (itemKind(id) != ArchiveItem_Sound) return NULL;
    u32 index = id & 0xFFFFFF;
    ArchiveReferenceTable* table = soundTable();
    if (index >= table->count) return NULL;
    return table->entries[index].resolve<void>(table);
}
/* 0288F6FC */
void* SoundArchiveInfo::bankInfo(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (itemKind(id) != ArchiveItem_Bank) return NULL;
    u32 index = id & 0xFFFFFF;
    ArchiveReferenceTable* table = bankTable();
    if (index >= table->count) return NULL;
    return table->entries[index].resolve<void>(table);
}
/* 0288F774 */
void* SoundArchiveInfo::playerInfo(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (itemKind(id) != ArchiveItem_Player) return NULL;
    u32 index = id & 0xFFFFFF;
    ArchiveReferenceTable* table = playerTable();
    if (index >= table->count) return NULL;
    return table->entries[index].resolve<void>(table);
}
/* 0288F7EC */
void* SoundArchiveInfo::soundGroupInfo(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (itemKind(id) != ArchiveItem_SoundGroup) return NULL;
    u32 index = id & 0xFFFFFF;
    ArchiveReferenceTable* table = soundGroupTable();
    if (index >= table->count) return NULL;
    return table->entries[index].resolve<void>(table);
}
/* 0288F864 */
void* SoundArchiveInfo::groupInfo(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (itemKind(id) != ArchiveItem_Group) return NULL;
    u32 index = id & 0xFFFFFF;
    ArchiveReferenceTable* table = groupTable();
    if (index >= table->count) return NULL;
    return table->entries[index].resolve<void>(table);
}
/* 0288F8DC */
void* SoundArchiveInfo::waveArchiveInfo(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (itemKind(id) != ArchiveItem_WaveArchive) return NULL;
    u32 index = id & 0xFFFFFF;
    ArchiveReferenceTable* table = waveArchiveTable();
    if (index >= table->count) return NULL;
    return table->entries[index].resolve<void>(table);
}
/* 0288F954 */
void* SoundArchiveInfo::fileInfo(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 index = id & 0xFFFFFF;
    ArchiveReferenceTable* table = fileTable();
    if (index >= table->count) return NULL;
    return table->entries[index].resolve<void>(table);
}
/* 0288FAA0 */
u32 ArchiveSoundRecord::labelId() {
    if (!(optionFlags & 1)) return 0xFFFFFFFF;
    return optionValues[0];
}
/* 0288FABC */
u32 ArchiveBankRecord::labelId() {
    if (!(optionFlags & 1)) return 0xFFFFFFFF;
    return optionValues[0];
}
/* 0288FAD8 */
u32 ArchiveWaveArchiveRecord::labelId() {
    if (!(optionFlags & 1)) return 0xFFFFFFFF;
    return optionValues[0];
}
/* 0288FAF4 */
u32 ArchiveSoundGroupRecord::labelId() {
    if (!(optionFlags & 1)) return 0xFFFFFFFF;
    return optionValues[0];
}
/* 0288FB10 */
u32 ArchiveGroupRecord::labelId() {
    if (!(optionFlags & 1)) return 0xFFFFFFFF;
    return optionValues[0];
}
/* 0288FB2C */
u32 ArchivePlayerRecord::labelId() {
    if (!(optionFlags & 1)) return 0xFFFFFFFF;
    return optionValues[0];
}
/* 0288FC60 */
void* ArchiveSoundRecord::streamDetail() {
    return detail.resolve<void>(this);
}
/* 0288FC6C */
void* ArchiveSoundRecord::waveDetail() {
    return detail.resolve<void>(this);
}
/* 0288FC78 */
void* ArchiveSoundRecord::sequenceDetail() {
    return detail.resolve<void>(this);
}
static inline u32 soundOption(const ArchiveSoundRecord* sound, u32 bit, u32 fallback) {
    u32 flags = sound->optionFlags;
    u32 rank = 0;
    bool present = false;
    for (u32 i = 0; i <= bit; ++i) {
        if (flags & (1u << i)) {
            ++rank;
            if (i == bit) present = true;
        }
    }
    if (!present || rank == 0) return fallback;
    return sound->optionValues[rank - 1];
}
/* 0288FC84 */
u32 ArchiveSoundRecord::panMode() {
    return (soundOption(this, 1, 0) >> 0) & 255;
}
/* 0288FCF0 */
u32 ArchiveSoundRecord::panCurve() {
    return (soundOption(this, 1, 0) >> 8) & 255;
}
/* 0288FD5C */
u32 ArchiveSoundRecord::playerPriority() {
    return (soundOption(this, 2, 64) >> 0) & 255;
}
/* 0288FDC8 */
u32 ArchiveSoundRecord::actorPlayerId() {
    return (soundOption(this, 2, 0) >> 8) & 255;
}
/* 0288FE34 */
u32 ArchiveSoundRecord::frontBypass() {
    return (soundOption(this, 17, 0) >> 0) & 1;
}
/* 02890164 */
u32 ArchiveFileRecord::locationKind() {
    if (location.kind == 0x220C) return 0;
    if (location.kind == 0x220D) return 1;
    return 2;
}
/* 02890190 */
void* ArchiveFileRecord::embeddedLocation() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (locationKind() != 0) return NULL;
    return location.resolve<void>(this);
}
/* 028901CC */
void* ArchiveFileRecord::externalLocation() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (locationKind() != 1) return NULL;
    return location.resolve<void>(this);
}
/* 02890208 */
SoundArchiveReader* SoundArchiveReader::create(SoundArchiveReader* reader) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(reader) == 0) reader = (SoundArchiveReader*)archiveConstructStorage(sizeof(SoundArchiveReader));
    if (gabi::ea(reader) != 0) {
        reader->strings = NULL;
        reader->files = NULL;
        reader->info = NULL;
    }
    return reader;
}
/* 0289024C */
u32 SoundArchiveReader::validateHeader(const SoundArchiveHeader* file) {
    const u32 SoundArchiveMagic = 0x46534152;
    const u32 MinimumVersion = 0x10000;
    const u32 VersionSpan = 0x10101;
    if (file->magic != SoundArchiveMagic) return 0;
    return (u32)((u32)file->version - MinimumVersion) < VersionSpan;
}
/* 0289028C */
u32 SoundArchiveReader::readHeader(const SoundArchiveHeader* file) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 valid = validateHeader(file);
    if (valid) {
        const be<u32>* from = (const be<u32>*)file;
        be<u32>* to = (be<u32>*)&header;
        for (u32 i = 0; i < sizeof(SoundArchiveHeader) / sizeof(u32); ++i) to[i] = (u32)from[i];
    }
    return valid;
}
/* 028902E0 */
void SoundArchiveReader::clear() {
    strings = NULL;
    info = NULL;
    files = NULL;
}
/* 028902F4 */
u32 SoundArchiveReader::isVersionTwoOneOrLater() {
    return header.version >= 0x20100;
}
/* 02890310 */
u32 SoundArchiveReader::isInfoVersionTwoOneOrLater() {
    return header.version >= 0x20100;
}
/* 0289032C */
void SoundArchiveReader::setStringBlock(void* block) {
    strings = gabi::at<ArchiveStringBlock>(gabi::ea(block) + 8);
}
/* 02890338 */
void SoundArchiveReader::setInfoBlock(void* block) {
    info = gabi::at<SoundArchiveInfo>(gabi::ea(block) + 8);
}
/* 0288F9B4 */
u32 SoundArchiveInfo::itemFileId(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* record = NULL;
    switch (id >> 24) {
    case ArchiveItem_Sound: record = soundInfo(id); break;
    case ArchiveItem_Bank: record = bankInfo(id); break;
    case ArchiveItem_WaveArchive: record = waveArchiveInfo(id); break;
    case ArchiveItem_Group: record = groupInfo(id); break;
    case ArchiveItem_SoundGroup: {
        const ArchiveSoundGroupRecord* group = (const ArchiveSoundGroupRecord*)soundGroupInfo(id);
        if (gabi::ea(group) == 0) return 0xFFFFFFFF;
        record = soundInfo(group->firstSoundId);
        break;
    }
    default: return 0xFFFFFFFF;
    }
    if (gabi::ea(record) == 0) return 0xFFFFFFFF;
    return *(const be<u32>*)record;
}
/* 0288FB48 */
u32 SoundArchiveInfo::itemLabelId(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    switch (id >> 24) {
    case ArchiveItem_Sound: {
        ArchiveSoundRecord* record = (ArchiveSoundRecord*)soundInfo(id);
        if (gabi::ea(record)) return record->labelId();
        break;
    }
    case ArchiveItem_Bank: {
        ArchiveBankRecord* record = (ArchiveBankRecord*)bankInfo(id);
        if (gabi::ea(record)) return record->labelId();
        break;
    }
    case ArchiveItem_WaveArchive: {
        ArchiveWaveArchiveRecord* record = (ArchiveWaveArchiveRecord*)waveArchiveInfo(id);
        if (gabi::ea(record)) return record->labelId();
        break;
    }
    case ArchiveItem_SoundGroup: {
        ArchiveSoundGroupRecord* record = (ArchiveSoundGroupRecord*)soundGroupInfo(id);
        if (gabi::ea(record)) return record->labelId();
        break;
    }
    case ArchiveItem_Group: {
        ArchiveGroupRecord* record = (ArchiveGroupRecord*)groupInfo(id);
        if (gabi::ea(record)) return record->labelId();
        break;
    }
    case ArchiveItem_Player: {
        ArchivePlayerRecord* record = (ArchivePlayerRecord*)playerInfo(id);
        if (gabi::ea(record)) return record->labelId();
        break;
    }
    }
    return 0xFFFFFFFF;
}
/* 02890478 */
u32 SoundArchiveReader::readSoundInfo(u32 id, ArchiveSoundSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveSoundRecord* record = (ArchiveSoundRecord*)info->soundInfo(id);
    if (gabi::ea(record) == 0) return 0;
    out->fileId = (u32)record->fileId;
    out->playerId = (u32)record->playerId;
    out->volume = (u8)record->volume;
    out->panMode = record->panMode();
    out->panCurve = record->panCurve();
    out->playerPriority = record->playerPriority();
    out->actorPlayerId = record->actorPlayerId();
    out->remoteFilter = (u8)record->remoteFilter;
    out->frontBypass = record->frontBypass();
    return 1;
}
/* 02890534 */
u32 SoundArchiveReader::readBankInfo(u32 id, ArchiveBankSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveBankRecord* record = (ArchiveBankRecord*)info->bankInfo(id);
    if (gabi::ea(record) == 0) return 0;
    out->fileId = (u32)record->fileId;
    return 1;
}
/* 028905F4 */
u32 SoundArchiveReader::readSoundGroupInfo(u32 id, ArchiveSoundGroupSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveSoundGroupRecord* record = (ArchiveSoundGroupRecord*)info->soundGroupInfo(id);
    if (gabi::ea(record) == 0) return 0;
    out->firstSoundId = (u32)record->firstSoundId;
    out->lastSoundId = (u32)record->lastSoundId;
    out->files = record->files.resolve<void>(record);
    return 1;
}
/* 02890824 */
u32 SoundArchiveReader::readProjectInfo(ArchiveProjectSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    const be<u16>* project = (const be<u16>*)info->projectInfo();
    if (gabi::ea(project) == 0) return 0;
    for (u32 i = 0; i < 7; ++i) out->counts[i] = (u16)project[i];
    return 1;
}
/* 02890094 */
u32 ArchivePlayerRecord::heapSize() {
    u32 flags = optionFlags;
    if (!(flags & 2)) return 0;
    return optionValues[(flags & 1) ? 1 : 0];
}
/* 028900FC */
u32 ArchiveWaveArchiveRecord::waveCount() {
    u32 flags = optionFlags;
    if (!(flags & 2)) return 0;
    return optionValues[(flags & 1) ? 1 : 0];
}
/* 028903D0 */
u32 SoundArchiveReader::itemFileId(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return info->itemFileId(id);
}
/* 028903D8 */
u32 SoundArchiveReader::playerCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return info->playerTable()->count;
}
/* 02890400 */
u32 SoundArchiveReader::groupCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return info->groupTable()->count;
}
/* 02890428 */
u32 SoundArchiveReader::waveArchiveCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return info->waveArchiveTable()->count;
}
/* 02890450 */
u32 SoundArchiveReader::fileCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return info->fileTable()->count;
}
/* 02890590 */
u32 SoundArchiveReader::readPlayerInfo(u32 id, ArchivePlayerSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchivePlayerRecord* record = (ArchivePlayerRecord*)info->playerInfo(id);
    if (gabi::ea(record) == 0) return 0;
    out->soundLimit = (u32)record->soundLimit;
    out->option = record->heapSize();
    return 1;
}
/* 028907A8 */
u32 SoundArchiveReader::readWaveArchiveInfo(u32 id, ArchiveWaveArchiveSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveWaveArchiveRecord* record = (ArchiveWaveArchiveRecord*)info->waveArchiveInfo(id);
    if (gabi::ea(record) == 0) return 0;
    out->fileId = (u32)record->fileId;
    out->option = record->waveCount();
    out->loadingFlags = (u8)record->loadingFlags;
    return 1;
}
/* 0288F528 */
ArchiveStringMatch* ArchiveStringTrie::find(const char* text, s32 length) {
    u32 root = rootIndex;
    if (root >= count) return NULL;
    ArchiveStringNode* node = &nodes[root];
    if (length == 0) {
        const char* end = text;
        while (*(const be<u8>*)end) ++end;
        length = (u32)(gabi::ea(end) - gabi::ea(text));
    }
    while (!(node->flags & 1)) {
        u32 bit = node->bitIndex;
        s32 byte = bit >> 3;
        u32 next;
        if (byte < length && (((const be<u8>*)text)[byte] & (1u << (7 - (bit & 7))))) next = node->right;
        else next = node->left;
        node = &nodes[next];
    }
    return &node->match;
}
/* 0288F5DC */
u32 ArchiveStringBlock::lookup(s32 tableIndex, const char* text) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveStringTrie* trie = (ArchiveStringTrie*)table(tableIndex);
    ArchiveStringMatch* match = trie->find(text, 0);
    if (gabi::ea(match) == 0) return 0xFFFFFFFF;
    const char* stored = stringAt(match->stringId);
    const be<u8>* input = (const be<u8>*)text;
    const be<u8>* candidate = (const be<u8>*)stored;
    u32 a, b;
    do {
        a = *input++;
        b = *candidate++;
    } while (a == b && a != 0);
    if (a != b) return 0xFFFFFFFF;
    return match->itemId;
}
/* 0288FC34 */
u32 ArchiveSoundRecord::soundKind() {
    switch ((u16)detail.kind) {
    case 0x2201: return ArchiveSound_Stream;
    case 0x2202: return ArchiveSound_Wave;
    case 0x2203: return ArchiveSound_Sequence;
    default: return 0;
    }
}
/* 02890344 */
u32 SoundArchiveReader::findItem(const char* text) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea((ArchiveStringBlock*)strings) == 0) return 0xFFFFFFFF;
    return strings->lookup(1, text);
}
/* 02890368 */
const char* SoundArchiveReader::itemLabel(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea((ArchiveStringBlock*)strings) == 0) return NULL;
    u32 label = info->itemLabelId(id);
    if (label == 0xFFFFFFFF) return NULL;
    return strings->stringAt(label);
}
/* 028908B0 */
u32 SoundArchiveReader::soundKind(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveSoundRecord* record = (ArchiveSoundRecord*)info->soundInfo(id);
    if (gabi::ea(record) == 0) return 0;
    return record->soundKind();
}
/* 0288E878 */
SoundArchive* SoundArchive::create(SoundArchive* archive) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(archive) == 0) archive = (SoundArchive*)archiveConstructStorage(sizeof(SoundArchive));
    if (gabi::ea(archive) != 0) {
        archive->extension = NULL;
        archive->reader = NULL;
        archive->rootDirectory[1] = 0;
        archive->vtable = &sArchiveVtableHeader;
        archive->rootDirectory[0] = '/';
    }
    return archive;
}
/* 0288E8D0 */
void SoundArchive::destroy(SoundArchive* archive, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(archive) != 0 && (flags & 1)) destroyArchiveStorage(archive);
}
/* 0288E8E4 */
u32 SoundArchive::isAvailable() {
    return gabi::ea((SoundArchiveReader*)reader) != 0;
}
/* 0288E8F4 */
u32 SoundArchive::groupCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->groupCount();
}
/* 0288E8FC */
u32 SoundArchive::playerCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->playerCount();
}
/* 0288E904 */
u32 SoundArchive::waveArchiveCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->waveArchiveCount();
}
/* 0288E90C */
u32 SoundArchive::fileCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->fileCount();
}
/* 0288E914 */
const char* SoundArchive::itemLabel(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->itemLabel(id);
}
/* 0288E91C */
u32 SoundArchive::findItem(const char* text) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->findItem(text);
}
/* 0288E924 */
u32 SoundArchive::itemFileId(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->itemFileId(id);
}
/* 0288E92C */
u32 SoundArchive::soundKind(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->soundKind(id);
}
/* 0288FEA0 */
ArchiveReferenceTable* ArchiveStreamDetail::trackTable() {
    if (tracks.kind != 0x0101) return NULL;
    return tracks.resolve<ArchiveReferenceTable>(this);
}
/* 0288FEC0 */
void* ArchiveStreamDetail::routingData() {
    return routing.resolve<void>(this);
}
/* 0288FECC */
void* ArchiveStreamDetail::prefetchData() {
    u32 offset = prefetch.offset;
    if (offset == 0xFFFFFFFF || prefetch.kind != 0x2210) return NULL;
    return gabi::at<void>(gabi::ea(this) + offset);
}
/* 0288FEF4 */
u32 ArchiveWaveDetail::priority() {
    if (!(optionFlags & 1)) return 64;
    return packedPriority & 0xFF;
}
/* 0288FF14 */
u32 ArchiveWaveDetail::priorityFix() {
    if (!(optionFlags & 1)) return 0;
    return ((u32)packedPriority >> 8) & 0xFF;
}
/* 0288FF34 */
ArchiveIndexTable* ArchiveSequenceDetail::bankTable() {
    return bankIds.resolve<ArchiveIndexTable>(this);
}
/* 0288FF40 */
void ArchiveSequenceDetail::copyBankIds(be<u32>* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveIndexTable* table = bankTable();
    for (u32 i = 0; i < 4; ++i) {
        u32 id = 0xFFFFFFFF;
        if (i < table->count) id = table->entries[i];
        out[i] = id;
    }
}
/* 0288FF98 */
u32 ArchiveSequenceDetail::optionalId() {
    if (!(optionFlags & 1)) return 0;
    return optionValues[0];
}
/* 0288FFB4 */
u32 ArchiveSequenceDetail::priority() {
    u32 flags = optionFlags;
    if (!(flags & 2)) return 64;
    return (u32)optionValues[(flags & 1) ? 1 : 0] & 0xFF;
}
/* 02890020 */
u32 ArchiveSequenceDetail::priorityFix() {
    u32 flags = optionFlags;
    if (!(flags & 2)) return 1;
    return (((u32)optionValues[(flags & 1) ? 1 : 0] >> 8) & 0xFF) != 0;
}
/* 02890664 */
u32 SoundArchiveReader::readFileInfo(u32 id, ArchiveFileSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveFileRecord* record = (ArchiveFileRecord*)info->fileInfo(id);
    if (gabi::ea(record) == 0) return 0;
    u32 kind = record->locationKind();
    if (kind == 0) {
        const ArchiveEmbeddedLocation* location = (const ArchiveEmbeddedLocation*)record->embeddedLocation();
        out->size = (u32)location->size;
        u32 offset = location->offset;
        if (offset != 0xFFFFFFFF) offset += 8;
        out->offset = offset;
    } else if (kind == 1) {
        out->externalPath = (const char*)record->externalLocation();
    } else if (kind == 2) return 0;
    return 1;
}
/* 0289070C */
u32 SoundArchiveReader::readGroupInfo(u32 id, ArchiveGroupSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveGroupRecord* record = (ArchiveGroupRecord*)info->groupInfo(id);
    if (gabi::ea(record) == 0) return 0;
    u32 fileId = record->fileId;
    out->fileId = fileId;
    gabi::FrameLocal<ArchiveFileSummary> file(8);
    file->size = 0xFFFFFFFF;
    file->offset = 0xFFFFFFFF;
    file->externalPath = NULL;
    if (!readFileInfo(fileId, file.get())) return 0;
    out->size = (u32)file->size;
    return 1;
}
/* 028908EC */
u32 SoundArchiveReader::readSequenceInfo(u32 id, ArchiveSequenceSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveSoundRecord* record = (ArchiveSoundRecord*)info->soundInfo(id);
    if (gabi::ea(record) == 0 || record->soundKind() != ArchiveSound_Sequence) return 0;
    ArchiveSequenceDetail* detail = (ArchiveSequenceDetail*)record->sequenceDetail();
    out->startOffset = detail->optionalId();
    out->trackFlags = (u32)detail->trackFlags;
    out->priority = detail->priority();
    out->priorityFix = detail->priorityFix();
    detail->copyBankIds(out->bankIds);
    return 1;
}
/* 02890CA4 */
u32 SoundArchiveReader::readPrefetchInfo(u32 id, ArchivePrefetchSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveSoundRecord* record = (ArchiveSoundRecord*)info->soundInfo(id);
    if (gabi::ea(record) == 0 || record->soundKind() != ArchiveSound_Stream) return 0;
    ArchiveStreamDetail* detail = (ArchiveStreamDetail*)record->streamDetail();
    const ArchivePrefetchRecord* prefetch = (const ArchivePrefetchRecord*)detail->prefetchData();
    if (gabi::ea(prefetch) == 0) return 0;
    out->enabled = (((u32)prefetch->flags >> 8) & 0xFF) == 1;
    out->offset = (u32)prefetch->offset;
    out->size = (u32)prefetch->size;
    return 1;
}
/* 02890D50 */
u32 SoundArchiveReader::readWaveInfo(u32 id, ArchiveWaveSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveSoundRecord* record = (ArchiveSoundRecord*)info->soundInfo(id);
    if (gabi::ea(record) == 0 || record->soundKind() != ArchiveSound_Wave) return 0;
    ArchiveWaveDetail* detail = (ArchiveWaveDetail*)record->waveDetail();
    out->waveIndex = (u32)detail->waveIndex;
    out->trackFlags = (u32)detail->trackFlags;
    out->priority = detail->priority();
    out->priorityFix = detail->priorityFix() != 0;
    return 1;
}
/* 0289099C */
void* ArchiveReferenceTable::streamTrack(u32 index, const ArchiveReferenceTable* table) {
    if (index >= table->count) return NULL;
    const ArchiveReference* entry = &table->entries[index];
    if (entry->kind != 0x220E) return NULL;
    return entry->resolve<void>(table);
}
/* 02890F88 */
ArchiveLoader* ArchiveLoader::create(ArchiveLoader* loader) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(loader) == 0) loader = (ArchiveLoader*)archiveConstructStorage(sizeof(ArchiveLoader));
    if (gabi::ea(loader) != 0) {
        loader->archive = NULL;
        loader->vtable = (ArchiveLoaderVtable*)&sArchiveLoaderVtableHeader;
    }
    return loader;
}
/* 02890FD0 */
void ArchiveLoader::destroy(ArchiveLoader* loader, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(loader) == 0) return;
    loader->archive = NULL;
    loader->vtable = (ArchiveLoaderVtable*)&sArchiveLoaderVtableHeader;
    if (flags & 1) destroyArchiveStorage(loader);
}
/* 02890FF8 */
u32 ArchiveLoader::isAvailable() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    SoundArchive* source = archive;
    if (gabi::ea(source) == 0) return 0;
    return source->isAvailable() != 0;
}
/* 0288DBE8 */
void SequenceTrack::releaseVoices(s32 release) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    updateSequenceTrackParameters(this);
    SequenceVoice_l* voice = firstVoice;
    while (gabi::ea(voice) != 0) {
        if (voice->active != 0) {
            if (release >= 0) setSequenceEnvelopeRelease(voice->envelope(), (u8)release);
            releaseSequenceVoice(voice);
        }
        voice = voice->next;
    }
}
/* 0288DC60 */
void SequenceTrack::detachVoiceCallbacks() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    SequenceVoice_l* voice = firstVoice;
    while (gabi::ea(voice) != 0) {
        clearSequenceVoiceCallback(voice);
        voice = voice->next;
    }
    firstVoice = NULL;
}
/* 0288DCB8 */
void SequenceTrack::stop() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    releaseVoices(-1);
    detachVoiceCallbacks();
    active = 0;
}
/* 0288DCF8 */
void SequenceTrack::destroy(SequenceTrack* track, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(track) == 0) return;
    track->vtable = (SequenceTrackVtable*)&sSequenceTrackVtableHeader;
    track->stop();
    if (flags & 1) destroyArchiveStorage(track);
}
/* 0288DD80 */
s32 SequenceTrack::tick(u32 parameter) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (active == 0) return 0;
    s32 limit0 = progress[0].limit, position0 = progress[0].progress;
    s32 limit1 = progress[1].limit, position1 = progress[1].progress;
    if (position0 < limit0) progress[0].progress = position0 + 1;
    if (position1 < limit1) progress[1].progress = position1 + 1;
    s32 position2 = progress[2].progress, limit2 = progress[2].limit;
    s32 limit3 = progress[3].limit, position3 = progress[3].progress;
    if (position2 < limit2) progress[2].progress = position2 + 1;
    if (position3 < limit3) progress[3].progress = position3 + 1;
    s32 position4 = progress[4].progress, limit4 = progress[4].limit;
    u32 waitingForVoice = noteWait;
    if (position4 < limit4) progress[4].progress = position4 + 1;
    if (waitingForVoice != 0) {
        if (gabi::ea((SequenceVoice_l*)firstVoice) != 0) return 1;
        noteWait = 0;
    }
    s32 count = waitCount;
    if (count > 0) {
        count -= 1;
        waitCount = count;
        if (count > 0) return 1;
    }
    if (gabi::ea((const be<u8>*)sequenceEnd) == 0) return 1;
    u32 iterations = 0;
    while (count == 0 && noteWait == 0) {
        if (++iterations > 10000) break;
        u32 target = vtable->step.function;
        if (gabi::call_ptr<s32>(target, this, parameter) == 1) return -1;
        count = waitCount;
    }
    return 1;
}
/* 0288DF18 */
void SequenceTrack::releaseExpiredVoice(SequenceVoice_l* voice) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (voice->releaseCountdown != 0 || voice->envelopeState == 4 || releaseGuard != 0) return;
    tryReleaseSequenceVoice(voice);
}
/* 0288DF48 */
void SequenceTrack::tickVoices() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (active == 0) return;
    SequenceVoice_l* voice = firstVoice;
    while (gabi::ea(voice) != 0) {
        s32 count = voice->releaseCountdown;
        if (count > 0) voice->releaseCountdown = count - 1;
        releaseExpiredVoice(voice);
        if (voice->counterHeld == 0) advanceSequenceVoiceCounter(voice, 1);
        voice = voice->next;
    }
}
/* 0288DFE4 */
SequenceVoice_l* SequenceTrack::playNote(u32 key, s32 velocity, s32 duration, u32 reuse) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 scale = velocityScale;
    s32 scaledVelocity = (s32)((u32)velocity * scale) / 127;
    SequencePlayer_l* initialPlayer = player;
    SequenceVoice_l* voice = NULL;
    if (reuse != 0) {
        voice = firstVoice;
        if (gabi::ea(voice) != 0) {
            gabi::FrameLocal<SequenceRegion_l> region(0x10);
            u32 selectedBank = bank;
            u32 fixedPriority = releasePriorityFixed;
            for (u32 i = 0; i < 5; ++i) region->clearedParameters[i] = 0;
            void* bankPool = initialPlayer->bankPools[fixedPriority];
            if (lookupSequenceRegion(bankPool, region.get(), selectedBank, key, scaledVelocity) == 0) return NULL;
            u32 square = (u32)scaledVelocity * (u32)scaledVelocity;
            voice->noteKey = key;
            u32 regionVolume = region->volume;
            s32 gainNumerator = (s32)(square * regionVolume);
            voice->noteGain = gabi::fdivs_ppc((f32)gainNumerator, 2048383.0f);
        }
    }
    bool allocate = false;
    if (reuseVoice != 0) {
        voice = firstVoice;
        if (gabi::ea(voice) == 0) allocate = true;
        else if (voice->envelopeState == 4) {
            stopSequenceVoiceHardware(voice);
            invokeSequenceVoiceCallback(voice, 0);
            freeSequenceVoice(voice);
            allocate = true;
        } else {
            f32 ratio = gabi::fdivs_ppc((f32)scaledVelocity, 127.0f);
            voice->noteKey = key;
            voice->releaseCountdown = duration;
            voice->noteGain = gabi::fmuls_ppc(ratio, ratio);
        }
    } else if (gabi::ea(voice) == 0) allocate = true;
    if (allocate) {
        gabi::FrameLocal<SequenceVoiceRequest_l> request(0x28);
        // The original copies nine default words before overriding the request fields.
        for (u32 i = 0; i < 9; ++i)
            *gabi::at<be<u32>>(gabi::ea(request.get()) + i * 4) =
                *gabi::at<be<u32>>(gabi::ea(&sSequenceVoiceRequestDefaults) + i * 4);
        request->bank = bank;
        request->key = key;
        s32 pan = notePan;
        request->velocity = scaledVelocity;
        request->duration = reuse != 0 ? -1 : duration;
        request->pan = pan;
        u32 playerPriority = initialPlayer->notePriority;
        u32 trackPriority = notePriority;
        request->track = this;
        request->priority = playerPriority + trackPriority;
        SequencePlayer_l* currentPlayer = player;
        request->playerWord = currentPlayer->word4C;
        u32 fixedPriority = releasePriorityFixed;
        voice = allocateSequenceVoice(currentPlayer, fixedPriority, request.get());
        if (gabi::ea(voice) == 0) return NULL;
        u32 group = voice->exclusiveGroup;
        if (group != 0) {
            SequenceVoice_l* previous = firstVoice;
            while (gabi::ea(previous) != 0) {
                if (previous->exclusiveGroup == group) {
                    setSequenceEnvelopeRelease(previous->envelope(), 126);
                    releaseSequenceVoice(previous);
                }
                previous = previous->next;
                if (gabi::ea(previous) != 0) group = voice->exclusiveGroup;
            }
        }
        prependVoice(voice);
    }
    u32 value = envelope1;
    if (value <= 127) setSequenceEnvelope1(voice->envelope(), value);
    value = envelope2;
    if (value <= 127) setSequenceEnvelope2(voice->envelope(), value);
    value = envelope3;
    if (value <= 127) setSequenceEnvelope3(voice->envelope(), value);
    value = envelope4;
    if (value <= 127) setSequenceEnvelopeRelease(voice->envelope(), value);
    s32 signedValue = envelope5;
    if (signedValue <= 127) setSequenceEnvelope5(voice->envelope(), signedValue);
    u32 pitchMode = status;
    f32 pitch = notePitchOffset;
    if (pitchMode != 0) pitch = gabi::fadds_ppc(pitch, (f32)(s32)((u32)lastNoteKey - key));
    u32 time = pitchTime;
    if (time == 0) setSequenceVoicePitch(voice, duration, 0, pitch);
    else {
        f32 magnitude = __builtin_fabsf(pitch);
        s32 ticks = gabi::ftoi(gabi::fmuls_ppc((f32)(time * time), magnitude));
        s32 scaledTicks = (s32)((u32)(ticks >> 5) * 5);
        setSequenceVoicePitch(voice, scaledTicks, 1, pitch);
    }
    u32 silent = silenced;
    lastNoteKey = key;
    u32 oldFrames = voice->fadeFrames;
    u32 oldCounter = voice->fadeCounter;
    u8 target = silent == 0 ? 255 : 0;
    PlaybackVoice* synth;
    bool completedFade = oldCounter >= oldFrames;
    if (completedFade) {
        u8 oldTarget = voice->fadeTarget;
        voice->fadeFrames = 0;
        voice->fadeCounter = 0;
        voice->fadeOrigin = oldTarget;
        voice->fadeTarget = target;
    } else {
        s32 oldTarget = voice->fadeTarget;
        s32 origin = voice->fadeOrigin;
        voice->fadeFrames = 0;
        voice->fadeCounter = 0;
        s32 portion = ((oldTarget - origin) * (s32)oldCounter) / (s32)oldFrames;
        synth = voice->synth;
        voice->fadeOrigin = origin + portion;
        voice->fadeTarget = target;
    }
    SequencePlayer_l* currentPlayer = player;
    voice->releaseSynthGuard = currentPlayer->releaseSynthGuard;
    currentPlayer = player;
    voice->playerWord40 = currentPlayer->word40;
    currentPlayer = player;
    u32 playerWord44 = currentPlayer->word44;
    if (completedFade) synth = voice->synth;
    voice->playerWord44 = playerWord44;
    u32 flag = synthFlag;
    synth->setGamepadSurround(flag);
    currentPlayer = player;
    voice->playerFlag3E = currentPlayer->flag3E;
    currentPlayer = player;
    voice->playerWord4C = currentPlayer->word4C;
    return voice;
}
/* 0288E4D8 */
void SequenceTrack::stopVoices() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    SequenceVoice_l* voice = firstVoice;
    while (gabi::ea(voice) != 0) {
        SequenceVoice_l* next = voice->next;
        stopSequenceVoiceHardware(voice);
        invokeSequenceVoiceCallback(voice, 0);
        freeSequenceVoice(voice);
        voice = next;
    }
    firstVoice = NULL;
}
/* 0288E54C */
void SequenceTrack::pauseVoices(u32 paused) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    SequenceVoice_l* voice = firstVoice;
    while (gabi::ea(voice) != 0) {
        if (voice->active != 0 && (u32)(voice->paused != 0) != paused) {
            PlaybackVoice* synth = voice->synth;
            voice->paused = paused;
            synth->requestPause(paused);
        }
        voice = voice->next;
    }
}
/* 0288E5C4 */
void SequenceTrack::setMuteMode(u32 mode) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    switch (mode) {
    case 0: muted = 0; break;
    case 1: muted = 1; break;
    case 2: releaseVoices(-1); detachVoiceCallbacks(); muted = 1; break;
    case 3: stopVoices(); muted = 1; break;
    }
}
/* 0288E660 */
void SequenceTrack::fadeVoices(u32 silent, u32 duration) {
    SequenceVoice_l* voice = firstVoice;
    silenced = silent;
    if (gabi::ea(voice) == 0) return;
    u32 frames = (duration + 2) / 3;
    do {
        u32 oldFrames = voice->fadeFrames;
        u32 counter = voice->fadeCounter;
        u8 target = silent == 0 ? 255 : 0;
        if (counter >= oldFrames) {
            u8 oldTarget = voice->fadeTarget;
            voice->fadeTarget = target;
            voice->fadeOrigin = oldTarget;
            voice->fadeCounter = 0;
            voice->fadeFrames = frames;
        } else {
            s32 origin = voice->fadeOrigin;
            s32 goal = voice->fadeTarget;
            voice->fadeFrames = frames;
            s32 portion = ((goal - origin) * (s32)counter) / (s32)oldFrames;
            voice->fadeCounter = 0;
            voice->fadeTarget = target;
            voice->fadeOrigin = origin + portion;
        }
        voice = voice->next;
    } while (gabi::ea(voice) != 0);
}
/* 0288DD54 */
void SequenceTrack::setSequence(const be<u8>* begin, u32 size) {
    sequenceBegin = begin;
    sequenceEnd = gabi::at<const be<u8>>(gabi::ea(begin) + size);
}
/* 0288DD64 */
void SequenceTrack::start() {
    waitCount = 0;
    active = 1;
    waiting = 0;
    noteWait = 0;
}
/* 0288DFD4 */
void SequenceTrack::prependVoice(SequenceVoice_l* voice) {
    voice->next = (SequenceVoice_l*)firstVoice;
    firstVoice = voice;
}
/* 0288E718 */
void SequenceTrack::setBiquad(s32 type, f32 value) {
    biquadType = type;
    biquadValue = value;
}
/* 0288E724 */
void SequenceTrack::setReleasePriorityFixed(u8 fixed) {
    releasePriorityFixed = fixed;
}
/* 0288E72C */
void SequenceTrack::setPanCurve(u8 curve) {
    panCurve = curve;
}
/* 0288E734 */
void SequenceTrack::setVelocityScale(u8 scale) {
    velocityScale = scale;
}
/* 0288E73C */
void SequenceTrack::setOutputLine(u32 line) {
    outputLine = line;
}
/* 0288E744 */
void SequenceTrack::setVariable(u32 index, s16 value) {
    be<s16>* variable = gabi::at<be<s16>>(gabi::ea(variables) + index * sizeof(s16));
    *variable = value;
}
/* 0288E754 */
be<s16>* SequenceTrack::variable(s32 index) {
    if (index >= 16) return NULL;
    return gabi::at<be<s16>>(gabi::ea(variables) + (u32)index * sizeof(s16));
}
/* 0288E774 */
void SequenceTrack::setUpdateRequested(u8 requested) {
    updateRequested = requested;
}
/* 0288E77C */
static MixVolume* createTrackMixVolume(MixVolume* volume) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(volume) == 0) volume = (MixVolume*)archiveConstructStorage(sizeof(MixVolume));
    if (gabi::ea(volume) != 0) {
        volume->frontRight = 1.0f;
        volume->lfe = 0.0f;
        volume->rearLeft = 0.0f;
        volume->center = 0.0f;
        volume->frontLeft = 1.0f;
        volume->rearRight = 0.0f;
    }
    return volume;
}
/* 0288E7D8 */
static OutputParam* createTrackOutput(OutputParam* param) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(param) == 0) param = (OutputParam*)archiveConstructStorage(sizeof(OutputParam));
    if (gabi::ea(param) != 0) __construct_array(param->mixVolume, 2, sizeof(MixVolume), (void* (*)(void*))createTrackMixVolume);
    return param;
}
/* 0288E830 */
static SequenceModulator_l* createTrackModulator(SequenceModulator_l* modulator) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (gabi::ea(modulator) == 0) modulator = (SequenceModulator_l*)archiveConstructStorage(sizeof(SequenceModulator_l));
    if (gabi::ea(modulator) != 0) resetSequenceModulator(modulator);
    return modulator;
}
/* 0288F304 */
void SoundArchive::setReader(SoundArchiveReader* source) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    reader = source;
    fileBlockOffset = source->header.fileBlockOffset();
}
/* 0288F33C */
void SoundArchive::clearReader() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    SoundArchiveReader* source = reader;
    if (gabi::ea(source) != 0) {
        source->clear();
        reader = NULL;
    }
    rootDirectory[1] = 0;
    rootDirectory[0] = '/';
}
/* 0288E934 */
u32 SoundArchive::readSoundInfo(u32 id, ArchiveSoundSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readSoundInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applySound(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288E9BC */
u32 SoundArchive::readSequenceInfo(u32 id, ArchiveSequenceSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readSequenceInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applySequence(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288EA44 */
u32 SoundArchive::readBankInfo(u32 id, ArchiveBankSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readBankInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyBank(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288EACC */
u32 SoundArchive::readPlayerInfo(u32 id, ArchivePlayerSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readPlayerInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyPlayer(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288EBCC */
u32 SoundArchive::readStreamInfo(u32 id, ArchiveStreamSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readStreamInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyStream(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288EC54 */
u32 SoundArchive::readPrefetchInfo(u32 id, ArchivePrefetchSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readPrefetchInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyPrefetch(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288ECDC */
u32 SoundArchive::readWaveInfo(u32 id, ArchiveWaveSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readWaveInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyWave(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288ED64 */
u32 SoundArchive::readWaveArchiveInfo(u32 id, ArchiveWaveArchiveSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readWaveArchiveInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyWaveArchive(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288EDEC */
u32 SoundArchive::readSoundGroupInfo(u32 id, ArchiveSoundGroupSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readSoundGroupInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applySoundGroup(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288EE74 */
u32 SoundArchive::readGroupInfo(u32 id, ArchiveGroupSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readGroupInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyGroup(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288EEFC */
u32 SoundArchive::readFileInfo(u32 id, ArchiveFileSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readFileInfo(id, out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyFile(id, out);
            result = 1;
        }
    }
    return result;
}
/* 0288EB54 */
u32 SoundArchive::readProjectInfo(ArchiveProjectSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 result = reader->readProjectInfo(out);
    if (result != 0) {
        ArchiveExtension_l* provider = extension;
        if (gabi::ea(provider) != 0) {
            if (provider->enabled) provider->applyProject(out);
            result = 1;
        }
    }
    return result;
}
/* 028909D0 */
u32 SoundArchiveReader::readStreamInfo(u32 id, ArchiveStreamSummary* out) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveSoundRecord* sound = (ArchiveSoundRecord*)info->soundInfo(id);
    if (gabi::ea(sound) == 0 || sound->soundKind() != ArchiveSound_Stream) return 0;
    ArchiveStreamDetail* detail = (ArchiveStreamDetail*)sound->streamDetail();
    out->trackFlags = (u16)detail->trackFlags;
    out->channelCount = (u16)detail->channelCount;
    const ArchivePrefetchRecord* prefetch = (const ArchivePrefetchRecord*)detail->prefetchData();
    u32 prefetchChannels = 1;
    if (gabi::ea(prefetch) != 0) prefetchChannels = (u32)prefetch->flags & 0xFF;
    out->prefetchChannels = prefetchChannels;
    if (isVersionTwoOneOrLater()) {
        reinterpret_cast<be<u32>&>(out->pitch) = (u32)reinterpret_cast<const be<u32>&>(detail->pitch);
        const be<u32>* routing = (const be<u32>*)detail->routingData();
        u32 packed = *routing;
        out->routing[0] = packed >> 24;
        out->routing[1] = packed >> 16;
        out->routing[2] = packed >> 8;
        out->routing[3] = packed;
    } else {
        out->routing[0] = 127;
        out->pitch = 1.0f;
        for (u32 i = 1; i < 4; ++i) out->routing[i] = 0;
    }
    const ArchiveReferenceTable* tracks = detail->trackTable();
    if (gabi::ea(tracks) == 0) return 1;
    u32 count = tracks->count;
    for (u32 i = 0; i < count && i < 8; ++i) {
        const ArchiveStreamTrackRecord* track = (const ArchiveStreamTrackRecord*)ArchiveReferenceTable::streamTrack(i, tracks);
        ArchiveStreamTrackSummary* destination = &out->tracks[i];
        destination->volume = (u8)track->volume;
        destination->pan = (u8)track->pan;
        destination->surroundPan = (u8)track->surroundPan;
        destination->flags = (u8)track->flags;
        const ArchiveChannelTable* channels = track->channels.resolve<ArchiveChannelTable>(track);
        destination->channelCount = (u32)channels->count;
        if (isVersionTwoOneOrLater()) {
            const be<u32>* routing = track->routing.resolve<be<u32>>(track);
            u32 packed = *routing;
            destination->routing[0] = packed >> 24;
            destination->routing[1] = packed >> 16;
            destination->routing[2] = packed >> 8;
            destination->routing[3] = packed;
        } else {
            destination->routing[0] = 127;
            for (u32 j = 1; j < 4; ++j) destination->routing[j] = 0;
        }
        u32 channelCount;
        if (isInfoVersionTwoOneOrLater()) {
            channelCount = destination->channelCount;
            destination->priority = (u8)track->priority;
            destination->filter = (u8)track->filter;
            destination->filterValue = (u8)track->filterValue;
        } else {
            channelCount = destination->channelCount;
            destination->priority = 64;
            destination->filter = 0;
            destination->filterValue = 0;
        }
        if (channelCount > 2) channelCount = 2;
        for (u32 j = 0; j < channelCount; ++j) {
            const ArchiveChannelTable* channels = track->channels.resolve<ArchiveChannelTable>(track);
            destination->channels[j] = (u8)channels->channels[j];
        }
    }
    return 1;
}
static inline u32 archivePathLength(const char* text) {
    const be<u8>* bytes = (const be<u8>*)text;
    u32 length = 0;
    while (bytes[length]) ++length;
    return length;
}
/* 0288F190 */
void SoundArchive::setRootDirectory(const char* directory) {
    u32 length = archivePathLength(directory);
    u32 count = length;
    if (count > 0x27E) count = 0x27E;
    const be<u8>* input = (const be<u8>*)directory;
    u32 copied = 0;
    while (copied < count) {
        u8 byte = input[copied];
        if (byte == 0) break;
        rootDirectory[copied++] = byte;
    }
    rootDirectory[copied] = 0;
    u32 end = length;
    if (input[length - 1] != '/') {
        *gabi::at<be<u8>>(gabi::ea(rootDirectory) + length) = '/';
        ++end;
    }
    *gabi::at<be<u8>>(gabi::ea(rootDirectory) + end) = 0;
}
/* 0288F218 */
const char* SoundArchive::resolvePath(const char* path, char* output, u32 capacity) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    const be<u8>* input = (const be<u8>*)path;
    if (input[0] == '/') return path;
    u32 length = archivePathLength(path);
    u32 prefixLength = archivePathLength((const char*)rootDirectory);
    if ((u32)(length + prefixLength) >= capacity) return NULL;
    u32 count = prefixLength + 1;
    if (count > capacity - 1) count = capacity - 1;
    be<u8>* destination = (be<u8>*)output;
    u32 copied = 0;
    while (copied < count) {
        u8 byte = rootDirectory[copied];
        if (byte == 0) break;
        destination[copied++] = byte;
    }
    destination[copied] = 0;
    appendArchivePath(output, capacity, path, length + 1);
    return output;
}
/* 0288EF94 */
ArchiveIoStream_l* SoundArchive::openExternalStream(void* storage, u32 storageSize, const char* path, u32 offset, u32 size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<be<u8>[0x280]> resolved(8);
    be<u8>* buffer = (be<u8>*)resolved.get();
    const char* selected = path;
    const be<u8>* input = (const be<u8>*)path;
    if (input[0] != '/') {
        u32 length = archivePathLength(path);
        u32 prefixLength = archivePathLength((const char*)rootDirectory);
        if ((u32)(length + prefixLength) >= 0x27F) return NULL;
        u32 count = prefixLength + 1;
        if (count > 0x27E) count = 0x27E;
        u32 copied = 0;
        while (copied < count) {
            u8 byte = rootDirectory[copied];
            if (byte == 0) break;
            buffer[copied++] = byte;
        }
        buffer[copied] = 0;
        appendArchivePath((char*)resolved.get(), 0x27F, path, length + 1);
        selected = (const char*)resolved.get();
    }
    ArchiveBackendVtable* backend = (ArchiveBackendVtable*)(void*)vtable;
    return gabi::call_ptr<ArchiveIoStream_l*>(backend->openExternal.function, this, storage, storageSize, selected, offset, size);
}
/* 0288F0A8 */
ArchiveIoStream_l* SoundArchive::openFileStream(u32 fileId, void* storage, u32 storageSize, u32 offset, u32 size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<ArchiveFileSummary> file(8);
    file->size = 0xFFFFFFFF;
    file->externalPath = NULL;
    file->offset = 0xFFFFFFFF;
    if (!readFileInfo(fileId, file.get())) return NULL;
    const char* external = file->externalPath;
    if (gabi::ea(external) != 0) return openExternalStream(storage, storageSize, external, offset, size);
    u32 fileSize = file->size;
    if (fileSize == 0xFFFFFFFF) return NULL;
    u32 fileOffset = file->offset;
    if (fileOffset == 0xFFFFFFFF) return NULL;
    ArchiveBackendVtable* backend = (ArchiveBackendVtable*)(void*)vtable;
    u32 target = backend->openEmbedded.function;
    u32 position = fileOffset + (u32)fileBlockOffset;
    return gabi::call_ptr<ArchiveIoStream_l*>(target, this, storage, storageSize, position, fileSize);
}
/* Default virtual effects. The three values are forwarded into the stream setup descriptor. */
/* 0288F390 */
void SoundArchive::emptyBackendHook() {
}
/* 0288F394 */
void SoundArchive::emptyBackendFinalHook() {
}
/* 0288F398 */
u32 SoundArchive::defaultStreamDescriptorValue() {
    return 0;
}
/* 0288F3A0 */
u32 SoundArchive::defaultStreamDescriptorExtraValue() {
    return 0;
}
/* 0288F3A8 */
u32 SoundArchive::streamDescriptorConstant() {
    return 16;
}
static inline const ArchiveIndexTable* groupWaveArchives(const ArchiveSoundGroupRecord* group) {
    const ArchiveReference* reference = NULL;
    if (group->waveArchive.kind == 0x2205) reference = group->waveArchive.resolve<ArchiveReference>(group);
    return reference->resolve<ArchiveIndexTable>(reference);
}
/* 02890DF8 */
const ArchiveIndexTable* SoundArchiveReader::waveArchiveDependencies(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    switch (id >> 24) {
    case ArchiveItem_Sound: {
        ArchiveSoundRecord* sound = (ArchiveSoundRecord*)info->soundInfo(id);
        if (sound->soundKind() != ArchiveSound_Wave) return NULL;
        u32 index = 0;
        while (index < info->soundGroupTable()->count) {
            const ArchiveSoundGroupRecord* group = (const ArchiveSoundGroupRecord*)info->soundGroupInfo(0x02000000 | index);
            if (id >= group->firstSoundId && group->lastSoundId >= id) return groupWaveArchives(group);
            ++index;
        }
        return NULL;
    }
    case ArchiveItem_SoundGroup: {
        const ArchiveSoundGroupRecord* group = (const ArchiveSoundGroupRecord*)info->soundGroupInfo(id);
        return groupWaveArchives(group);
    }
    case ArchiveItem_Bank: {
        const ArchiveBankRecord* bank = (const ArchiveBankRecord*)info->bankInfo(id);
        return bank->waveArchives.resolve<ArchiveIndexTable>(bank);
    }
    default: return NULL;
    }
}
/* 0288EF8C */
const ArchiveIndexTable* SoundArchive::waveArchiveDependencies(u32 id) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return reader->waveArchiveDependencies(id);
}
/* 02891044 */
s32 ArchiveLoader::loadFile(u32 fileId, void* destination, s32 size, s32 offset, s32 chunkSize) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveIoStream_l* stream = archive->openFileStream(fileId, scratch, sizeof(scratch), 0, 0);
    if (gabi::ea(stream) == 0) return -1;
    if (!stream->isOpen() || !stream->canRead()) {
        stream->close();
        return -1;
    }
    stream->seek(offset, 0);
    if (chunkSize == 0) {
        u32 amount = ((u32)size + 63) & ~63u;
        if (stream->read(destination, amount) < 0) {
            stream->close();
            return -1;
        }
    } else {
        s32 remaining = size;
        while (remaining > 0) {
            s32 part = remaining < chunkSize ? remaining : chunkSize;
            u32 amount = ((u32)part + 63) & ~63u;
            s32 count = stream->read(destination, amount);
            if (count < 0) {
                stream->close();
                return -1;
            }
            if (remaining <= count) break;
            remaining -= count;
            destination = gabi::at<void>(gabi::ea(destination) + (u32)count);
        }
    }
    stream->close();
    return size;
}
/* WWHD glue
 ext 028A3350 u32 lookupSequenceRegion(void*,nw::snd::SequenceRegion_l*,u32,u32,s32)
 ext 0288BF34 nw::snd::SequenceVoice_l* allocateSequenceVoice(void*,u32,nw::snd::SequenceVoiceRequest_l*)
 ext 028A5098 void setSequenceEnvelope1(void*,u32)
 ext 028A5190 void setSequenceEnvelope2(void*,u32)
 ext 028A51BC void setSequenceEnvelope3(void*,u32)
 ext 028A50B0 void setSequenceEnvelope5(void*,s32)
 ext 028A4AB0 void setSequenceVoicePitch(nw::snd::SequenceVoice_l*,s32,u32,f32)
 ext 0289ECC4 void nw::snd::PlaybackVoice::setGamepadSurround(u32)
 data sSequenceVoiceRequestDefaults 101820A0
 data sSequenceTrackVtableHeader 101820C4
 ext 0288D2E8 void updateSequenceTrackParameters(nw::snd::SequenceTrack*)
 ext 028A51C4 void setSequenceEnvelopeRelease(void*,u32)
 ext 028A4A30 void releaseSequenceVoice(nw::snd::SequenceVoice_l*)
 ext 028A3EE0 void clearSequenceVoiceCallback(nw::snd::SequenceVoice_l*)
 ext 028A4A94 void tryReleaseSequenceVoice(nw::snd::SequenceVoice_l*)
 ext 028A4144 void advanceSequenceVoiceCounter(nw::snd::SequenceVoice_l*,u32)
 ext 028A40F4 void stopSequenceVoiceHardware(nw::snd::SequenceVoice_l*)
 ext 028A3A44 void invokeSequenceVoiceCallback(nw::snd::SequenceVoice_l*,u32)
 ext 028A3EAC void freeSequenceVoice(nw::snd::SequenceVoice_l*)
 ext 0289D8BC void nw::snd::PlaybackVoice::requestPause(u32)
 ext 028F6930 u32 appendArchivePath(char*,u32,const char*,u32)
 ext 028A54F4 void resetSequenceModulator(nw::snd::SequenceModulator_l*)
 data sArchiveLoaderVtableHeader 10182144
 data sArchiveVtableHeader 101820DC
 ext 0273AF40 void destroyArchiveStorage(void*)
 ext 0273AD10 void* archiveConstructStorage(u32)
*/
