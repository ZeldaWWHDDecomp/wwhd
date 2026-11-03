/* sead audio player launch configuration: opens the output player's archive and sizes its playback storage and
 * sound heap. Reconstructed from this game's instructions. No external implementation used. */
#include "sead/AudioLaunchConfiguration.h"
using namespace sead;

static void* allocateObject(u32 size);
static void releaseObject(void* object);

static DisposerVtable sAudioLaunchConfigurationVtable;
static SafeTextVtable sAudioLaunchTextVtable;
static DisposerVtable sAudioPlayerTypeTable;
static RuntimeType sAudioPlayerType;
static u32 sAudioPlayerTypeGuard;

/* inlined: the output service of the audio services as an audio player (checked downcast) */
static AudioPlayer* outputPlayer(AudioServices* services) {
    AudioPlayer* player = reinterpret_cast<AudioPlayer*>((AudioService*)services->mOutput);
    initializeRuntimeType(sAudioPlayerType, sAudioPlayerTypeGuard, &sAudioPlayerTypeTable);
    if (player != nullptr && player->isKindOfVirtual(&sAudioPlayerType) != 0) return player;
    return nullptr;
}

/* inlined: the configuration's heap, or the current sead heap */
static RegistryHeap* configurationHeap(RegistryHeap* heap) {
    if (heap == nullptr) heap = registryCurrentHeap(sRegistryManager);
    return heap;
}

/* inlined SafeText constructor: empty literal text */
static void constructLaunchText(AudioLaunchText* text) {
    if (gabi::ea(text) == 0) text = static_cast<AudioLaunchText*>(allocateObject(sizeof(AudioLaunchText)));
    if (gabi::ea(text) != 0) {
        text->mText.v = gabi::guest_str("");
        text->mVtable = &sAudioLaunchTextVtable;
    }
}

/* 027650B4 */
AudioLaunchConfiguration* AudioLaunchConfiguration::construct(AudioLaunchConfiguration* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioLaunchConfiguration*>(allocateObject(sizeof(AudioLaunchConfiguration)));
    if (object != nullptr) {
        object->mStreams = 0;
        object->mUserBytes = 0;
        object->mExtraStreamBytes = 0;
        object->mLoadStrings = 1;
        object->mHeap = nullptr;
        object->mArchiveSource = AudioArchiveSource_None;
        object->mVtable = &sAudioLaunchConfigurationVtable;
        object->mHeapBytes = 0;
        object->mStreamScaleBits = 0x3F800000;   // 1.0f
        constructLaunchText(&object->mFileName);
        constructLaunchText(&object->mRootDirectory);
        object->mServices = nullptr;
        object->mImage = nullptr;
        object->mStreamOption = 0;
    }
    return object;
}

/* 027651A4 */
void AudioLaunchConfiguration::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 027651B8 */
void AudioLaunchConfiguration::openArchive(AudioServices* services) {
    gabi::AutoFrame frame{gabi::current_function()};
    mServices = services;
    AudioPlayer* player = outputPlayer(services);
    RegistryHeap* heap = configurationHeap(mHeap);
    if (mArchiveSource == AudioArchiveSource_File) {
        if (*(be<u8>*)mRootDirectory.mText != 0) {
            mRootDirectory.assureTermination();
            player->mLoader->setRootDirectory(reinterpret_cast<const char*>((be<u8>*)mRootDirectory.mText));
        }
        player->mLoader->openFile(&mFileName, heap, mStreamOption, mLoadStrings);
    } else if (mArchiveSource == AudioArchiveSource_Memory) {
        player->mLoader->openMemory(mImage, heap);
    }
}

/* 0276530C */
void AudioLaunchConfiguration::initializePlayer() {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioPlayer* player = outputPlayer(mServices);
    RegistryHeap* heap = configurationHeap(mHeap);
    u32 extraStreamBytes = mExtraStreamBytes;
    if (extraStreamBytes != 0) {
        player->initializeStorageWithStreams(extraStreamBytes, mStreams, mUserBytes, heap);
    } else {
        gabi::FrameLocal<AudioPlayerStorageScale> scale(8);
        scale->mStreamScale = gabi::f32_from_bits(mStreamScaleBits);
        scale->mUserBytes = mUserBytes;
        scale->mHeap = heap;
        scale->mStreams = mStreams;
        player->initializeStorageScaled(scale.get());
    }
    player->createHeap(mHeapBytes, heap);
}

/* WWHD glue
data sAudioLaunchConfigurationVtable 1015F090
data sAudioLaunchTextVtable 1015F054
data sAudioPlayerTypeTable 1015F07C
data sAudioPlayerType 101FD950
data sAudioPlayerTypeGuard 101FD634
str 10000160 "" in 027650B4
ext 0273AD10 void* allocateObject(u32)
ext 0273AF40 void releaseObject(void*)
*/
