/* Archive playback: player slots, sound and work pools, priority update, sequence and stream preparation. Binary-derived; own names. */
#include "nw/snd/ArchivePlayback.h"
#include "nw/snd/AudioFrameDriver.h"
#include "nw/snd/WaveSound.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;
static PlaybackSequenceFormat_l sSequenceFormat;
static u32 sPlaybackBaseVtable;
static u32 sPlaybackVtable;
static u32 sSequenceWorkVtable;
static u32 sWaveWorkVtable;
static TrackAllocatorVtable sTrackAllocatorVtable;
static u32 sTrackPoolVtable;
static u32 sSequenceEntryVtable;
static u32 sWaveEntryVtable;
struct PlaybackStaticWord { be<u32> value; };
// the sound driver's event log (function-local static of an inlined accessor, shared with NW-SND#7/#8)
static AudioEventLog sEventLog;
static PlaybackStaticWord sEventLogGuard;
static PlaybackStaticWord sSequencePriorityGuard;
static PlaybackStaticWord sSequencePriorityLimit;
static PlaybackStaticWord sWavePriorityGuard;
static PlaybackStaticWord sWavePriorityLimit;
static PlaybackStaticWord sStreamPriorityGuard;
static PlaybackStaticWord sStreamPriorityLimit;
/* Pool strides of the sequence sounds and tracks (32-bit, as the binary computes them). */
enum : u32 { SequenceSoundBytes = sizeof(SequenceSound), SequenceTrackBytes = sizeof(SequenceTrack_l) };

static void initializePlaybackPool(PlaybackPool* pool) {
    if (!pool) {
        pool = (PlaybackPool*)sndAllocate(sizeof(PlaybackPool));
        if (!pool) return;
    }
    pool->storage = nullptr;
    pool->capacity = 0;
    pool->activeCount = 0;
    pool->active.next = &pool->active;
    pool->active.previous = &pool->active;
    pool->freeCount = 0;
    pool->free.previous = &pool->free;
    pool->free.next = &pool->free;
}

static void initializeManagedPlaybackPool(ManagedPlaybackPool* pool, void* vtable, bool stream) {
    if (!pool) {
        pool = (ManagedPlaybackPool*)sndAllocate(sizeof(ManagedPlaybackPool));
        if (!pool) return;
    }
    pool->driverLink.next = nullptr;
    pool->storage = 0;
    if (stream) {
        pool->driverLink.previous = nullptr;
        pool->vtable = vtable;
        pool->bytes = 0;
    } else {
        pool->bytes = 0;
        pool->vtable = vtable;
        pool->driverLink.previous = nullptr;
    }
    pool->availableCount = 0;
    pool->available.next = &pool->available;
    pool->available.previous = &pool->available;
    pool->loadingCount = 0;
    pool->loading.previous = &pool->loading;
    pool->loading.next = &pool->loading;
}

/* 028929AC */
ArchivePlayback* ArchivePlayback::create(ArchivePlayback* self) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!self) {
        self = (ArchivePlayback*)sndAllocate(sizeof(ArchivePlayback));
        if (!self) return self;
    }
    self->noteCallback.playback = nullptr;
    self->noteCallback.vtable = &sPlaybackBaseVtable;
    self->playerCount = 0;
    self->vtable = &sPlaybackVtable;
    self->state = 0;
    self->archive = nullptr;
    self->players = nullptr;
    self->reserved = 0;
    initializePlaybackPool(gabi::at<PlaybackPool>(gabi::ea(self) + 0x20u));
    initializeManagedPlaybackPool(gabi::at<ManagedPlaybackPool>(gabi::ea(self) + 0x40u),
                                  &sSequenceWorkVtable, false);
    initializePlaybackPool(gabi::at<PlaybackPool>(gabi::ea(self) + 0x6Cu));
    initializeManagedPlaybackPool(gabi::at<ManagedPlaybackPool>(gabi::ea(self) + 0x8Cu),
                                  &sWaveWorkVtable, true);
    initializePlaybackPool(gabi::at<PlaybackPool>(gabi::ea(self) + 0xB8u));
    self->activeTrackPool = nullptr;
    self->updating = 0;
    self->userStride = 0;
    self->trackAllocator.vtable = &sTrackAllocatorVtable;
    self->trackPool.mArgument = gabi::ea(&self->trackAllocator);
    self->noteCallback.playback = self;
    self->trackPool.mReserved = gabi::ea(&sTrackPoolVtable);
    self->resourceHook = nullptr;
    self->initialized = 0;
    self->trackPool.mStorage[1] = 0;
    self->addressCache = nullptr;
    self->trackPool.mStorage[0] = 0;
    self->trackPool.mStorage[2] = 0;
    return self;
}

/* 02892BD8 */
void ArchivePlayback::destroy(u32 flags) {
    if (this && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)archivePlaybackDelete), this);
}

/* 02892BEC */
u32 ArchivePlayback::storageSize(SoundArchive* image, u32 userBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchivePlayerSummary> playerInfo(8);
    gabi::FrameLocal<ArchiveProjectSummary> info(0x10);
    u32 players = archivePlayerCount(image);
    u32 total = (players * 0xA0u + 31u) & ~31u;
    for (u32 index = 0; index != players; ++index) {
        playerInfo->soundLimit = 0;
        playerInfo->option = 0;
        if (readArchivePlayerCapacity(image, 0x04000000u | index, playerInfo.get())) {
            u32 bytes = playerInfo->option;
            if (bytes && (s32)playerInfo->soundLimit > 0) {
                u32 count = playerInfo->soundLimit;
                u32 aligned = (bytes + 31u) & ~31u;
                do {
                    total += 0x40u;
                    total += aligned;
                } while (--count);
            }
        }
    }
    if (readArchiveCapacity(image, info.get())) {
        u32 waveSounds = info->counts[Capacity_WaveSound];
        u32 sequences = info->counts[Capacity_SequenceSound];
        u32 streams = info->counts[Capacity_StreamSound];
        u32 tracks = info->counts[Capacity_SequenceTrack];
        total += (sequences * SequenceSoundBytes + 31u) & ~31u;
        total += (sequences * 0xA4u + 31u) & ~31u;
        total += (waveSounds * 0x4A0u + 31u) & ~31u;
        total += (waveSounds * 0x68u + 31u) & ~31u;
        total += (streams * 0x7670u + 31u) & ~31u;
        total += (tracks * SequenceTrackBytes + 31u) & ~31u;
    }
    /* The binary reads this scratch record even when the query reports failure. */
    if (userBytes) {
        u32 count = (u32)info->counts[Capacity_SequenceSound] + (u32)info->counts[Capacity_WaveSound] + (u32)info->counts[Capacity_StreamSound];
        total += ((userBytes + 3u) & ~3u) * count;
    }
    return total;
}

