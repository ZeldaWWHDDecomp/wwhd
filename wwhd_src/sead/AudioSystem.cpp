/* sead audio system: the audio manager object, its sound state and the audio launch thread. Reconstructed from
 * this game's instructions; the SDK rendezvous size was checked against wut declarations (facts only). No external
 * implementation or library layout used. The translation unit continues in SEAD#18. */
#include "sead/AudioSystem.h"
using namespace sead;

static void* platformAllocateObject(u32 size);
static void platformInitRendezvous(PlatformRendezvous* rendezvous);
static u32 platformWaitRendezvous(PlatformRendezvous* rendezvous, u32 coreMask);
static u32 platformCreateThread(PlatformThread* thread, u32 entry, u32 argument, void* context,
                         u8* stackTop, u32 stackSize, s32 priority, u32 attributes);
static void platformSetThreadName(PlatformThread* thread, const be<u8>* name);
static s32 platformResumeThread(PlatformThread* thread);
static void platformConstructSoundState(void* state);
static void platformConstructAudioControl(void* control);

struct SoundModeSetting { u8 mPrefix[8]; be<u32> mMode; };
static void platformConstructSoundMode(SoundModeSetting* setting);

static AudioSystemVtable sAudioSystemVtable;
static DisposerVtable sAudioSystemSecondaryVtable;
static DisposerVtable sAudioSystemTypeTable;
static u32 sThreadPriorityBase;
static SoundModeSetting sSoundMode;
static u32 sSoundModeGuard;
static const u8 sSoundModeMap[3] = {1, 0, 2};

/* inlined into the audio system constructor */
static void initializeAudioList(OffsetObjectList* list) {
    if (gabi::ea(list) == 0) list = static_cast<OffsetObjectList*>(platformAllocateObject(sizeof(OffsetObjectList)));
    if (gabi::ea(list) != 0) {
        list->mCount = 0;
        list->mPrevious = list;
        list->mLinkOffset = 0xFFFFFFFF;
        list->mNext = list;
    }
}

/* 0276231C */
AudioSystem* AudioSystem::construct(AudioSystem* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioSystem*>(platformAllocateObject(sizeof(AudioSystem)));
    if (object != nullptr) {
        object->mVtable = &sAudioSystemVtable;
        object->mSecondaryVtable = &sAudioSystemSecondaryVtable;
        AudioSystemState* state = &object->mState;
        if (gabi::ea(state) == 0) state = static_cast<AudioSystemState*>(platformAllocateObject(sizeof(AudioSystemState)));
        if (gabi::ea(state) != 0) {
            state->mFirst = nullptr;
            state->mSecond = nullptr;
            platformConstructSoundState(state->mSoundState);
            state->mMixerSelection = 0;
            state->mCompressorEnabled = 1;
            CriticalSection::construct(&state->mLock);
            state->mUpsamplePolicies[0] = 1;
            state->mUpsamplePolicies[1] = 1;
        }
        object->mWorkStorage = nullptr;
        object->mLaunchMode = 0;
        object->mLaunchEnabled = 1;
        object->mInitialized = 0;
        object->mWorkHeap = nullptr;
        initializeAudioList(&object->mServices);
        CriticalSection::construct(&object->mSystemLock);
        object->mAuxiliaryWorkerEnabled = 0;
        object->mLaunchStack = nullptr;
        object->mLaunchThread = nullptr;
        object->mLaunchPriority = 0x10;
        object->mLaunchStackSize = 0x1000;
        object->mAuxiliaryWorker = nullptr;
        object->mAuxiliaryWorkerPriority = sThreadPriorityBase + 1;
        platformConstructAudioControl(object->mControl);
        object->mServices.mLinkOffset = 0;
    }
    return object;
}

/* 02762468 */
void* AudioSystemState::soundState() { return mSoundState; }

/* 02762470 */
u32 AudioSystem::queryStartMode() {
    if (mLaunchEnabled == 0) return 1;
    u32 mode = mLaunchMode;
    if (mode == 0) return 0;
    if (mode == 1) return 2;
    return 1;
}

/* 027624AC */
void AudioSystemState::setPair(void* first, void* second) {
    gabi::AutoFrame frame{gabi::current_function()};
    mLock.lock();
    mFirst = first;
    mSecond = second;
    mLock.unlock();
}

