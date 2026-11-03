/* Archive playback interfaces recovered from WWHD. Own names; no external source. */
#pragma once
#include "nw/snd/BasicSound.h"
#include "nw/snd/ExternalSoundQueue.h"
#include "nw/snd/ArchiveLoading.h"
#include "nw/snd/SequenceFileReader.h"
#include "nw/snd/SequenceSound.h"
#include "nw/snd/SoundMemoryArena.h"
#include "nw/snd/SoundRuntimeSupport.h"
#include "nw/snd/SequenceRuntimeSupport.h"
#include "nw/snd/SequenceTrackPool.h"
#include "nw/snd/ArchiveAddressCache.h"
#include "nw/snd/AudioEventLog.h"

namespace nw { namespace snd {
enum ArchiveCapacitySlot {
    Capacity_SequenceSound, Capacity_SequenceTrack, Capacity_StreamSound, Capacity_Reserved,
    Capacity_StreamBuffer, Capacity_WaveSound, Capacity_Extra
};


struct PlaybackPool {
    gptr<void> storage;
    be<u32> capacity, activeCount;
    SoundListNode active;
    be<u32> freeCount;
    SoundListNode free;
};
WWHD_SIZE(PlaybackPool, 0x20);
/* A work pool: entries ready for use wait in `available` (all of them after initialization); an entry
 * whose background load is running sits in `loading` until the per-frame collection (0289608C,
 * 0289612C) moves it back once the load has finished. The pool is registered with the frame driver
 * through driverLink (AudioFrameDriver::addPlayer 02898DB0 / removePlayer 02898E18). */
struct ManagedPlaybackPool {
    nw::ut::HeapLink driverLink;
    gptr<void> vtable;
    be<u32> storage, bytes;   // the entries' storage (ArchivePlayback::initializeSequencePool / initializeWavePool)
    be<u32> availableCount;
    SoundListNode available;
    be<u32> loadingCount;
    SoundListNode loading;
    /* NW-SND#6: the per-frame collection of the sequence and the wave work pool */
    void collectFinishedSequences();
    void collectFinishedWaves();
};
WWHD_OFFSET(ManagedPlaybackPool, vtable, 8);
WWHD_OFFSET(ManagedPlaybackPool, available, 0x18);
WWHD_OFFSET(ManagedPlaybackPool, loading, 0x24);
WWHD_SIZE(ManagedPlaybackPool, 0x2C);
struct ArchivePlayback;
struct PlaybackListRange {
    SoundListIterator begin, end;
};
struct PlaybackPriorityBucket { be<u32> count; SoundListNode sentinel; };
struct PlaybackPriorityBuckets { PlaybackPriorityBucket entries[128]; };
struct ArchivePlaybackConfig {
    gptr<SoundArchive> image;
    gptr<void> loader, storage;   // loader: the sound-data loader, an ArchiveAddressCache (sead::AudioArchiveLoader)
    be<u32> bytes;
    gptr<void> streamStorage;
    be<u32> streamBytes;
    gptr<void> streamUserStorage;
    be<u32> streamUserBytes, userBytes;
};
WWHD_SIZE(ArchivePlaybackConfig, 0x24);
/* Narrow views of the pool back-pointers. The sequence view names only the work-pool word
 * inside SequenceSound's player suffix (SequenceSound + 0x54C, i.e. SequenceSoundPlayer_l + 0x2A8),
 * which SequenceSound.h does not name yet; the stream sound belongs to an unmerged unit. The wave
 * sounds and their load tasks are WaveSound / WaveSoundLoadTask (WaveSound.h). */
struct ArchiveSequenceSound_l : BasicSound {
    u8 sequenceState[0x2B4];
    gptr<ManagedPlaybackPool> workPool;
};
WWHD_OFFSET(ArchiveSequenceSound_l, workPool, 0x54C);
static_assert(sizeof(ArchiveSequenceSound_l) <= sizeof(SequenceSound), "view inside SequenceSound");
struct ArchiveStreamSound_l : BasicSound {
    u8 streamState[0x28C];
    gptr<void> userBuffer;
    be<u32> userBufferSize;
};
WWHD_OFFSET(ArchiveStreamSound_l, userBuffer, 0x524);
WWHD_OFFSET(ArchiveStreamSound_l, userBufferSize, 0x528);
struct SequenceWorkEntry_l {
    u8 base[0x34];
    gptr<void> vtable;
    be<u32> state, owner, voice, id, pending;
    SoundResourceChoice files[4];
    be<u32> count, values[4], extra[4];
    be<u8> flags[4];
    u8 reserved[8];
    SoundListNode link;
};
WWHD_OFFSET(SequenceWorkEntry_l, files, 0x4C);
WWHD_OFFSET(SequenceWorkEntry_l, link, 0x9C);
WWHD_SIZE(SequenceWorkEntry_l, 0xA4);

/* Four bank or wave-archive resource choices (each built by the file-slot constructor 02895F68). */
struct PlaybackResourceChoices { SoundResourceChoice entries[4]; };
WWHD_SIZE(PlaybackResourceChoices, 0x20);
struct PlaybackSequenceOptions_l { gptr<void> image; gptr<const char> label; be<u32> banks[4]; };
struct PlaybackSequenceIdentity_l { be<u32> soundId; gptr<void> image; };
/* The optional resource hook (ArchivePlayback::resourceHook): a user object that can supply sound files by label. */
struct ArchiveResourceHookVtable {
    VtableEntry header, reserved08, acceptsLabel, beginStart, endStart, reserved28, findFile;
};
WWHD_OFFSET(ArchiveResourceHookVtable, findFile, 0x30);
struct ArchiveResourceHook {
    be<u8> enabled;
    u8 padding[3];
    gptr<ArchiveResourceHookVtable> vtable;
    u32 acceptsLabel(const char* label) { return gabi::call_ptr<u32>(vtable->acceptsLabel.function, this, label); }
    void beginStart() { gabi::call_ptr<void>(vtable->beginStart.function, this); }
    void endStart() { gabi::call_ptr<void>(vtable->endStart.function, this); }
    const void* findFile(const char* label, const char* kind, const char* format, u32 index) {
        return gabi::call_ptr<const void*>(vtable->findFile.function, this, label, kind, format, index);
    }
};
struct PlaybackSequenceFormat_l { be<u8> individual[4]; };

struct PlaybackStreamFile_l { gptr<be<u8>> path; be<u32> offset, bytes; };
struct PlaybackStreamSetup_l {
    gptr<void> subsystem;
    be<u32> channels;
    be<u16> trackFlags;
    be<u8> format, prefetchEnabled;
    ArchiveStreamTrackSummary tracks[8];
    be<u32> prefetchOffset, prefetchSize;   // the archive's prefetch summary (ArchivePrefetchSummary)
    be<f32> pitch;
    be<u8> routing[4];
};
WWHD_SIZE(PlaybackStreamSetup_l, 0x8C);
struct PlaybackStreamOpen_l {
    be<u32> useDefaultDevice, mode, offset, bytes;
    be<u8> path[0x27F];
    u8 padding;
    gptr<void> hook, hookStorage;
    be<u32> hookBytes;
    gptr<const char> label;
    be<u32> deviceOption;
    gptr<const void> descriptorOption;
    be<u32> extraOption;
};
WWHD_OFFSET(PlaybackStreamOpen_l, hook, 0x290);
WWHD_SIZE(PlaybackStreamOpen_l, 0x2AC);
struct PlaybackStreamPath_l { be<u8> bytes[0x27F]; };

struct SoundStartInfo;
struct WaveStartOverride;
/* The archive player's note-on callback object (ArchivePlayback + 8): its vtable (101821C8) calls
 * playArchiveBankNote (02895E24) for the sequences the player prepares. */
struct ArchiveNoteCallback {
    gptr<void> vtable;
    gptr<ArchivePlayback> playback;
};
/* Game-thread playback pools; archive metadata uses the canonical facade and summaries. The sound-start
 * methods (startSound and its helpers) are NW-SND#6's (ArchiveStartContext.h has their records). */
struct ArchivePlayback {
    gptr<void> vtable;
    gptr<SoundArchive> archive;
    ArchiveNoteCallback noteCallback;
    be<u32> state, reserved, playerCount;
    gptr<SoundPlayer> players;
    PlaybackPool sequences;
    ManagedPlaybackPool sequenceWork;
    PlaybackPool waves;
    ManagedPlaybackPool waveWork;
    PlaybackPool streams;
    gptr<SequenceTrackPool> activeTrackPool;
    SequenceTrackPool trackPool;
    u8 streamBuffers[0x18];
    RuntimeTrackAllocator trackAllocator;
    be<u32> userStride;
    gptr<ArchiveAddressCache> addressCache;
    gptr<ArchiveResourceHook> resourceHook;
    be<u8> initialized, updating;
    u8 tailPadding[2];