/* 02892D50 */
u32 ArchivePlayback::streamStorageSize(SoundArchive* image) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveProjectSummary> info(8);
    u32 count = 0;
    if (readArchiveCapacity(image, info.get())) count = info->counts[Capacity_StreamBuffer];
    return count * 0xCD00u;
}

/* 02892D9C */
SoundMemoryArena* ArchivePlayback::allocateWork(be<u32>* cursor, u32 limit, u32 bytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 begin = *cursor;
    if (begin & 31) return nullptr;
    u32 next = (begin + 0x5Fu) & ~63u;
    if (next > limit) return nullptr;
    SoundMemoryArena* work = gabi::at<SoundMemoryArena>(begin);
    *cursor = next;
    if (work) {
        work = SoundMemoryArena::construct(work);
        next = *cursor;
    }
    u32 end = next + ((bytes + 31u) & ~31u);
    if (end > limit) return nullptr;
    *cursor = end;
    if (!work->initialize(gabi::at<void>(next), bytes)) return nullptr;
    return work;
}

/* 02892E54 */
u32 ArchivePlayback::initializePlayers(SoundArchive* image, be<u32>* cursor, u32 limit) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchivePlayerSummary> info(8);
    if ((u32)*cursor & 31) return 0;
    u32 count = archivePlayerCount(image);
    u32 bytes = (count * 0xA0u + 31u) & ~31u;
    u32 begin = *cursor;
    u32 end = begin + bytes;
    if (end > limit) return 0;
    *cursor = end;
    players = gabi::at<SoundPlayer>(begin);
    playerCount = count;
    u32 address = begin;
    for (u32 index = 0; index != count; ++index, address += 0xA0u) {
        SoundPlayer* slot = gabi::at<SoundPlayer>(address);
        if (slot) slot = SoundPlayer::create(slot);
        info->soundLimit = 0;
        info->option = 0;
        if (!readArchivePlayerCapacity(image, 0x04000000u | index, info.get())) continue;
        slot->setSoundLimit((s32)info->soundLimit);
        if (!(u32)info->option) continue;
        u32 workIndex = 0;
        while ((s32)workIndex < (s32)info->soundLimit) {
            SoundMemoryArena* work = allocateWork(cursor, limit, info->option);
            if (!work) return 0;
            slot->addHeap(work);
            ++workIndex;
        }
        slot->setMaximumSounds((s32)info->soundLimit);
    }
    return 1;
}

/* 02893348 */
u32 ArchivePlayback::initializeStreamPool(u32 count, be<u32>* cursor, u32 limit) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SoundListIterator> position(8);
    u32 begin = *cursor;
    if (begin & 31) return 0;
    u32 bytes = (count * 0x7670u + 31u) & ~31u;
    u32 end = begin + bytes;
    if (end > limit) return 0;
    u32 remaining = bytes / 0x7670u;
    u32 address = begin;
    while (remaining) {
        BasicSound* sound = gabi::at<BasicSound>(address);
        if (sound) sound = constructArchiveStreamSound(sound, &streams);
        position->node = &streams.free;
        insertSoundList(&streams.freeCount, position.get(),
                        gabi::at<SoundListNode>(gabi::ea(sound) + offsetof(BasicSound, poolLink)));
        --remaining;
        address += 0x7670u;
    }
    streams.capacity = bytes;
    streams.storage = gabi::at<void>(begin);
    *cursor = end;
    return 1;
}

/* 02892FA0 */
u32 ArchivePlayback::initializeSequencePool(u32 count, be<u32>* cursor, u32 limit) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SoundListIterator> soundPosition(8);
    gabi::FrameLocal<SoundListIterator> workPosition(0xC);
    u32 begin = *cursor;
    if (begin & 31) return 0;
    u32 bytes = (count * SequenceSoundBytes + 31u) & ~31u;
    u32 end = begin + bytes;
    if (end > limit) return 0;
    u32 remaining = bytes / SequenceSoundBytes;
    u32 address = begin;
    while (remaining) {
        SequenceSound* sound = gabi::at<SequenceSound>(address);
        if (sound) sound = SequenceSound::construct(sound, &sequences);
        soundPosition->node = &sequences.free;
        insertSoundList(&sequences.freeCount, soundPosition.get(),
                        gabi::at<SoundListNode>(gabi::ea(sound) + offsetof(BasicSound, poolLink)));
        --remaining;
        address += SequenceSoundBytes;
    }
    u32 workBytes = (count * 0xA4u + 31u) & ~31u;
    sequences.storage = gabi::at<void>(begin);
    u32 workBegin = end;
    u32 workEnd = workBegin + workBytes;
    sequences.capacity = bytes;
    *cursor = workBegin;
    if (workEnd > limit) return 0;
    remaining = workBytes / 0xA4u;
    address = workBegin;
    while (remaining) {
        SequenceWorkEntry_l* work = gabi::at<SequenceWorkEntry_l>(address);
        if (work) {
            constructArchiveWorkBase(work);
            work->owner = 0;
            work->pending = 0;
            work->state = 0;
            work->id = 0xFFFFFFFFu;
            work->voice = 0;
            work->vtable = &sSequenceEntryVtable;
            __construct_array(work->files, 4, sizeof(SoundResourceChoice), (void* (*)(void*))constructResourceChoice);
            work->count = 0;
            for (u32 index = 0; index != 4; ++index) {
                work->values[index] = 0;
                work->extra[index] = 0;
                work->flags[index] = 0;
            }
            work->link.previous = nullptr;
            work->link.next = nullptr;
        }
        workPosition->node = &sequenceWork.available;
        insertSoundList(&sequenceWork.availableCount, workPosition.get(),
                        gabi::at<SoundListNode>(address + offsetof(SequenceWorkEntry_l, link)));
        --remaining;
        address += 0xA4u;
    }
    sequenceWork.bytes = workBytes;
    sequenceWork.storage = workBegin;
    *cursor = workEnd;
    SoundListNode* node = sequences.free.next;
    while (gabi::ea(node) != gabi::ea(&sequences.free)) {
        ArchiveSequenceSound_l* sound = gabi::at<ArchiveSequenceSound_l>(
            gabi::ea(node) - offsetof(BasicSound, poolLink));
        sound->workPool = &sequenceWork;
        node = node->next;
    }
    AudioFrameDriver::instance()->addPlayer(&sequenceWork.driverLink);
    return 1;
}

