/* sead audio archives: the file and memory sound archives the audio player loads from, the archive loader that owns
 * them, the sound heap the loader allocates from, and the fade monitor that stops the output player once the master
 * volume has faded to silence. Layouts reconstructed from this game's constructors and instructions; the embedded
 * nw::snd objects use the canonical nw/snd headers. */
#pragma once
#include "sead/AudioServices.h"
#include "sead/HeapRegistry.h"
#include "sead/FileDevice.h"
#include "nw/snd/MemorySoundArchive.h"
#include "nw/snd/StreamSource.h"
#include "nw/snd/ArchiveLoading.h"

namespace sead {

/* ---- per-translation-unit text copies: SafeText / SafeTextBuffer with this unit's vtables ---- */
struct AudioArchiveText : SafeText {
    void destroy(u32 flags);
    void assureTermination();
};
struct AudioArchiveTextBuffer : SafeTextBuffer {
    void destroy(u32 flags);
    void assureTermination();
};
/* FixedSafeText instances of the archive translation unit (only their deleting destructors are out of line) */
struct AudioArchiveFixedText : SafeTextBuffer {
    void destroy(u32 flags);
};
struct ArchivePathText : SafeTextBuffer {
    void destroy(u32 flags);
};
struct SoundHeapText : SafeText {
    void destroy(u32 flags);
    void assureTermination();
};
struct SoundHeapTextBuffer : SafeTextBuffer {
    void destroy(u32 flags);
    void assureTermination();
};

/* ---- fade monitor: an audio-services query that stops the output player once the master volume reached zero ---- */
enum FadeStopState { FadeStop_Idle = 0, FadeStop_Requested = 1, FadeStop_Done = 2 };
enum FadeWaitState { FadeWait_Idle = 0, FadeWait_Waiting = 1, FadeWait_Silent = 2 };
struct FadeMonitor : AudioServicesQuery {
    be<u32> mStopState;
    be<u32> mWaitState;
    be<f32> mVolume;
    static FadeMonitor* construct(FadeMonitor* object);
    void destroy(u32 flags);
    void attach(AudioServices* services);
    void update();
    u32 isBusy();
};

/* ---- archives: a kind word and a vtable in front of the nw::snd archive object ---- */
enum AudioArchiveKind { AudioArchiveKind_File = 0, AudioArchiveKind_Memory = 1 };
struct AudioArchiveVtable {
    VtableSlot mHeader;
    VtableSlot mIsKindOf;
    VtableSlot mReserved;
    VtableSlot mDestroy;
    VtableSlot mOpen;
    VtableSlot mClose;
};
struct AudioArchive {
    be<u32> mKind;
    gptr<AudioArchiveVtable> mVtable;
    u32 isKindOf(const RuntimeType* type);
    void destroy(u32 flags);
    /* both archive kinds embed their nw::snd archive right behind this 8-byte header */
    nw::snd::SoundArchive* image() { return reinterpret_cast<nw::snd::SoundArchive*>(this + 1); }
    /* the original's virtual calls */
    u32 isKindOfVirtual(const RuntimeType* type) {
        return gabi::call_ptr<u32>(mVtable->mIsKindOf.mFunction, this, type);
    }
    void destroyVirtual(u32 flags) { gabi::call_ptr<void>(mVtable->mDestroy.mFunction, this, flags); }
    void openVirtual(const void* source) { gabi::call_ptr<void>(mVtable->mOpen.mFunction, this, source); }
};

/* this translation unit's copies of the nw::snd archive objects' inline virtuals */
struct AudioFileArchiveSource : nw::snd::StreamSource {
    u32 hasExternalHandle();
    nw::snd::AudioFileWorkContext* fileHandle();
    void* fileObject();
    u8 maxChannels();
};
struct AudioMemoryArchiveSource : nw::snd::MemorySoundArchive {
    void emptyBackendHook();
    void emptyBackendFinalHook();
    u32 defaultStreamDescriptorValue();
    u32 defaultStreamDescriptorExtraValue();
    u32 streamDescriptorConstant();
};

/* An archive opened from a file under a root directory, with its info and string blocks read into heap work. */
struct AudioFileArchive : AudioArchive {
    AudioFileArchiveSource mSource;
    gptr<RegistryHeap> mHeap;
    gptr<void> mInfoBlock;
    gptr<void> mStringBlock;
    be<u8> mLoadStrings;
    u8 mLoadStringsPadding[3];
    gptr<const char> mRootDirectory;
    be<u8> mStreamOption;
    u8 mStreamOptionPadding[3];
    gptr<void> mReaderWork;
    gptr<u8> mFsClient;
    static AudioFileArchive* construct(AudioFileArchive* object, RegistryHeap* heap);
    void destroy(u32 flags);
    u32 isKindOf(const RuntimeType* type);
    u32 open(const be<u8>* fileName);
    void close();
    void setLoadStrings(u32 enabled);
    void setRootDirectory(const char* directory);
    void setStreamOption(u32 enabled);
};

/* An archive whose whole image is already in memory. */
struct AudioMemoryArchive : AudioArchive {
    AudioMemoryArchiveSource mSource;
    static AudioMemoryArchive* construct(AudioMemoryArchive* object);
    void destroy(u32 flags);
    u32 isKindOf(const RuntimeType* type);
    u32 open(const u8* image);
    void close();
};

/* ---- the archive loader: an nw::snd sound-data loader (parked NW-SND code) with the archive it owns ---- */
struct SoundHeap;
struct ArchiveDataLoaderVtable_l {
    VtableSlot mHeader;
    VtableSlot mDestroy;
};
struct AudioArchiveLoader {
    u8 mLoaderPrefix[8];
    gptr<ArchiveDataLoaderVtable_l> mVtable;
    nw::snd::ArchiveFileLoader mFileLoader;
    u8 mLoaderTail[8];
    gptr<AudioArchive> mArchive;
    gptr<void> mWork;
    gptr<SoundHeap> mHeap;
    gptr<const char> mRootDirectory;
    be<u8> mInitialized;
    u8 mInitializedPadding[3];
    static AudioArchiveLoader* construct(AudioArchiveLoader* object);
    void destroy(u32 flags);
    void close();
    nw::snd::SoundArchive* archiveImage();
    void setHeap(SoundHeap* heap);
    void setRootDirectory(const char* directory);
    void initializeArchive(RegistryHeap* heap);
    void openFile(SafeText* fileName, RegistryHeap* heap, u32 streamOption, u32 loadStrings);
    void openMemory(const u8* image, RegistryHeap* heap);
    u32 resolveAllocator(gptr<nw::snd::ArchiveAllocator_l>* allocator);
    u32 loadByLabel(const char* label, u32 flags, s32 chunkBytes, nw::snd::ArchiveAllocator_l* allocator);
    void destroyVirtual(u32 flags) { gabi::call_ptr<void>(mVtable->mDestroy.mFunction, this, flags); }
};

/* ---- the sound heap: a locked nw::snd heap (parked NW-SND base) that archive loads allocate from ---- */
struct SoundHeapVtable {
    VtableSlot mHeader;
    VtableSlot mDestroy;
};
struct SoundHeap {
    gptr<SoundHeapVtable> mVtable;
    u8 mMutex[0x2C];
    u8 mBaseReserved[4];
    nw::snd::SoundHeap mHeap;
    gptr<void> mAllocatorVtable;
    gptr<void> mWork;
    gptr<AudioArchiveLoader> mLoader;
    gptr<nw::snd::SoundArchive> mArchive;
    static SoundHeap* construct(SoundHeap* object, u32 size, RegistryHeap* heap);
    void destroy(u32 flags);
    u32 allocateWork(u32 size, RegistryHeap* heap);
    u32 crossesMemoryBoundary();
    void setLoader(AudioArchiveLoader* loader, nw::snd::SoundArchive* archive);
    void destroyVirtual(u32 flags) { gabi::call_ptr<void>(mVtable->mDestroy.mFunction, this, flags); }
};

WWHD_OFFSET(FadeMonitor, mStopState, 8);
WWHD_OFFSET(FadeMonitor, mVolume, 0x10);
WWHD_SIZE(FadeMonitor, 0x14);
WWHD_SIZE(AudioArchive, 8);
WWHD_OFFSET(AudioArchiveVtable, mOpen, 0x20);
WWHD_OFFSET(AudioFileArchive, mSource, 8);
WWHD_OFFSET(AudioFileArchive, mHeap, 0x638);
WWHD_OFFSET(AudioFileArchive, mLoadStrings, 0x644);
WWHD_OFFSET(AudioFileArchive, mRootDirectory, 0x648);
WWHD_OFFSET(AudioFileArchive, mStreamOption, 0x64C);
WWHD_OFFSET(AudioFileArchive, mReaderWork, 0x650);
WWHD_OFFSET(AudioFileArchive, mFsClient, 0x654);
WWHD_SIZE(AudioFileArchive, 0x658);
WWHD_OFFSET(AudioMemoryArchive, mSource, 8);
WWHD_SIZE(AudioMemoryArchive, 0x2E0);
WWHD_OFFSET(AudioArchiveLoader, mVtable, 8);
WWHD_OFFSET(AudioArchiveLoader, mFileLoader, 0xC);
WWHD_OFFSET(AudioArchiveLoader, mArchive, 0x220);
WWHD_OFFSET(AudioArchiveLoader, mHeap, 0x228);
WWHD_OFFSET(AudioArchiveLoader, mInitialized, 0x230);
WWHD_SIZE(AudioArchiveLoader, 0x234);
WWHD_OFFSET(SoundHeap, mHeap, 0x34);
WWHD_OFFSET(SoundHeap, mAllocatorVtable, 0x44);
WWHD_OFFSET(SoundHeap, mWork, 0x48);
WWHD_SIZE(SoundHeap, 0x54);
} // namespace sead
