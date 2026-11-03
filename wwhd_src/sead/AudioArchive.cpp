/* sead audio archives: the fade monitor of the output player, the file and memory sound archives and the archive
 * loader that opens them. Reconstructed from this game's instructions; the embedded nw::snd objects use the
 * canonical nw/snd headers, the parked nw::snd loader base is called by address. No external implementation used. */
#include "sead/AudioPlayer.h"
#include "nw/snd/AudioMixRuntime.h"
using namespace sead;

static void* allocateObject(u32 size);
static void* allocateAlignedObject(u32 size, RegistryHeap* heap, s32 alignment);
static void* allocateArray(u32 size, RegistryHeap* heap, s32 alignment);
static void freeArray(void* memory);
static void releaseObject(void* object);
static void* osBlockMove(void* destination, const void* source, u32 size, u32 flush);
/* the parked nw::snd sound-data loader that AudioArchiveLoader derives from */
static void constructDataLoader(AudioArchiveLoader* loader);
static void finalizeDataLoader(AudioArchiveLoader* loader);
static void destroyDataLoader(AudioArchiveLoader* loader, u32 flags);
static u32 dataLoaderWorkSize(AudioArchiveLoader* loader, nw::snd::SoundArchive* archive);
static void initializeDataLoader(AudioArchiveLoader* loader, nw::snd::SoundArchive* archive, void* work, u32 size);
static const void* dataLoaderStoreFileAddress(AudioArchiveLoader* loader, u32 fileId, const void* address);
static const void* dataLoaderCachedFileAddress(AudioArchiveLoader* loader, u32 fileId);
static const void* dataLoaderFileAddress(AudioArchiveLoader* loader, u32 fileId);

/* this translation unit's tables, type nodes and the shared mixer runtime */
static DisposerVtable sFadeMonitorVtable;
static AudioArchiveVtable sAudioArchiveVtable;
static AudioArchiveVtable sAudioFileArchiveVtable;
static DisposerVtable sAudioFileArchiveSourceVtable;
static AudioArchiveVtable sAudioMemoryArchiveVtable;
static DisposerVtable sAudioMemoryArchiveSourceVtable;
static ArchiveDataLoaderVtable_l sAudioArchiveLoaderVtable;
static DisposerVtable sAudioArchiveLoaderFileVtable;
static DisposerVtable sAudioPlayerTypeTable;
static DisposerVtable sAudioArchiveTypeTable;
static DisposerVtable sAudioArchiveSubtypeTable;
static RuntimeType sAudioPlayerType;
static u32 sAudioPlayerTypeGuard;
static RuntimeType sAudioArchiveType;
static u32 sAudioArchiveTypeGuard;
static RuntimeType sAudioFileArchiveType;
static u32 sAudioFileArchiveTypeGuard;
static RuntimeType sAudioMemoryArchiveType;
static u32 sAudioMemoryArchiveTypeGuard;
static SafeTextVtable sAudioArchiveTextVtable;
static SafeTextVtable sArchivePathTextVtable;
static nw::snd::AudioMixRuntime sMixRuntime;
static u32 sMixRuntimeGuard;
static const char* sDefaultRootDirectory;

/* GHS's int/float conversion scratch in the frame at sp+8 */
struct ConversionScratch { be<u32> mHigh; be<u32> mLow; };

/* ---- fade monitor ---- */

/* 02763C8C */
FadeMonitor* FadeMonitor::construct(FadeMonitor* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<FadeMonitor*>(allocateObject(sizeof(FadeMonitor)));
    if (object != nullptr) {
        AudioServicesQuery::construct(object);
        object->mWaitState = FadeWait_Idle;
        object->mVolume = 1.0f;
        object->mVtable = &sFadeMonitorVtable;
        object->mStopState = FadeStop_Idle;
    }
    return object;
}

/* 02763CF8 */
void FadeMonitor::attach(AudioServices* services) {
    gabi::AutoFrame frame{gabi::current_function()};
    setServices(services);
}