/* 02893194 */
u32 ArchivePlayback::initializeWavePool(u32 count, be<u32>* cursor, u32 limit) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SoundListIterator> soundPosition(8);
    gabi::FrameLocal<SoundListIterator> workPosition(0xC);
    u32 begin = *cursor;
    if (begin & 31) return 0;
    u32 bytes = (count * 0x4A0u + 31u) & ~31u;
    u32 end = begin + bytes;
    if (end > limit) return 0;
    u32 remaining = bytes / 0x4A0u;
    u32 address = begin;
    while (remaining) {
        WaveSound* sound = gabi::at<WaveSound>(address);
        if (sound) sound = WaveSound::construct(sound, &waves);
        soundPosition->node = &waves.free;
        insertSoundList(&waves.freeCount, soundPosition.get(),
                        gabi::at<SoundListNode>(gabi::ea(sound) + offsetof(BasicSound, poolLink)));
        --remaining;
        address += 0x4A0u;
    }
    u32 workBytes = (count * 0x68u + 31u) & ~31u;
    waves.storage = gabi::at<void>(begin);
    u32 workBegin = end;
    u32 workEnd = workBegin + workBytes;
    waves.capacity = bytes;
    *cursor = workBegin;
    if (workEnd > limit) return 0;
    remaining = workBytes / 0x68u;
    address = workBegin;
    while (remaining) {
        WaveSoundLoadTask* work = gabi::at<WaveSoundLoadTask>(address);
        if (work) {   // the inlined WaveSoundLoadTask constructor
            constructArchiveWorkBase(work);   // AudioWork::construct (0289C050), as for the sequence entries
            work->mConfiguration.waveIndex = 0;
            work->mLink.next = nullptr;
            work->mConfiguration.player = nullptr;
            work->mLink.previous = nullptr;
            work->vtable = reinterpret_cast<AudioWorkVtable*>(&sWaveEntryVtable);
            work->mImage = 0;
            work->mConfiguration.addressCache = nullptr;
            work->mConfiguration.itemId = NoFile;
            work->mConfiguration.image = 0;
            work->mConfiguration.archive = nullptr;
            work->mWaveData = 0;
        }
        workPosition->node = &waveWork.available;
        insertSoundList(&waveWork.availableCount, workPosition.get(),
                        gabi::at<SoundListNode>(address + offsetof(WaveSoundLoadTask, mLink)));
        --remaining;
        address += 0x68u;
    }
    waveWork.bytes = workBytes;
    waveWork.storage = workBegin;
    *cursor = workEnd;
    SoundListNode* node = waves.free.next;
    while (gabi::ea(node) != gabi::ea(&waves.free)) {
        WaveSound* sound = gabi::at<WaveSound>(gabi::ea(node) - offsetof(BasicSound, poolLink));
        sound->mPlayer.mLoadPool = &waveWork;
        node = node->next;
    }
    AudioFrameDriver::instance()->addPlayer(&waveWork.driverLink);
    return 1;
}

/* 02893418 */
u32 ArchivePlayback::reserveSequenceTracks(u32 count, be<u32>* cursor, u32 limit) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 begin = *cursor;
    if (begin & 31) return 0;
    u32 bytes = (count * SequenceTrackBytes + 31u) & ~31u;
    u32 end = begin + bytes;
    if (end > limit) return 0;
    trackPool.initialize(gabi::at<void>(begin), bytes);
    *cursor = end;
    return 1;
}

static u32 assignPoolUserStorage(SoundListNode* head, u32 address, u32 bytes) {
    SoundListNode* node = head->next;
    while (gabi::ea(node) != gabi::ea(head)) {
        BasicSound* sound = gabi::at<BasicSound>(gabi::ea(node) - offsetof(BasicSound, poolLink));
        sound->userData = gabi::at<void>(address);
        sound->userDataSize = bytes;
        node = node->next;
        address += bytes;
    }
    return address;
}

/* 0289349C */
u32 ArchivePlayback::assignUserStorage(ArchiveProjectSummary* info, be<u32>* cursor, u32 limit, u32 bytes) {
    u32 address = *cursor;
    if (address & 31) return 0;
    u32 stride = (bytes + 3u) & ~3u;
    address = assignPoolUserStorage(&waves.free, address, stride);
    address = assignPoolUserStorage(&sequences.free, address, stride);
    address = assignPoolUserStorage(&streams.free, address, stride);
    userStride = stride;
    return address <= limit;
}

/* 02893690 */
u32 ArchivePlayback::initializeStreamStorage(SoundArchive* image, void* storage, u32 bytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveProjectSummary> info(8);
    if (gabi::ea(storage) & 7) return 0;
    if (bytes < streamStorageSize(image)) return 0;
    u32 count = 0;
    if (readArchiveCapacity(image, info.get())) count = info->counts[Capacity_StreamBuffer];
    initializeArchiveStreamStorage(streamBuffers, storage, bytes, count);
    return 1;
}

/* 02893818 */
u32 ArchivePlayback::streamUserStorageSize(SoundArchive* image, u32 stride) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveProjectSummary> info(8);
    u32 count = 0;
    if (readArchiveCapacity(image, info.get())) count = info->counts[Capacity_StreamSound];
    return count * stride;
}

