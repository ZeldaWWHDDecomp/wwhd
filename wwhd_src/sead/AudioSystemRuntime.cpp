/* sead audio system, second part of the translation unit begun in sead/AudioSystem.cpp: start-up of the device and
 * the sound system, shutdown (directly or on the launch thread), mixer/compressor/upsampler options and the runtime
 * type. Reconstructed from this game's instructions; SDK argument facts (AX device calls, OS thread and rendezvous)
 * from wut declarations. No external implementation used. */
#include "sead/AudioSystemRuntime.h"
#include "nw/snd/AudioMixRuntime.h"
using namespace sead;

/* the auxiliary worker is SEAD#19's tablet-audio worker, a sead::Thread subclass */
static void* allocateAlignedObject(u32 size, RegistryHeap* heap, s32 alignment);
static void* allocateArray(u32 size, RegistryHeap* heap, s32 alignment);
static void freeArray(void* memory);
static void releaseObject(void* object);
static u32 soundSystemWorkSize(SoundSystemSetup_l* setup);
static void initializeSoundSystemWork(SoundSystemSetup_l* setup, void* buffer, u32 size);
static void finalizeSoundSystem();
static void axInitialize();
static void axQuit();
static void axSetDefaultMixerSelect(u32 selection);
static u32 axSetDeviceCompressor(u32 device, u32 enabled);
static u32 axSetDeviceLinearUpsampler(u32 device, u32 channel, u32 enabled);
static void osInitRendezvous(PlatformRendezvous* rendezvous);
static u32 osWaitRendezvous(PlatformRendezvous* rendezvous, u32 coreMask);
static u32 osCreateThread(PlatformThread* thread, u32 entry, u32 argument, void* context, u8* stackTop, u32 stackSize,
                          s32 priority, u32 attributes);
static void osSetThreadName(PlatformThread* thread, const be<u8>* name);
static u32 osResumeThread(PlatformThread* thread);

/* this translation unit's copies of the audio system's tables and of the shared mixer runtime */
static DisposerVtable sAudioSystemSecondaryVtable;
static DisposerVtable sAudioSystemTypeTable;
static DisposerVtable sAudioSystemParentTypeTable;
static RuntimeType sAudioSystemParentType;
static u32 sAudioSystemParentTypeGuard;
static nw::snd::AudioMixRuntime sMixRuntime;
static u32 sMixRuntimeGuard;
static AudioSystemText sAudioSystemName;
static SafeTextVtable sAudioSystemTextVtable;
static const u8 sMixerSelections[3] = {0, 1, 2};
static const u8 sOutputModeSelections[3] = {1, 0, 2};

/* the auxiliary worker: a 0x94-byte tablet-audio worker with this stack size and message count */
static const u32 kAuxiliaryWorkerSize = 0x94;
static const u32 kAuxiliaryWorkerStackSize = 0x1000;
static const s32 kAuxiliaryWorkerMessages = 0x20;
/* OSCreateThread attribute bit for a detached thread */
static const u32 kThreadAttributeDetached = 8;
/* the main core joins every launch-thread rendezvous */
static const u32 kMainCoreBit = 2;

/* inlined: AudioServiceVtable slot 1 (+0x08) is the isKindOf query of the service's runtime type */
static u32 serviceIsKindOf(AudioService* service, const RuntimeType* type) {
    return gabi::call_ptr<u32>(service->mVtable->mReservedBeforeDestroy[0].mFunction, service, type);
}

/* inlined: the audio system behind the audio services, through the original's checked downcast */
static AudioSystem* audioSystemFromServices() {
    AudioService* backend = sAudioServices->mBackend;
    initializeRuntimeType(sAudioSystemType, sAudioSystemTypeGuard, &sAudioSystemTypeTable);
    if (backend != nullptr && serviceIsKindOf(backend, &sAudioSystemType) != 0)
        return reinterpret_cast<AudioSystem*>(backend);
    return nullptr;
}

