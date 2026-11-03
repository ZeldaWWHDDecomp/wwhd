/* Sound runtime buffers and memory-stream helpers reconstructed from WWHD's own code. */
#include "nw/snd/SoundRuntimeSupport.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;
static u32 sMemoryStreamVtable;
static TrackRuntimeVtable_l sTrackVtable;

/* 0288A094 */
SoundWorkRecord* SoundWorkRecord::create(SoundWorkRecord* record) {
    if (!record) {
        record = (SoundWorkRecord*)runtimeAllocate(sizeof(SoundWorkRecord));
        if (!record) return record;
    }
    record->current = 0;
    record->end = 0;
    record->flags = 0;
    record->generation = 0;
    record->readOffset = 0;
    record->writeOffset = 0;
    record->readCount = 0;
    record->writeCount = 0;
    return record;
}
/* 0288A0EC */
void* constructAuxMove(void* storage) {
    MoveValue* ramp = static_cast<MoveValue*>(storage);
    if (!ramp) {
        ramp = (MoveValue*)runtimeAllocate(sizeof(MoveValue));
        if (!ramp) return ramp;
    }
    ramp->frames = 0;
    ramp->target = 0.0f;
    ramp->counter = 0;
    ramp->origin = 0.0f;
    return ramp;
}
/* 0288A184 */
u32 SoundBufferPool::addBuffer(void* storage,u32 bytes,u32 stride) {
    u32 start = (gabi::ea(storage) + 3) & ~3u;
    stride = (stride + 3) & ~3u;
    u32 count = (bytes - (start - gabi::ea(storage))) / stride;
    SoundPoolLink* link = gabi::at<SoundPoolLink>(start);
    for (u32 i = 0; i < count; ++i) {
        link->next = freeHead.get();
        SoundPoolLink* added = link;
        link = gabi::at<SoundPoolLink>(gabi::ea(link) + stride);
        freeHead = added;
    }
    buffer = storage;
    length = bytes;
    return count;
}
/* 0288A224 */
u32 SoundBufferPool::countFree() {
    u32 count = 0;
    SoundPoolLink* link = freeHead;
    while (link) {
        link = link->next;
        ++count;
    }
    return count;
}
/* 0288A248 */
SoundPoolLink* SoundBufferPool::allocate() {
    SoundPoolLink* link = freeHead;
    if (!link) return nullptr;
    freeHead = link->next.get();
    return link;
}
/* 0288A26C */
void SoundBufferPool::release(SoundPoolLink* link) {
    link->next = freeHead.get();
    freeHead = link;
}
/* 0288A57C */
SoundMemoryStream* SoundMemoryStream::create(SoundMemoryStream* stream,const void* buffer,u32 length) {
    if (!stream) {
        stream = (SoundMemoryStream*)runtimeAllocate(sizeof(SoundMemoryStream));
        if (!stream) return stream;
    }
    stream->length = length;
    stream->buffer = (const u8*)buffer;
    stream->position = 0;
    stream->vtable = gabi::ea(&sMemoryStreamVtable);
    stream->pendingRead = 0;
    stream->pendingWrite = 0;
    return stream;
}
/* 0288A650 */
u32 SoundMemoryStream::isBusy() { return 0; }
/* 0288A658 */
void SoundMemoryStream::reset() { buffer = nullptr; length = 0; position = 0; }
/* 0288A66C */
u32 SoundMemoryStream::read(void* output,u32 bytes) {
    u32 available = (u32)length - (u32)position;
    u32 offset = position;
    const u8* source = buffer;
    if (available < bytes) bytes = available;
    runtimeCopy(output,gabi::at<const u8>(gabi::ea(source) + offset),bytes);
    return bytes;
}
/* 0288A6C0 */
u32 SoundMemoryStream::seek(u32 offset,u32 origin) {
    switch (origin) {
    case 0: position = offset; return 1;
    case 1: position = (u32)position + offset; return 1;
    case 2: position = (u32)length - offset; return 1;
    default: return 0;
    }
}
/* 0288A710 */
void SoundMemoryStream::close() {}
/* 0288A714 */
void SoundMemoryStream::cancel() {}
/* 0288A718 */
u32 SoundMemoryStream::canRead() { return 0; }
/* 0288A720 */
u32 SoundMemoryStream::canWrite() { return 0; }
/* 0288A728 */
u32 SoundMemoryStream::alignment() { return 16; }
/* 0288A730 */
u32 SoundMemoryStream::canSeek() { return 1; }
/* 0288A738 */
u32 SoundMemoryStream::canCancel() { return 1; }
/* 0288A740 */
u32 SoundMemoryStream::tell() { return position; }
/* 0288A748 */
u32 SoundMemoryStream::size() { return length; }
/* 0288A13C */
void* constructMixChain(void* storage) {
    MixEffectChain* list = static_cast<MixEffectChain*>(storage);
    if (!list) {
        list = (MixEffectChain*)runtimeAllocate(sizeof(MixEffectChain));
        if (!list) return list;
    }
    list->count = 0;
    SoundListNode* sentinel = &list->sentinel;
    list->sentinel.previous = sentinel;
    list->sentinel.next = sentinel;
    return list;
}
/* 0288C5B8 */
SoundResourceChoice* SoundResourceChoice::create(SoundResourceChoice* choice) {
    if (!choice) {
        choice = (SoundResourceChoice*)runtimeAllocate(sizeof(SoundResourceChoice));
        if (!choice) return choice;
    }
    choice->type = -1;
    choice->value = 0.0f;
    return choice;
}
/* 0288C600 */
void TrackRuntimeControl::setVolume(f32 value) { volume = value; }
/* 0288C608 */
void TrackRuntimeControl::setPitch(f32 value) { pitch = value; }
/* 0288C610 */
void TrackRuntimeControl::setLpfOffset(f32 value) { lpfOffset = value; }
/* 0288C618 */
void TrackRuntimeControl::setTvVolume(f32 value) { tvVolume = value; }
/* 0288C620 */
void TrackRuntimeControl::setTvPan(f32 value) { tvPan = value; }
/* 0288C628 */
void TrackRuntimeControl::setTvSurroundPan(f32 value) { tvSurroundPan = value; }
/* 0288C630 */
void TrackRuntimeControl::setTvMainSend(f32 value) { tvMainSend = value; }
/* 0288C638 */
void TrackRuntimeControl::setTvEffectSend(u32 effect,f32 value) { tvEffectSend[effect] = value; }
/* 0288C648 */
void TrackRuntimeControl::setOutputVolume(u32 line,f32 value) { output[line].volume = value; }
/* 0288C658 */
void TrackRuntimeControl::setOutputPan(u32 line,f32 value) { output[line].pan = value; }
/* 0288C668 */
void TrackRuntimeControl::setOutputSurroundPan(u32 line,f32 value) { output[line].surroundPan = value; }
/* 0288C678 */
void TrackRuntimeControl::setOutputMainSend(u32 line,f32 value) { output[line].mainSend = value; }
/* 0288C688 */
void TrackRuntimeControl::setOutputEffectSend(u32 line,u32 effect,f32 value) { output[line].effectSend[effect] = value; }
/* 0288A1D0 */
void SoundBufferPool::removeBuffer() {
    u32 begin = gabi::ea(buffer.get());
    u32 end = begin + (u32)length;
    SoundPoolLink* current = freeHead;
    gptr<SoundPoolLink>* previous = &freeHead;
    while (current) {
        u32 address = gabi::ea(current);
        if (address >= begin && address < end) {
            *previous = current->next.get();
        } else {
            previous = &current->next;
        }
        current = current->next;
    }
}
/* 0288ABD8 */
void SequenceLoadTask::reset() {
    sequence = 0;
    status = 0;
    for (u32 i = 0; i < 4; ++i) {
        banks[i] = 0;
        waveArchiveImages[i] = 0;
        individualWaveLoads[i] = 0;
    }
    loadSucceeded = 0;
}
/* 0288AC18 */
void SequenceLoadTask::initialize(const be<u32>* source) {
    reset();
    for (u32 i = 0; i < 13; ++i) parameters[i] = (u32)source[i];
    heap = nullptr;
}
/* 0288AC5C */
u32 SequenceLoadTask::ensureHeap() {
    if (!heap) {
        void* player = gabi::at<void>((u32)parameters[2]);
        void* acquired = runtimeAcquirePlayerHeap(player);
        heap = acquired;
        if (!acquired) return 0;
        runtimeInitializePlayerHeap(acquired);
    }
    return 1;
}
/* 0288ACC4 */
u32 SequenceLoadTask::request() {
    if (!ensureHeap()) return 0;
    u32 state = status;
    if (state == 0) {
        void* scheduler = runtimeLoadScheduler();
        runtimeScheduleLoad(scheduler,this,1);
        return 0;
    }
    return state == 3 || state == 4;
}
/* 0288B600 */
SequenceResourceParameters* SequenceResourceParameters::create(SequenceResourceParameters* params,u32 sequence,u32 offset,u32 archive,u8* banks,u32 options) {
    if (!params) {
        params = (SequenceResourceParameters*)runtimeAllocate(sizeof(SequenceResourceParameters));
        if (!params) return params;
    }
    params->sequence = sequence;
    params->offset = offset;
    params->archive = archive;
    params->options = options;
    for (u32 i = 0; i < 4; ++i) params->banks[i] = gabi::at<u8>(gabi::ea(banks) + i * 8);
    return params;
}
/* 0288C5FC */
static void sequenceRuntimeEmptyCallback() {}
/* 0288A86C */
u32 SequenceLoadTask::isPending() { return runtimeEventStatus(&event) ^ 1; }
/* 0288A894 */
void SequenceLoadTask::releaseHeap() {
    void* acquired = heap;
    if (acquired) runtimeReleasePlayerHeap(gabi::at<void>((u32)parameters[2]),acquired);
}
/* 0288A8A8 */
void SequenceLoadTask::releaseHeapCallback() { releaseHeap(); }
/* 0288CF30 */
void TrackRuntimeControl::reset() {
    pitch = 1.0f;
    outputMask = 0;
    tvMainSend = 0.0f;
    panRange = 1.0f;
    tvPan = 0.0f;
    tvSurroundPan = 0.0f;
    tvVolume = 1.0f;
    volume = 1.0f;
    for (u32 i = 0; i < 3; ++i) tvEffectSend[i] = 0.0f;
    for (u32 i = 0; i < 2; ++i) {
        TrackOutputControl* line = &output[i];
        line->volume = 1.0f;
        line->mainSend = 0.0f;
        line->mixMode = 0;
        line->pan = 0.0f;
        line->surroundPan = 0.0f;
        for (u32 effect = 0; effect < 3; ++effect) line->effectSend[effect] = 0.0f;
    }
    muted = 0;
    transpose = 0;
    waitCounter = 0;
    voiceMode = 0;
    ready = 1;
    silent = 0;
    active = 1;
    sequenceData = nullptr;
    tied = 0;
    portamento = 0;
    allocationMode = 0;
    delayTicks = 0;
    oneShot = 0;
    noteState = 0;
    readerCursor = 0;
    for (u32 i = 0; i < 4; ++i) {
        runtimeTrackModulationReset(modulationState[i]);
        modulationType[i] = 0;
    }
    expressionRamp.counter = 0;
    transposeControl = 0;
    expressionRamp.target = 127;
    keyControl = 60;
    attack = 255;
    priority = 127;
    pitchBendRange = 2;
    pitchBendRamp.counter = 0;
    expressionRamp.origin = 127;
    expressionRamp.frames = 0;
    volumeRamp.origin = 127;
    surroundOffsetRamp.frames = 0;
    phaseControl = 0;
    pitchBendRamp.origin = 0;
    panControl = 64;
    release = 255;
    surroundOffsetRamp.origin = 0;
    panOffsetRamp.frames = 0;
    envelopeRange = 255;
    noteLength = 0.0f;
    panOffsetRamp.counter = 0;
    voiceFlags = 0;
    sustain = 255;
    volumeRamp.frames = 0;
    panOffsetRamp.origin = 0;
    volumeRamp.counter = 0;
    decay = 255;
    pitchBendRamp.target = 0;
    surroundOffsetRamp.target = 0;
    panOffsetRamp.target = 0;
    surroundOffsetRamp.counter = 0;
    volumeRamp.target = 127;
    velocity = 127;
    pitchBendRamp.frames = 0;
    for (u32 i = 0; i < 3; ++i) envelopeFlags[i] = 0;
    lpfOffset = 0.0f;
    biquadValue = 0.0f;
    biquadType = 0;
    outputLine = -1;
    for (u32 i = 0; i < 16; ++i) variables[i] = -1;
}
static inline void constructTrackRamp(ByteRamp_l* ramp) {
    if (!gabi::ea(ramp)) ramp = (ByteRamp_l*)runtimeAllocate(sizeof(ByteRamp_l));
    if (!gabi::ea(ramp)) return;
    ramp->target = 0;
    ramp->frames = 0;
    ramp->origin = 0;
    ramp->counter = 0;
}
/* 0288D158 */
TrackRuntimeControl* TrackRuntimeControl::create(TrackRuntimeControl* track) {
    if (!track) {
        track = (TrackRuntimeControl*)runtimeAllocate(sizeof(TrackRuntimeControl));
        if (!track) return track;
    }
    track->vtable = &sTrackVtable;
    track->activeFlag = 0;
    __construct_array(track->outputMix,2,0x18,runtimeTrackMixConstruct);
    __construct_array(track->output,2,0x50,runtimeTrackOutputConstruct);
    constructTrackRamp(&track->volumeRamp);
    constructTrackRamp(&track->expressionRamp);
    constructTrackRamp(&track->panOffsetRamp);
    constructTrackRamp(&track->surroundOffsetRamp);
    constructTrackRamp(&track->pitchBendRamp);
    __construct_array(track->modulationState,4,0x10,runtimeTrackModulationConstruct);
    track->owner = nullptr;
    track->channels = nullptr;
    track->reset();
    return track;
}
/* WWHD glue
data sTrackVtable 101820C4
ext 0288E77C void* runtimeTrackMixConstruct(void*) callback
ext 0288E7D8 void* runtimeTrackOutputConstruct(void*) callback
ext 0288E830 void* runtimeTrackModulationConstruct(void*) callback
ext 028A54F4 void runtimeTrackModulationReset(void*)
data sMemoryStreamVtable 10181DCC
ext 02897B84 void* runtimeAcquirePlayerHeap(void*)
ext 028AD91C void runtimeInitializePlayerHeap(void*)
ext 0289C1A4 void* runtimeLoadScheduler()
ext 0289C464 void runtimeScheduleLoad(void*,nw::snd::SequenceLoadTask*,u32)
ext 028F6A30 u32 runtimeEventStatus(nw::ut::Event*)
ext 02897BE8 void runtimeReleasePlayerHeap(void*,void*)
ext 0273AD10 void* runtimeAllocate(u32)
ext C000A848 void runtimeCopy(void*,const void*,u32)
*/