/* 02762510 */
u32 AudioSystem::getLaunchAffinity() {
    u32 mode = mLaunchMode;
    if (mode == 0) return 1;
    if (mode == 1) return 4;
    return 0;
}

/* 02762538 */
u32 AudioSystem::getWaitAffinity() {
    u32 mode = mLaunchMode;
    if (mode == 0) return 1;
    if (mode == 1) return 4;
    return 0;
}

/* 02762560 */
void AudioSystem::updateLocked() {
    gabi::AutoFrame frame{gabi::current_function()};
    mState.mLock.lock();
    gabi::call_ptr<void>(mVtable->mBeforeUpdate.mFunction, this);
    gabi::call_ptr<void>(mVtable->mAfterUpdate.mFunction, this);
    Thread* worker = mAuxiliaryWorker;
    if (worker != nullptr) gabi::call_ptr<void>(worker->methods()->mTick.mFunction, worker);
    mState.mLock.unlock();
}

/* 027625E8 */
u32 AudioSystem::launchEntry(u32 argument, AudioSystem* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioService* backend = sAudioServices->mBackend;
    initializeRuntimeType(sAudioSystemType, sAudioSystemTypeGuard, &sAudioSystemTypeTable);
    if (backend != nullptr) {
        RuntimeTypedObject* object = reinterpret_cast<RuntimeTypedObject*>(backend);
        u32 matches = gabi::call_ptr<u32>(object->mVtable->mIsKindOf.mFunction, object, &sAudioSystemType);
        if (matches == 0) backend = nullptr;
    }
    AudioSystem* system = reinterpret_cast<AudioSystem*>(backend);
    system->updateLocked();
    u32 mask = system->getWaitAffinity();
    platformWaitRendezvous(&sAudioRendezvous, mask | 2);
    return 0;
}

/* 02762690 */
void AudioSystem::launch() {
    gabi::AutoFrame frame{gabi::current_function()};
    platformInitRendezvous(&sAudioRendezvous);
    u32 attributes = getLaunchAffinity();
    u32 waitMask = getWaitAffinity();
    u8* stack = mLaunchStack;
    u32 stackSize = mLaunchStackSize;
    s32 priority = mLaunchPriority;
    PlatformThread* thread = mLaunchThread;
    platformCreateThread(thread, gabi::guest_fn(reinterpret_cast<const void*>(AudioSystem::launchEntry)),
                         0, this, stack + stackSize, stackSize, priority, static_cast<u16>(attributes | 8));
    sAudioLaunchThreadName.assureTermination();
    platformSetThreadName(mLaunchThread, sAudioLaunchThreadName.mText);
    platformResumeThread(mLaunchThread);
    platformWaitRendezvous(&sAudioRendezvous, waitMask);
}

/* 02762760 */
u32 AudioSystem::querySoundMode() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sSoundModeGuard == 0) {
        sSoundModeGuard = 1;
        platformConstructSoundMode(&sSoundMode);
    }
    u32 mode = sSoundMode.mMode;
    if (mode > 2) return 4;
    return sSoundModeMap[mode];
}

/* WWHD glue
data sAudioSystemVtable 1015EB28
data sAudioSystemSecondaryVtable 1015EBB8
data sAudioSystemTypeTable 1015EAE8
data sThreadPriorityBase 101F8B9C
data sSoundMode 101FCE00
data sSoundModeGuard 101FD8C8
data sSoundModeMap 1015EAF8
ext 0273AD10 void* platformAllocateObject(u32)
ext C0009D78 void platformInitRendezvous(sead::PlatformRendezvous*)
ext C000A208 u32 platformWaitRendezvous(sead::PlatformRendezvous*,u32)
ext C0009A48 u32 platformCreateThread(sead::PlatformThread*,u32,u32,void*,u8*,u32,s32,u32)
ext C000A078 void platformSetThreadName(sead::PlatformThread*,const be<u8>*)
ext C0009F38 s32 platformResumeThread(sead::PlatformThread*)
ext 028984F0 void platformConstructSoundState(void*)
ext 02765F80 void platformConstructAudioControl(void*)
ext 02888A50 void platformConstructSoundMode(SoundModeSetting*)
*/
