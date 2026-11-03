/* sead tablet (DRC) audio: the software surround mixer that runs on its own core, its worker thread and the
 * text objects of its translation unit. Layouts reconstructed from this game's constructors and instructions;
 * the worker is a sead::Thread (sead/Thread.h) and the device lookup goes through the audio system
 * (sead/AudioSystem.h). */
#pragma once
#include "sead/Foundation.h"
#include "sead/AudioSystem.h"
namespace sead { namespace tablet_audio {
/* the release slot of an object the worker holds a reference to */
struct ReferenceTable { u8 mPrefix[0xC]; be<u32> mRelease; };
struct ReferenceCount { be<u32> mReferences; };
struct Reference { be<u32> mReferences; gptr<ReferenceTable> mTable; };
/* the mixer's worker thread: a sead::Thread with its own vtable */
struct Worker : Thread {
    static Worker* construct(Worker* object, s32 priority, RegistryHeap* heap, u32 stackSize, s32 messageCount);
    void destroy(u32 flags);
    void startOnCurrentCore();
    u32 sendReference(Reference* reference);
    void releaseReference(Reference* reference);
    void dispatchRun();
};
/* a SafeText with this translation unit's own vtable copy */
struct WorkerText : SafeText { void destroy(u32 flags); void reserved(); };
struct Settings {
    be<u32> mMode;
    be<u32> mSpeakerPosition[2];
    be<f32> mDepth[2];
    be<f32> mDownmix[2];
    be<u32> mSurroundGain;
    be<f32> mOutputGain[2];
    void initialize();
};
struct MixChannels { gptr<be<u32>> mChannels[6]; };
struct MixDescriptor { gptr<MixChannels> mChannels; };
struct Runtime {
    be<u32> mTable;
    Reference mCompletion;
    Settings mSettings;
    Settings mPrevious;
    be<u8> mSoftwareMix;
    u8 mSoftwarePadding[3];
    be<u32> mLc;
    gptr<PlatformThread> mThread;
    gptr<be<u8>> mStack;
    be<u32> mStackSize;
    be<u32> mPriority;
    be<u32> mCore;
    be<u8> mActive;
    be<u8> mHeadphones;
    u8 mActivePadding[2];
    gptr<Worker> mWorker;
    gptr<be<u32>> mBuffers[8];
    be<u32> mBlocks;
    be<u32> mPrebufferBlocks;
    be<u32> mWriteCursor;
    be<u32> mReadCursor;
    be<u32> mProcessCursor;
    be<u32> mQueuedBlocks;
    be<u8> mBypassSoftware;
    be<u8> mLimit;
    u8 mLimitPadding[2];
    be<f32> mInputGain;
    be<f32> mThreshold;
    be<f32> mSlope;
    be<f32> mNormalization;
    be<u32> mDeviceMode;
    be<u8> mModeDirty;
    be<u8> mDownmixExtra;
    u8 mTail[2];
    u32 headphones(); void setOutputMode(u32 enabled); void updateHeadphones(); void resetHeadphones();
    void limitSample(be<s32>* sample); void limitChannels(const gptr<be<s32>>* channels);
    void process(); void callback(u32 device);
    void setLimiter(f32 threshold, f32 slope, f32 inputGain); void resetLimiter();
    void unregisterMixCallback(); u32 coreAffinity(); u32 rendezvousMask();
    void stop(); void destroy(u32 flags);
    void mixBlock(const be<u32>*,const be<u32>*,const be<u32>*,const be<u32>*,const be<u32>*,const be<u32>*,be<u32>*,be<u32>*);
    void mixCallback(const MixDescriptor* descriptor);
    void registerMixCallback(); void initialize(u32 core,void* heap);
    void applyMode(); void applyLc(); void applySpeaker(u32 channel); void applyDepth(u32 channel);
    void applyDownmix(u32 channel); void applySurroundGain(); void applyOutputGain(u32 channel);
    void applyAll(); void initializeSettings(); void applyChanged(); void updateSettings();
    void setSettings(const Settings* settings); void setDeviceMode(u32 mode);
};
}}
WWHD_SIZE(sead::tablet_audio::ReferenceCount,4);
WWHD_OFFSET(sead::tablet_audio::ReferenceCount,mReferences,0);
WWHD_SIZE(sead::tablet_audio::Reference,8);
WWHD_OFFSET(sead::tablet_audio::Reference,mTable,4);
WWHD_OFFSET(sead::tablet_audio::ReferenceTable,mRelease,0xC);
WWHD_SIZE(sead::tablet_audio::WorkerText,8);
WWHD_SIZE(sead::tablet_audio::Worker,0x94);
WWHD_OFFSET(sead::tablet_audio::Runtime,mSettings,0xC);
WWHD_OFFSET(sead::tablet_audio::Runtime,mPrevious,0x34);
WWHD_OFFSET(sead::tablet_audio::Runtime,mSoftwareMix,0x5C);
WWHD_OFFSET(sead::tablet_audio::Runtime,mThread,0x64);
WWHD_OFFSET(sead::tablet_audio::Runtime,mActive,0x78);
WWHD_OFFSET(sead::tablet_audio::Runtime,mWorker,0x7C);
WWHD_OFFSET(sead::tablet_audio::Runtime,mBuffers,0x80);
WWHD_OFFSET(sead::tablet_audio::Runtime,mBlocks,0xA0);
WWHD_OFFSET(sead::tablet_audio::Runtime,mProcessCursor,0xB0);
WWHD_OFFSET(sead::tablet_audio::Runtime,mInputGain,0xBC);
WWHD_OFFSET(sead::tablet_audio::Runtime,mDeviceMode,0xCC);
WWHD_SIZE(sead::tablet_audio::Runtime,0xD4);
WWHD_SIZE(sead::tablet_audio::Settings,0x28);
void* allocateTabletAudioObject(u32 size);
u32 tabletAudioCoreId();
u32 tabletAudioHeadphoneStatus(u32);
void tabletAudioOutputMode(u32);

