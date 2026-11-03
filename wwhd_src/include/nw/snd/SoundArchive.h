/* nw::snd sound archive layouts (NW-SND#4): archive blocks and relative references, item records
 * and summaries, the archive facade and loader, and the sequence track. From WWHD's binary only.
 * _l types are local views of classes owned by other nw::snd units. */
#pragma once
#include "wwhd.h"
#include "nw/snd/BasicSound.h"
namespace nw { namespace snd { struct PlaybackVoice; } }
namespace nw { namespace snd {
struct ArchiveReference {
    be<u16> kind;
    be<u16> reserved;
    be<u32> offset;
    /* The referenced object: offset bytes past base (32-bit guest address arithmetic). */
    template <class T> T* resolve(const void* base) const { return gabi::at<T>(gabi::ea(base) + (u32)offset); }
};
WWHD_OFFSET(ArchiveReference, offset, 4);
WWHD_SIZE(ArchiveReference, 8);
enum ArchiveBlockKind {
    ArchiveBlock_Strings = 0x2000, ArchiveBlock_Info = 0x2001, ArchiveBlock_Files = 0x2002,
};
struct ArchiveBlockReference {
    be<u16> kind;
    be<u16> reserved;
    be<u32> offset;
    be<u32> size;
};
WWHD_OFFSET(ArchiveBlockReference, offset, 4);
WWHD_OFFSET(ArchiveBlockReference, size, 8);
WWHD_SIZE(ArchiveBlockReference, 12);
struct SoundArchiveHeader {
    be<u32> magic;
    be<u32> byteOrderAndHeaderSize;
    be<u32> version;
    be<u32> fileSize;
    be<u16> blockCount;
    be<u16> reserved;
    ArchiveBlockReference blocks[3];
    ArchiveBlockReference* findBlock(u32 kind);
    u32 stringBlockSize();
    u32 infoBlockSize();
    u32 stringBlockOffset();
    u32 infoBlockOffset();
    u32 fileBlockOffset();
};
WWHD_OFFSET(SoundArchiveHeader, blocks, 0x14);
WWHD_SIZE(SoundArchiveHeader, 0x38);
struct ArchiveStringEntry {
    ArchiveReference string;
    be<u32> length;
};
WWHD_SIZE(ArchiveStringEntry, 12);
struct ArchiveStringTable {
    be<u32> count;
    ArchiveStringEntry entries[1];
};
WWHD_OFFSET(ArchiveStringTable, entries, 4);
struct ArchiveStringMatch { be<u32> stringId, itemId; };
struct ArchiveStringNode {
    be<u16> flags;
    be<u16> bitIndex;
    be<u32> left, right;
    ArchiveStringMatch match;
};
WWHD_OFFSET(ArchiveStringNode, match, 0xC);
WWHD_SIZE(ArchiveStringNode, 0x14);
struct ArchiveStringTrie {
    be<u32> rootIndex, count;
    ArchiveStringNode nodes[1];
    ArchiveStringMatch* find(const char* text, s32 length);
};
WWHD_OFFSET(ArchiveStringTrie, nodes, 8);
struct ArchiveStringBlock {
    ArchiveReference tables[2];
    void* table(s32 index);
    const char* stringAt(s32 index);
    u32 lookup(s32 tableIndex, const char* text);
};
WWHD_SIZE(ArchiveStringBlock, 16);
struct ArchiveReferenceTable {
    static void* streamTrack(u32 index, const ArchiveReferenceTable* table);
    be<u32> count;
    ArchiveReference entries[1];
};
WWHD_OFFSET(ArchiveReferenceTable, entries, 4);
enum ArchiveItemKind {
    ArchiveItem_Sound = 1, ArchiveItem_SoundGroup = 2, ArchiveItem_Bank = 3,
    ArchiveItem_Player = 4, ArchiveItem_WaveArchive = 5, ArchiveItem_Group = 6,
};
struct SoundArchiveInfo {
    ArchiveReference sounds;
    ArchiveReference soundGroups;
    ArchiveReference banks;
    ArchiveReference waveArchives;
    ArchiveReference groups;
    ArchiveReference players;
    ArchiveReference files;
    ArchiveReference project;
    ArchiveReferenceTable* soundTable();
    ArchiveReferenceTable* soundGroupTable();
    ArchiveReferenceTable* bankTable();
    ArchiveReferenceTable* waveArchiveTable();
    ArchiveReferenceTable* groupTable();
    ArchiveReferenceTable* playerTable();
    ArchiveReferenceTable* fileTable();
    void* projectInfo();
    static u32 itemKind(u32 id);
    u32 itemFileId(u32 id);
    u32 itemLabelId(u32 id);
    void* soundInfo(u32 id);
    void* soundGroupInfo(u32 id);
    void* bankInfo(u32 id);
    void* waveArchiveInfo(u32 id);
    void* groupInfo(u32 id);
    void* playerInfo(u32 id);
    void* fileInfo(u32 id);
};
WWHD_OFFSET(SoundArchiveInfo, soundGroups, 8);
WWHD_OFFSET(SoundArchiveInfo, banks, 0x10);
WWHD_OFFSET(SoundArchiveInfo, waveArchives, 0x18);
WWHD_OFFSET(SoundArchiveInfo, groups, 0x20);
WWHD_OFFSET(SoundArchiveInfo, players, 0x28);
WWHD_OFFSET(SoundArchiveInfo, files, 0x30);
WWHD_OFFSET(SoundArchiveInfo, project, 0x38);
WWHD_SIZE(SoundArchiveInfo, 0x40);
/* SoundArchive::soundKind(): ArchivePlayback::startSound (0289533C) prepares kind 1 as a sequence (resource-hook
 * query "bxseq", 028950CC; the record with bank ids, 0288E9BC) and kind 3 as a wave sound ("bxwsd", 028951C4;
 * 0288ECDC); ArchiveFileLoader::loadNamedGroup loads them with loadSequence / loadWaveSound. */
enum ArchiveSoundKind { ArchiveSound_None, ArchiveSound_Sequence, ArchiveSound_Stream, ArchiveSound_Wave };
struct ArchiveSoundRecord {
    be<u32> fileId;
    be<u32> playerId;
    be<u8> volume;
    be<u8> remoteFilter;
    be<u16> reserved;
    ArchiveReference detail;
    be<u32> optionFlags;
    be<u32> optionValues[1];
    u32 labelId();
    void* streamDetail();
    void* waveDetail();
    void* sequenceDetail();
    u32 panMode();
    u32 panCurve();
    u32 playerPriority();
    u32 actorPlayerId();
    u32 frontBypass();
    u32 soundKind();
};
WWHD_OFFSET(ArchiveSoundRecord, detail, 0xC);
WWHD_OFFSET(ArchiveSoundRecord, optionFlags, 0x14);
WWHD_OFFSET(ArchiveSoundRecord, optionValues, 0x18);
struct ArchiveBankRecord {
    be<u32> fileId;
    ArchiveReference waveArchives;
    be<u32> optionFlags;
    be<u32> optionValues[1];
    u32 labelId();
};
WWHD_OFFSET(ArchiveBankRecord, optionFlags, 0xC);
struct ArchiveWaveArchiveRecord {
    be<u32> fileId;
    be<u8> loadingFlags;
    u8 padding[3];
    be<u32> optionFlags;
    be<u32> optionValues[1];
    u32 labelId();
    u32 waveCount();
};
WWHD_OFFSET(ArchiveWaveArchiveRecord, optionFlags, 8);
struct ArchiveSoundGroupRecord {
    be<u32> firstSoundId;
    be<u32> lastSoundId;
    ArchiveReference files;
    ArchiveReference waveArchive;
    be<u32> optionFlags;
    be<u32> optionValues[1];
    u32 labelId();
};
WWHD_OFFSET(ArchiveSoundGroupRecord, optionFlags, 0x18);
struct ArchiveGroupRecord {
    be<u32> fileId;
    be<u32> optionFlags;
    be<u32> optionValues[1];
    u32 labelId();
};
WWHD_OFFSET(ArchiveGroupRecord, optionFlags, 4);
struct ArchivePlayerRecord {
    be<u32> soundLimit;
    be<u32> optionFlags;
    be<u32> optionValues[1];
    u32 labelId();
    u32 heapSize();
};
WWHD_OFFSET(ArchivePlayerRecord, optionFlags, 4);
struct ArchiveIndexTable { be<u32> count; be<u32> entries[1]; };
WWHD_OFFSET(ArchiveIndexTable, entries, 4);
struct ArchiveStreamDetail {
    be<u16> trackFlags, channelCount;
    ArchiveReference tracks;
    be<f32> pitch;
    ArchiveReference routing;
    ArchiveReference prefetch;
    ArchiveReferenceTable* trackTable();
    void* routingData();
    void* prefetchData();
};
WWHD_OFFSET(ArchiveStreamDetail, tracks, 4);
WWHD_OFFSET(ArchiveStreamDetail, routing, 0x10);
WWHD_OFFSET(ArchiveStreamDetail, prefetch, 0x18);
WWHD_SIZE(ArchiveStreamDetail, 0x20);
struct ArchiveWaveDetail {
    be<u32> waveIndex, trackFlags;
    be<u32> optionFlags;
    be<u32> packedPriority;
    u32 priority();
    u32 priorityFix();
};
WWHD_SIZE(ArchiveWaveDetail, 0x10);
struct ArchiveSequenceDetail {
    ArchiveReference bankIds;
    be<u32> trackFlags;
    be<u32> optionFlags;
    be<u32> optionValues[1];
    ArchiveIndexTable* bankTable();
    void copyBankIds(be<u32>* out);
    u32 optionalId();
    u32 priority();
    u32 priorityFix();
};
WWHD_OFFSET(ArchiveSequenceDetail, trackFlags, 8);
WWHD_OFFSET(ArchiveSequenceDetail, optionFlags, 0xC);
struct ArchiveFileRecord {
    ArchiveReference location;
    u32 locationKind();
    void* embeddedLocation();
    void* externalLocation();
};
WWHD_SIZE(ArchiveFileRecord, 8);
struct ArchiveSoundSummary {
    be<u32> fileId, playerId;
    be<u8> actorPlayerId, playerPriority, volume, remoteFilter;
    be<u32> panMode, panCurve;
    be<u8> frontBypass;
};
WWHD_OFFSET(ArchiveSoundSummary, actorPlayerId, 8);
WWHD_OFFSET(ArchiveSoundSummary, panMode, 0xC);
WWHD_OFFSET(ArchiveSoundSummary, frontBypass, 0x14);
struct ArchiveBankSummary { be<u32> fileId; };
struct ArchivePlayerSummary { be<u32> soundLimit, option; };
struct ArchiveSoundGroupSummary { be<u32> firstSoundId, lastSoundId; gptr<void> files; };
struct ArchiveWaveArchiveSummary { be<u32> fileId, option; be<u8> loadingFlags; };
struct ArchiveProjectSummary { be<u32> counts[7]; };
WWHD_SIZE(ArchiveProjectSummary, 0x1C);
struct ArchiveEmbeddedLocation { be<u32> reserved, offset, size; };
struct ArchiveFileSummary { be<u32> size, offset; gptr<const char> externalPath; };
WWHD_SIZE(ArchiveFileSummary, 12);
struct ArchiveGroupSummary { be<u32> fileId, size; };
struct ArchiveSequenceSummary {
    be<u32> startOffset;
    be<u32> bankIds[4];
    be<u32> trackFlags;
    be<u8> priority, priorityFix;
};
WWHD_OFFSET(ArchiveSequenceSummary, bankIds, 4);
WWHD_OFFSET(ArchiveSequenceSummary, trackFlags, 0x14);
WWHD_OFFSET(ArchiveSequenceSummary, priority, 0x18);
struct ArchiveWaveSummary { be<u32> waveIndex, trackFlags; be<u8> priority, priorityFix; };
struct ArchivePrefetchRecord { be<u32> flags, offset, size; };
struct ArchivePrefetchSummary { be<u8> enabled; u8 padding[3]; be<u32> offset, size; };
struct ArchiveChannelTable { be<u32> count; be<u8> channels[1]; };
struct ArchiveStreamTrackRecord {
    be<u8> volume, pan, surroundPan, flags;
    ArchiveReference channels;
    ArchiveReference routing;
    be<u8> priority, filter, filterValue, reserved;
};
WWHD_OFFSET(ArchiveStreamTrackRecord, channels, 4);
WWHD_OFFSET(ArchiveStreamTrackRecord, routing, 0xC);
WWHD_OFFSET(ArchiveStreamTrackRecord, priority, 0x14);
struct ArchiveStreamTrackSummary {
    be<u8> volume, pan, surroundPan, flags;
    be<u8> routing[4];
    be<u8> priority, filter, filterValue, channelCount;
    be<u8> channels[2];
};
WWHD_SIZE(ArchiveStreamTrackSummary, 0xE);
struct ArchiveStreamSummary {
    be<u16> trackFlags, channelCount;
    be<f32> pitch;
    be<u8> routing[4];
    ArchiveStreamTrackSummary tracks[8];
    be<u32> prefetchChannels;
};
WWHD_OFFSET(ArchiveStreamSummary, tracks, 0xC);
WWHD_OFFSET(ArchiveStreamSummary, prefetchChannels, 0x7C);
WWHD_SIZE(ArchiveStreamSummary, 0x80);
struct SoundArchiveReader {
    SoundArchiveHeader header;
    gptr<ArchiveStringBlock> strings;
    gptr<SoundArchiveInfo> info;
    gptr<void> files;
    static SoundArchiveReader* create(SoundArchiveReader* reader);
    static u32 validateHeader(const SoundArchiveHeader* file);
    u32 readHeader(const SoundArchiveHeader* file);
    void clear();
    u32 isVersionTwoOneOrLater();
    u32 isInfoVersionTwoOneOrLater();
    void setStringBlock(void* block);
    void setInfoBlock(void* block);
    u32 itemFileId(u32 id);
    u32 findItem(const char* text);
    const char* itemLabel(u32 id);
    u32 soundKind(u32 id);
    u32 playerCount();
    u32 groupCount();
    u32 waveArchiveCount();
    u32 fileCount();
    u32 readPlayerInfo(u32 id, ArchivePlayerSummary* out);
    u32 readWaveArchiveInfo(u32 id, ArchiveWaveArchiveSummary* out);
    u32 readSoundInfo(u32 id, ArchiveSoundSummary* out);
    u32 readBankInfo(u32 id, ArchiveBankSummary* out);
    u32 readSoundGroupInfo(u32 id, ArchiveSoundGroupSummary* out);
    u32 readProjectInfo(ArchiveProjectSummary* out);
    u32 readFileInfo(u32 id, ArchiveFileSummary* out);
    u32 readGroupInfo(u32 id, ArchiveGroupSummary* out);
    u32 readSequenceInfo(u32 id, ArchiveSequenceSummary* out);
    u32 readWaveInfo(u32 id, ArchiveWaveSummary* out);
    u32 readPrefetchInfo(u32 id, ArchivePrefetchSummary* out);
    u32 readStreamInfo(u32 id, ArchiveStreamSummary* out);
    const ArchiveIndexTable* waveArchiveDependencies(u32 id);
};
WWHD_OFFSET(SoundArchiveReader, strings, 0x38);
WWHD_OFFSET(SoundArchiveReader, info, 0x3C);
WWHD_OFFSET(SoundArchiveReader, files, 0x40);
WWHD_SIZE(SoundArchiveReader, 0x44);
struct ArchiveExtensionVtable_l {
    VtableEntry header, destroy, reserved;
    VtableEntry sound, bank, player, soundGroup, group, waveArchive, file, project;
    VtableEntry reservedDetail;
    VtableEntry sequence, stream, prefetch, wave;
};
WWHD_OFFSET(ArchiveExtensionVtable_l, sound, 0x18);
WWHD_OFFSET(ArchiveExtensionVtable_l, project, 0x50);
WWHD_OFFSET(ArchiveExtensionVtable_l, wave, 0x78);
WWHD_SIZE(ArchiveExtensionVtable_l, 0x80);
struct ArchiveExtension_l {
    be<u8> enabled;
    u8 padding[3];
    gptr<ArchiveExtensionVtable_l> vtable;
    void applySound(u32 id, ArchiveSoundSummary* out) { gabi::call_ptr<void>(vtable->sound.function, this, id, out); }
    void applySequence(u32 id, ArchiveSequenceSummary* out) { gabi::call_ptr<void>(vtable->sequence.function, this, id, out); }
    void applyBank(u32 id, ArchiveBankSummary* out) { gabi::call_ptr<void>(vtable->bank.function, this, id, out); }
    void applyPlayer(u32 id, ArchivePlayerSummary* out) { gabi::call_ptr<void>(vtable->player.function, this, id, out); }
    void applyStream(u32 id, ArchiveStreamSummary* out) { gabi::call_ptr<void>(vtable->stream.function, this, id, out); }
    void applyPrefetch(u32 id, ArchivePrefetchSummary* out) { gabi::call_ptr<void>(vtable->prefetch.function, this, id, out); }
    void applyWave(u32 id, ArchiveWaveSummary* out) { gabi::call_ptr<void>(vtable->wave.function, this, id, out); }
    void applyWaveArchive(u32 id, ArchiveWaveArchiveSummary* out) { gabi::call_ptr<void>(vtable->waveArchive.function, this, id, out); }
    void applySoundGroup(u32 id, ArchiveSoundGroupSummary* out) { gabi::call_ptr<void>(vtable->soundGroup.function, this, id, out); }
    void applyGroup(u32 id, ArchiveGroupSummary* out) { gabi::call_ptr<void>(vtable->group.function, this, id, out); }
    void applyFile(u32 id, ArchiveFileSummary* out) { gabi::call_ptr<void>(vtable->file.function, this, id, out, 0); }
    void applyProject(ArchiveProjectSummary* out) { gabi::call_ptr<void>(vtable->project.function, this, out); }
};
WWHD_OFFSET(ArchiveExtension_l, vtable, 4);
WWHD_SIZE(ArchiveExtension_l, 8);
struct ArchiveIoStreamVtable_l {
    u8 prefix[0x18];
    VtableEntry close, read;
    u8 basicOperations[0x30];
    VtableEntry canRead;
    u8 cursorOperations[0x30];
    VtableEntry seek;
    u8 statusOperations[0x10];
    VtableEntry isOpen;
};
WWHD_OFFSET(ArchiveIoStreamVtable_l, read, 0x20);
WWHD_OFFSET(ArchiveIoStreamVtable_l, canRead, 0x58);
WWHD_OFFSET(ArchiveIoStreamVtable_l, seek, 0x90);
WWHD_OFFSET(ArchiveIoStreamVtable_l, isOpen, 0xA8);
struct ArchiveIoStream_l {
    u8 prefix[0xC];
    gptr<ArchiveIoStreamVtable_l> vtable;
    void close() { gabi::call_ptr<void>(vtable->close.function, this); }
    s32 read(void* destination, u32 amount) { return gabi::call_ptr<s32>(vtable->read.function, this, destination, amount); }
    u32 canRead() { return gabi::call_ptr<u32>(vtable->canRead.function, this); }
    void seek(s32 offset, u32 origin) { gabi::call_ptr<void>(vtable->seek.function, this, offset, origin); }
    u32 isOpen() { return gabi::call_ptr<u32>(vtable->isOpen.function, this); }
};
WWHD_OFFSET(ArchiveIoStream_l, vtable, 0xC);
struct ArchiveBackendVtable {
    u8 otherEntries[0x50];
    VtableEntry openEmbedded;
    VtableEntry openExternal;
};
WWHD_OFFSET(ArchiveBackendVtable, openEmbedded, 0x50);
WWHD_OFFSET(ArchiveBackendVtable, openExternal, 0x58);
struct SoundArchive {
    gptr<SoundArchiveReader> reader;
    gptr<ArchiveExtension_l> extension;
    be<u8> rootDirectory[0x280];
    be<u32> fileBlockOffset;
    gptr<void> vtable;
    static SoundArchive* create(SoundArchive* archive);
    static void destroy(SoundArchive* archive, u32 flags);
    u32 isAvailable();
    void setReader(SoundArchiveReader* source);
    void clearReader();
    void emptyBackendHook();
    void emptyBackendFinalHook();
    u32 defaultStreamDescriptorValue();
    u32 defaultStreamDescriptorExtraValue();
    u32 streamDescriptorConstant();
    void setRootDirectory(const char* directory);
    const char* resolvePath(const char* path, char* output, u32 capacity);
    ArchiveIoStream_l* openExternalStream(void* storage, u32 storageSize, const char* path, u32 offset, u32 size);
    ArchiveIoStream_l* openFileStream(u32 fileId, void* storage, u32 storageSize, u32 offset, u32 size);
    u32 readSoundInfo(u32 id, ArchiveSoundSummary* out);
    u32 readSequenceInfo(u32 id, ArchiveSequenceSummary* out);
    u32 readBankInfo(u32 id, ArchiveBankSummary* out);
    u32 readPlayerInfo(u32 id, ArchivePlayerSummary* out);
    u32 readStreamInfo(u32 id, ArchiveStreamSummary* out);
    const ArchiveIndexTable* waveArchiveDependencies(u32 id);
    u32 readPrefetchInfo(u32 id, ArchivePrefetchSummary* out);
    u32 readWaveInfo(u32 id, ArchiveWaveSummary* out);
    u32 readWaveArchiveInfo(u32 id, ArchiveWaveArchiveSummary* out);
    u32 readSoundGroupInfo(u32 id, ArchiveSoundGroupSummary* out);
    u32 readGroupInfo(u32 id, ArchiveGroupSummary* out);
    u32 readFileInfo(u32 id, ArchiveFileSummary* out);
    u32 readProjectInfo(ArchiveProjectSummary* out);