/* 02763CFC */
void FadeMonitor::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sMixRuntimeGuard == 0) {
        sMixRuntimeGuard = 1;
        nw::snd::AudioMixRuntime::create(&sMixRuntime);
    }
    const nw::snd::MoveValue& mainVolume = sMixRuntime.mainVolume;
    if (mainVolume.counter < mainVolume.frames) {
        /* GHS's int-to-float conversion scratch at sp+8 (high word 0x43300000, low word the sign-flipped
         * integer); only its final contents, the frame count, are observable */
        gabi::FrameLocal<ConversionScratch> scratch(8);
        scratch->mHigh = 0x43300000;
        scratch->mLow = (u32)(s32)mainVolume.frames ^ 0x80000000u;
    }
    f32 volume = mainVolume.value();
    u32 waitState = mWaitState;
    if (waitState == FadeWait_Waiting && volume == 0.0f) {
        mWaitState = FadeWait_Silent;
        return;
    }
    if (waitState != FadeWait_Silent && mStopState == FadeStop_Requested && volume == 0.0f) {
        /* checked downcast of the output service; the original uses the result without a null test */
        AudioPlayer* player = reinterpret_cast<AudioPlayer*>((AudioService*)mServices->mOutput);
        initializeRuntimeType(sAudioPlayerType, sAudioPlayerTypeGuard, &sAudioPlayerTypeTable);
        if (player != nullptr && player->isKindOfVirtual(&sAudioPlayerType) == 0) player = nullptr;
        player->stopAllSounds(0);
        mStopState = FadeStop_Done;
    }
}

/* 02763E90 */
u32 FadeMonitor::isBusy() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mStopState != FadeStop_Idle) return 1;
    return anyBusy() != 0;
}

/* 02763ED8 */
void FadeMonitor::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* ---- file and memory archives ---- */

/* 02763EEC */
void AudioFileArchive::close() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (image()->isAvailable() != 0) mSource.finalize();
    mFsClient = nullptr;
}

/* 02763F34 */
void AudioFileArchive::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sAudioFileArchiveVtable;
    mSource.vtable = reinterpret_cast<nw::snd::StreamSourceVtable*>(&sAudioFileArchiveSourceVtable);
    close();
    void* work = mReaderWork;
    if (work != nullptr) {
        freeArray(work);
        mReaderWork = nullptr;
    }
    work = mInfoBlock;
    if (work != nullptr) {
        freeArray(work);
        mInfoBlock = nullptr;
    }
    work = mStringBlock;
    if (work != nullptr) {
        freeArray(work);
        mStringBlock = nullptr;
    }
    mSource.destroy(0);
    if (flags & 1) releaseObject(this);
}

/* 02763FF0 */
void AudioMemoryArchive::close() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mSource.isAvailable() != 0) mSource.close();
}

/* 02764030 */
void AudioMemoryArchive::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sAudioMemoryArchiveVtable;
    mSource.vtable = &sAudioMemoryArchiveSourceVtable;
    close();
    mSource.destroy(0);
    if (flags & 1) releaseObject(this);
}

/* ---- archive loader ---- */

/* 027640A4 */
void AudioArchiveLoader::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    u8 initialized = mInitialized;
    mVtable = &sAudioArchiveLoaderVtable;
    mFileLoader.vtable = reinterpret_cast<nw::snd::ArchiveLoaderVtable*>(&sAudioArchiveLoaderFileVtable);
    if (initialized != 0) {
        finalizeDataLoader(this);
        mInitialized = 0;
    }
    AudioArchive* archive = mArchive;
    if (archive != nullptr) {
        archive->destroyVirtual(3);
        mArchive = nullptr;
    }
    void* work = mWork;
    if (work != nullptr) {
        freeArray(work);
        mWork = nullptr;
    }
    destroyDataLoader(this, 0);
    if (flags & 1) releaseObject(this);
}