/* 027627C8 */
void AudioSystem::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mAuxiliaryWorkerEnabled != 0) {
        Thread* worker = static_cast<Thread*>(allocateAlignedObject(kAuxiliaryWorkerSize, static_cast<RegistryHeap*>((void*)mWorkHeap), 4));
        if (worker != nullptr)
            worker = tablet_audio::Worker::construct(static_cast<tablet_audio::Worker*>(worker), mAuxiliaryWorkerPriority, static_cast<RegistryHeap*>((void*)mWorkHeap), kAuxiliaryWorkerStackSize,
                                                kAuxiliaryWorkerMessages);
        mAuxiliaryWorker = worker;
    }
    u32 workSize = soundSystemWorkSize(static_cast<SoundSystemSetup_l*>(mState.soundState()));
    void* work = allocateArray(workSize, static_cast<RegistryHeap*>((void*)mWorkHeap), 64);
    mWorkStorage = work;
    mState.setPair(work, reinterpret_cast<void*>(workSize));
    if (mLaunchEnabled != 0) {
        mLaunchThread = static_cast<PlatformThread*>(allocateAlignedObject(sizeof(PlatformThread), static_cast<RegistryHeap*>((void*)mWorkHeap), 8));
        mLaunchStack = static_cast<u8*>(allocateArray(mLaunchStackSize, static_cast<RegistryHeap*>((void*)mWorkHeap), 4));
        launch();
    } else {
        updateLocked();
    }
    u32 core = queryStartMode();
    u32 mode = AudioSystem::querySoundMode();
    reinterpret_cast<tablet_audio::Runtime*>(&mControl[0])->setDeviceMode(mode);
    reinterpret_cast<tablet_audio::Runtime*>(&mControl[0])->initialize(core, mWorkHeap);
    mInitialized = 1;
}

/* 0276291C */
void AudioSystem::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    Thread* worker = mAuxiliaryWorker;
    if (worker != nullptr) gabi::call_ptr<void>(worker->methods()->mRun.mFunction, worker, 0);
    finalizeSoundSystem();
    gabi::call_ptr<void>(mVtable->mFinish.mFunction, this);
}

/* 0276297C */
static u32 finalizeEntry() {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioSystem* system = audioSystemFromServices();
    system->finalize();
    osWaitRendezvous(&sAudioRendezvous, system->getWaitAffinity() | kMainCoreBit);
    return 0;
}

/* inlined: runs ENTRY on the audio system's launch thread and waits until it has passed the rendezvous */
static void runOnLaunchThread(AudioSystem* system, u32 entry) {
    osInitRendezvous(&sAudioRendezvous);
    u32 core = system->getLaunchAffinity();
    u32 waitMask = system->getWaitAffinity();
    u32 stack = gabi::ea((u8*)system->mLaunchStack);
    u32 stackSize = system->mLaunchStackSize;
    osCreateThread(system->mLaunchThread, entry, 0, system, gabi::at<u8>(stack + stackSize), stackSize,
                   system->mLaunchPriority, (core | kThreadAttributeDetached) & 0xFFFF);
    sAudioLaunchThreadName.assureTermination();
    osSetThreadName(system->mLaunchThread, sAudioLaunchThreadName.mText);
    osResumeThread(system->mLaunchThread);
    osWaitRendezvous(&sAudioRendezvous, waitMask);
}

/* 02762A24 */
void AudioSystem::finalizeOnLaunchThread() {
    gabi::AutoFrame frame{gabi::current_function()};
    runOnLaunchThread(this, gabi::guest_fn((const void*)&finalizeEntry));
}

/* 02762AF4 */
void AudioSystem::shutdown() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mInitialized == 0) return;
    if (mLaunchEnabled != 0) finalizeOnLaunchThread();
    else finalize();
    void* work = mWorkStorage;
    if (work != nullptr) freeArray(work);
    u8* stack = mLaunchStack;
    mWorkStorage = nullptr;
    if (stack != nullptr) {
        freeArray(stack);
        mLaunchStack = nullptr;
    }
    PlatformThread* thread = mLaunchThread;
    if (thread != nullptr) {
        releaseObject(thread);
        mLaunchThread = nullptr;
    }
    Thread* worker = mAuxiliaryWorker;
    if (worker != nullptr) {
        gabi::call_ptr<void>(worker->methods()->mDestroy.mFunction, worker, 3);
        mAuxiliaryWorker = nullptr;
    }
    reinterpret_cast<tablet_audio::Runtime*>(&mControl[0])->stop();
    mInitialized = 0;
}

/* 02762BC8 */
u32 AudioSystem::setOutputMode(u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mode > 2) return 0;
    u32 selection = sOutputModeSelections[mode];
    if (sMixRuntimeGuard == 0) {
        sMixRuntimeGuard = 1;
        nw::snd::AudioMixRuntime::create(&sMixRuntime);
    }
    sMixRuntime.setOutputMode(selection, 1);
    reinterpret_cast<tablet_audio::Runtime*>(&mControl[0])->setDeviceMode(mode);
    return 1;
}

/* 02762C88 */
u32 AudioSystem::soundMode() {
    gabi::AutoFrame frame{gabi::current_function()};
    return AudioSystem::querySoundMode();
}

