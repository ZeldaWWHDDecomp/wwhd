/* sead audio services: the audio task, the audio-services singleton with its configuration and busy query,
 * sound handle banks and volume ramps. Layouts reconstructed from this game's constructors and instructions. */
#pragma once
#include "sead/Thread.h"

namespace nw { namespace snd { struct BasicSound; } }

namespace sead {
struct VolumeSinkVtable { VtableSlot mHeader; VtableSlot mApply; };
struct VolumeSink { gptr<VolumeSinkVtable> mVtable; };
struct VolumeRamp {
    be<f32> mCurrent;
    be<f32> mTarget;
    be<f32> mStep;
    be<u32> mRemaining;
    gptr<VolumeSink> mSink;
    gptr<DisposerVtable> mVtable;
    static VolumeRamp* construct(VolumeRamp* object, f32 value);
    void advance();
    void set(f32 value, u32 duration);
};

/* a runtime type identity behind a GHS guard: the guard word is stored first, then the type's table */
template <class Guard> inline void initializeRuntimeType(RuntimeType& type, Guard& guard, DisposerVtable* table) {
    if (guard == 0) {
        guard = 1;
        type.mTable = table;
    }
}

struct RuntimeTypedObject { gptr<RuntimeTypedVtable_l> mVtable; };
struct AudioServiceVtable {
    VtableSlot mHeader;
    VtableSlot mReservedBeforeDestroy[2];
    VtableSlot mDestroy;
    VtableSlot mInitialize;
    VtableSlot mShutdown;
    VtableSlot mUpdate;
    VtableSlot mPostUpdate;
    VtableSlot mReservedBeforeBusy[3];
    VtableSlot mIsBusy;
};
struct AudioService {
    gptr<AudioServiceVtable> mVtable;
    u32 isKindOf(RuntimeType* type);
};
struct AudioAllocatorVtable {
    VtableSlot mHeader;
    VtableSlot mDestroy;
    VtableSlot mInitialize;
    VtableSlot mUpdate;
};
struct AudioAllocator { be<u32> mState; gptr<AudioAllocatorVtable> mVtable; };
/* a list whose elements embed their Link at mLinkOffset */
struct OffsetObjectList : LinkList { be<u32> mLinkOffset; };
struct AudioServicesConfig {
    gptr<RuntimeTypedVtable_l> mVtable;
    gptr<AudioService> mBackend;
    gptr<AudioAllocator> mAllocator;
    gptr<AudioService> mOutput;
    gptr<AudioService> mRouting;
    OffsetObjectList mServices;
    static AudioServicesConfig* construct(AudioServicesConfig* object);
    void setBackend(AudioService* backend);
    void setOutput(AudioService* output);
    void setRouting(AudioService* routing);
    void addService(AudioService* service);
    u32 isKindOf(RuntimeType* type);
};
struct AudioServices {
    gptr<ThreadManagerVtable> mVtable;
    RegisteredObject mDisposer;
    gptr<AudioService> mBackend;
    gptr<AudioAllocator> mAllocator;
    gptr<AudioService> mOutput;
    gptr<AudioService> mRouting;
    OffsetObjectList mServices;
    gptr<RegistryHeap> mHeap;
    be<u8> mReady;
    be<u8> mOwnsBackend;
    be<u8> mOwnsAllocator;
    be<u8> mOwnsOutput;
    static AudioServices* construct(AudioServices* object);
    static AudioServices* create(RegistryHeap* heap);
    static void releaseSingleton();
    void destroy(u32 flags);
    void shutdown();
    void initialize(AudioServicesConfig* config, RegistryHeap* heap);
    void update();
    void emptyHook();
};
struct AudioServicesDisposer : RegisteredObject { void destroy(u32 flags); };
struct AudioTaskOwnership {
    be<u8> mOwnsSingleton;
    void destroy(u32 flags);
};
struct AudioTask : CalcTask {
    AudioTaskOwnership mOwnership;
    u8 mOwnershipPadding[3];
    gptr<SafeText> mLabel;
    static AudioTask* construct(AudioTask* object, void* parameters);
    void destroy(u32 flags);
    static void destroyFromDisposer(Disposer* disposer, u32 flags);
    void publish();
    void initializeAudio();
    void emptyHook();
    u32 isKindOf(RuntimeType* type);
    static void updateAudio();
    static void shutdownAudio();
};
/* the task parameters the audio task was created with, as its task base stores them */
struct AudioTaskParametersView {
    u8 mTaskPrefix[0x2C];
    gptr<RuntimeTypedObject> mParameters;
};
struct AudioServicesQuery {
    gptr<AudioServices> mServices;
    gptr<DisposerVtable> mVtable;
    static AudioServicesQuery* construct(AudioServicesQuery* object);
    void setServices(AudioServices* services);
    u32 anyBusy();
    void destroy(u32 flags);
};
struct AudioLifecycleHooks {
    gptr<DisposerVtable> mVtable;
    void destroy(u32 flags);
    void initialize();
    void shutdown();
    void update();
};

struct SoundHandleSlot {
    gptr<nw::snd::BasicSound> mSound;
    static SoundHandleSlot* construct(SoundHandleSlot* object);
    void destroy(u32 flags);
};
struct SoundHandleBank : RegisteredObject {
    gptr<SoundHandleSlot> mSlots;
    be<u32> mCount;
    be<u8> mStopOnDestroy;
    u8 mPadding[3];
    static SoundHandleBank* construct(SoundHandleBank* object, u32 count, RegistryHeap* heap);
    void stopAll(s32 fadeFrames);
    void destroy(u32 flags);
    SoundHandleSlot* findFree();
    SoundHandleSlot* find(u32 soundId, u32 stop);
    SoundHandleSlot* findOrFree(u32 soundId, u32 stop);
};

/* Compiler-kept out-of-line copies of the text-object inlines in the audio-services translation units. */
struct AudioTextEntryPoints {
    static void destroyAudioTaskLabel(SafeText* text, u32 flags);
    static void assureAudioTaskLabel(SafeText* text);
    static void destroyAudioServicesLabel(SafeText* text, u32 flags);
    static void assureAudioServicesLabel(SafeText* text);
};

WWHD_OFFSET(VolumeRamp, mTarget, 4);
WWHD_OFFSET(VolumeRamp, mStep, 8);
WWHD_OFFSET(VolumeRamp, mRemaining, 0xC);
WWHD_OFFSET(VolumeRamp, mSink, 0x10);
WWHD_OFFSET(VolumeRamp, mVtable, 0x14);
WWHD_SIZE(VolumeRamp, 0x18);
WWHD_OFFSET(AudioServiceVtable, mDestroy, 0x18);
WWHD_OFFSET(AudioServiceVtable, mInitialize, 0x20);
WWHD_OFFSET(AudioServiceVtable, mShutdown, 0x28);
WWHD_OFFSET(AudioServiceVtable, mUpdate, 0x30);
WWHD_OFFSET(AudioServiceVtable, mPostUpdate, 0x38);
WWHD_OFFSET(AudioServiceVtable, mIsBusy, 0x58);
WWHD_SIZE(OffsetObjectList, 0x10);
WWHD_OFFSET(AudioServicesConfig, mServices, 0x14);
WWHD_SIZE(AudioServicesConfig, 0x24);
WWHD_OFFSET(AudioServices, mDisposer, 4);
WWHD_OFFSET(AudioServices, mBackend, 0x14);
WWHD_OFFSET(AudioServices, mAllocator, 0x18);
WWHD_OFFSET(AudioServices, mOutput, 0x1C);
WWHD_OFFSET(AudioServices, mRouting, 0x20);
WWHD_OFFSET(AudioServices, mServices, 0x24);
WWHD_OFFSET(AudioServices, mHeap, 0x34);
WWHD_OFFSET(AudioServices, mReady, 0x38);
WWHD_OFFSET(AudioServices, mOwnsOutput, 0x3B);
WWHD_SIZE(AudioServices, 0x3C);
WWHD_OFFSET(AudioTask, mTaskBase.mDisposer, 0x14);
WWHD_OFFSET(AudioTask, mOwnership, 0xC8);
WWHD_OFFSET(AudioTask, mLabel, 0xCC);
WWHD_SIZE(AudioTask, 0xD0);
WWHD_OFFSET(AudioTaskParametersView, mParameters, 0x2C);
WWHD_SIZE(AudioServicesQuery, 8);
WWHD_SIZE(AudioLifecycleHooks, 4);
WWHD_SIZE(SoundHandleSlot, 4);
WWHD_OFFSET(SoundHandleBank, mSlots, 0x10);
WWHD_OFFSET(SoundHandleBank, mCount, 0x14);
WWHD_OFFSET(SoundHandleBank, mStopOnDestroy, 0x18);
WWHD_SIZE(SoundHandleBank, 0x1C);
} // namespace sead

/* The audio-services singleton (sead/AudioStatics.cpp); SEAD#18's audio-system code reads it too. */
extern gptr<sead::AudioServices>& sAudioServices;