/* the loader's file-loader interface (embedded at +0xC) forwards to the loader */
struct AudioArchiveLoaderFiles : nw::snd::ArchiveFileLoader {
    AudioArchiveLoader* loader() { return gabi::at<AudioArchiveLoader>(gabi::ea(this) - 0xC); }
    const void* forwardStoreFileAddress(u32 fileId, const void* address);
    const void* forwardCachedFileAddress(u32 fileId);
    const void* forwardFileAddress(u32 fileId);
};

/* 0276416C */
const void* AudioArchiveLoaderFiles::forwardStoreFileAddress(u32 fileId, const void* address) {
    gabi::AutoFrame frame{gabi::current_function()};
    return dataLoaderStoreFileAddress(loader(), fileId, address);
}

/* 02764174 */
const void* AudioArchiveLoaderFiles::forwardCachedFileAddress(u32 fileId) {
    gabi::AutoFrame frame{gabi::current_function()};
    return dataLoaderCachedFileAddress(loader(), fileId);
}

/* 0276417C */
const void* AudioArchiveLoaderFiles::forwardFileAddress(u32 fileId) {
    gabi::AutoFrame frame{gabi::current_function()};
    return dataLoaderFileAddress(loader(), fileId);
}

/* 02764184 */
AudioFileArchive* AudioFileArchive::construct(AudioFileArchive* object, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioFileArchive*>(allocateObject(sizeof(AudioFileArchive)));
    if (object != nullptr) {
        object->mKind = AudioArchiveKind_File;
        object->mVtable = &sAudioArchiveVtable;
        nw::snd::StreamSource::create(&object->mSource);
        object->mInfoBlock = nullptr;
        object->mHeap = heap;
        object->mVtable = &sAudioFileArchiveVtable;
        object->mStringBlock = nullptr;
        object->mSource.vtable = reinterpret_cast<nw::snd::StreamSourceVtable*>(&sAudioFileArchiveSourceVtable);
        object->mLoadStrings = 0;
        object->mFsClient = nullptr;
        object->mRootDirectory = sDefaultRootDirectory;
        object->mStreamOption = 0;
        object->mReaderWork = nullptr;
    }
    return object;
}

/* 02764234 */
void AudioFileArchive::setLoadStrings(u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    mLoadStrings = enabled;
}

/* 0276423C */
void AudioFileArchive::setRootDirectory(const char* directory) {
    gabi::AutoFrame frame{gabi::current_function()};
    mRootDirectory = directory;
}

/* 02764244 */
void AudioFileArchive::setStreamOption(u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    mStreamOption = enabled;
}

/* inlined: length of a NUL-terminated text, 0 when it is longer than the text limit */
static s32 boundedTextLength(const be<u8>* text) {
    u32 address = gabi::ea(text);
    if (*gabi::at<const be<u8>>(address) == 0) return 0;
    s32 length = 0;
    do {
        ++length;
        ++address;
        if (length > kMaxTextLength) return 0;
    } while (*gabi::at<const be<u8>>(address) != 0);
    return length;
}

/* the path the file archive opens: root directory, "/", file name, in a fixed 512-byte text */
struct ArchivePathBuilder {
    AudioArchiveText mPart;
    FixedSafeText<512> mPath;
    /* inlined SafeTextBuffer append: copies as much of mPart as fits behind the current text */
    void appendPart() {
        u32 text = gabi::ea((be<u8>*)mPath.mText);
        mPath.assureTermination();
        s32 oldLength = boundedTextLength(mPath.mText);
        s32 at = oldLength >= 0 ? oldLength : 0;
        mPart.SafeText::assureTermination();
        s32 count = boundedTextLength(mPart.mText);
        s32 available = (s32)((u32)mPath.mCapacity - (u32)at);
        if (count >= available) count = available - 1;
        if (count > 0) {
            mPart.SafeText::assureTermination();
            osBlockMove(gabi::at<u8>(text + at), (be<u8>*)mPart.mText, count, 0);
            s32 end = at + count;
            if (end > oldLength) *gabi::at<be<u8>>(text + end) = 0;
        }
    }
};