    static ArchivePlayback* create(ArchivePlayback* self);
    void destroy(u32 flags);
    u32 hasArchiveData();
    SoundPlayer* player(u32 id);
    SoundPlayer* playerEntry(u32 id);
    SoundArchive* archiveImage();
    SoundPlayer* playerForLabel(const char* label);
    SoundPlayer* playerForLabelEntry(const char* label);
    u32 streamStorageSize(SoundArchive* image);
    u32 storageSize(SoundArchive* image, u32 userBytes);
    SoundMemoryArena* allocateWork(be<u32>* cursor, u32 limit, u32 bytes);
    u32 reserveSequenceTracks(u32 count, be<u32>* cursor, u32 limit);
    u32 initializeStreamStorage(SoundArchive* image, void* storage, u32 bytes);
    u32 streamUserStorageSize(SoundArchive* image, u32 stride);
    u32 assignUserStorage(ArchiveProjectSummary* info, be<u32>* cursor, u32 limit, u32 bytes);
    u32 initializePlayers(SoundArchive* image, be<u32>* cursor, u32 limit);
    u32 initializeStreamPool(u32 count, be<u32>* cursor, u32 limit);
    u32 initializeSequencePool(u32 count, be<u32>* cursor, u32 limit);
    u32 initializeWavePool(u32 count, be<u32>* cursor, u32 limit);
    u32 initializeSoundStorage(SoundArchive* image, void* storage, u32 bytes, u32 userBytes);
    u32 initialize(SoundArchive* image, void* dataLoader, void* storage, u32 bytes,
                   void* streamStorage, u32 streamBytes, u32 userBytes);
    u32 initializeWithConfig(ArchivePlaybackConfig* config);
    void finalize();
    void update();
    u32 prepareSequence(BasicSound* sound, ArchiveSoundSummary* soundInfo, ArchiveSequenceSummary* info,
                        u32 mode, u32 value, PlaybackSequenceOptions_l* options);
    u32 prepareStream(BasicSound* sound, ArchiveSoundSummary* soundInfo, ArchiveStreamSummary* info,
                      ArchivePrefetchSummary* prefetch, u32 mode, u32 startOffset, PlaybackStreamFile_l* file);
    /* NW-SND#6: starting a sound */
    u32 isHookEnabled();
    void beginHookedStart();
    void endHookedStart();
    u32 hasSequenceFile(BasicSound* sound);
    u32 hasSoundLabel(BasicSound* sound);
    u32 hasWaveSoundFile(BasicSound* sound);
    void applySoundParameters(BasicSound* sound, const ArchiveSoundSummary* info);
    u32 startSound(SoundHandle* handle, u32 id, const AmbientInfo* ambient, SoundActor* actor, u32 hold,
                   SoundStartInfo* info);
    u32 startSoundWithoutActor(SoundHandle* handle, u32 id, u32 hold, SoundStartInfo* info);
    u32 findLabel(const char* label);
    BasicSound* allocateSequenceSound(PlaybackPool* pool, u32 id, s32 priority, s32 ambientPriority, const AmbientInfo* ambient);
    BasicSound* allocateStreamSound(PlaybackPool* pool, u32 id, s32 priority, s32 ambientPriority, const AmbientInfo* ambient);
    BasicSound* allocateWaveSound(PlaybackPool* pool, u32 id, s32 priority, s32 ambientPriority, const AmbientInfo* ambient);
    u32 prepareWaveSound(BasicSound* sound, const ArchiveSoundSummary* soundInfo, const ArchiveWaveSummary* info,
                         u32 offsetKind, u32 offset, const WaveStartOverride* override);
};
WWHD_OFFSET(ArchivePlayback, archive, 4);
WWHD_OFFSET(ArchivePlayback, noteCallback, 8);
WWHD_OFFSET(ArchivePlayback, players, 0x1C);
WWHD_OFFSET(ArchivePlayback, sequences, 0x20);
WWHD_OFFSET(ArchivePlayback, sequenceWork, 0x40);
WWHD_OFFSET(ArchivePlayback, waves, 0x6C);
WWHD_OFFSET(ArchivePlayback, waveWork, 0x8C);
WWHD_OFFSET(ArchivePlayback, streams, 0xB8);
WWHD_OFFSET(ArchivePlayback, activeTrackPool, 0xD8);
WWHD_OFFSET(ArchivePlayback, trackPool, 0xDC);
WWHD_OFFSET(ArchivePlayback, streamBuffers, 0xF0);
WWHD_OFFSET(ArchivePlayback, trackAllocator, 0x108);
WWHD_OFFSET(ArchivePlayback, addressCache, 0x110);
WWHD_OFFSET(ArchivePlayback, resourceHook, 0x114);
WWHD_OFFSET(ArchivePlayback, initialized, 0x118);
WWHD_SIZE(ArchivePlayback, 0x11C);
}}

