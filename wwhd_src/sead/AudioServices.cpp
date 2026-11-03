/* sead audio services: the audio task, the audio-services singleton with its configuration and busy query, sound
 * handle banks and volume ramps. Reconstructed from this game's instructions; no external implementation or library
 * layout used. */
#include "sead/AudioSystem.h"
using namespace sead;

static void* platformAllocateObject(u32 size);
static void* platformAllocateArray(u32 size, RegistryHeap* heap, s32 alignment);
static void* platformAllocateAlignedObject(u32 size, RegistryHeap* heap, s32 alignment);
static void platformReleaseObject(void* object);
static RegisteredObject* platformConstructDisposer(RegisteredObject* object);
static RegisteredObject* platformConstructHeapDisposer(RegisteredObject* object, RegistryHeap* heap, u32 option);
// RegisteredObject::destroy and Link::insertBefore are bound once, in sead/Thread.cpp (every spec links both).

static DisposerVtable sVolumeRampVtable;

/* a direct lfs/stfs copy keeps its original single-precision representation */
static void copyPlatformFloat(be<f32>* destination, const be<f32>* source) {
    *reinterpret_cast<be<u32>*>(destination) = static_cast<u32>(*reinterpret_cast<const be<u32>*>(source));
}

/* 02762170 */
VolumeRamp* VolumeRamp::construct(VolumeRamp* object, f32 value) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<VolumeRamp*>(platformAllocateObject(sizeof(VolumeRamp)));
    if (object != nullptr) {
        object->mTarget = value;
        object->mCurrent = value;
        object->mRemaining = 0;
        object->mSink = nullptr;
        object->mVtable = &sVolumeRampVtable;
        object->mStep = 0.0f;
    }
    return object;
}

/* 027621EC */
void VolumeRamp::advance() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = mRemaining;
    if (count == 0) return;
    --count;
    VolumeSink* sink = mSink;
    mRemaining = count;
    if (count != 0) {
        f32 current = mCurrent;
        f32 step = mStep;
        mCurrent = gabi::fsubs_ppc(current, step);
    } else {
        copyPlatformFloat(&mCurrent, &mTarget);
        mStep = 0.0f;
    }
    if (sink != nullptr) {
        u32 method = sink->mVtable->mApply.mFunction;
        gabi::call_ptr<void>(method, sink, static_cast<f32>(mCurrent));
    }
}

/* 02762258 */
void VolumeRamp::set(f32 value, u32 duration) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 current = mCurrent;
    if (current == value) {
        mRemaining = 0;
        return;
    }
    mTarget = value;
    mRemaining = duration;
    if (duration == 0) {
        VolumeSink* sink = mSink;
        mCurrent = value;
        mStep = 0.0f;
        if (sink != nullptr) {
            u32 method = sink->mVtable->mApply.mFunction;
            gabi::call_ptr<void>(method, sink, static_cast<f32>(mCurrent));
        }
    } else {
        f32 delta = gabi::fsubs_ppc(static_cast<f32>(mCurrent), value);
        mStep = gabi::fdivs_ppc(delta, static_cast<f32>(duration));
    }
}

static CalcTask* platformConstructAudioTaskBase(CalcTask* object, void* parameters, const char* name);
static AudioService* platformConstructAudioOutput(AudioService* object);
static AudioAllocator* platformConstructAudioAllocator(AudioAllocator* object);

static AudioTask* sAudioTask;
static AudioServicesDisposer* sAudioServicesDisposer;
static RuntimeTypedVtable_l sAudioTaskVtable;
static DisposerVtable sAudioTaskDisposerVtable;
static SafeTextVtable sAudioTaskLabelVtable;
static u8 sAudioTaskEmptyLabel;
static ThreadManagerVtable sAudioServicesVtable;
static DisposerVtable sAudioServicesDisposerVtable;
static DisposerVtable sAudioQueryVtable;
static RuntimeTypedVtable_l sAudioConfigVtable;
static DisposerVtable sAudioTaskTypeTable;
static DisposerVtable sCalcTaskTypeTable;
static DisposerVtable sTaskBaseTypeTable;
static DisposerVtable sAudioConfigInitTypeTable;
static DisposerVtable sAudioConfigTypeTable;
static DisposerVtable sAudioConfigBaseTypeTable;
static DisposerVtable sAudioServiceTypeTable;
static RuntimeType sAudioTaskType;
static RuntimeType sCalcTaskType;
static RuntimeType sTaskBaseType;
static RuntimeType sAudioConfigType;
static RuntimeType sAudioConfigBaseType;
static RuntimeType sAudioServiceType;
static u32 sAudioTaskTypeGuard;
static u32 sCalcTaskTypeGuard;
static u32 sTaskBaseTypeGuard;
static u32 sAudioConfigTypeGuard;
static u32 sAudioConfigBaseTypeGuard;
static u32 sAudioServiceTypeGuard;