/* 0276424C */
u32 AudioFileArchive::open(const be<u8>* fileName) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mStreamOption != 0) mSource.loading = 1;
    mFsClient = &sFileDeviceManager->mClient[0];
    u32 readerWorkSize = nw::snd::StreamSource::readerWorkSize();
    mReaderWork = allocateArray(readerWorkSize, mHeap, 64);

    gabi::FrameLocal<ArchivePathBuilder> path(8);
    path->mPath.mCapacity = 512;
    path->mPath.mVtable = &sArchivePathTextVtable;
    path->mPath.mStorage[511] = 0;
    path->mPart.mVtable = &sAudioArchiveTextVtable;
    path->mPath.mStorage[0] = 0;
    path->mPath.mText = path->mPath.mStorage;
    /* copy the root directory */
    path->mPart.mText = reinterpret_cast<be<u8>*>(const_cast<char*>((const char*)mRootDirectory));
    path->mPart.assureTermination();
    s32 count = boundedTextLength(path->mPart.mText);
    s32 capacity = path->mPath.mCapacity;
    if (count >= capacity) count = capacity - 1;
    path->mPart.SafeText::assureTermination();
    osBlockMove(path->mPath.mStorage, (be<u8>*)path->mPart.mText, count, 0);
    *gabi::at<be<u8>>(gabi::ea(&path->mPath.mStorage[0]) + count) = 0;
    /* append "/" and the file name */
    path->mPart.mVtable = &sAudioArchiveTextVtable;
    path->mPart.mText.v = gabi::guest_str("/");
    path->appendPart();
    path->mPart.mVtable = &sAudioArchiveTextVtable;
    path->mPart.mText = const_cast<be<u8>*>(fileName);
    path->appendPart();
    path->mPath.assureTermination();

    if (mSource.openSource(mFsClient, path->mPath.mText, gabi::ea((void*)mReaderWork), readerWorkSize, 16, 8) == 0)
        return 0;
    nw::snd::SoundArchiveHeader* header = reinterpret_cast<nw::snd::SoundArchiveHeader*>(&mSource.header);
    u32 infoBytes = header->infoBlockSize();
    void* info = allocateArray(infoBytes, mHeap, 64);
    mInfoBlock = info;
    mSource.readDataInfo(info, infoBytes);
    if (mLoadStrings != 0) {
        u32 stringBytes = header->stringBlockSize();
        void* strings = allocateArray(stringBytes, mHeap, 64);
        mStringBlock = strings;
        mSource.readSeekInfo(strings, stringBytes);
    }
    return 1;
}

/* 02764660 */
AudioMemoryArchive* AudioMemoryArchive::construct(AudioMemoryArchive* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioMemoryArchive*>(allocateObject(sizeof(AudioMemoryArchive)));
    if (object != nullptr) {
        object->mKind = AudioArchiveKind_Memory;
        object->mVtable = &sAudioArchiveVtable;
        nw::snd::MemorySoundArchive::create(&object->mSource);
        object->mVtable = &sAudioMemoryArchiveVtable;
        object->mSource.vtable = &sAudioMemoryArchiveSourceVtable;
    }
    return object;
}

/* 027646D4 */
u32 AudioMemoryArchive::open(const u8* image) {
    gabi::AutoFrame frame{gabi::current_function()};
    mSource.open(image);
    return 1;
}

/* 027646FC */
AudioArchiveLoader* AudioArchiveLoader::construct(AudioArchiveLoader* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioArchiveLoader*>(allocateObject(sizeof(AudioArchiveLoader)));
    if (object != nullptr) {
        constructDataLoader(object);
        object->mVtable = &sAudioArchiveLoaderVtable;
        object->mFileLoader.vtable = reinterpret_cast<nw::snd::ArchiveLoaderVtable*>(&sAudioArchiveLoaderFileVtable);
        object->mInitialized = 0;
        object->mArchive = nullptr;
        object->mRootDirectory = nullptr;
        object->mWork = nullptr;
        object->mHeap = nullptr;
    }
    return object;
}