/* 02893544 */
u32 ArchivePlayback::initializeSoundStorage(SoundArchive* image, void* storage, u32 bytes, u32 userBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> cursor(8);
    gabi::FrameLocal<ArchiveProjectSummary> info(0xC);
    u32 begin = gabi::ea(storage);
    if (begin & 31) return 0;
    *cursor = begin;
    u32 limit = begin + bytes;
    if (!initializePlayers(image, cursor.get(), limit)) return 0;
    if (readArchiveCapacity(image, info.get())) {
        if (!initializeSequencePool(info->counts[Capacity_SequenceSound], cursor.get(), limit)) return 0;
        if (!initializeWavePool(info->counts[Capacity_WaveSound], cursor.get(), limit)) return 0;
        if (!initializeStreamPool(info->counts[Capacity_StreamSound], cursor.get(), limit)) return 0;
        if (!reserveSequenceTracks(info->counts[Capacity_SequenceTrack], cursor.get(), limit)) return 0;
    }
    if (userBytes && !assignUserStorage(info.get(), cursor.get(), limit, userBytes)) return 0;
    archive = image;
    return 1;
}

/* 02893750 */
u32 ArchivePlayback::initialize(SoundArchive* image, void* dataLoader, void* storage, u32 bytes,
                               void* streamStorage, u32 streamBytes, u32 userBytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (updating) return 0;
    if (!isSoundSystemInitialized()) return 0;
    if (!initializeSoundStorage(image, storage, bytes, userBytes)) return 0;
    if (!initializeStreamStorage(image, streamStorage, streamBytes)) return 0;
    addressCache = static_cast<ArchiveAddressCache*>(dataLoader);
    updating = 1;
    activeTrackPool = &trackPool;
    return 1;
}

/* 02893868 */
u32 ArchivePlayback::initializeWithConfig(ArchivePlaybackConfig* config) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ArchiveProjectSummary> info(8);
    if (!initialize(config->image, config->loader, config->storage, config->bytes,
                    config->streamStorage, config->streamBytes, config->userBytes)) return 0;
    if (config->streamUserStorage) {
        if (!readArchiveCapacity(config->image, info.get())) {
            updating = 0;
            return 0;
        }
        SoundListNode* node = streams.free.next;
        u32 bytes = ppc_divwu(config->streamUserBytes, info->counts[Capacity_StreamSound]);
        u32 address = gabi::ea(config->streamUserStorage);
        while (gabi::ea(node) != gabi::ea(&streams.free)) {
            ArchiveStreamSound_l* sound = gabi::at<ArchiveStreamSound_l>(
                gabi::ea(node) - offsetof(BasicSound, poolLink));
            sound->userBuffer = gabi::at<void>(address);
            sound->userBufferSize = bytes;
            node = node->next;
            address += bytes;
        }
    }
    return 1;
}

static void erasePlaybackList(be<u32>* count, SoundListNode* head, PlaybackListRange* range, bool work) {
    SoundListNode* first = head->next;
    if (work) {
        range->begin.node = first;
        range->end.node = head;
    } else {
        range->end.node = head;
        range->begin.node = first;
    }
    eraseSoundListRange(count, &range->begin, &range->end);
}

static void destroyArchiveSounds(u32 address, u32 count, u32 stride) {
    while (count) {
        BasicSound* sound = gabi::at<BasicSound>(address);
        gabi::call_ptr<void>(sound->vtable->destroyDeleting.function, sound, 2);
        --count;
        address += stride;
    }
}

static void destroyArchiveWorkEntries(u32 address, u32 count, u32 stride) {
    while (count) {
        if (address) destroyArchiveWorkBase(gabi::at<void>(address), 0);
        --count;
        address += stride;
    }
}

/* 02893964 */
void ArchivePlayback::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<PlaybackListRange> sequenceFree(8);
    gabi::FrameLocal<PlaybackListRange> sequenceActive(0x10);
    gabi::FrameLocal<PlaybackListRange> sequenceWorkActive(0x18);
    gabi::FrameLocal<PlaybackListRange> waveFree(0x20);
    gabi::FrameLocal<PlaybackListRange> waveActive(0x28);
    gabi::FrameLocal<PlaybackListRange> waveWorkActive(0x30);
    gabi::FrameLocal<PlaybackListRange> streamFree(0x38);
    gabi::FrameLocal<PlaybackListRange> streamActive(0x40);
    if (!updating) return;
    u32 count = playerCount;
    archive = nullptr;
    activeTrackPool = nullptr;
    u32 index = 0;
    u32 offset = 0;
    while (index < count) {
        gabi::at<SoundPlayer>(gabi::ea(players) + offset)->stopAll(0);
        count = playerCount;
        ++index;
        offset += 0xA0u;
    }
    DriverCommandManager* commands = DriverCommandManager::instance();
    u32 serial = commands->submit(1);
    commands->waitFor(serial);
    u32 sequenceCount = (u32)sequences.capacity / SequenceSoundBytes;
    u32 sequenceAddress = gabi::ea(sequences.storage);
    playerCount = 0;
    players = nullptr;
    destroyArchiveSounds(sequenceAddress, sequenceCount, SequenceSoundBytes);
    erasePlaybackList(&sequences.freeCount, &sequences.free, sequenceFree.get(), false);
    erasePlaybackList(&sequences.activeCount, &sequences.active, sequenceActive.get(), false);
    destroyArchiveWorkEntries(sequenceWork.storage, (u32)sequenceWork.bytes / 0xA4u, 0xA4u);
    erasePlaybackList(&sequenceWork.availableCount, &sequenceWork.available, sequenceWorkActive.get(), true);
    AudioFrameDriver::instance()->removePlayer(&sequenceWork.driverLink);
    trackPool.reset();
    destroyArchiveSounds(gabi::ea(waves.storage), (u32)waves.capacity / 0x4A0u, 0x4A0u);
    erasePlaybackList(&waves.freeCount, &waves.free, waveFree.get(), false);
    erasePlaybackList(&waves.activeCount, &waves.active, waveActive.get(), false);
    destroyArchiveWorkEntries(waveWork.storage, (u32)waveWork.bytes / 0x68u, 0x68u);
    erasePlaybackList(&waveWork.availableCount, &waveWork.available, waveWorkActive.get(), true);
    AudioFrameDriver::instance()->removePlayer(&waveWork.driverLink);
    finalizeArchiveStreamStorage(streamBuffers);
    destroyArchiveSounds(gabi::ea(streams.storage), (u32)streams.capacity / 0x7670u, 0x7670u);
    erasePlaybackList(&streams.freeCount, &streams.free, streamFree.get(), false);
    erasePlaybackList(&streams.activeCount, &streams.active, streamActive.get(), false);
    updating = 0;
}