static AudioService* audioServiceForLink(Link* link, u32 offset) {
    return objectOf<AudioService>(link, offset);
}
static Link* audioServiceLink(AudioService* service, u32 offset) {
    return linkAt(service, offset);
}
/* inlined into the audio-services and configuration constructors */
static void initializeAudioList(OffsetObjectList* list) {
    if (gabi::ea(list) == 0) list = static_cast<OffsetObjectList*>(platformAllocateObject(sizeof(OffsetObjectList)));
    if (gabi::ea(list) != 0) {
        list->mCount = 0;
        list->mPrevious = list;
        list->mLinkOffset = 0xFFFFFFFF;
        list->mNext = list;
    }
}

/* 02760F0C */
void AudioTaskOwnership::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    if (mOwnsSingleton != 0) sAudioTask = nullptr;
    if (flags & 1) platformReleaseObject(this);
}

/* 02760F38 */
void AudioTask::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    SafeText* label = mLabel;
    mTaskBase.mVtable = reinterpret_cast<TaskBaseVtable_l*>(&sAudioTaskVtable);
    mTaskBase.mDisposer.mVtable = &sAudioTaskDisposerVtable;
    if (label != nullptr) {
        platformReleaseObject(label);
        mLabel = nullptr;
    }
    if (sAudioServices != nullptr) AudioServices::releaseSingleton();
    mOwnership.destroy(2);
    CalcTask::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

/* 02760FDC */
void AudioTask::destroyFromDisposer(Disposer* disposer, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioTask* task = reinterpret_cast<AudioTask*>(reinterpret_cast<u8*>(disposer) - offsetof(AudioTask, mTaskBase.mDisposer));
    task->destroy(flags);
}

/* 02760FE4 */
void AudioTask::publish() {
    if (sAudioTask == nullptr) {
        sAudioTask = this;
        mOwnership.mOwnsSingleton = 1;
    }
}

/* 02761004 */
AudioTask* AudioTask::construct(AudioTask* object, void* parameters) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioTask*>(platformAllocateObject(sizeof(AudioTask)));
    if (object != nullptr) {
        platformConstructAudioTaskBase(object, parameters, "AudioTask");
        object->mTaskBase.mVtable = reinterpret_cast<TaskBaseVtable_l*>(&sAudioTaskVtable);
        object->mTaskBase.mDisposer.mVtable = &sAudioTaskDisposerVtable;
        object->mOwnership.mOwnsSingleton = 0;
        object->mLabel = nullptr;
        SafeText* label = static_cast<SafeText*>(platformAllocateObject(sizeof(SafeText)));
        if (label != nullptr) {
            SafeText* storage = label;
            if (storage == nullptr) storage = static_cast<SafeText*>(platformAllocateObject(sizeof(SafeText)));
            if (storage != nullptr) {
                storage->mVtable = &sAudioTaskLabelVtable;
                storage->mText = &sAudioTaskEmptyLabel;
            }
        }
        object->mLabel = label;
    }
    return object;
}

/* 027610CC */
void AudioTask::initializeAudio() {
    gabi::AutoFrame frame{gabi::current_function()};
    RegistryHeap* heap = registryCurrentHeap(sRegistryManager);
    initializeRuntimeType(sAudioConfigType, sAudioConfigTypeGuard, &sAudioConfigInitTypeTable);
    RuntimeTypedObject* parameters = reinterpret_cast<AudioTaskParametersView*>(this)->mParameters;
    if (parameters != nullptr) {
        u32 matches = gabi::call_ptr<u32>(parameters->mVtable->mIsKindOf.mFunction, parameters, &sAudioConfigType);
        if (matches == 0) parameters = nullptr;
    }
    AudioServices* services = AudioServices::create(heap);
    services->initialize(reinterpret_cast<AudioServicesConfig*>(parameters), heap);
}