u32 archiveHasData(nw::snd::SoundArchive*);
void archivePlaybackDelete(void*);
u32 readArchiveCapacity(nw::snd::SoundArchive*, nw::snd::ArchiveProjectSummary*);
u32 archivePlayerCount(nw::snd::SoundArchive*);
u32 readArchivePlayerCapacity(nw::snd::SoundArchive*, u32, nw::snd::ArchivePlayerSummary*);
void initializeArchiveStreamStorage(void*, void*, u32, u32);
nw::snd::BasicSound* constructArchiveStreamSound(nw::snd::BasicSound*, nw::snd::PlaybackPool*);
void constructArchiveWorkBase(void*);
u32 isSoundSystemInitialized();   // NW-SND#6 (SoundSystemSetup.cpp)
/* SoundPlayer::stopAll (028972D4) as sead::AudioPlayer binds it (sead/AudioPlayer.cpp glue). */
void stopArchivePlayer(nw::snd::SoundPlayer*, u32);
void destroyArchiveWorkBase(void*, u32);
void finalizeArchiveStreamStorage(void*);
/* NW-SND#6's per-TU constructors of small archive records, run through __construct_array (SoundStartStorage.cpp) */
nw::snd::ArchiveStreamTrackSummary* constructStreamTrack(nw::snd::ArchiveStreamTrackSummary* track);
nw::snd::SoundResourceChoice* constructResourceChoice(nw::snd::SoundResourceChoice* choice);
nw::snd::PlaybackPriorityBucket* constructSequencePoolBucket(nw::snd::PlaybackPriorityBucket* bucket);
nw::snd::PlaybackPriorityBucket* constructWavePoolBucket(nw::snd::PlaybackPriorityBucket* bucket);
nw::snd::PlaybackPriorityBucket* constructStreamPoolBucket(nw::snd::PlaybackPriorityBucket* bucket);
nw::snd::SoundListNode* removeArchiveListIterator(void*, nw::snd::SoundListIterator*);