static void initializeEventLog() {
    if (!sEventLogGuard.value) {
        sEventLog.mWriting = &sEventLog.mBuffers[0];
        sEventLog.mBuffers[1].mCount = 0;
        sEventLogGuard.value = 1;
        sEventLog.mBuffers[0].mCount = 0;
    }
}

static void initializePlaybackPriorityLimit(PlaybackStaticWord* guard, PlaybackStaticWord* limit) {
    if (!guard->value) {
        guard->value = 1;
        limit->value = 128;
    }
}

static void sortPlaybackPool(PlaybackPool* pool, PlaybackPriorityBuckets* buckets,
                             PlaybackListRange* split, PlaybackListRange* join,
                             void* (*constructBucket)(void*)) {
    if ((u32)pool->activeCount < 2) return;
    __construct_array(buckets->entries, 128, sizeof(PlaybackPriorityBucket), constructBucket);
    while ((u32)pool->activeCount) {
        SoundListNode* node = pool->active.next;
        split->begin.node = node;
        removeArchiveListIterator(&pool->activeCount, &split->begin);
        BasicSound* sound = gabi::at<BasicSound>(gabi::ea(node) - offsetof(BasicSound, poolLink));
        s32 priority = (s32)((u32)sound->ambient.priority + (u8)(s8)sound->playerPriority);
        if (priority > 127) priority = 127;
        else if (priority < 0) priority = 0;
        PlaybackPriorityBucket* bucket = &buckets->entries[priority];
        split->end.node = &bucket->sentinel;
        insertSoundList(&bucket->count, &split->end, node);
    }
    for (u32 index = 0; index != 128; ++index) {
        PlaybackPriorityBucket* bucket = &buckets->entries[index];
        while ((u32)bucket->count) {
            SoundListNode* node = bucket->sentinel.next;
            join->begin.node = node;
            removeArchiveListIterator(&bucket->count, &join->begin);
            join->end.node = &pool->active;
            insertSoundList(&pool->activeCount, &join->end, node);
        }
    }
}

/* 02893CEC */
void ArchivePlayback::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<PlaybackListRange> splitSequence(8);
    gabi::FrameLocal<PlaybackListRange> splitWave(0x10);
    gabi::FrameLocal<PlaybackListRange> splitStream(0x18);
    gabi::FrameLocal<PlaybackListRange> joinSequence(0x20);
    gabi::FrameLocal<PlaybackListRange> joinWave(0x28);
    gabi::FrameLocal<PlaybackListRange> joinStream(0x30);
    gabi::FrameLocal<PlaybackPriorityBuckets> buckets(0x38);
    initializeEventLog();
    sEventLog.switchBuffers();
    if (initialized) {
        initializeEventLog();
        sEventLog.clearReading();
    }
    u32 count = playerCount;
    u32 index = 0;
    while (index < count) {
        playerEntry(0x04000000u | index)->update();
        count = playerCount;
        ++index;
    }
    initializePlaybackPriorityLimit(&sSequencePriorityGuard, &sSequencePriorityLimit);
    sortPlaybackPool(&sequences, buckets.get(), splitSequence.get(), joinSequence.get(), (void* (*)(void*))constructSequencePoolBucket);
    initializePlaybackPriorityLimit(&sWavePriorityGuard, &sWavePriorityLimit);
    sortPlaybackPool(&waves, buckets.get(), splitWave.get(), joinWave.get(), (void* (*)(void*))constructWavePoolBucket);
    initializePlaybackPriorityLimit(&sStreamPriorityGuard, &sStreamPriorityLimit);
    sortPlaybackPool(&streams, buckets.get(), splitStream.get(), joinStream.get(), (void* (*)(void*))constructStreamPoolBucket);
    DriverCommandManager* commands = DriverCommandManager::instance();
    commands->collect();
    commands->submit(0);
}

/* 02893C88 */
u32 ArchivePlayback::hasArchiveData() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!archive) return 0;
    return archiveHasData(archive) != 0;
}

/* 02893CD4 */
SoundPlayer* ArchivePlayback::player(u32 id) {
    /* The canonical SoundPlayer describes its used prefix; archive slots are 0xA0 bytes. */
    u32 address = gabi::ea(players) + (id & 0x00FFFFFFu) * 0xA0u;
    return gabi::at<SoundPlayer>(address);
}

/* 02893CE8 */
SoundPlayer* ArchivePlayback::playerEntry(u32 id) {
    return gabi::tail_ptr<SoundPlayer*>(gabi::guest_fn((const void*)archivePlayerSlotTail), this, id);
}

/* 02894158 */
SoundArchive* ArchivePlayback::archiveImage() {
    return archive;
}

/* 02894160 */
SoundPlayer* ArchivePlayback::playerForLabel(const char* label) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 id = archiveFindItem(archive, label);
    return player(id);
}

/* 0289419C */
SoundPlayer* ArchivePlayback::playerForLabelEntry(const char* label) {
    return gabi::tail_ptr<SoundPlayer*>(gabi::guest_fn((const void*)archivePlayerForLabelTail), this, label);
}

static void* queryPlaybackResource(ArchiveResourceHook* hook, const char* label, const char* format, u32 index) {
    if (!hook->enabled) return nullptr;
    return (void*)hook->findFile(label, "seq", format, index);
}

static void setPlaybackHookEnabled(SoundArchive* image, ArchiveResourceHook* hook, u8 enabled) {
    SoundArchive* backend = image;
    backend->extension->enabled = enabled;
    hook->enabled = enabled;
}

