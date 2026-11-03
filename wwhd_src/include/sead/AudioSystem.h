/* sead audio system: the audio manager object, its sound state and the audio launch thread. Layouts
 * reconstructed from this game's constructors and instructions; the translation unit continues in SEAD#18
 * (worker, mixer and device setup), which uses these layouts and statics. */
#pragma once
#include "sead/AudioServices.h"

namespace sead {
/* SDK rendezvous object the launch thread and its starter meet at */
struct PlatformRendezvous { u8 mStorage[0x10]; };

struct AudioSystemState {
    gptr<void> mFirst;
    gptr<void> mSecond;
    u8 mSoundState[0x24];
    be<u32> mMixerSelection;
    be<u32> mUpsamplePolicies[2];
    be<u8> mCompressorEnabled;
    u8 mCompressorPadding[3];
    CriticalSection mLock;
    void setPair(void* first, void* second);
    void* soundState();  /* 02762468: &mSoundState (the parked nw::snd sound-system setup record) */
    /* SEAD#18 (sead/AudioSystemRuntime.cpp); mFirst/mSecond are the sound system's work buffer and its size */
    void setMixerSelection(u32 selection);
    void setCompressorEnabled(u32 enabled);
    u32 mixerSelection() const;
    u32 upsamplePolicy(u32 device) const;
    u32 compressorEnabled() const;
    void* workBuffer() const;
    u32 workSize() const;
};
struct AudioSystemVtable {
    VtableSlot mReservedBeforeUpdate[15];
    VtableSlot mBeforeUpdate;
    VtableSlot mFinish;
    VtableSlot mAfterUpdate;
};
struct AudioSystem {
    gptr<AudioSystemVtable> mVtable;
    gptr<DisposerVtable> mSecondaryVtable;
    AudioSystemState mState;
    be<u8> mLaunchEnabled;
    u8 mLaunchEnabledPadding[3];
    be<u32> mLaunchMode;
    gptr<void> mWorkHeap;
    gptr<void> mWorkStorage;
    be<u8> mInitialized;
    u8 mInitializedPadding[3];
    OffsetObjectList mServices;
    CriticalSection mSystemLock;
    gptr<PlatformThread> mLaunchThread;
    gptr<u8> mLaunchStack;
    be<u32> mLaunchStackSize;
    be<s32> mLaunchPriority;
    gptr<Thread> mAuxiliaryWorker;
    be<u8> mAuxiliaryWorkerEnabled;
    u8 mAuxiliaryWorkerPadding[3];
    be<u32> mAuxiliaryWorkerPriority;
    u8 mControl[0xD4];
    static AudioSystem* construct(AudioSystem* object);
    u32 queryStartMode();
    u32 getLaunchAffinity();
    u32 getWaitAffinity();
    void updateLocked();
    static u32 launchEntry(u32 argument, AudioSystem* context);
    void launch();
    static u32 querySoundMode();
    /* SEAD#18 (sead/AudioSystemRuntime.cpp) */
    u32 isKindOf(const RuntimeType* type);
    void destroy(u32 flags);
    void initialize();
    void shutdown();
    void stop();
    void finalize();
    void quitDevice();
    void finalizeOnLaunchThread();
    void quitOnLaunchThread();
    void initializeDevice();
    void finalizeDevice();
    void initializeSoundSystem();
    u32 setOutputMode(u32 mode);
    u32 soundMode();
    void setMixerSelection(u32 selection);
    void setCompressorEnabled(u32 enabled);
    void setLaunchMode(u32 enabled, u32 mode);
    void setSoundSystemOption(u32 enabled);
};

WWHD_SIZE(PlatformRendezvous, 0x10);
WWHD_OFFSET(AudioSystemState, mSoundState, 8);
WWHD_OFFSET(AudioSystemState, mMixerSelection, 0x2C);
WWHD_OFFSET(AudioSystemState, mUpsamplePolicies, 0x30);
WWHD_OFFSET(AudioSystemState, mCompressorEnabled, 0x38);
WWHD_OFFSET(AudioSystemState, mLock, 0x3C);
WWHD_SIZE(AudioSystemState, 0x78);
WWHD_OFFSET(AudioSystemVtable, mBeforeUpdate, 0x78);
WWHD_OFFSET(AudioSystemVtable, mFinish, 0x80);
WWHD_OFFSET(AudioSystemVtable, mAfterUpdate, 0x88);
WWHD_OFFSET(AudioSystem, mState, 8);
WWHD_OFFSET(AudioSystem, mLaunchEnabled, 0x80);
WWHD_OFFSET(AudioSystem, mLaunchMode, 0x84);
WWHD_OFFSET(AudioSystem, mWorkHeap, 0x88);
WWHD_OFFSET(AudioSystem, mWorkStorage, 0x8C);
WWHD_OFFSET(AudioSystem, mInitialized, 0x90);
WWHD_OFFSET(AudioSystem, mServices, 0x94);
WWHD_OFFSET(AudioSystem, mSystemLock, 0xA4);
WWHD_OFFSET(AudioSystem, mLaunchThread, 0xE0);
WWHD_OFFSET(AudioSystem, mLaunchStack, 0xE4);
WWHD_OFFSET(AudioSystem, mLaunchStackSize, 0xE8);
WWHD_OFFSET(AudioSystem, mLaunchPriority, 0xEC);
WWHD_OFFSET(AudioSystem, mAuxiliaryWorker, 0xF0);
WWHD_OFFSET(AudioSystem, mAuxiliaryWorkerEnabled, 0xF4);
WWHD_OFFSET(AudioSystem, mAuxiliaryWorkerPriority, 0xF8);
WWHD_OFFSET(AudioSystem, mControl, 0xFC);
WWHD_SIZE(AudioSystem, 0x1D0);
} // namespace sead

/* Statics of the audio-system translation unit (sead/AudioStatics.cpp), shared with SEAD#18. */
extern sead::RuntimeType& sAudioSystemType;
extern be<u32>& sAudioSystemTypeGuard;
extern sead::PlatformRendezvous& sAudioRendezvous;
extern sead::SafeText& sAudioLaunchThreadName;