void processTabletAudioChannels(const gptr<be<s32>>*,const gptr<be<s32>>*,u32);

void registerTabletFinalMix(u32 device,u32 callback);
void setTabletSurroundMode(u32 mode);
void setTabletSurroundLc(u32 enabled);
void setTabletSurroundSpeaker(u32 channel,u32 position);
void setTabletSurroundDepth(u32 channel,f32 depth);
void setTabletSurroundDownmix(u32 channel,f32 balance);
void setTabletSurroundLevel(u32 gain);
void setTabletSurroundOutput(u32 channel,f32 gain);
void dispatchTabletAudioSettings(sead::tablet_audio::Runtime* runtime);
void initializeTabletAudioRendezvous(void* rendezvous);
void waitTabletAudioRendezvous(void* rendezvous,u32 mask);
u32 createTabletAudioThread(sead::PlatformThread* thread,u32 entry,u32 argc,void* argv,void* stackTop,u32 stackSize,u32 priority,u32 attributes);
void nameTabletAudioThread(sead::PlatformThread* thread,const be<u8>* name);
void resumeTabletAudioThread(sead::PlatformThread* thread);
const be<u32>* tabletAudioSystemInfo();
Pair32 divideTabletAudioTicks(u32 high,u32 low,u32 divisorHigh,u32 divisorLow);
void sleepTabletAudioTicks(u32 high,u32 low);
void getTabletAudioDeviceMode(u32 device,be<u32>* mode);

void zeroTabletAudioRange(void* pointer,u32 size);
void setTabletAudioBytes(void* pointer,u32 byte,u32 size);
void getTabletFinalMix(u32 device,be<u32>* callback);
sead::PlatformThread* allocateTabletAudioThread(u32 size,void* heap,s32 alignment);