/* 02764774 */
nw::snd::SoundArchive* AudioArchiveLoader::archiveImage() {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioArchive* archive = mArchive;
    if (archive == nullptr) return nullptr;
    RuntimeType* type;
    if (archive->mKind == AudioArchiveKind_File) {
        initializeRuntimeType(sAudioFileArchiveType, sAudioFileArchiveTypeGuard, &sAudioArchiveSubtypeTable);
        type = &sAudioFileArchiveType;
    } else if (archive->mKind == AudioArchiveKind_Memory) {
        initializeRuntimeType(sAudioMemoryArchiveType, sAudioMemoryArchiveTypeGuard, &sAudioArchiveSubtypeTable);
        type = &sAudioMemoryArchiveType;
    } else {
        return nullptr;
    }
    if (archive->isKindOfVirtual(type) == 0) return nullptr;
    return archive->image();
}

/* 0276488C */
void AudioArchiveLoader::setHeap(SoundHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mHeap = heap;
    if (heap == nullptr) return;
    nw::snd::SoundArchive* archive = archiveImage();
    if (archive != nullptr) mHeap->setLoader(this, archive);
}

/* 027648DC */
void AudioArchiveLoader::setRootDirectory(const char* directory) {
    gabi::AutoFrame frame{gabi::current_function()};
    mRootDirectory = directory;
}

/* 027648E4 */
void AudioArchiveLoader::initializeArchive(RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 size = dataLoaderWorkSize(this, archiveImage());
    mWork = allocateArray(size, heap, 64);
    initializeDataLoader(this, archiveImage(), mWork, size);
    mInitialized = 1;
}

/* 02764968 */
void AudioArchiveLoader::openFile(SafeText* fileName, RegistryHeap* heap, u32 streamOption, u32 loadStrings) {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioFileArchive* archive = static_cast<AudioFileArchive*>(allocateAlignedObject(sizeof(AudioFileArchive), heap, 64));
    if (archive != nullptr) archive = AudioFileArchive::construct(archive, heap);
    mArchive = archive;
    archive->setLoadStrings(loadStrings);
    const char* directory = mRootDirectory;
    if (directory != nullptr) archive->setRootDirectory(directory);
    archive->setStreamOption(streamOption);
    AudioArchiveVtable* table = archive->mVtable;   // loaded before the name's termination call
    fileName->assureTermination();
    gabi::call_ptr<void>(table->mOpen.mFunction, archive, (be<u8>*)fileName->mText);
    initializeArchive(heap);
}

/* 02764A28 */
void AudioArchiveLoader::openMemory(const u8* image, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioMemoryArchive* archive =
        static_cast<AudioMemoryArchive*>(allocateAlignedObject(sizeof(AudioMemoryArchive), heap, 64));
    if (archive != nullptr) archive = AudioMemoryArchive::construct(archive);
    mArchive = archive;
    archive->openVirtual(image);
    initializeArchive(heap);
}

/* 02764AAC */
void AudioArchiveLoader::close() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mInitialized != 0) {
        finalizeDataLoader(this);
        mInitialized = 0;
    }
    AudioArchive* archive = mArchive;
    if (archive != nullptr) archive->destroyVirtual(3);
    void* work = mWork;
    mArchive = nullptr;
    if (work != nullptr) {
        freeArray(work);
        mWork = nullptr;
    }
}

/* 02764B30 */
u32 AudioArchiveLoader::resolveAllocator(gptr<nw::snd::ArchiveAllocator_l>* allocator) {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((nw::snd::ArchiveAllocator_l*)*allocator == nullptr) {
        /* no allocator given: the loader's sound heap */
        SoundHeap* heap = mHeap;
        *allocator = reinterpret_cast<nw::snd::ArchiveAllocator_l*>(heap);
        if (heap == nullptr) return 0;
    }
    return mFileLoader.isAvailable() != 0;
}

/* 02764B90 */
u32 AudioArchiveLoader::loadByLabel(const char* label, u32 flags, s32 chunkBytes, nw::snd::ArchiveAllocator_l* allocator) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<nw::snd::ArchiveAllocator_l>> resolved(8);
    *resolved = allocator;
    if (resolveAllocator(resolved.get()) == 0) return 0;
    return mFileLoader.loadByLabel(label, *resolved, flags, chunkBytes);
}