/* 02761178 */
void AudioTask::emptyHook() {}

/* 0276117C */
void AudioTask::updateAudio() {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioServices* services = sAudioServices;
    if (services != nullptr) services->update();
}

/* 02761190 */
void AudioTask::shutdownAudio() {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioServices* services = sAudioServices;
    if (services != nullptr) services->shutdown();
}

/* 027611BC */
u32 AudioTask::isKindOf(RuntimeType* type) {
    initializeRuntimeType(sAudioTaskType, sAudioTaskTypeGuard, &sAudioTaskTypeTable);
    if (type == &sAudioTaskType) return 1;
    initializeRuntimeType(sCalcTaskType, sCalcTaskTypeGuard, &sCalcTaskTypeTable);
    if (type == &sCalcTaskType) return 1;
    initializeRuntimeType(sTaskBaseType, sTaskBaseTypeGuard, &sTaskBaseTypeTable);
    return type == &sTaskBaseType;
}

/* 02761278 */
AudioServices* AudioServices::construct(AudioServices* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioServices*>(platformAllocateObject(sizeof(AudioServices)));
    if (object != nullptr) {
        object->mOutput = nullptr;
        object->mVtable = &sAudioServicesVtable;
        object->mRouting = nullptr;
        object->mBackend = nullptr;
        object->mAllocator = nullptr;
        initializeAudioList(&object->mServices);
        object->mHeap = nullptr;
        object->mOwnsOutput = 0;
        object->mServices.mLinkOffset = 4;
        object->mReady = 0;
        object->mOwnsAllocator = 0;
        object->mOwnsBackend = 0;
    }
    return object;
}

/* 02761328 */
AudioServices* AudioServices::create(RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioServices* services = sAudioServices;
    if (services == nullptr) {
        AudioServices* storage = static_cast<AudioServices*>(platformAllocateArray(sizeof(AudioServices), heap, 4));
        RegisteredObject* disposer = &storage->mDisposer;
        if (gabi::ea(disposer) != 0) {
            platformConstructHeapDisposer(disposer, heap, 3);
            disposer->mVtable = &sAudioServicesDisposerVtable;
        }
        sAudioServicesDisposer = reinterpret_cast<AudioServicesDisposer*>(disposer);
        services = storage;
        if (services != nullptr) services = construct(services);
        sAudioServices = services;
    }
    return services;
}

/* 027613C8 */
void AudioServices::releaseSingleton() {
    gabi::AutoFrame frame{gabi::current_function()};
    Disposer* disposer = sAudioServicesDisposer;
    if (gabi::ea(disposer) != 0) {
        sAudioServicesDisposer = nullptr;
        gabi::call_ptr<void>(disposer->mVtable->mDestroy.mFunction, disposer, 2);
        AudioServices* services = sAudioServices;
        if (services != nullptr) gabi::call_ptr<void>(services->mVtable->mDestroy.mFunction, services, 3);
        sAudioServices = nullptr;
    }
}

/* 02761448 */
void AudioServices::shutdown() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mReady == 0) return;
    AudioService* output = mOutput;
    if (output != nullptr) gabi::call_ptr<void>(output->mVtable->mShutdown.mFunction, output);
    AudioService* routing = mRouting;
    if (routing != nullptr) gabi::call_ptr<void>(routing->mVtable->mUpdate.mFunction, routing);
    if (mServices.mCount != 0) {
        u32 offset = mServices.mLinkOffset;
        AudioService* service = audioServiceForLink(mServices.mNext, offset);
        AudioService* end = audioServiceForLink(&mServices, offset);
        while (service != end) {
            gabi::call_ptr<void>(service->mVtable->mUpdate.mFunction, service);
            Link* link = audioServiceLink(service, offset);
            u32 nextOffset = mServices.mLinkOffset;
            Link* next = link->mNext;
            service = audioServiceForLink(next, offset);
            end = audioServiceForLink(&mServices, nextOffset);
        }
    }
    AudioService* backend = mBackend;
    if (backend != nullptr) gabi::call_ptr<void>(backend->mVtable->mShutdown.mFunction, backend);
    mReady = 0;
}