/* 028941A0 */
u32 ArchivePlayback::prepareSequence(BasicSound* sound, ArchiveSoundSummary* soundInfo, ArchiveSequenceSummary* info,
                                     u32 mode, u32 value, PlaybackSequenceOptions_l* options) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u8>> deferred(8);
    gabi::FrameLocal<be<u32>> bankFileId(0x24);
    gabi::FrameLocal<be<u32>> offset(0x28);
    gabi::FrameLocal<PlaybackSequenceFormat_l> format(0x2C);
    gabi::FrameLocal<be<u32>> trackFlags(0x30);
    gabi::FrameLocal<PlaybackSequenceIdentity_l> identity(0x34);
    gabi::FrameLocal<SequenceCommandParameters> request(0x3C);
    gabi::FrameLocal<SequenceFileReader> reader(0x48);
    gabi::FrameLocal<SequenceConfiguration> resources(0x54);
    gabi::FrameLocal<SequenceResourceParameters> context(0x7C);
    gabi::FrameLocal<PlaybackResourceChoices> banks(0x9C);
    gabi::FrameLocal<PlaybackResourceChoices> waves(0xBC);
    void* image = nullptr;
    if (resourceHook) {
        const char* label = archiveSoundLabel(archive, sound->soundId);
        if (label) image = queryPlaybackResource(resourceHook, label, "bxseq", 0);
    }
    *offset = info->startOffset;
    *trackFlags = info->trackFlags;
    if (!image) {
        image = findArchivePlaybackFile(addressCache, soundInfo->fileId);
        if (options) {
            if (options->image) image = options->image;
            constructPlaybackSequenceReader(reader.get(), image);
            if (options->label) {
                if (!reader->findLabel(options->label, offset.get())) return 0xF;
                void* data = playbackSequenceData(reader.get());
                *offset = decodePlaybackSequenceOffset(data, *offset, trackFlags.get());
            }
        }
    }
    bool hasWork = sound->soundPlayer->heapCount != 0;
    *deferred = 0;
    identity->soundId = 0xFFFFFFFFu;
    identity->image = nullptr;
    __construct_array(banks->entries, 4, sizeof(SoundResourceChoice), (void* (*)(void*))constructResourceChoice);
    __construct_array(waves->entries, 4, sizeof(SoundResourceChoice), (void* (*)(void*))constructResourceChoice);
    for (u32 i = 0; i != 4; ++i) format->individual[i] = sSequenceFormat.individual[i];
    identity->soundId = sound->soundId;
    identity->image = image;
    if (!image) {
        if (!hasWork) return 5;
        *deferred = 1;
    }
    if (resourceHook) {
        const char* label = archiveSoundLabel(archive, sound->soundId);
        if (label) {
            for (u32 i = 0; i != 4; ++i) {
                if ((u32)info->bankIds[i] == 0xFFFFFFFFu) continue;
                u8 wasEnabled = resourceHook->enabled;
                if (!wasEnabled) setPlaybackHookEnabled(archive, resourceHook, 1);
                void* bank = queryPlaybackResource(resourceHook, label, "bxbnk", i);
                void* wave = queryPlaybackResource(resourceHook, label, "bxwar", i);
                if (!wasEnabled) setPlaybackHookEnabled(archive, resourceHook, 0);
                if (bank && wave) {
                    waves->entries[i].value = gabi::ea(wave);
                    banks->entries[i].value = gabi::ea(bank);
                    format->individual[i] = 0;
                } else {
                    waves->entries[i].value = 0;
                    banks->entries[i].value = 0;
                }
            }
        }
    }
    for (u32 i = 0; i != 4; ++i) banks->entries[i].type = (s32)(options ? (u32)options->banks[i] : (u32)info->bankIds[i]);
    bool queued = false;
    for (u32 i = 0; i != 4; ++i) {
        SoundResourceChoice* bank = &banks->entries[i];
        SoundResourceChoice* wave = &waves->entries[i];
        if (resourceHook && bank->value && wave->value) continue;
        *bankFileId = 0xFFFFFFFFu;
        if (!readArchiveBankFileId(archiveImage(), (u32)bank->type, bankFileId.get())) continue;
        void* bankImage = findArchivePlaybackFile(addressCache, *bankFileId);
        bank->value = gabi::ea(bankImage);
        if (!bankImage) {
            if (hasWork) {
                *deferred = 1;
                queueArchiveSequencePreparation(sound, gabi::ea(activeTrackPool), *trackFlags, &noteCallback, info->priority,
                                                 info->priorityFix, state, reserved);
                queued = true;
                break;
            }
            continue;
        }
        ArchiveAddressCache* cache = addressCache;
        ArchiveFileLoader* provider = cache ? &cache->loader : nullptr;
        u32 result = resolvePlaybackBankWave(wave, &format->individual[i], bankImage, archive, provider);
        if (result < 2) continue;
        if (result == 2) {
            if (hasWork) *deferred = 1;
        } else if (result == 0xFFFFFFFFu) {
            if (!hasWork) return 8;
            *deferred = 1;
        } else if (result == 0xFFFFFFFEu) return 0x12;
        else return 0xFF;
    }
    if (!queued) queueArchiveSequencePreparation(sound, gabi::ea(activeTrackPool), *trackFlags, &noteCallback, info->priority,
                                                 info->priorityFix, state, reserved);
    request->mValue = *offset;
    request->mOption = mode == 0;
    request->mCount = mode > 1 ? 0 : value;
    if (*deferred) {
        constructPlaybackSequenceContext(context.get(), archive, addressCache, identity.get(), banks.get(), sound->soundPlayer);
        prepareDeferredArchiveSequence(sound, context.get(), request.get());
    } else {
        for (u32 i = 0; i != 4; ++i) {
            resources->mFirst[i] = 0;
            resources->mSecond[i] = 0;
            resources->mFlags[i] = 0;
        }
        resources->mHeader = gabi::ea(image);
        for (u32 i = 0; i != 4; ++i) {
            resources->mFirst[i] = banks->entries[i].value;
            resources->mSecond[i] = waves->entries[i].value;
            resources->mFlags[i] = format->individual[i];
        }
        prepareResidentArchiveSequence(sound, resources.get(), request.get());
    }
    return 0;
}

