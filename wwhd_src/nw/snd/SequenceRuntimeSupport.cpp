/* Sequence runtime track selection and parameter dispatch, reconstructed from WWHD. */
#include "nw/snd/SequenceRuntimeSupport.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;
struct DispatchVtableEntry { be<u32> adjustment,target; };
struct DispatchReceiver { gptr<DispatchVtableEntry> vtable; };
struct SequenceVariables { be<s16> value[16]; };
static SequenceVariables sSequenceVariables;
static SequenceRuntimeVtable sSequenceVtable;
static u32 sDisposalVtable;
static u32 sUpdateVtable;
struct SequenceSkipLimit { be<s32> ticks; };
static SequenceSkipLimit sSkipLimit;
static TrackMethod sMuteMethod;
static TrackMethod sSilenceMethod;
static TrackMethod sVolumeMethod;
static TrackMethod sPitchMethod;
static TrackMethod sLpfOffsetMethod;
static TrackMethod sBiquadFilterMethod;
static TrackMethod sResourceMethod;
static TrackMethod sTransposeMethod;
static TrackMethod sVelocityMethod;
static TrackMethod sOptionMethod;
static TrackMethod sTvVolumeMethod;
static TrackMethod sTvPanMethod;
static TrackMethod sTvSurroundPanMethod;
static TrackMethod sTvMainSendMethod;
static TrackMethod sTvEffectMethod;
static TrackMethod sOutputVolumeMethod;
static TrackMethod sOutputPanMethod;
static TrackMethod sOutputSurroundMethod;
static TrackMethod sOutputMainMethod;
static TrackMethod sOutputEffectMethod;

/* A track slot by an unchecked (possibly negative) index: the 32-bit element address the original computes. */
static inline gptr<TrackRuntimeControl>* trackSlot(gptr<TrackRuntimeControl>* tracks, s32 index) {
    return gabi::at<gptr<TrackRuntimeControl>>(gabi::ea(tracks) + (u32)index * 4u);
}
/* 0288A750 */
TrackRuntimeControl* SequenceRuntimeSupport::getTrack(s32 index) {
    if (index > 15) return nullptr;
    return *trackSlot(tracks, index);
}
/* 0288BEA8 */
void SequenceRuntimeSupport::requestStart() { startRequested = 1; }
/* 0288BF58 */
void SequenceRuntimeSupport::setCallback(u32 function,u32 argument) { callback = function; callbackArgument = argument; }
/* 0288BF64 */
be<s16>* SequenceRuntimeSupport::variable(s32 index) {
    if (index < 16) return gabi::at<be<s16>>(gabi::ea(variables) + (u32)index * 2u);
    if (index >= 32) return nullptr;
    return &sSequenceVariables.value[index - 16];
}
/* 0288C040 */
void SequenceRuntimeSupport::setTempoRatio(f32 value) { tempoRatio = value; }
/* 0288C048 */
void SequenceRuntimeSupport::setSequenceOption(u8 value) { sequenceOption = value; }
/* 0288C050 */
void SequenceRuntimeSupport::setNoteWait(u8 value) { noteWait = value; }
/* 0288C580 */
void SequenceRuntimeSupport::setVariable(u32 index,s16 value) {
    // unchecked index: 32-bit element address as the original computes it
    *gabi::at<be<s16>>(gabi::ea(variables) + index * 2u) = value;
}
/* 0288C590 */
void SequenceRuntimeSupport::setGlobalVariable(u32 index,s16 value) { sSequenceVariables.value[index] = value; }
/* 0288B8F4 */
void SequenceRuntimeSupport::initializeGlobalVariables() {
    for (u32 i = 0; i < 16; ++i) sSequenceVariables.value[i] = -1;
}

/* A GHS compact member-pointer stores a signed receiver adjustment and a virtual-slot index.
 * The target word is a direct function address, or the signed offset of the receiver's vptr. */
static inline u32 resolveTrackMethod(void*& receiver,s16 adjustment,s16 slot,u32 target) {
    receiver = gabi::at<void>(gabi::ea(receiver) + (s32)adjustment);
    if (slot < 0) return target;
    DispatchReceiver* view = gabi::at<DispatchReceiver>(gabi::ea(receiver) + (s16)target);
    DispatchVtableEntry* table = view->vtable;
    return table[slot].target;
}