/* 02761548 */
void AudioServices::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mVtable = &sAudioServicesVtable;
    shutdown();
    if (mOwnsBackend != 0) {
        AudioService* backend = mBackend;
        if (backend != nullptr) {
            gabi::call_ptr<void>(backend->mVtable->mDestroy.mFunction, backend, 3);
            mBackend = nullptr;
        }
    }
    if (mOwnsOutput != 0) {
        AudioService* output = mOutput;
        if (output != nullptr) {
            gabi::call_ptr<void>(output->mVtable->mDestroy.mFunction, output, 3);
            mOutput = nullptr;
        }
    }
    if (mOwnsAllocator != 0) {
        AudioAllocator* allocator = mAllocator;
        if (allocator != nullptr) {
            gabi::call_ptr<void>(allocator->mVtable->mDestroy.mFunction, allocator, 3);
            mAllocator = nullptr;
        }
    }
    if (flags & 1) platformReleaseObject(this);
}

/* 02761640 */
void AudioServices::initialize(AudioServicesConfig* config, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (heap == nullptr) mHeap = registryCurrentHeap(sRegistryManager);
    else mHeap = heap;
    if (config != nullptr) {
        mBackend = config->mBackend;
        mAllocator = config->mAllocator;
        mOutput = config->mOutput;
        mRouting = config->mRouting;
        while (config->mServices.mCount != 0) {
            Link* node = config->mServices.popFront();
            u32 offset = mServices.mLinkOffset;
            AudioService* service = node == nullptr ? nullptr : audioServiceForLink(node, config->mServices.mLinkOffset);
            mServices.insertBefore(audioServiceLink(service, offset));
            mServices.mCount = static_cast<u32>(mServices.mCount) + 1;
        }
    }
    if (mBackend == nullptr) {
        AudioSystem* system = static_cast<AudioSystem*>(platformAllocateAlignedObject(sizeof(AudioSystem), heap, 4));
        if (system != nullptr) system = AudioSystem::construct(system);
        mBackend = reinterpret_cast<AudioService*>(system);  // the audio system is the default backend service
        mOwnsBackend = 1;
    }
    if (mOutput == nullptr) {
        AudioService* output = static_cast<AudioService*>(platformAllocateAlignedObject(0x194, heap, 4));
        if (output != nullptr) output = platformConstructAudioOutput(output);
        mOutput = output;
        mOwnsOutput = 1;
    }
    if (mAllocator == nullptr) {
        AudioAllocator* allocator = static_cast<AudioAllocator*>(platformAllocateAlignedObject(0x14, heap, 4));
        if (allocator != nullptr) allocator = platformConstructAudioAllocator(allocator);
        mAllocator = allocator;
        mOwnsAllocator = 1;
    }
    AudioService* backend = mBackend;
    if (backend != nullptr) gabi::call_ptr<void>(backend->mVtable->mInitialize.mFunction, backend);
    AudioService* output = mOutput;
    if (output != nullptr) gabi::call_ptr<void>(output->mVtable->mInitialize.mFunction, output);
    AudioAllocator* allocator = mAllocator;
    if (allocator != nullptr) gabi::call_ptr<void>(allocator->mVtable->mInitialize.mFunction, allocator, this);
    AudioService* routing = mRouting;
    if (routing != nullptr) gabi::call_ptr<void>(routing->mVtable->mInitialize.mFunction, routing, this);
    if (mServices.mCount != 0) {
        u32 offset = mServices.mLinkOffset;
        AudioService* service = audioServiceForLink(mServices.mNext, offset);
        AudioService* end = audioServiceForLink(&mServices, offset);
        while (service != end) {
            gabi::call_ptr<void>(service->mVtable->mShutdown.mFunction, service, this, heap);
            Link* link = audioServiceLink(service, offset);
            u32 nextOffset = mServices.mLinkOffset;
            Link* next = link->mNext;
            service = audioServiceForLink(next, offset);
            end = audioServiceForLink(&mServices, nextOffset);
        }
    }
    routing = mRouting;
    if (routing != nullptr) gabi::call_ptr<void>(routing->mVtable->mShutdown.mFunction, routing);
    mReady = 1;
}