/* 02762C8C */
void AudioSystemState::setMixerSelection(u32 selection) {
    gabi::AutoFrame frame{gabi::current_function()};
    mLock.lock();
    mMixerSelection = selection;
    mLock.unlock();
}

/* 02762CE0 */
void AudioSystem::setMixerSelection(u32 selection) {
    gabi::AutoFrame frame{gabi::current_function()};
    mState.setMixerSelection(selection);
}

/* 02762CE8 */
void AudioSystem::setLaunchMode(u32 enabled, u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    mLaunchEnabled = enabled;
    mLaunchMode = mode;
}

/* 02762CF4 */
void AudioSystemState::setCompressorEnabled(u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    mLock.lock();
    mCompressorEnabled = enabled;
    mLock.unlock();
}

/* 02762D48 */
void AudioSystem::setCompressorEnabled(u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    mState.setCompressorEnabled(enabled);
}

/* 02762D50 */
void AudioSystem::setSoundSystemOption(u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    mState.mLock.lock();
    static_cast<SoundSystemSetup_l*>(mState.soundState())->mOption = enabled;
    mState.mLock.unlock();
}

/* 02762DAC */
void AudioSystem::quitDevice() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::tail_ptr<void>(mVtable->mFinish.mFunction, this);
}

/* 02762DBC */
static u32 quitEntry() {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioSystem* system = audioSystemFromServices();
    system->quitDevice();
    osWaitRendezvous(&sAudioRendezvous, system->getWaitAffinity() | kMainCoreBit);
    return 0;
}

/* 02762E64 */
void AudioSystem::quitOnLaunchThread() {
    gabi::AutoFrame frame{gabi::current_function()};
    runOnLaunchThread(this, gabi::guest_fn((const void*)&quitEntry));
}

/* 02762F34 */
void AudioSystem::stop() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mInitialized == 0) return;
    if (mLaunchEnabled != 0) quitOnLaunchThread();
    else quitDevice();
    mInitialized = 0;
}

/* 02762F94 */
u32 AudioSystemState::mixerSelection() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return mMixerSelection;
}

/* 02762F9C */
u32 AudioSystemState::upsamplePolicy(u32 device) const {
    gabi::AutoFrame frame{gabi::current_function()};
    /* the index is not range-checked: guest address arithmetic */
    return *gabi::at<be<u32>>(gabi::ea(&mUpsamplePolicies[0]) + device * 4);
}

/* 02762FAC */
u32 AudioSystemState::compressorEnabled() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return mCompressorEnabled;
}

/* 02762FB4 */
void AudioSystem::initializeDevice() {
    gabi::AutoFrame frame{gabi::current_function()};
    axInitialize();
    u32 selection = mState.mixerSelection();
    if (selection <= 2) axSetDefaultMixerSelect(sMixerSelections[selection]);
    /* policy 0 enables the device's linear upsampler, 1 disables it, anything else keeps the default */
    u32 policy = mState.upsamplePolicy(0);
    if (policy == 0) axSetDeviceLinearUpsampler(0, 0, 1);
    else if (policy == 1) axSetDeviceLinearUpsampler(0, 0, 0);
    policy = mState.upsamplePolicy(1);
    if (policy <= 1) {
        u32 enabled = policy == 0 ? 1 : 0;
        for (u32 channel = 0; channel < 2; ++channel) axSetDeviceLinearUpsampler(1, channel, enabled);
    }
    u32 compressor = mState.compressorEnabled() != 0;
    axSetDeviceCompressor(0, compressor);
    axSetDeviceCompressor(1, compressor);
    axSetDeviceCompressor(2, compressor);
}

/* 02763148 */
void AudioSystem::finalizeDevice() {
    gabi::AutoFrame frame{gabi::current_function()};
    axQuit();
}

/* 0276314C */
void* AudioSystemState::workBuffer() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return mFirst;
}

/* 02763154 */
u32 AudioSystemState::workSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return gabi::ea((void*)mSecond);
}

/* 0276315C */
void AudioSystem::initializeSoundSystem() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* buffer = mState.workBuffer();
    u32 size = mState.workSize();
    initializeSoundSystemWork(static_cast<SoundSystemSetup_l*>(mState.soundState()), buffer, size);
}

/* 027631A0 */
static void initializeAudioSystemNames() {
    gabi::AutoFrame frame{gabi::current_function()};
    sAudioSystemName.mVtable = &sAudioSystemTextVtable;
    sAudioSystemName.mText.v = gabi::guest_str("AudioSystem");
    sAudioLaunchThreadName.mVtable = &sAudioSystemTextVtable;
    sAudioLaunchThreadName.mText.v = gabi::guest_str("AudioLaunchThread");
}

