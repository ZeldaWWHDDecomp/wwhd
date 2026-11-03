/* sead audio player: the output service of the audio services, playing sounds from the sound archive its loader
 * opened. Reconstructed from this game's instructions; the embedded nw::snd playback uses the canonical
 * nw/snd/ArchivePlayback.h. No external implementation used. */
#include "sead/AudioPlayer.h"
#include "sead/AudioSystem.h"
using namespace sead;
// SEAD#19's queue constructor (file-static there, bound by address)
static remote_audio::CommandQueue* createRemoteAudioQueue(remote_audio::CommandQueue* queue);

static void* allocateObject(u32 size);
static void* allocateAlignedObject(u32 size, RegistryHeap* heap, s32 alignment);
static void* allocateArray(u32 size, RegistryHeap* heap, s32 alignment);
static void freeArray(void* memory);
static void releaseObject(void* object);
static u32 soundSystemInitialized();
static u32 startPlaybackSound(nw::snd::ArchivePlayback* playback, void* handle, u32 soundId, void* startInfo, u32 flags);

/* this translation unit's tables, type nodes and name */
static DisposerVtable sAudioServiceBaseVtable;
static AudioPlayerVtable sAudioPlayerVtable;
static DisposerVtable sAudioPlayerPlaybackVtable;
static DisposerVtable sAudioPlayerHandleVtable;
static DisposerVtable sAudioPlayerTypeTable;
static DisposerVtable sAudioServiceTypeTable;
static DisposerVtable sAudioSystemTypeTable;
static RuntimeType sAudioPlayerType;
static u32 sAudioPlayerTypeGuard;
static RuntimeType sAudioServiceType;
static u32 sAudioServiceTypeGuard;
static AudioPlayerText sAudioPlayerName;
static SafeTextVtable sAudioPlayerTextVtable;

/* the start result when the player or the audio allocator refuses new sounds */
static const u32 kStartRejected = 0x80;
static const u32 kRemoteQueueSize = 0x28;
static const f32 kTwoPow31 = 2147483648.0f;

/* inlined: AudioServiceVtable slot 1 (+0x08) is the isKindOf query of the service's runtime type */
static u32 serviceIsKindOf(AudioService* service, const RuntimeType* type) {
    return gabi::call_ptr<u32>(service->mVtable->mReservedBeforeDestroy[0].mFunction, service, type);
}

/* 027632E4 */
void AudioPlayer::stopAllSounds(u32 fadeFrames) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = mPlayback.playerCount;
    for (u32 index = 0; count != 0; --count, ++index) {
        nw::snd::SoundPlayer* player = mPlayback.playerEntry(kPlayerItemIdBase + index);
        stopArchivePlayer(player, fadeFrames);
    }
}

/* 02763360 */
void AudioPlayer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sAudioPlayerVtable;
    mPlayback.vtable = &sAudioPlayerPlaybackVtable;
    mHandleVtable = &sAudioPlayerHandleVtable;
    stopAllSounds(0);
    void* storage = mStorage;
    if (storage != nullptr) {
        freeArray(storage);
        mStorage = nullptr;
    }
    storage = mStreamStorage;
    if (storage != nullptr) {
        freeArray(storage);
        mStreamStorage = nullptr;
    }
    storage = mStreamUserStorage;
    if (storage != nullptr) {
        freeArray(storage);
        mStreamUserStorage = nullptr;
    }
    SoundHeap* heap = mHeap;
    if (heap != nullptr) {
        heap->destroyVirtual(3);
        mHeap = nullptr;
    }
    remote_audio::CommandQueue* queue = mRemoteQueue;
    if (queue != nullptr) {
        queue->destroy(3);
        mRemoteQueue = nullptr;
    }
    AudioArchiveLoader* loader = mLoader;
    if (loader != nullptr) {
        loader->destroyVirtual(3);
        mLoader = nullptr;
    }
    mUpdateLock.destroy(2);
    mPlayback.destroy(0);
    if (flags & 1) releaseObject(this);
}