/* 027618B8 */
void AudioServices::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mReady == 0) return;
    AudioService* output = mOutput;
    if (output != nullptr) gabi::call_ptr<void>(output->mVtable->mUpdate.mFunction, output);
    AudioAllocator* allocator = mAllocator;
    if (allocator != nullptr) gabi::call_ptr<void>(allocator->mVtable->mUpdate.mFunction, allocator);
    if (mServices.mCount != 0) {
        u32 offset = mServices.mLinkOffset;
        AudioService* service = audioServiceForLink(mServices.mNext, offset);
        AudioService* end = audioServiceForLink(&mServices, offset);
        while (service != end) {
            gabi::call_ptr<void>(service->mVtable->mPostUpdate.mFunction, service);
            Link* next = audioServiceLink(service, offset)->mNext;
            u32 nextOffset = mServices.mLinkOffset;
            service = audioServiceForLink(next, offset);
            end = audioServiceForLink(&mServices, nextOffset);
        }
    }
}

/* 0276198C */
void AudioServices::emptyHook() {}

/* 02761990 */
void AudioServicesDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mVtable = &sAudioServicesDisposerVtable;
    if (this == sAudioServicesDisposer) {
        AudioServices* services = sAudioServices;
        sAudioServicesDisposer = nullptr;
        gabi::call_ptr<void>(services->mVtable->mDestroy.mFunction, services, 2);
        sAudioServices = nullptr;
    }
    RegisteredObject::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

/* 02761A50 */
u32 AudioService::isKindOf(RuntimeType* type) {
    initializeRuntimeType(sAudioServiceType, sAudioServiceTypeGuard, &sAudioServiceTypeTable);
    return type == &sAudioServiceType;
}

/* 02761AAC */
AudioServicesQuery* AudioServicesQuery::construct(AudioServicesQuery* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioServicesQuery*>(platformAllocateObject(sizeof(AudioServicesQuery)));
    if (object != nullptr) {
        object->mServices = nullptr;
        object->mVtable = &sAudioQueryVtable;
    }
    return object;
}

/* 02761AF4 */
void AudioServicesQuery::setServices(AudioServices* services) { mServices = services; }

/* 02761AFC */
u32 AudioServicesQuery::anyBusy() {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioServices* services = mServices;
    if (services->mServices.mCount == 0) return 0;
    u32 offset = services->mServices.mLinkOffset;
    AudioService* service = audioServiceForLink(services->mServices.mNext, offset);
    AudioService* end = audioServiceForLink(&services->mServices, offset);
    while (service != end) {
        if (gabi::call_ptr<u32>(service->mVtable->mIsBusy.mFunction, service) != 0) return 1;
        services = mServices;
        Link* next = audioServiceLink(service, offset)->mNext;
        u32 nextOffset = services->mServices.mLinkOffset;
        service = audioServiceForLink(next, offset);
        end = audioServiceForLink(&services->mServices, nextOffset);
    }
    return 0;
}

/* 02761BD8 */
AudioServicesConfig* AudioServicesConfig::construct(AudioServicesConfig* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<AudioServicesConfig*>(platformAllocateObject(sizeof(AudioServicesConfig)));
    if (object != nullptr) {
        object->mOutput = nullptr;
        object->mVtable = &sAudioConfigVtable;
        object->mRouting = nullptr;
        object->mBackend = nullptr;
        object->mAllocator = nullptr;
        initializeAudioList(&object->mServices);
        object->mServices.mLinkOffset = 4;
    }
    return object;
}

/* 02761C74 */
void AudioServicesConfig::setBackend(AudioService* backend) { mBackend = backend; }
/* 02761C7C */
void AudioServicesConfig::setOutput(AudioService* output) { mOutput = output; }
/* 02761C84 */
void AudioServicesConfig::setRouting(AudioService* routing) { mRouting = routing; }

/* 02761C8C */
void AudioServicesConfig::addService(AudioService* service) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 offset = mServices.mLinkOffset;
    mServices.insertBefore(audioServiceLink(service, offset));
    mServices.mCount = static_cast<u32>(mServices.mCount) + 1;
}

/* 02761CD0 */
u32 AudioServicesConfig::isKindOf(RuntimeType* type) {
    initializeRuntimeType(sAudioConfigType, sAudioConfigTypeGuard, &sAudioConfigTypeTable);
    if (type == &sAudioConfigType) return 1;
    initializeRuntimeType(sAudioConfigBaseType, sAudioConfigBaseTypeGuard, &sAudioConfigBaseTypeTable);
    return type == &sAudioConfigBaseType;
}