    u32 groupCount();
    u32 playerCount();
    u32 waveArchiveCount();
    u32 fileCount();
    const char* itemLabel(u32 id);
    u32 findItem(const char* text);
    u32 itemFileId(u32 id);
    u32 soundKind(u32 id);
};
WWHD_OFFSET(SoundArchive, rootDirectory, 8);
WWHD_OFFSET(SoundArchive, fileBlockOffset, 0x288);
WWHD_OFFSET(SoundArchive, vtable, 0x28C);
WWHD_SIZE(SoundArchive, 0x290);
/* Local views of the unmerged sequence-track/voice interfaces. */
struct SequenceVoice_l {
    u8 prefix[0x10];
    be<u32> envelopeState;
    u8 envelopeParameters[0x9C];
    /* The voice's envelope object starts with envelopeState (0x10). */
    void* envelope() { return &envelopeState; }
    be<u8> paused, active;
    u8 reservedB2;
    be<u8> counterHeld;
    be<u8> releaseSynthGuard;
    u8 parameters[0x147];
    be<f32> noteGain;
    u8 reserved200[0xC];
    be<u8> fadeOrigin, fadeTarget;
    be<u16> fadeFrames, fadeCounter;
    u8 reserved212[0xA];
    be<s32> releaseCountdown;
    be<u32> playerWord40, playerWord44;
    be<u8> noteKey;
    u8 reserved229;
    be<u8> exclusiveGroup;
    u8 reserved22B[9];
    gptr<PlaybackVoice> synth;   // the note's playback voice
    gptr<SequenceVoice_l> next;
    u8 reserved23C[0x98];
    be<u32> playerWord4C;
    u8 reserved2D8[5];
    be<u8> playerFlag3E;
};
WWHD_OFFSET(SequenceVoice_l, active, 0xB1);
WWHD_OFFSET(SequenceVoice_l, fadeOrigin, 0x20C);
WWHD_OFFSET(SequenceVoice_l, releaseCountdown, 0x21C);
WWHD_OFFSET(SequenceVoice_l, synth, 0x234);
WWHD_OFFSET(SequenceVoice_l, next, 0x238);
struct SequencePlayer_l {
    u8 prefix[0x3E];
    be<u8> flag3E;
    u8 reserved3F;
    be<u32> word40, word44, word48, word4C;
    u8 reserved50[0x140];
    be<u8> releaseSynthGuard;
    u8 reserved191[0x17];
    be<u8> notePriority;
    u8 reserved1A9[0xBF];
    /* Bank pools selected by the track's fixed release priority (12 bytes each). */
    u8 bankPools[2][12];
};
WWHD_OFFSET(SequencePlayer_l, releaseSynthGuard, 0x190);
WWHD_OFFSET(SequencePlayer_l, bankPools, 0x268);
WWHD_OFFSET(SequencePlayer_l, notePriority, 0x1A8);
struct SequenceTrack;
/* The track's GHS vtable: header, deleting destructor, then the per-step sequence command. */
struct SequenceTrackVtable { VtableEntry header, destroy, step; };
WWHD_OFFSET(SequenceTrackVtable, step, 0x10);
struct SequenceProgress_l { be<s16> origin, limit, progress; };
struct SequenceVoiceRequest_l {
    be<u32> bank, key, velocity, duration;
    be<s32> pan, priority;
    be<u32> reserved;
    gptr<SequenceTrack> track;
    be<u32> playerWord;
};
struct SequenceRegion_l {
    u8 prefix[0xC];
    be<u8> clearedParameters[5];
    u8 reserved11;
    be<u8> volume;
    u8 remainingParameters[4];
};
WWHD_OFFSET(SequenceRegion_l, clearedParameters, 0xC);
WWHD_OFFSET(SequenceRegion_l, volume, 0x12);
WWHD_SIZE(SequenceRegion_l, 0x17);
struct SequenceTrack {
    be<u8> updateRequested, active;
    u8 prefix[0xFE];
    gptr<const be<u8>> sequenceBegin, sequenceEnd;
    u8 playbackState[3];
    be<u8> reuseVoice;
    u8 remainingPlaybackState[0x50];
    be<u8> waiting;
    be<u8> synthFlag;
    be<u8> muted, silenced;
    be<s32> waitCount;
    be<u8> noteWait, status, releaseGuard, releasePriorityFixed;
    be<u32> bank;
    be<f32> notePitchOffset;
    SequenceProgress_l progress[5];
    u8 parameterState[0x46];
    be<u8> velocityScale;
    u8 panPadding;
    be<s8> notePan;
    u8 reserved1D7;
    be<u8> panCurve;
    be<u8> notePriority, lastNoteKey, pitchTime;
    be<u8> envelope1, envelope2, envelope3, envelope4;
    be<s16> envelope5;
    be<u8> biquadType;
    u8 biquadPadding[9];
    be<f32> biquadValue;
    be<u32> outputLine;
    be<s16> variables[16];
    gptr<SequencePlayer_l> player;
    gptr<SequenceVoice_l> firstVoice;
    gptr<SequenceTrackVtable> vtable;
    s32 tick(u32 parameter);
    SequenceVoice_l* playNote(u32 key, s32 velocity, s32 duration, u32 reuse);
    void releaseVoices(s32 release);
    void detachVoiceCallbacks();
    void stop();
    static void destroy(SequenceTrack* track, u32 flags);
    void releaseExpiredVoice(SequenceVoice_l* voice);
    void tickVoices();
    void stopVoices();
    void pauseVoices(u32 paused);
    void setMuteMode(u32 mode);
    void fadeVoices(u32 silent, u32 duration);
    void setSequence(const be<u8>* begin, u32 size);
    void start();
    void prependVoice(SequenceVoice_l* voice);
    void setBiquad(s32 type, f32 value);
    void setReleasePriorityFixed(u8 fixed);
    void setPanCurve(u8 curve);
    void setVelocityScale(u8 scale);
    void setOutputLine(u32 line);
    void setVariable(u32 index, s16 value);
    be<s16>* variable(s32 index);
    void setUpdateRequested(u8 requested);
};
WWHD_OFFSET(SequenceTrack, sequenceBegin, 0x100);
WWHD_OFFSET(SequenceTrack, waiting, 0x15C);
WWHD_OFFSET(SequenceTrack, waitCount, 0x160);
WWHD_OFFSET(SequenceTrack, releasePriorityFixed, 0x167);
WWHD_OFFSET(SequenceTrack, velocityScale, 0x1D4);
WWHD_OFFSET(SequenceTrack, panCurve, 0x1D8);
WWHD_OFFSET(SequenceTrack, biquadType, 0x1E2);
WWHD_OFFSET(SequenceTrack, biquadValue, 0x1EC);
WWHD_OFFSET(SequenceTrack, outputLine, 0x1F0);
WWHD_OFFSET(SequenceTrack, variables, 0x1F4);
WWHD_OFFSET(SequenceTrack, firstVoice, 0x218);
WWHD_SIZE(SequenceTrack, 0x220);
struct SequenceModulator_l { u8 state[0x10]; };
WWHD_SIZE(SequenceModulator_l, 0x10);
/* The loader's GHS vtable (10182144 is the abstract base: the three file-address slots are
 * pure virtual there and implemented by the derived loaders). Function words at +0xC/+0x14/+0x1C/+0x24. */
struct ArchiveLoaderVtable {
    VtableEntry header;
    VtableEntry destroy;
    VtableEntry storeFileAddress;
    VtableEntry cachedFileAddress;
    VtableEntry fileAddress;
};
WWHD_OFFSET(ArchiveLoaderVtable, storeFileAddress, 0x10);
WWHD_OFFSET(ArchiveLoaderVtable, cachedFileAddress, 0x18);
WWHD_OFFSET(ArchiveLoaderVtable, fileAddress, 0x20);
WWHD_SIZE(ArchiveLoaderVtable, 0x28);
struct ArchiveLoader {
    gptr<SoundArchive> archive;
    u8 scratch[0x200];
    be<u32> reserved;
    gptr<ArchiveLoaderVtable> vtable;
    /* Virtual dispatch through the slots (r3 this, r4 file id, r5 address). */
    void storeFileAddress(u32 fileId, const void* address) {
        gabi::call_ptr<void>(vtable->storeFileAddress.function, this, fileId, address);
    }
    const void* cachedFileAddress(u32 fileId) {
        return gabi::call_ptr<const void*>(vtable->cachedFileAddress.function, this, fileId);
    }
    const void* fileAddress(u32 fileId) {
        return gabi::call_ptr<const void*>(vtable->fileAddress.function, this, fileId);
    }
    static ArchiveLoader* create(ArchiveLoader* loader);
    static void destroy(ArchiveLoader* loader, u32 flags);
    u32 isAvailable();
    s32 loadFile(u32 fileId, void* destination, s32 size, s32 offset, s32 chunkSize);
};
WWHD_OFFSET(ArchiveLoader, scratch, 4);
WWHD_OFFSET(ArchiveLoader, vtable, 0x208);
WWHD_SIZE(ArchiveLoader, 0x20C);
} }
void destroyArchiveStorage(void* storage);
void* archiveConstructStorage(u32 size);