/* 02763498 */
u32 AudioPlayer::startSound(void* handle, u32 soundId, void* startInfo, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    /* the original queries the backend's audio-system type and drops the answer */
    AudioService* backend = sAudioServices->mBackend;
    initializeRuntimeType(sAudioSystemType, sAudioSystemTypeGuard, &sAudioSystemTypeTable);
    if (backend != nullptr) serviceIsKindOf(backend, &sAudioSystemType);
    if (mStartBlocked != 0) return kStartRejected;
    AudioAllocator* allocator = sAudioServices->mAllocator;
    auto allocatorTable = reinterpret_cast<AudioAllocatorQueryVtable_l*>(allocator->mVtable.get());
    if (gabi::call_ptr<u32>(allocatorTable->mIsBusy.mFunction, allocator) != 0) return kStartRejected;
    return startPlaybackSound(&mPlayback, handle, soundId, startInfo, flags);
}

/* 02763594 */
u32 AudioPlayerPlayback::startSound(void* handle, u32 soundId, void* startInfo, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    /* the playback is embedded at +4 of its audio player */
    AudioPlayer* player = gabi::at<AudioPlayer>(gabi::ea(this) - 4);
    return player->startSound(handle, soundId, startInfo, flags);
}

/* 0276359C */
AudioPlayer* AudioPlayer::construct(AudioPlayer* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioPlayer*>(allocateObject(sizeof(AudioPlayer)));
    if (object == nullptr) return object;
    object->mVtable = reinterpret_cast<AudioPlayerVtable*>(&sAudioServiceBaseVtable);
    nw::snd::ArchivePlayback::create(&object->mPlayback);
    object->mVtable = &sAudioPlayerVtable;
    object->mPlayback.vtable = &sAudioPlayerPlaybackVtable;
    object->mHandleVtable = &sAudioPlayerHandleVtable;
    object->mStorage = nullptr;
    object->mStreamStorageBytes = 0;
    object->mStartBlocked = 0;
    object->mRemoteQueue = nullptr;
    object->mHeap = nullptr;
    object->mReserved = 0;
    object->mReservedFlag = 0;
    object->mStreamUserBytes = 0;
    object->mStreamUserStorage = nullptr;
    object->mStreamBytes = 0;
    object->mStorageBytes = 0;
    object->mLoader = nullptr;
    object->mStreamStorage = nullptr;
    CriticalSection::construct(&object->mUpdateLock);
    object->mLockUpdates = 0;
    AudioArchiveLoader* loader = static_cast<AudioArchiveLoader*>(allocateObject(sizeof(AudioArchiveLoader)));
    if (loader != nullptr) loader = AudioArchiveLoader::construct(loader);
    object->mLoader = loader;
    remote_audio::CommandQueue* queue = static_cast<remote_audio::CommandQueue*>(allocateObject(kRemoteQueueSize));
    if (queue != nullptr) queue = createRemoteAudioQueue(queue);
    object->mRemoteQueue = queue;
    return object;
}

/* 02763698 */
void AudioPlayer::releaseHeap() {
    gabi::AutoFrame frame{gabi::current_function()};
    mLoader->setHeap(nullptr);
    SoundHeap* heap = mHeap;
    if (heap != nullptr) {
        heap->destroyVirtual(3);
        mHeap = nullptr;
    }
}

/* 027636F4 */
void AudioPlayer::releaseStorage() {
    gabi::AutoFrame frame{gabi::current_function()};
    mPlayback.finalize();
    void* storage = mStreamStorage;
    if (storage != nullptr) {
        freeArray(storage);
        mStreamStorage = nullptr;
    }
    storage = mStorage;
    if (storage != nullptr) {
        freeArray(storage);
        mStorage = nullptr;
    }
}

/* 02763758 */
void AudioPlayer::shutdown() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mPlayback.hasArchiveData() == 0) return;
    stopAllSounds(0);
    releaseHeap();
    releaseStorage();
    mLoader->close();
}

/* 027637B4 */
void AudioPlayer::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (soundSystemInitialized() == 0) return;
    if (mLockUpdates != 0) {
        mUpdateLock.lock();
        mPlayback.update();
        mUpdateLock.unlock();
    } else {
        mPlayback.update();
    }
    remote_audio::CommandQueue* queue = mRemoteQueue;
    if (queue != nullptr) queue->update();
}