void configureArchiveStream(nw::snd::BasicSound*, nw::snd::PlaybackStreamSetup_l*);
void openArchiveStream(nw::snd::BasicSound*, nw::snd::PlaybackStreamOpen_l*);
void clearArchiveStreamPath(void*, u32, u32);
const char* archiveSoundLabel(nw::snd::SoundArchive*, u32);
const be<u8>* resolveArchiveExternalPath(nw::snd::SoundArchive*, const be<u8>*, be<u8>*, u32);

/* ArchiveAddressCache::fileAddress (02896B5C, NW-SND#6), bound as a free function: the archive player
 * calls it with its address cache unchecked and tests that pointer for NULL afterwards (prepareSequence). */
void* findArchivePlaybackFile(nw::snd::ArchiveAddressCache*, u32);
nw::snd::SequenceFileReader* constructPlaybackSequenceReader(nw::snd::SequenceFileReader*, void*);
void* playbackSequenceData(nw::snd::SequenceFileReader*);
u32 decodePlaybackSequenceOffset(void*, u32, be<u32>*);
u32 resolvePlaybackBankWave(nw::snd::SoundResourceChoice*, be<u8>*, void*, nw::snd::SoundArchive*, void*);
void queueArchiveSequencePreparation(nw::snd::BasicSound*, u32, u32, void*, u32, u32, u32, u32);
nw::snd::SequenceResourceParameters* constructPlaybackSequenceContext(nw::snd::SequenceResourceParameters*, nw::snd::SoundArchive*, nw::snd::ArchiveAddressCache*, nw::snd::PlaybackSequenceIdentity_l*, nw::snd::PlaybackResourceChoices*, nw::snd::SoundPlayer*);
void prepareDeferredArchiveSequence(nw::snd::BasicSound*, nw::snd::SequenceResourceParameters*, nw::snd::SequenceCommandParameters*);
void prepareResidentArchiveSequence(nw::snd::BasicSound*, nw::snd::SequenceConfiguration*, nw::snd::SequenceCommandParameters*);

nw::snd::SoundPlayer* archivePlayerSlotTail(nw::snd::ArchivePlayback*, u32);
nw::snd::SoundPlayer* archivePlayerForLabelTail(nw::snd::ArchivePlayback*, const char*);