void resetSequenceModulator(nw::snd::SequenceModulator_l* modulator);

u32 appendArchivePath(char* output, u32 capacity, const char* input, u32 count);

void updateSequenceTrackParameters(nw::snd::SequenceTrack*);
void setSequenceEnvelopeRelease(void*, u32);
void releaseSequenceVoice(nw::snd::SequenceVoice_l*);
void clearSequenceVoiceCallback(nw::snd::SequenceVoice_l*);
void tryReleaseSequenceVoice(nw::snd::SequenceVoice_l*);
void advanceSequenceVoiceCounter(nw::snd::SequenceVoice_l*, u32);
void stopSequenceVoiceHardware(nw::snd::SequenceVoice_l*);
void invokeSequenceVoiceCallback(nw::snd::SequenceVoice_l*, u32);
void freeSequenceVoice(nw::snd::SequenceVoice_l*);

u32 lookupSequenceRegion(void*, nw::snd::SequenceRegion_l*, u32, u32, s32);
nw::snd::SequenceVoice_l* allocateSequenceVoice(void*, u32, nw::snd::SequenceVoiceRequest_l*);
void setSequenceEnvelope1(void*, u32);
void setSequenceEnvelope2(void*, u32);
void setSequenceEnvelope3(void*, u32);
void setSequenceEnvelope5(void*, s32);
void setSequenceVoicePitch(nw::snd::SequenceVoice_l*, s32, u32, f32);