/* HD: compiler-generated and template code of this translation unit */

/* 02764BFC */
void AudioArchiveText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 02764C10 */
void AudioArchiveText::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* 02764C14 */
void AudioArchiveTextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 02764C28 */
void AudioArchiveTextBuffer::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
    /* the last byte of the buffer, by guest address arithmetic (capacity is not checked) */
    *gabi::at<be<u8>>(gabi::ea((be<u8>*)mText) + mCapacity - 1) = 0;
}

/* 02764C40 */
u32 AudioArchive::isKindOf(const RuntimeType* type) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeRuntimeType(sAudioArchiveType, sAudioArchiveTypeGuard, &sAudioArchiveTypeTable);
    return type == &sAudioArchiveType;
}

/* 02764C7C */
void AudioArchive::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 02764C90 */
u32 AudioFileArchive::isKindOf(const RuntimeType* type) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeRuntimeType(sAudioFileArchiveType, sAudioFileArchiveTypeGuard, &sAudioArchiveSubtypeTable);
    if (type == &sAudioFileArchiveType) return 1;
    initializeRuntimeType(sAudioArchiveType, sAudioArchiveTypeGuard, &sAudioArchiveTypeTable);
    return type == &sAudioArchiveType;
}

/* 02764D08 */
void AudioArchiveFixedText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 02764D1C */
u32 AudioMemoryArchive::isKindOf(const RuntimeType* type) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeRuntimeType(sAudioMemoryArchiveType, sAudioMemoryArchiveTypeGuard, &sAudioArchiveSubtypeTable);
    if (type == &sAudioMemoryArchiveType) return 1;
    initializeRuntimeType(sAudioArchiveType, sAudioArchiveTypeGuard, &sAudioArchiveTypeTable);
    return type == &sAudioArchiveType;
}