template<typename... Args>
static inline void dispatchTracks(SequenceRuntimeSupport* sequence,u32 mask,const TrackMethod* method,Args... args) {
    if (!mask) return;
    s16 slot = method->virtualSlot;
    u32 target = method->target;
    s16 adjustment = method->thisAdjustment;
    for (s32 index = 0; index < 16 && mask; ++index,mask >>= 1) {
        if (!(mask & 1)) continue;
        void* receiver = sequence->getTrack(index);
        if (!receiver) continue;
        u32 function = resolveTrackMethod(receiver,adjustment,slot,target);
        gabi::call_ptr<void>(function,receiver,args...);
    }
}
/* 0288C6A4 */
void SequenceRuntimeSupport::dispatchInteger(u32 mask,const TrackMethod* method,u32 value) {
    dispatchTracks(this,mask,method,value);
}
/* 0288C758 */
void SequenceRuntimeSupport::dispatchIntegerPair(u32 mask,const TrackMethod* method,u32 value,u32 extra) {
    dispatchTracks(this,mask,method,value,extra);
}
/* 0288C818 */
void SequenceRuntimeSupport::dispatchFloat(u32 mask,const TrackMethod* method,f32 value) {
    dispatchTracks(this,mask,method,value);
}
/* 0288C8E4 */
void SequenceRuntimeSupport::dispatchFloatInteger(u32 mask,const TrackMethod* method,u32 argument,f32 value) {
    dispatchTracks(this,mask,method,argument,value);
}
/* 0288C9BC */
void SequenceRuntimeSupport::dispatchSigned(u32 mask,const TrackMethod* method,u32 value) {
    dispatchTracks(this,mask,method,value);
}
/* 0288CA70 */
void SequenceRuntimeSupport::dispatchByte(u32 mask,const TrackMethod* method,u32 value) {
    dispatchTracks(this,mask,method,value);
}
/* 0288CB24 */
void SequenceRuntimeSupport::dispatchShort(u32 mask,const TrackMethod* method,u32 value) {
    dispatchTracks(this,mask,method,value);
}
/* 0288CBD8 */
void SequenceRuntimeSupport::dispatchFloatByte(u32 mask,const TrackMethod* method,u32 argument,f32 value) {
    dispatchTracks(this,mask,method,argument,value);
}
/* 0288CCB0 */
void SequenceRuntimeSupport::dispatchOutputFloat(u32 mask,const TrackMethod* method,u32 output,f32 value) {
    dispatchTracks(this,mask,method,output,value);
}
/* 0288CD88 */
void SequenceRuntimeSupport::dispatchOutputEffect(u32 mask,const TrackMethod* method,u32 output,u32 effect,f32 value) {
    dispatchTracks(this,mask,method,output,effect,value);
}
static inline void copyTrackMethod(TrackMethod* destination,const TrackMethod* source) {
    s16 adjustment = source->thisAdjustment;
    s16 slot = source->virtualSlot;
    u32 target = source->target;
    destination->thisAdjustment = adjustment;
    destination->virtualSlot = slot;
    destination->target = target;
}
/* 0288C058 */
void SequenceRuntimeSupport::setTrackMute(u32 mask,u32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sMuteMethod);
    dispatchInteger(mask,method.get(),value);
}
/* 0288C098 */
void SequenceRuntimeSupport::setTrackSilence(u32 mask,u32 value,u32 frames) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sSilenceMethod);
    dispatchIntegerPair(mask,method.get(),value,frames);
}
/* 0288C0DC */
void SequenceRuntimeSupport::setTrackVolume(u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sVolumeMethod);
    dispatchFloat(mask,method.get(),value);
}
/* 0288C114 */
void SequenceRuntimeSupport::setTrackPitch(u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sPitchMethod);
    dispatchFloat(mask,method.get(),value);
}
/* 0288C14C */
void SequenceRuntimeSupport::setTrackLpfOffset(u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sLpfOffsetMethod);
    dispatchFloat(mask,method.get(),value);
}
/* 0288C184 */
void SequenceRuntimeSupport::setTrackBiquadFilter(u32 mask,u32 mode,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sBiquadFilterMethod);
    dispatchFloatInteger(mask,method.get(),mode,value);
}
/* 0288C228 */
void SequenceRuntimeSupport::setTrackTranspose(u32 mask,u32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sTransposeMethod);
    dispatchByte(mask,method.get(),value);
}
/* 0288C264 */
void SequenceRuntimeSupport::setTrackVelocity(u32 mask,u32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sVelocityMethod);
    dispatchShort(mask,method.get(),value);
}
/* 0288C2A0 */
void SequenceRuntimeSupport::setTrackOption(u32 mask,u32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sOptionMethod);
    dispatchSigned(mask,method.get(),value);
}
/* 0288C2E0 */
void SequenceRuntimeSupport::resetTrackOption(u32 mask) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sOptionMethod);
    dispatchSigned(mask,method.get(),0xFFFFFFFF);
}
/* 0288C31C */
void SequenceRuntimeSupport::setTrackTvVolume(u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sTvVolumeMethod);
    dispatchFloat(mask,method.get(),value);
}
/* 0288C354 */
void SequenceRuntimeSupport::setTrackTvPan(u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sTvPanMethod);
    dispatchFloat(mask,method.get(),value);
}
/* 0288C38C */
void SequenceRuntimeSupport::setTrackTvSurroundPan(u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sTvSurroundPanMethod);
    dispatchFloat(mask,method.get(),value);
}
/* 0288C3C4 */
void SequenceRuntimeSupport::setTrackTvMainSend(u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sTvMainSendMethod);
    dispatchFloat(mask,method.get(),value);
}
/* 0288C3FC */
void SequenceRuntimeSupport::setTrackTvEffect(u32 mask,u32 effect,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sTvEffectMethod);
    dispatchFloatByte(mask,method.get(),effect,value);
}
/* 0288C438 */
void SequenceRuntimeSupport::setTrackOutputVolume(u32 output,u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sOutputVolumeMethod);
    dispatchOutputFloat(mask,method.get(),output,value);
}
/* 0288C478 */
void SequenceRuntimeSupport::setTrackOutputPan(u32 output,u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sOutputPanMethod);
    dispatchOutputFloat(mask,method.get(),output,value);
}
/* 0288C4B8 */
void SequenceRuntimeSupport::setTrackOutputSurroundPan(u32 output,u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sOutputSurroundMethod);
    dispatchOutputFloat(mask,method.get(),output,value);
}
/* 0288C4F8 */
void SequenceRuntimeSupport::setTrackOutputMainSend(u32 output,u32 mask,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sOutputMainMethod);
    dispatchOutputFloat(mask,method.get(),output,value);
}
/* 0288C538 */
void SequenceRuntimeSupport::setTrackOutputEffect(u32 output,u32 mask,u32 effect,f32 value) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sOutputEffectMethod);
    dispatchOutputEffect(mask,method.get(),output,effect,value);
}
/* 0288C1C0 */
u32 SequenceRuntimeSupport::setResourceTrackOption(u32 mask,u32 resource) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!banks[resource].attached) return 0;
    gabi::FrameLocal<TrackMethod> method(8);
    copyTrackMethod(method.get(),&sResourceMethod);
    dispatchSigned(mask,method.get(),resource);
    return 1;
}
/* 0288A770 */
void SequenceRuntimeSupport::releaseTrack(s32 index) {
    TrackRuntimeControl* track = getTrack(index);
    if (!track) return;
    runtimeStopTrack(track);
    RuntimeTrackAllocator* owner = trackAllocator;
    u32 release = owner->vtable->release;
    TrackRuntimeControl* allocated = *trackSlot(tracks, index);
    gabi::call_ptr<void>(release,owner,allocated);
    *trackSlot(tracks, index) = nullptr;
}
/* 0288A7E4 */
void SequenceRuntimeSupport::resetTracks() {
    if (linked) {
        void* manager = runtimeSequenceUpdateManager();
        runtimeRemoveSequenceUpdate(manager,&updateLink);
        linked = 0;
    }
    if (startRequested) startRequested = 0;
    for (s32 i = 0; i < 16; ++i) releaseTrack(i);
}
/* 0288A8AC */
void SequenceRuntimeSupport::returnLoadTask() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    SequenceLoadTask* task = loadTask;
    if (!task) return;
    SequenceTaskPool* pool = taskPool;
    gabi::FrameLocal<gptr<SoundListNode>> cursor(8);
    if (task->isPending()) {
        *cursor = &pool->finished.sentinel;
        runtimeListInsert(&pool->finished,cursor.get(),&task->listNode);
        loadTask = nullptr;
    } else {
        task->releaseHeapCallback();
        *cursor = &pool->waiting.sentinel;
        runtimeListInsert(&pool->waiting,cursor.get(),&task->listNode);
        loadTask = nullptr;
    }
}
/* 0288AB44 */
u32 SequenceRuntimeSupport::acquireLoadTask() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    SequenceTaskPool* pool = taskPool;
    if (!pool || !pool->waiting.count) return 0;
    void* node = pool->waiting.sentinel.next;
    SequenceLoadTask* task = gabi::at<SequenceLoadTask>(gabi::ea(node) - 0x9C);
    gabi::FrameLocal<gptr<void>> cursor(8);
    *cursor = node;
    runtimeListEraseFront(&pool->waiting,cursor.get());
    if (!task) return 0;
    loadTask = task;
    preparationState = 2;
    return 1;
}
/* 0288AE6C */
void SequenceRuntimeSupport::advance(u32 mode,s32 amount) {
    if (!active) return;
    if (mode == 0) pendingTicks = (u32)pendingTicks + (u32)amount;
    else if (mode == 1) pendingTime = gabi::fadds_ppc(pendingTime,(f32)amount);
}
/* 0288AEDC */
u32 SequenceRuntimeSupport::updateTracks(u32 step) {
    s16 frames = waitFrames;
    s16 counter = waitCounter;
    if (counter < frames) waitCounter = counter + 1;
    u32 anyActive = 0;
    for (s32 i = 0; i < 16; ++i) {
        TrackRuntimeControl* track = getTrack(i);
        if (!track) continue;
        runtimeTrackTick(track);
        if (runtimeTrackStep(track,step) < 0) releaseTrack(i);
        if (track->activeFlag) anyActive = 1;
    }
    return anyActive ^ 1;
}
/* 0288B3C4 */
void SequenceRuntimeSupport::updateTrackParameters() {
    for (s32 i = 0; i < 16; ++i) {
        TrackRuntimeControl* track = getTrack(i);
        if (track) runtimeTrackUpdate(track);
    }
}
/* 0288BBA8 */
void SequenceRuntimeSupport::attachTrack(s32 index,TrackRuntimeControl* track) {
    if (index > 15) return;
    *trackSlot(tracks, index) = track;
    runtimeTrackReset(track, index); // the original passes the slot index on (r4)
    track->routeMode = (u8)routeMode;
}
/* 0288BEB8 */
void SequenceRuntimeSupport::setPaused(u32 value) {
    paused = value;
    for (s32 i = 0; i < 16; ++i) {
        TrackRuntimeControl* track = getTrack(i);
        if (track) runtimeTrackPause(track,value);
    }
}
/* 0288A9F0 */
void SequenceRuntimeSupport::destroy(u32 flags) {
    if (!gabi::ea(this)) return;
    vtable = &sSequenceVtable;
    updateVtable = gabi::ea(&sUpdateVtable);
    disposalVtable = gabi::ea(&sDisposalVtable);
    finish();
    runtimeDestroyEvent(this,2);
    if (flags & 1) runtimeDelete(this);
}
/* 0288A950 */
void SequenceRuntimeSupport::finish() {
    finished = 1;
    resetTracks();
    if (active) {
        void* manager = runtimeDisposalManager();
        runtimeRemoveDisposal(manager,&disposalLink);
        active = 0;
    }
    for (u32 i = 0; i < 4; ++i) {
        banks[i].detach();
        waveArchives[i].detach();
    }
    if (ownsResources) {
        runtimeSequenceReleaseResources(this);
        ownsResources = 0;
    }
    returnLoadTask();
}
/* 0288AA70 */
void SequenceRuntimeSupport::disposeRange(u32 begin,u32 end) {
    if (!active) return;
    for (s32 i = 0; i < 16; ++i) {
        TrackRuntimeControl* track = getTrack(i);
        if (!track) continue;
        u32 data = gabi::ea(track->sequenceData.get());
        if (begin <= data && data <= end) {
            gabi::call_ptr<void>(vtable->stop,this);
            break;
        }
    }
    for (u32 i = 0; i < 4; ++i) {
        u32 data = gabi::ea(banks[i].file.get());
        if (begin <= data && data <= end) banks[i].detach();
    }
}
/* 0288AB3C */
static void sequenceDisposeRangeCallback(void* link,u32 begin,u32 end) {
    SequenceRuntimeSupport* sequence = gabi::at<SequenceRuntimeSupport>(gabi::ea(link) - 0x178);
    sequence->disposeRange(begin,end);
}
/* 0288BEB4 */
void SequenceRuntimeSupport::resetTracksCallback() { resetTracks(); }
/* 0288BF34 */
TrackRuntimeControl* SequenceRuntimeSupport::allocateTrack(u32 index,u32 priority) {
    struct AllocationVtable { u8 reserved[0x14]; be<u32> allocate; };
    struct AllocationInterface { gptr<AllocationVtable> vtable; };
    AllocationInterface* owner = (AllocationInterface*)allocator.get();
    return gabi::tail_ptr<TrackRuntimeControl*>(owner->vtable->allocate,owner,this,index,priority);
}
/* 0288BF9C */
void SequenceRuntimeSupport::invokeCallback(u32 context,TrackRuntimeControl* track) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!callback) return;
    struct CallbackState {
        gptr<be<s16>> localVariables,globalVariables;
        be<u32> value;
        be<u8> mode;
        u8 padding[3];
    };
    gabi::FrameLocal<CallbackState> state(8);
    state->localVariables = variable(0);
    state->globalVariables = variable(16);
    u32 value = runtimeTrackUserValue(track,0);
    u32 function = callback;
    u32 argument = callbackArgument;
    state->value = value;
    state->mode = (u8)track->ready;
    gabi::call_ptr<void>(function,context,state.get(),argument);
    track->ready = (u8)state->mode;
}
/* 0288C5A4 */
void SequenceRuntimeSupport::update() { tick(); }
/* 0288C5A8 */
void SequenceRuntimeSupport::callUpdate() { gabi::tail_ptr<void>(vtable->update,this); }
/* 0288B5F0 */
static void sequenceUpdateCallback(void* link) {
    SequenceRuntimeSupport* sequence = gabi::at<SequenceRuntimeSupport>(gabi::ea(link) - 0x184);
    sequence->update();
}
/* 0288B5F8 */
static void sequenceVirtualUpdateCallback(void* link) {
    SequenceRuntimeSupport* sequence = gabi::at<SequenceRuntimeSupport>(gabi::ea(link) - 0x184);
    sequence->callUpdate();
}
/* 0288BACC */
void SequenceRuntimeSupport::initialize() {
    runtimeSequenceBaseInitialize(this);
    active = 0;
    callbackArgument = 0;
    timeBase = 48;
    allocator = nullptr;
    sequenceOption = 64;
    modulationStart = 127;
    callback = 0;
    waitFrames = 0;
    startRequested = 0;
    pendingTime = 0.0f;
    tickFraction = 0.0f;
    tempo = 120;
    pendingTicks = 0;
    noteWait = 0;
    paused = 0;
    panRange = 1.0f;
    processedTicks = 0;
    modulationTarget = 127;
    waitCounter = 0;
    tempoRatio = 1.0f;
    for (u32 i = 0; i < 16; ++i) variables[i] = -1;
    for (u32 i = 0; i < 16; ++i) tracks[i] = nullptr;
    linked = 0;
    ownsResources = 1;
}
struct SequenceModulation {
    be<u8> origin,target;
    be<u16> frames,counter;
};
static inline void constructSequenceModulation(SequenceModulation* value) {
    if (!gabi::ea(value)) value = (SequenceModulation*)runtimeAllocate(sizeof(SequenceModulation));
    if (!gabi::ea(value)) return;
    value->target = 0;
    value->frames = 0;
    value->origin = 0;
    value->counter = 0;
}
/* 0288B914 */
SequenceRuntimeSupport* SequenceRuntimeSupport::create(SequenceRuntimeSupport* sequence) {
    if (!sequence) {
        sequence = (SequenceRuntimeSupport*)runtimeAllocate(sizeof(SequenceRuntimeSupport));
        if (!sequence) return sequence;
    }
    runtimeSequenceBaseConstruct(sequence);
    sequence->tempoRatio = 1.0f;
    sequence->tickFraction = 0.0f;
    sequence->disposalVtable = gabi::ea(&sDisposalVtable);
    sequence->timeBase = 48;
    sequence->tempo = 120;
    reinterpret_cast<be<u32>*>(sequence->updateLink)[0] = 0;
    sequence->vtable = &sSequenceVtable;
    reinterpret_cast<be<u32>*>(sequence->disposalLink)[0] = 0;
    sequence->panRange = 1.0f;
    sequence->pendingTime = 0.0f;
    sequence->sequenceOption = 64;
    sequence->updateVtable = gabi::ea(&sUpdateVtable);
    reinterpret_cast<be<u32>*>(sequence->updateLink)[1] = 0;
    reinterpret_cast<be<u32>*>(sequence->disposalLink)[1] = 0;
    sequence->noteWait = 0;
    constructSequenceModulation(reinterpret_cast<SequenceModulation*>(&sequence->modulationStart));
    sequence->processedTicks = 0;
    sequence->callback = 0;
    sequence->allocator = nullptr;
    sequence->waitCounter = 0;
    sequence->waitFrames = 0;
    sequence->modulationStart = 127;
    sequence->modulationTarget = 127;
    sequence->callbackArgument = 0;
    __construct_array(sequence->waveArchives,4,sizeof(WaveArchiveReader),(void* (*)(void*))WaveArchiveReader::constructEmpty);
    __construct_array(sequence->banks,4,sizeof(BankReader),(void* (*)(void*))BankReader::construct);
    sequence->preparedParameters[2] = 0;
    sequence->taskPool = nullptr;
    sequence->preparedParameters[1] = 0;
    sequence->preparedParameters[4] = 0;
    sequence->linked = 0;
    sequence->preparedParameters[0] = 0;
    sequence->preparedParameters[3] = -1;
    sequence->ownsResources = 0;
    sequence->loadTask = nullptr;
    __construct_array(&sequence->preparedParameters[5],4,8,(void* (*)(void*))SoundResourceChoice::create);
    for (u32 i = 0; i < 16; ++i) sequence->variables[i] = -1;
    for (u32 i = 0; i < 16; ++i) sequence->tracks[i] = nullptr;
    return sequence;
}
static inline f32 sequenceTickRate(const SequenceRuntimeSupport* sequence) {
    u32 tempo = sequence->tempo;
    u32 timeBase = sequence->timeBase;
    f32 rate = (f32)(s32)(timeBase * tempo);
    rate = gabi::fmuls_ppc(rate,sequence->tempoRatio);
    return gabi::fdivs_ppc(rate,60000.0f);
}
/* 0288AF88 */
void SequenceRuntimeSupport::skipPending() {
    for (s32 i = 0; i < 16; ++i) {
        TrackRuntimeControl* track = getTrack(i);
        if (!track) continue;
        runtimeTrackStopNotes(track,127);
        runtimeTrackReleaseNotes(track);
    }
    s32 processed = 0;
    while (true) {
        u32 ticks = pendingTicks;
        if (!ticks) {
            f32 rate = sequenceTickRate(this);
            f32 time = pendingTime;
            if (gabi::fmuls_ppc(time,rate) < 1.0f) {
                pendingTime = 0.0f;
                return;
            }
        }
        if (processed >= sSkipLimit.ticks) return;
        if (ticks) pendingTicks = ticks - 1;
        else {
            f32 tickTime = gabi::fdivs_ppc(1.0f,sequenceTickRate(this));
            pendingTime = gabi::fsubs_ppc(pendingTime,tickTime);
        }
        if (updateTracks(0)) {
            resetTracks();
            finished = 1;
            return;
        }
        processedTicks = (u32)processedTicks + 1;
        ++processed;
    }
}
/* 0288B198 */
void SequenceRuntimeSupport::advanceTime() {
    f32 rate = sequenceTickRate(this);
    if (rate == 0.0f) return;
    f32 scaled = gabi::fmuls_ppc(65536.0f,tickFraction);
    auto initial = runtimeTimeToTicks(gabi::fdivs_ppc(scaled,rate));
    u64 elapsed = (u64(initial.r3) << 32) | initial.r4;
    u64 remaining = 3u << 16;
    while (elapsed < remaining) {
        remaining -= elapsed;
        if (updateTracks(1)) {
            resetTracks();
            finished = 1;
            return;
        }
        rate = sequenceTickRate(this);
        processedTicks = (u32)processedTicks + 1;
        if (rate == 0.0f) return;
        auto next = runtimeTimeToTicks(gabi::fdivs_ppc(65536.0f,rate));
        elapsed = (u64(next.r3) << 32) | next.r4;
    }
    u64 leftover = elapsed - remaining;
    f64 time = runtimeTicksToTime((u32)(leftover >> 32),(u32)leftover);
    f32 fraction = (f32)gabi::fmul_ppc(time,(f64)rate);
    tickFraction = gabi::fmuls_ppc(fraction,1.52587890625e-05f);
}
static inline void startPreparedSequence(SequenceRuntimeSupport* sequence,const SequencePreparation* info,bool linkUpdates,RuntimeSequenceReader* reader) {
    if (sequence->active) sequence->resetTracks();
    TrackRuntimeControl* track = sequence->getTrack(0);
    if (!track) {
        gabi::call_ptr<void>(sequence->vtable->stop,sequence);
        return;
    }
    runtimeOpenSequenceReader(reader,info->sequence);
    u32 data = runtimeSequenceData(reader);
    u32 offset = info->offset;
    runtimeTrackSetSequence(track,data,offset);
    runtimeTrackStart(track);
    for (u32 i = 0; i < 4; ++i) {
        sequence->banks[i].attach(gabi::at<const BankFileHeader>(info->banks[i]));
        u32 individual = info->individualWaveLoads[i];
        u32 image = info->waveArchiveImages[i];
        sequence->waveArchives[i].attach(gabi::at<const WaveArchiveHeader>(image),individual);
    }
    sequence->preparationState = 3;
    sequence->active = 1;
    void* manager = runtimeDisposalManager();
    runtimeAddDisposal(manager,&sequence->disposalLink);
    if (linkUpdates) {
        manager = runtimeSequenceUpdateManager();
        runtimeAddSequenceUpdate(manager,&sequence->updateLink);
    }
    sequence->linked = 1;
}
/* 0288AD60 */
void SequenceRuntimeSupport::startPrepared(const SequencePreparation* info) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<RuntimeSequenceReader> reader(8);
    startPreparedSequence(this,info,false,reader.get());
}
/* 0288BD14 */
void SequenceRuntimeSupport::startPreparedAndLink(const SequencePreparation* info) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<RuntimeSequenceReader> reader(8);
    startPreparedSequence(this,info,true,reader.get());
}
/* 0288BE2C */
void SequenceRuntimeSupport::prepareAsync(const be<u32>* header,const be<u32>* parameters) {
    preparedHeader[0] = (u32)header[0];
    preparedHeader[1] = (u32)header[1];
    preparedHeader[2] = (u32)header[2];
    for (u32 i = 0; i < 13; ++i) preparedParameters[i] = (u32)parameters[i];
    preparationState = 1;
    void* manager = runtimeSequenceUpdateManager();
    runtimeAddSequenceUpdate(manager,&updateLink);
    linked = 1;
}
/* 0288B42C */
void SequenceRuntimeSupport::tick() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 preparation = preparationState;
    if (preparation == 1 || preparation == 2) {
        if (preparation == 1) {
            if (!acquireLoadTask()) return;
            loadTask->initialize(preparedParameters);
        }
        if (!loadTask->request()) return;
        SequenceLoadTask* task = loadTask;
        if (!task->loadSucceeded) {
            finished = 1;
            resetTracks();
            return;
        }
        gabi::FrameLocal<SequencePreparation> info(8);
        info->sequence = 0;
        info->offset = 0;
        for (u32 i = 0; i < 4; ++i) {
            info->banks[i] = 0;
            info->waveArchiveImages[i] = 0;
            info->individualWaveLoads[i] = 0;
        }
        info->sequence = (u32)task->sequence;
        info->offset = (u32)preparedHeader[0];
        for (u32 i = 0; i < 4; ++i) {
            info->banks[i] = (u32)task->banks[i];
            info->waveArchiveImages[i] = (u32)task->waveArchiveImages[i];
            info->individualWaveLoads[i] = (u8)task->individualWaveLoads[i];
        }
        startPrepared(info.get());
        u32 amount = preparedHeader[2];
        u32 mode = preparedHeader[1];
        advance(mode,amount);
    }
    if (!active || !startRequested) return;
    if (pendingTicks || pendingTime > 0.0f) {
        skipPending();
        updateTrackParameters();
        return;
    }
    if (!paused) advanceTime();
    updateTrackParameters();
}
/* 0288BC00 */
void SequenceRuntimeSupport::configureTracks(const SequenceTrackSetup* setup) {
    allocator = setup->allocator.get();
    u32 remaining = setup->mask;
    s32 requested = 0;
    while (remaining) {
        requested += remaining & 1;
        remaining >>= 1;
    }
    TrackProvider* provider = setup->provider;
    s32 capacity = gabi::call_ptr<s32>(provider->vtable->capacity,provider);
    if (requested > capacity) {
        gabi::call_ptr<void>(vtable->stop,this);
        return;
    }
    u32 mask = setup->mask;
    for (s32 index = 0; mask; ++index,mask >>= 1) {
        if (!(mask & 1)) continue;
        provider = setup->provider;
        TrackRuntimeControl* track = gabi::call_ptr<TrackRuntimeControl*>(provider->vtable->allocate,provider,this);
        attachTrack(index,track);
    }
    trackAllocator = (RuntimeTrackAllocator*)setup->provider.get();
}
/* WWHD glue
ext 028AE1F4 void runtimeOpenSequenceReader(nw::snd::RuntimeSequenceReader*,u32)
ext 028AE2B0 u32 runtimeSequenceData(nw::snd::RuntimeSequenceReader*)
ext 0288DD54 void runtimeTrackSetSequence(nw::snd::TrackRuntimeControl*,u32,u32)
ext 0288DD64 void runtimeTrackStart(nw::snd::TrackRuntimeControl*)
ext 028A3258 void nw::snd::BankReader::attach(const nw::snd::BankFileHeader*)
ext 0289FA7C void nw::snd::WaveArchiveReader::attach(const nw::snd::WaveArchiveHeader*,u32)
ext 028A5BD0 void runtimeAddDisposal(void*,void*)
ext 02898E74 void runtimeAddSequenceUpdate(void*,void*)
data sSkipLimit 101FC8D4
ext 0288DBE8 void runtimeTrackStopNotes(nw::snd::TrackRuntimeControl*,u32)
ext 0288DC60 void runtimeTrackReleaseNotes(nw::snd::TrackRuntimeControl*)
ext 028F5678 Pair32 runtimeTimeToTicks(f64)
ext 028F543C f64 runtimeTicksToTime(u32,u32)
ext 028A3610 void runtimeSequenceBaseConstruct(nw::snd::SequenceRuntimeSupport*)
ext 028A36C8 void runtimeSequenceBaseInitialize(nw::snd::SequenceRuntimeSupport*)
ext 0289FB9C nw::snd::WaveArchiveReader* nw::snd::WaveArchiveReader::constructEmpty(nw::snd::WaveArchiveReader*) callback
ext 028A31DC nw::snd::BankReader* nw::snd::BankReader::construct(nw::snd::BankReader*) callback
ext 028A5B00 void* runtimeDisposalManager()
ext 028A5C00 void runtimeRemoveDisposal(void*,void*)
ext 028A3330 void nw::snd::BankReader::detach()
ext 0289FBE4 void nw::snd::WaveArchiveReader::detach()
ext 028A370C void runtimeSequenceReleaseResources(nw::snd::SequenceRuntimeSupport*)
ext 028F6A10 void runtimeDestroyEvent(void*,u32)
ext 0288E754 u32 runtimeTrackUserValue(nw::snd::TrackRuntimeControl*,u32)
ext 0288DCB8 void runtimeStopTrack(nw::snd::TrackRuntimeControl*)
ext 028986B0 void* runtimeSequenceUpdateManager()
ext 02898EA8 void runtimeRemoveSequenceUpdate(void*,void*)
ext 0286DB18 void runtimeListEraseFront(nw::snd::MixEffectChain*,const gptr<void>*)
ext 0288DF48 void runtimeTrackTick(nw::snd::TrackRuntimeControl*)
ext 0288DD80 s32 runtimeTrackStep(nw::snd::TrackRuntimeControl*,u32)
ext 0288E54C void runtimeTrackPause(nw::snd::TrackRuntimeControl*,u32)
ext 0288E774 void runtimeTrackReset(nw::snd::TrackRuntimeControl*,u32)
ext 0288D2E8 void runtimeTrackUpdate(nw::snd::TrackRuntimeControl*)
data sSequenceVtable 10181FC8
data sDisposalVtable 10182020
data sUpdateVtable 10182038
data sSequenceVariables 104B71EC
data sMuteMethod 10181EF0
data sSilenceMethod 10181EF8
data sVolumeMethod 10181F00
data sPitchMethod 10181F08
data sLpfOffsetMethod 10181F10
data sBiquadFilterMethod 10181F18
data sResourceMethod 10181F20
data sTransposeMethod 10181F28
data sVelocityMethod 10181F30
data sOptionMethod 10181F38
data sTvVolumeMethod 10181F40
data sTvPanMethod 10181F48
data sTvSurroundPanMethod 10181F50
data sTvMainSendMethod 10181F58
data sTvEffectMethod 10181F60
data sOutputVolumeMethod 10181F68
data sOutputPanMethod 10181F70
data sOutputSurroundMethod 10181F78
data sOutputMainMethod 10181F80
data sOutputEffectMethod 10181F88
*/