/* 02894924 */
u32 ArchivePlayback::prepareStream(BasicSound* sound, ArchiveSoundSummary* soundInfo, ArchiveStreamSummary* info,
                                   ArchivePrefetchSummary* prefetch, u32 mode, u32 startOffset, PlaybackStreamFile_l* file) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 format = info->prefetchChannels;
    if (format != 1 && format != 2) return 0xFF;
    {
        gabi::FrameLocal<PlaybackStreamSetup_l> setup(8);
        setup->format = format == 2;
        for (u32 track = 0; track != 8; ++track) {
            ArchiveStreamTrackSummary* target = &setup->tracks[track];
            ArchiveStreamTrackSummary* source = &info->tracks[track];
            target->volume = source->volume;
            target->pan = source->pan;
            target->surroundPan = source->surroundPan;
            target->flags = source->flags;
            for (u32 i = 0; i != 4; ++i) target->routing[i] = source->routing[i];
            target->priority = source->priority;
            target->filter = source->filter;
            target->filterValue = source->filterValue;
            target->channelCount = source->channelCount;
            for (u32 i = 0; i != 2; ++i) target->channels[i] = source->channels[i];
        }
        setup->channels = info->channelCount;
        setup->trackFlags = info->trackFlags;
        setup->subsystem = streamBuffers;
        if (!prefetch || !prefetch->enabled) {
            setup->prefetchSize = 0xFFFFFFFFu;
            setup->prefetchEnabled = prefetch ? (u8)prefetch->enabled : 0;
            setup->prefetchOffset = 0;
            setup->pitch = info->pitch;
        } else {
            setup->prefetchEnabled = prefetch->enabled;
            setup->pitch = info->pitch;
            setup->prefetchOffset = prefetch->offset;
            setup->prefetchSize = prefetch->size;
        }
        for (u32 i = 0; i != 4; ++i) setup->routing[i] = info->routing[i];
        configureArchiveStream(sound, setup.get());
    }
    u32 useDefaultDevice = mode == 0;
    if (mode != 0 && mode != 2) startOffset = 0;
    gabi::FrameLocal<ArchiveFileSummary> fileInfo(8);
    fileInfo->externalPath = nullptr;
    fileInfo->offset = 0xFFFFFFFFu;
    fileInfo->size = 0xFFFFFFFFu;
    if (!readArchiveFileInfo(archive, soundInfo->fileId, fileInfo.get())) return 0x13;
    if (!fileInfo->externalPath) return 0x14;
    gabi::FrameLocal<PlaybackStreamOpen_l> open(0x14);
    gabi::FrameLocal<PlaybackStreamPath_l> resolved(0x2C0);
    open->hook = nullptr;
    open->hookBytes = 0;
    open->offset = 0;
    open->mode = 0;
    open->useDefaultDevice = 0;
    open->bytes = 0;
    open->hookStorage = nullptr;
    open->label = nullptr;
    clearArchiveStreamPath(open->path, 0, sizeof(open->path));
    if (resourceHook) {
        open->hook = resourceHook.get();
        open->hookBytes = 0x200;
        open->hookStorage = gabi::at<void>(gabi::ea(sound) + 0x320u);
        open->label = archiveSoundLabel(archive, sound->soundId);
    }
    open->useDefaultDevice = useDefaultDevice;
    open->mode = startOffset;
    open->offset = file ? (u32)file->offset : 0;
    open->bytes = file ? (u32)file->bytes : 0;
    SoundArchive* backend = archive;
    ArchiveBackendTable_l* table = reinterpret_cast<ArchiveBackendTable_l*>(backend->vtable.get());
    open->deviceOption = gabi::call_ptr<u32>(table->defaultStreamDescriptorExtraValue.function, backend);
    backend = archive;
    table = reinterpret_cast<ArchiveBackendTable_l*>(backend->vtable.get());
    open->descriptorOption = gabi::call_ptr<const void*>(table->defaultStreamDescriptorValue.function, backend);
    backend = archive;
    table = reinterpret_cast<ArchiveBackendTable_l*>(backend->vtable.get());
    open->extraOption = gabi::call_ptr<u32>(table->streamDescriptorConstant.function, backend);
    const be<u8>* path;
    if (file && file->path) path = file->path;
    else {
        resolveArchiveExternalPath(archive, reinterpret_cast<const be<u8>*>(fileInfo->externalPath.get()), resolved->bytes, sizeof(resolved->bytes));
        path = resolved->bytes;
    }
    u32 length = 0;
    while (length != 0x27E) {
        u8 byte = path[length];
        if (!byte) break;
        open->path[length++] = byte;
    }
    open->path[length] = 0;
    openArchiveStream(sound, open.get());
    return 0;
}