/* HD: compiler-generated and template code of this translation unit */

/* 027631DC */
void AudioSystemText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 027631F0 */
void AudioSystemText::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* 027631F4 */
u32 AudioSystem::isKindOf(const RuntimeType* type) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeRuntimeType(sAudioSystemType, sAudioSystemTypeGuard, &sAudioSystemTypeTable);
    if (type == &sAudioSystemType) return 1;
    initializeRuntimeType(sAudioSystemParentType, sAudioSystemParentTypeGuard, &sAudioSystemParentTypeTable);
    return type == &sAudioSystemParentType;
}

/* 0276326C */
void AudioSystem::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mSecondaryVtable = &sAudioSystemSecondaryVtable;
    reinterpret_cast<tablet_audio::Runtime*>(&mControl[0])->destroy(2);
    mSystemLock.destroy(2);
    mState.mLock.destroy(2);
    if (flags & 1) releaseObject(this);
}

/* WWHD glue
data sAudioSystemSecondaryVtable 1015EBB8
data sAudioSystemTypeTable 1015EAE8
data sAudioSystemParentTypeTable 1015EAD8
data sAudioSystemParentType 101FD95C
data sAudioSystemParentTypeGuard 101FD588
data sMixRuntime 101FCE00
data sMixRuntimeGuard 101FD8C8
data sAudioSystemName 104A11D0
data sAudioSystemTextVtable 1015EAA0
data sMixerSelections 1015EB04
data sOutputModeSelections 1015EB01
str 1015EB08 "AudioSystem"
str 1015EB14 "AudioLaunchThread"
ext 0276590C sead::tablet_audio::Worker* sead::tablet_audio::Worker::construct(sead::tablet_audio::Worker*,s32,sead::RegistryHeap*,u32,s32)
ext 0273B050 void* allocateAlignedObject(u32,sead::RegistryHeap*,s32)
ext 0273B0D4 void* allocateArray(u32,sead::RegistryHeap*,s32)
ext 0273AFC8 void freeArray(void*)
ext 0273AF40 void releaseObject(void*)
ext 02762468 void* sead::AudioSystemState::soundState()
ext 02898364 u32 soundSystemWorkSize(sead::SoundSystemSetup_l*)
ext 0289806C void initializeSoundSystemWork(sead::SoundSystemSetup_l*,void*,u32)
ext 028983F8 void finalizeSoundSystem()
ext 027624AC void sead::AudioSystemState::setPair(void*,void*)
ext 02762690 void sead::AudioSystem::launch()
ext 02762560 void sead::AudioSystem::updateLocked()
ext 02762470 u32 sead::AudioSystem::queryStartMode()
ext 02762510 u32 sead::AudioSystem::getLaunchAffinity()
ext 02762538 u32 sead::AudioSystem::getWaitAffinity()
ext 02762760 u32 sead::AudioSystem::querySoundMode()
ext 027601BC void sead::CriticalSection::lock()
ext 027601F0 void sead::CriticalSection::unlock()
ext 02760168 void sead::CriticalSection::destroy(u32)
ext 02888A50 nw::snd::AudioMixRuntime* nw::snd::AudioMixRuntime::create(nw::snd::AudioMixRuntime*)
ext 02889064 void nw::snd::AudioMixRuntime::setOutputMode(u32,u32)
ext 02766FC8 void sead::tablet_audio::Runtime::setDeviceMode(u32)
ext 02766B8C void sead::tablet_audio::Runtime::initialize(u32,void*)
ext 02766264 void sead::tablet_audio::Runtime::stop()
ext 02766334 void sead::tablet_audio::Runtime::destroy(u32)
ext C0006E30 void axInitialize()
ext C0006E90 void axQuit()
ext C0006F80 void axSetDefaultMixerSelect(u32)
ext C0006F88 u32 axSetDeviceCompressor(u32,u32)
ext C0006F98 u32 axSetDeviceLinearUpsampler(u32,u32,u32)
ext C0009D78 void osInitRendezvous(sead::PlatformRendezvous*)
ext C000A208 u32 osWaitRendezvous(sead::PlatformRendezvous*,u32)
ext C0009A48 u32 osCreateThread(sead::PlatformThread*,u32,u32,void*,u8*,u32,s32,u32)
ext C000A078 void osSetThreadName(sead::PlatformThread*,const be<u8>*)
ext C0009F38 u32 osResumeThread(sead::PlatformThread*)
*/
