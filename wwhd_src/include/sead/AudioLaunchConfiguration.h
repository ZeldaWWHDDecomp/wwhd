/* sead audio player launch configuration: which archive the output player opens (a file under a root directory or an
 * image in memory) and how its playback storage is sized. Layout reconstructed from this game's constructor
 * (0x40 bytes) and instructions. */
#pragma once
#include "sead/AudioPlayer.h"

namespace sead {
enum AudioArchiveSource { AudioArchiveSource_None = 0, AudioArchiveSource_File = 1, AudioArchiveSource_Memory = 2 };

/* a SafeText with this translation unit's vtable copy */
struct AudioLaunchText : SafeText {};

struct AudioLaunchConfiguration {
    gptr<DisposerVtable> mVtable;
    be<u32> mArchiveSource;
    gptr<RegistryHeap> mHeap;
    be<u32> mHeapBytes;
    be<u32> mExtraStreamBytes;
    be<u32> mStreamScaleBits;   // f32, copied bit for bit into the storage-scale record
    be<u32> mStreams;
    be<u32> mUserBytes;
    be<u8> mLoadStrings;
    u8 mLoadStringsPadding[3];
    AudioLaunchText mFileName;
    AudioLaunchText mRootDirectory;
    gptr<const u8> mImage;
    gptr<AudioServices> mServices;
    be<u8> mStreamOption;
    u8 mStreamOptionPadding[3];
    static AudioLaunchConfiguration* construct(AudioLaunchConfiguration* object);
    void destroy(u32 flags);
    void openArchive(AudioServices* services);
    void initializePlayer();
};

WWHD_OFFSET(AudioLaunchConfiguration, mStreamScaleBits, 0x14);
WWHD_OFFSET(AudioLaunchConfiguration, mLoadStrings, 0x20);
WWHD_OFFSET(AudioLaunchConfiguration, mFileName, 0x24);
WWHD_OFFSET(AudioLaunchConfiguration, mRootDirectory, 0x2C);
WWHD_OFFSET(AudioLaunchConfiguration, mServices, 0x38);
WWHD_OFFSET(AudioLaunchConfiguration, mStreamOption, 0x3C);
WWHD_SIZE(AudioLaunchConfiguration, 0x40);
} // namespace sead