/* 02763840 */
void AudioPlayer::createHeap(u32 size, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    SoundHeap* soundHeap = static_cast<SoundHeap*>(allocateAlignedObject(sizeof(SoundHeap), heap, 32));
    if (soundHeap != nullptr) soundHeap = SoundHeap::construct(soundHeap, size, heap);
    AudioArchiveLoader* loader = mLoader;
    mHeap = soundHeap;
    loader->setHeap(soundHeap);
}

/* 027638B8 */
void AudioPlayer::initializeStorage(nw::snd::SoundArchive* archive, u32 streamBytes, u32 streams, u32 userBytes,
                                    RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 storageBytes = mPlayback.storageSize(archive, userBytes);
    mStorage = allocateArray(storageBytes, heap, 64);
    gabi::FrameLocal<nw::snd::ArchivePlaybackConfig> config(8);
    if (streamBytes != 0) {
        mStreamStorage = allocateArray(streamBytes, heap, 64);
        if (streams != 0) {
            u32 stride = (streams + 63) & ~63u;
            mStreamUserBytes = mPlayback.streamUserStorageSize(archive, stride);
            mStreamUserStorage = allocateArray(mStreamUserBytes, heap, 64);
        }
    } else {
        mStreamStorage = nullptr;
    }
    config->image = archive;
    config->loader = mLoader;
    config->storage = mStorage;
    config->bytes = storageBytes;
    config->streamStorage = mStreamStorage;
    config->streamBytes = streamBytes;
    config->streamUserStorage = mStreamUserStorage;
    config->streamUserBytes = mStreamUserBytes;
    config->userBytes = userBytes;
    mPlayback.initializeWithConfig(config.get());
    mStreamBytes = streamBytes;
    mStorageBytes = storageBytes;
}

/* 02763A4C */
void AudioPlayer::initializeStorageWithStreams(u32 extraStreamBytes, u32 streams, u32 userBytes, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    nw::snd::SoundArchive* archive = mLoader->archiveImage();
    u32 bytes = mPlayback.streamStorageSize(archive);
    mStreamStorageBytes = bytes;
    u32 streamBytes = bytes != 0 ? bytes + extraStreamBytes : 0;
    initializeStorage(archive, streamBytes, streams, userBytes, heap);
}

/* GHS converts between integers and floats through an 8-byte frame scratch at sp+8 (u32 to double: high word
 * 0x43300000, low word the integer; float to int: fctiwz stored into the high word) */
struct ConversionScratch { be<u32> mHigh; be<u32> mLow; };

/* 02763AC4 */
void AudioPlayer::initializeStorageScaled(const AudioPlayerStorageScale* scale) {
    gabi::AutoFrame frame{gabi::current_function()};
    nw::snd::SoundArchive* archive = mLoader->archiveImage();
    u32 bytes = mPlayback.streamStorageSize(archive);
    mStreamStorageBytes = bytes;
    u32 streamBytes = bytes;
    if (bytes != 0) {
        gabi::FrameLocal<ConversionScratch> scratch(8);
        scratch->mLow = bytes;
        scratch->mHigh = 0x43300000;
        f32 scaled = gabi::fmuls_ppc(static_cast<f32>(bytes), scale->mStreamScale);
        /* float to unsigned: values from 2^31 up are converted as value - 2^31 */
        if (scaled < kTwoPow31) {
            streamBytes = (u32)gabi::ftoi(scaled);
            scratch->mHigh = streamBytes;
        } else {
            u32 low = (u32)gabi::ftoi(gabi::fsubs_ppc(scaled, kTwoPow31));
            scratch->mHigh = low;
            streamBytes = low + 0x80000000u;
        }
    }
    initializeStorage(archive, streamBytes, scale->mStreams, scale->mUserBytes, scale->mHeap);
}

/* 02763BA0 */
static void initializeAudioPlayerName() {
    gabi::AutoFrame frame{gabi::current_function()};
    sAudioPlayerName.mVtable = &sAudioPlayerTextVtable;
    sAudioPlayerName.mText.v = gabi::guest_str("AudioPlayer");
}