static void platformStopSoundSlot(SoundHandleSlot* slot, s32 fadeFrames);
static u32 platformHasSoundHandle(SoundHandleSlot* slot);
static u32 platformGetSoundId(SoundHandleSlot* slot);
static void platformReleaseSoundHandle(SoundHandleSlot* slot);
static void platformDestroySoundHandleSlot(SoundHandleSlot* slot, u32 flags);
static SoundHandleSlot* platformConstructHandleArray(void* storage, u32 count, u32 stride, u32 construct, u32 flags);
static void platformDestroyHandleArray(SoundHandleSlot* storage, s32 count, u32 stride, u32 destroy, u32 release, u32 flags);
static DisposerVtable sSoundHandleBankVtable;
static u32 sArrayHeaderSize;

/* 02761D48 */
SoundHandleBank* SoundHandleBank::construct(SoundHandleBank* object, u32 count, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<SoundHandleBank*>(platformAllocateObject(sizeof(SoundHandleBank)));
    if (object != nullptr) {
        platformConstructDisposer(object);
        object->mCount = 0;
        object->mVtable = &sSoundHandleBankVtable;
        object->mStopOnDestroy = 1;
        object->mSlots = nullptr;
        u32 size = count * sizeof(SoundHandleSlot) + sArrayHeaderSize;
        u8* storage = static_cast<u8*>(platformAllocateArray(size, heap, 4));
        if (storage != nullptr) {
            void* elements = storage + sArrayHeaderSize;
            SoundHandleSlot* slots = platformConstructHandleArray(elements, count, sizeof(SoundHandleSlot),
                gabi::guest_fn(reinterpret_cast<const void*>(SoundHandleSlot::construct)), 0);
            object->mSlots = slots;
            object->mCount = count;
        } else {
            object->mCount = count;
            object->mSlots = nullptr;
        }
    }
    return object;
}

/* 02761E2C */
void SoundHandleBank::stopAll(s32 fadeFrames) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 index = 0; index < mCount; ++index) {
        SoundHandleSlot* slots = mSlots;
        platformStopSoundSlot(&slots[index], fadeFrames);
    }
}

/* 02761EA8 */
void SoundHandleBank::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    SoundHandleSlot* slots = mSlots;
    mVtable = &sSoundHandleBankVtable;
    if (slots != nullptr) {
        if (mStopOnDestroy != 0) {
            stopAll(0);
            slots = mSlots;
        }
        platformDestroyHandleArray(slots, -1, sizeof(SoundHandleSlot),
            gabi::guest_fn(reinterpret_cast<const void*>(platformDestroySoundHandleSlot)), 1, 0);
        mSlots = nullptr;
    }
    RegisteredObject::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

/* 02761F54 */
SoundHandleSlot* SoundHandleBank::findFree() {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 index = 0; index < mCount; ++index) {
        SoundHandleSlot* slots = mSlots;
        if (platformHasSoundHandle(&slots[index]) == 0) {
            slots = mSlots;
            return &slots[index];
        }
    }
    return nullptr;
}

/* 02761FF0 */
SoundHandleSlot* SoundHandleBank::find(u32 soundId, u32 stop) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 index = 0; index < mCount; ++index) {
        SoundHandleSlot* slots = mSlots;
        if (platformHasSoundHandle(&slots[index]) != 0) {
            slots = mSlots;
            if (platformGetSoundId(&slots[index]) == soundId) {
                if (stop != 0) {
                    slots = mSlots;
                    platformStopSoundSlot(&slots[index], 0);
                }
                slots = mSlots;
                return &slots[index];
            }
        }
    }
    return nullptr;
}

/* 027620A8 */
SoundHandleSlot* SoundHandleBank::findOrFree(u32 soundId, u32 stop) {
    gabi::AutoFrame frame{gabi::current_function()};
    SoundHandleSlot* slot = find(soundId, stop);
    if (slot == nullptr) slot = findFree();
    return slot;
}

/* 027620E4 */
SoundHandleSlot* SoundHandleSlot::construct(SoundHandleSlot* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<SoundHandleSlot*>(platformAllocateObject(sizeof(SoundHandleSlot)));
    if (object != nullptr) object->mSound = nullptr;
    return object;
}

/* 02762120 */
void SoundHandleSlot::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    platformReleaseSoundHandle(this);
    if (flags & 1) platformReleaseObject(this);
}