/* WWHD glue
ext 02893CD4 nw::snd::SoundPlayer* archivePlayerSlotTail(nw::snd::ArchivePlayback*,u32) callback
ext 02894160 nw::snd::SoundPlayer* archivePlayerForLabelTail(nw::snd::ArchivePlayback*,const char*) callback
str 10187FD4 "seq"
str 10187FE8 "bxseq"
str 10187FF0 "bxbnk"
str 10187FF8 "bxwar"
data sSequenceFormat 101821B0
ext 02896B5C void* findArchivePlaybackFile(nw::snd::ArchiveAddressCache*,u32)
ext 028AE1F4 nw::snd::SequenceFileReader* constructPlaybackSequenceReader(nw::snd::SequenceFileReader*,void*)
ext 028AE2B8 u32 nw::snd::SequenceFileReader::findLabel(const char*,be<u32>*) const
ext 028AE2B0 void* playbackSequenceData(nw::snd::SequenceFileReader*)
ext 028AB2A8 u32 decodePlaybackSequenceOffset(void*,u32,be<u32>*)
ext 0289CEA0 u32 resolvePlaybackBankWave(nw::snd::SoundResourceChoice*,be<u8>*,void*,nw::snd::SoundArchive*,void*)
ext 028ADC58 void queueArchiveSequencePreparation(nw::snd::BasicSound*,u32,u32,void*,u32,u32,u32,u32)
ext 0288B600 nw::snd::SequenceResourceParameters* constructPlaybackSequenceContext(nw::snd::SequenceResourceParameters*,nw::snd::SoundArchive*,nw::snd::ArchiveAddressCache*,nw::snd::PlaybackSequenceIdentity_l*,nw::snd::PlaybackResourceChoices*,nw::snd::SoundPlayer*)
ext 028ADE8C void prepareDeferredArchiveSequence(nw::snd::BasicSound*,nw::snd::SequenceResourceParameters*,nw::snd::SequenceCommandParameters*)
ext 028ADD60 void prepareResidentArchiveSequence(nw::snd::BasicSound*,nw::snd::SequenceConfiguration*,nw::snd::SequenceCommandParameters*)
ext 028992FC void configureArchiveStream(nw::snd::BasicSound*,nw::snd::PlaybackStreamSetup_l*)
ext 02899388 void openArchiveStream(nw::snd::BasicSound*,nw::snd::PlaybackStreamOpen_l*)
ext C000A858 void clearArchiveStreamPath(void*,u32,u32)
ext 0288E914 const char* archiveSoundLabel(nw::snd::SoundArchive*,u32)
ext 0288F218 const be<u8>* resolveArchiveExternalPath(nw::snd::SoundArchive*,const be<u8>*,be<u8>*,u32)
data sPlaybackBaseVtable 101821C8
data sPlaybackVtable 101821E0
data sSequenceWorkVtable 10182170
data sWaveWorkVtable 10182190
data sTrackAllocatorVtable 10187EA0
data sTrackPoolVtable 10187EC0
data sSequenceEntryVtable 10182058
data sWaveEntryVtable 10185614
data sEventLog 101FE250
data sEventLogGuard 101FE24C
data sSequencePriorityGuard 101FEA5C
data sSequencePriorityLimit 101FEA60
data sWavePriorityGuard 101FEA64
data sWavePriorityLimit 101FEA68
data sStreamPriorityGuard 101FEA6C
data sStreamPriorityLimit 101FEA70
ext 0273AD10 void* sndAllocate(u32)
ext 0273AF40 void archivePlaybackDelete(void*) callback
ext 0288E8E4 u32 archiveHasData(nw::snd::SoundArchive*)
ext 0288EB54 u32 readArchiveCapacity(nw::snd::SoundArchive*,nw::snd::ArchiveProjectSummary*)
ext 028AD8BC nw::snd::SoundMemoryArena* nw::snd::SoundMemoryArena::construct(nw::snd::SoundMemoryArena*)
ext 028AD928 u32 nw::snd::SoundMemoryArena::initialize(void*,u32)
ext 0288E8FC u32 archivePlayerCount(nw::snd::SoundArchive*)
ext 0288EACC u32 readArchivePlayerCapacity(nw::snd::SoundArchive*,u32,nw::snd::ArchivePlayerSummary*)
ext 028AD8A0 void nw::snd::SequenceTrackPool::initialize(void*,u32)
ext 02899114 void initializeArchiveStreamStorage(void*,void*,u32,u32)
ext 02897134 nw::snd::SoundPlayer* nw::snd::SoundPlayer::create(nw::snd::SoundPlayer*)
ext 02897698 void nw::snd::SoundPlayer::setSoundLimit(s32)
ext 02897B34 void nw::snd::SoundPlayer::addHeap(nw::snd::SoundMemoryArena*)
ext 02897720 void nw::snd::SoundPlayer::setMaximumSounds(s32)
ext 02899268 nw::snd::BasicSound* constructArchiveStreamSound(nw::snd::BasicSound*,nw::snd::PlaybackPool*)
ext 0286DA70 nw::snd::SoundListNode* insertSoundList(void*,nw::snd::SoundListIterator*,nw::snd::SoundListNode*)
ext 028ADBE4 nw::snd::SequenceSound* nw::snd::SequenceSound::construct(nw::snd::SequenceSound*,void*)
ext 0289FE28 nw::snd::WaveSound* nw::snd::WaveSound::construct(nw::snd::WaveSound*,nw::snd::PlaybackPool*)
ext 0289C050 void constructArchiveWorkBase(void*)
ext 02895F68 nw::snd::SoundResourceChoice* constructResourceChoice(nw::snd::SoundResourceChoice*) callback
ext 028986B0 nw::snd::AudioFrameDriver* nw::snd::AudioFrameDriver::instance()
ext 02898DB0 void nw::snd::AudioFrameDriver::addPlayer(nw::ut::HeapLink*)
ext 02898568 u32 isSoundSystemInitialized()
ext 028972D4 void nw::snd::SoundPlayer::stopAll(s32)
ext 0289C0C8 void destroyArchiveWorkBase(void*,u32)
ext 02898E18 void nw::snd::AudioFrameDriver::removePlayer(nw::ut::HeapLink*)
ext 028AD8AC void nw::snd::SequenceTrackPool::reset()
ext 02899150 void finalizeArchiveStreamStorage(void*)
ext 02885F10 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::instance()
ext 02887314 u32 nw::snd::DriverCommandManager::submit(u32)
ext 028871BC void nw::snd::DriverCommandManager::waitFor(u32)
ext 0286DACC nw::snd::SoundListNode* eraseSoundListRange(void*,nw::snd::SoundListIterator*,nw::snd::SoundListIterator*)
ext 0289D220 void nw::snd::AudioEventLog::switchBuffers()
ext 0289D1FC void nw::snd::AudioEventLog::clearReading()
ext 0289755C void nw::snd::SoundPlayer::update()
ext 02895FAC nw::snd::PlaybackPriorityBucket* constructSequencePoolBucket(nw::snd::PlaybackPriorityBucket*) callback
ext 02895FF4 nw::snd::PlaybackPriorityBucket* constructWavePoolBucket(nw::snd::PlaybackPriorityBucket*) callback
ext 0289603C nw::snd::PlaybackPriorityBucket* constructStreamPoolBucket(nw::snd::PlaybackPriorityBucket*) callback
ext 0286DB18 nw::snd::SoundListNode* removeArchiveListIterator(void*,nw::snd::SoundListIterator*)
ext 02887158 void nw::snd::DriverCommandManager::collect()
*/