/* 02764D94 */
void ArchivePathText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 02764DA8 */
void AudioMemoryArchiveSource::emptyBackendHook() {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* 02764DAC */
void AudioMemoryArchiveSource::emptyBackendFinalHook() {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* 02764DB0 */
u32 AudioMemoryArchiveSource::defaultStreamDescriptorValue() {
    gabi::AutoFrame frame{gabi::current_function()};
    return 0;
}

/* 02764DB8 */
u32 AudioMemoryArchiveSource::defaultStreamDescriptorExtraValue() {
    gabi::AutoFrame frame{gabi::current_function()};
    return 0;
}

/* 02764DC0 */
u32 AudioMemoryArchiveSource::streamDescriptorConstant() {
    gabi::AutoFrame frame{gabi::current_function()};
    return 16;
}

/* 02764DC8 */
u32 AudioFileArchiveSource::hasExternalHandle() {
    gabi::AutoFrame frame{gabi::current_function()};
    return 0;
}

/* 02764DD0 */
nw::snd::AudioFileWorkContext* AudioFileArchiveSource::fileHandle() {
    gabi::AutoFrame frame{gabi::current_function()};
    return &fileContext;
}

/* 02764DD8 */
void* AudioFileArchiveSource::fileObject() {
    gabi::AutoFrame frame{gabi::current_function()};
    return file;
}

/* 02764DE0 */
u8 AudioFileArchiveSource::maxChannels() {
    gabi::AutoFrame frame{gabi::current_function()};
    return channelLimit;
}

/* WWHD glue
data sFadeMonitorVtable 1015ED3C
data sAudioArchiveVtable 1015EDD4
data sAudioFileArchiveVtable 1015EE58
data sAudioFileArchiveSourceVtable 1015EE88
data sAudioMemoryArchiveVtable 1015EEE8
data sAudioMemoryArchiveSourceVtable 1015EF18
data sAudioArchiveLoaderVtable 1015EF78
data sAudioArchiveLoaderFileVtable 1015EFA8
data sAudioPlayerTypeTable 1015ED18
data sAudioArchiveTypeTable 1015ED94
data sAudioArchiveSubtypeTable 1015EE04
data sAudioPlayerType 101FD950
data sAudioPlayerTypeGuard 101FD634
data sAudioArchiveType 101FDFC8
data sAudioArchiveTypeGuard 101FD63C
data sAudioFileArchiveType 101FDFC0
data sAudioFileArchiveTypeGuard 101FD640
data sAudioMemoryArchiveType 101FDFC4
data sAudioMemoryArchiveTypeGuard 101FD644
data sAudioArchiveTextVtable 1015EDA4
data sArchivePathTextVtable 1015EE2C
data sMixRuntime 101FCE00
data sMixRuntimeGuard 101FD8C8
data sDefaultRootDirectory 101F8BB0
str 1015EE54 "/"
ext 0273AD10 void* allocateObject(u32)
ext 0273B050 void* allocateAlignedObject(u32,sead::RegistryHeap*,s32)
ext 0273B0D4 void* allocateArray(u32,sead::RegistryHeap*,s32)
ext 0273AFC8 void freeArray(void*)
ext 0273AF40 void releaseObject(void*)
ext C0009988 void* osBlockMove(void*,const void*,u32,u32)
ext 02896898 void constructDataLoader(sead::AudioArchiveLoader*)
ext 02896AE0 void finalizeDataLoader(sead::AudioArchiveLoader*)
ext 02896740 void destroyDataLoader(sead::AudioArchiveLoader*,u32)
ext 02896920 u32 dataLoaderWorkSize(sead::AudioArchiveLoader*,nw::snd::SoundArchive*)
ext 02896A40 void initializeDataLoader(sead::AudioArchiveLoader*,nw::snd::SoundArchive*,void*,u32)
ext 028967A0 const void* dataLoaderStoreFileAddress(sead::AudioArchiveLoader*,u32,const void*)
ext 028967D0 const void* dataLoaderCachedFileAddress(sead::AudioArchiveLoader*,u32)
ext 02896808 const void* dataLoaderFileAddress(sead::AudioArchiveLoader*,u32)
ext 02761AAC sead::AudioServicesQuery* sead::AudioServicesQuery::construct(sead::AudioServicesQuery*)
ext 02761AF4 void sead::AudioServicesQuery::setServices(sead::AudioServices*)
ext 02761AFC u32 sead::AudioServicesQuery::anyBusy()
ext 02887E60 nw::snd::StreamSource* nw::snd::StreamSource::create(nw::snd::StreamSource*)
ext 02887F04 void nw::snd::StreamSource::finalize()
ext 02887F9C void nw::snd::StreamSource::destroy(u32)
ext 02888008 u32 nw::snd::StreamSource::readerWorkSize()
ext 028880A0 u32 nw::snd::StreamSource::openSource(void*,const be<u8>*,u32,u32,u32,u32)
ext 0288843C u32 nw::snd::StreamSource::readDataInfo(void*,u32)
ext 02888528 u32 nw::snd::StreamSource::readSeekInfo(void*,u32)
ext 0288F408 u32 nw::snd::SoundArchiveHeader::infoBlockSize()
ext 0288F3E0 u32 nw::snd::SoundArchiveHeader::stringBlockSize()
ext 0288E8E4 u32 nw::snd::SoundArchive::isAvailable()
ext 0288A27C nw::snd::MemorySoundArchive* nw::snd::MemorySoundArchive::create(nw::snd::MemorySoundArchive*)
ext 0288A2E0 void nw::snd::MemorySoundArchive::destroy(u32)
ext 0288A334 u32 nw::snd::MemorySoundArchive::open(const u8*)
ext 0288A3D0 void nw::snd::MemorySoundArchive::close()
ext 02890FF8 u32 nw::snd::ArchiveLoader::isAvailable()
ext 02892650 u32 nw::snd::ArchiveFileLoader::loadByLabel(const char*,nw::snd::ArchiveAllocator_l*,u32,s32)
*/