/* HD: compiler-generated and template code of this translation unit */

/* 02763BC4 */
void AudioPlayerText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 02763BD8 */
void AudioPlayerText::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* 02763BDC */
void AudioPlayerTextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 02763BF0 */
void AudioPlayerTextBuffer::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
    /* the last byte of the buffer, by guest address arithmetic (capacity is not checked) */
    *gabi::at<be<u8>>(gabi::ea((be<u8>*)mText) + mCapacity - 1) = 0;
}

/* 02763C08 */
u32 AudioPlayer::isKindOf(const RuntimeType* type) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeRuntimeType(sAudioPlayerType, sAudioPlayerTypeGuard, &sAudioPlayerTypeTable);
    if (type == &sAudioPlayerType) return 1;
    initializeRuntimeType(sAudioServiceType, sAudioServiceTypeGuard, &sAudioServiceTypeTable);
    return type == &sAudioServiceType;
}

/* 02763C80 */
void AudioPlayer::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* 02763C84 */
u32 AudioPlayerPlayback::itemId(const char* label) {
    gabi::AutoFrame frame{gabi::current_function()};
    return archive->findItem(label);
}

/* WWHD glue
data sAudioServiceBaseVtable 1015E964
data sAudioPlayerVtable 1015EC60
data sAudioPlayerPlaybackVtable 1015ECD8
data sAudioPlayerHandleVtable 1015ECF8
data sAudioPlayerTypeTable 1015EC38
data sAudioServiceTypeTable 1015EC18
data sAudioSystemTypeTable 1015EC28
data sAudioPlayerType 101FD950
data sAudioPlayerTypeGuard 101FD634
data sAudioServiceType 101FD954
data sAudioServiceTypeGuard 101FD2D8
data sAudioPlayerName 104A11F0
data sAudioPlayerTextVtable 1015EBC8
str 1015EC54 "AudioPlayer"
ext 0273AD10 void* allocateObject(u32)
ext 0273B050 void* allocateAlignedObject(u32,sead::RegistryHeap*,s32)
ext 0273B0D4 void* allocateArray(u32,sead::RegistryHeap*,s32)
ext 0273AFC8 void freeArray(void*)
ext 0273AF40 void releaseObject(void*)
ext 02898568 u32 soundSystemInitialized()
ext 02895DF4 u32 startPlaybackSound(nw::snd::ArchivePlayback*,void*,u32,void*,u32)
ext 028972D4 void stopArchivePlayer(nw::snd::SoundPlayer*,u32)
ext 028929AC nw::snd::ArchivePlayback* nw::snd::ArchivePlayback::create(nw::snd::ArchivePlayback*)
ext 02892BD8 void nw::snd::ArchivePlayback::destroy(u32)
ext 02893964 void nw::snd::ArchivePlayback::finalize()
ext 02893C88 u32 nw::snd::ArchivePlayback::hasArchiveData()
ext 02893CE8 nw::snd::SoundPlayer* nw::snd::ArchivePlayback::playerEntry(u32)
ext 02893CEC void nw::snd::ArchivePlayback::update()
ext 02892BEC u32 nw::snd::ArchivePlayback::storageSize(nw::snd::SoundArchive*,u32)
ext 02893818 u32 nw::snd::ArchivePlayback::streamUserStorageSize(nw::snd::SoundArchive*,u32)
ext 02893868 u32 nw::snd::ArchivePlayback::initializeWithConfig(nw::snd::ArchivePlaybackConfig*)
ext 02892D50 u32 nw::snd::ArchivePlayback::streamStorageSize(nw::snd::SoundArchive*)
ext 0288E91C u32 nw::snd::SoundArchive::findItem(const char*)
ext 02760084 sead::CriticalSection* sead::CriticalSection::construct(sead::CriticalSection*)
ext 02765478 sead::remote_audio::CommandQueue* createRemoteAudioQueue(sead::remote_audio::CommandQueue*)
ext 02765618 void sead::remote_audio::CommandQueue::destroy(u32)
ext 02765800 void sead::remote_audio::CommandQueue::update()
*/
