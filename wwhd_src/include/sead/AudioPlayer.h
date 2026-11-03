/* sead audio player: the output service of the audio services. It wraps an nw::snd::ArchivePlayback (canonical
 * nw/snd header) with the archive loader that owns the sound archive, the sound heap archive loads allocate from and
 * the remote-audio command queue it updates every frame. Layout reconstructed from this game's constructor
 * (0x194 bytes) and instructions. */
#pragma once
#include "sead/RemoteAudioCommands.h"
#include "sead/AudioArchive.h"
#include "nw/snd/ArchivePlayback.h"

namespace sead {

/* the audio services' allocator (sead/AudioServices.h AudioAllocator): slot 5 answers whether it is busy */
struct AudioAllocatorQueryVtable_l {
    VtableSlot mReserved[5];
    VtableSlot mIsBusy;
};

/* AudioServiceVtable order (sead/AudioServices.h), plus the start slot */
struct AudioPlayerVtable {
    VtableSlot mHeader;
    VtableSlot mIsKindOf;
    VtableSlot mReserved;
    VtableSlot mDestroy;
    VtableSlot mInitialize;
    VtableSlot mShutdown;
    VtableSlot mUpdate;
    VtableSlot mReservedBeforeStart[8];
    VtableSlot mStartSound;
};

/* the playback object as the sound-start interface sees it: this unit's overrides of its virtuals */
struct AudioPlayerPlayback : nw::snd::ArchivePlayback {
    u32 startSound(void* handle, u32 soundId, void* startInfo, u32 flags);
    u32 itemId(const char* label);
};

/* Item ids of sound players inside an archive start at this value. */
static const u32 kPlayerItemIdBase = 0x04000000;

struct AudioPlayer {
    gptr<AudioPlayerVtable> mVtable;
    AudioPlayerPlayback mPlayback;
    gptr<void> mHandleVtable;
    gptr<void> mStorage;
    be<u32> mStorageBytes;
    gptr<void> mStreamStorage;
    be<u32> mStreamBytes;
    be<u32> mStreamStorageBytes;
    be<u32> mReserved;
    gptr<void> mStreamUserStorage;
    be<u32> mStreamUserBytes;
    gptr<AudioArchiveLoader> mLoader;
    gptr<SoundHeap> mHeap;
    gptr<remote_audio::CommandQueue> mRemoteQueue;
    be<u8> mReservedFlag;
    be<u8> mStartBlocked;
    u8 mFlagsPadding[2];
    CriticalSection mUpdateLock;
    be<u8> mLockUpdates;
    u8 mLockUpdatesPadding[3];

    static AudioPlayer* construct(AudioPlayer* object);
    void destroy(u32 flags);
    u32 isKindOf(const RuntimeType* type);
    void initialize();
    void shutdown();
    void update();
    u32 startSound(void* handle, u32 soundId, void* startInfo, u32 flags);
    void stopAllSounds(u32 fadeFrames);
    void releaseHeap();
    void releaseStorage();
    void createHeap(u32 size, RegistryHeap* heap);
    void initializeStorage(nw::snd::SoundArchive* archive, u32 streamBytes, u32 streams, u32 userBytes, RegistryHeap* heap);
    void initializeStorageWithStreams(u32 extraStreamBytes, u32 streams, u32 userBytes, RegistryHeap* heap);
    void initializeStorageScaled(const struct AudioPlayerStorageScale* scale);
    u32 isKindOfVirtual(const RuntimeType* type) {
        return gabi::call_ptr<u32>(mVtable->mIsKindOf.mFunction, this, type);
    }
};

/* the stream sizing record of the launch configuration: stream storage scaled from the archive's requirement */
struct AudioPlayerStorageScale {
    be<f32> mStreamScale;
    be<u32> mStreams;
    be<u32> mUserBytes;
    gptr<RegistryHeap> mHeap;
};

/* per-translation-unit text copies */
struct AudioPlayerText : SafeText {
    void destroy(u32 flags);
    void assureTermination();
};
struct AudioPlayerTextBuffer : SafeTextBuffer {
    void destroy(u32 flags);
    void assureTermination();
};

WWHD_OFFSET(AudioPlayerVtable, mStartSound, 0x78);
WWHD_OFFSET(AudioPlayer, mPlayback, 4);
WWHD_OFFSET(AudioPlayer, mHandleVtable, 0x120);
WWHD_OFFSET(AudioPlayer, mStorage, 0x124);
WWHD_OFFSET(AudioPlayer, mStreamStorage, 0x12C);
WWHD_OFFSET(AudioPlayer, mStreamStorageBytes, 0x134);
WWHD_OFFSET(AudioPlayer, mStreamUserStorage, 0x13C);
WWHD_OFFSET(AudioPlayer, mLoader, 0x144);
WWHD_OFFSET(AudioPlayer, mHeap, 0x148);
WWHD_OFFSET(AudioPlayer, mRemoteQueue, 0x14C);
WWHD_OFFSET(AudioPlayer, mStartBlocked, 0x151);
WWHD_OFFSET(AudioPlayer, mUpdateLock, 0x154);
WWHD_OFFSET(AudioPlayer, mLockUpdates, 0x190);
WWHD_SIZE(AudioPlayer, 0x194);
WWHD_SIZE(AudioPlayerStorageScale, 0x10);
} // namespace sead