static void releaseInlineText(void* object, u32 flags) {
    if (object != nullptr && (flags & 1)) platformReleaseObject(object);
}

/* 027611A4 */
void AudioTextEntryPoints::destroyAudioTaskLabel(SafeText* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 027611B8 */
void AudioTextEntryPoints::assureAudioTaskLabel(SafeText* text) {}
/* 02761A38 */
void AudioTextEntryPoints::destroyAudioServicesLabel(SafeText* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 02761A4C */
void AudioTextEntryPoints::assureAudioServicesLabel(SafeText* text) {}
/* 02761A8C */
void AudioLifecycleHooks::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(this, flags);
}
/* 02761AA0 */
void AudioLifecycleHooks::initialize() {}
/* 02761AA4 */
void AudioLifecycleHooks::shutdown() {}
/* 02761AA8 */
void AudioLifecycleHooks::update() {}
/* 02761BC4 */
void AudioServicesQuery::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(this, flags);
}

/* WWHD glue
data sVolumeRampVtable 1015EA88
data sAudioTask 101F8BA4
data sAudioServicesDisposer 101F8BAC
data sAudioTaskVtable 1015E7FC
data sAudioTaskDisposerVtable 1015E8B4
data sAudioTaskLabelVtable 1015E798
data sAudioTaskEmptyLabel 1015E794
data sAudioServicesVtable 1015E93C
data sAudioServicesDisposerVtable 1015E92C
data sAudioQueryVtable 1015E9D4
data sAudioConfigVtable 1015EA3C
data sAudioTaskTypeTable 1015E7D0
data sCalcTaskTypeTable 1015E7C0
data sTaskBaseTypeTable 1015E7B0
data sAudioConfigInitTypeTable 1015E7E0
data sAudioConfigTypeTable 1015EA54
data sAudioConfigBaseTypeTable 1015EA2C
data sAudioServiceTypeTable 1015E954
data sAudioTaskType 101FD9D4
data sCalcTaskType 101FD97C
data sTaskBaseType 101FD980
data sAudioConfigType 101FDFB4
data sAudioConfigBaseType 101FD994
data sAudioServiceType 101FD954
data sAudioTaskTypeGuard 101FD59C
data sCalcTaskTypeGuard 101FD598
data sTaskBaseTypeGuard 101FD594
data sAudioConfigTypeGuard 101FD2E4
data sAudioConfigBaseTypeGuard 101FD2E0
data sAudioServiceTypeGuard 101FD2D8
data sSoundHandleBankVtable 1015EA64
data sArrayHeaderSize 101FCD08
str 1015E7F0
ext 0273AD10 void* platformAllocateObject(u32)
ext 0273B0D4 void* platformAllocateArray(u32,sead::RegistryHeap*,s32)
ext 0273B050 void* platformAllocateAlignedObject(u32,sead::RegistryHeap*,s32)
ext 0273AF40 void platformReleaseObject(void*)
ext 02752A84 sead::RegisteredObject* platformConstructDisposer(sead::RegisteredObject*)
ext 02752B0C sead::RegisteredObject* platformConstructHeapDisposer(sead::RegisteredObject*,sead::RegistryHeap*,u32)
ext 0273B494 sead::Link* sead::LinkList::popFront()
ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
ext 027454E0 sead::CalcTask* platformConstructAudioTaskBase(sead::CalcTask*,void*,const char*)
ext 027452C8 void sead::CalcTask::destroy(u32)
ext 0276359C sead::AudioService* platformConstructAudioOutput(sead::AudioService*)
ext 02763C8C sead::AudioAllocator* platformConstructAudioAllocator(sead::AudioAllocator*)
ext 0276AC1C void platformStopSoundSlot(sead::SoundHandleSlot*,s32)
ext 0276AC2C u32 platformHasSoundHandle(sead::SoundHandleSlot*)
ext 0276AC3C u32 platformGetSoundId(sead::SoundHandleSlot*)
ext 02896CD8 void platformReleaseSoundHandle(sead::SoundHandleSlot*)
ext 02762120 void platformDestroySoundHandleSlot(sead::SoundHandleSlot*,u32) callback
ext 028EFF8C sead::SoundHandleSlot* platformConstructHandleArray(void*,u32,u32,u32,u32)
ext 028F0164 void platformDestroyHandleArray(sead::SoundHandleSlot*,s32,u32,u32,u32,u32)
*/
