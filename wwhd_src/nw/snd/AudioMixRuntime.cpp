/* Output device selection and auxiliary-bus arithmetic. Derived from the supplied binary. */
// SDK boundary facts: wut confirms the five arguments of auxiliary callback registration.
#include "nw/snd/AudioMixRuntime.h"
#include "nw/snd/PlaybackVoice.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;
static AudioMixRuntime sMixRuntime;
struct MixRuntimeGuard {
    be<u32> value;
};
static MixRuntimeGuard sMixRuntimeGuard;
struct OutputPointerSlot {
    be<u32> value;
};
struct OutputPointerStorage {
    OutputPointerSlot slots[5];
};
static OutputPointerStorage sOutputPointers;
static const u8 sDrcVirtualModes[3] = {0, 0, 2};

/* inlined into AudioMixRuntime::create */
static void initializeMixMove(MoveValue* value) {
    if (!gabi::ea(value)) value = (MoveValue*)sndAllocate(sizeof(MoveValue));
    if (value) {
        value->target = 0.0f;
        value->frames = 0;
        value->origin = 0.0f;
        value->counter = 0;
    }
}

/* inlined into AudioMixRuntime::create */
static void initializeMixChain(MixEffectChain* chain) {
    if (!gabi::ea(chain)) chain = (MixEffectChain*)sndAllocate(sizeof(MixEffectChain));
    if (chain) {
        chain->count = 0;
        chain->sentinel.previous = &chain->sentinel;
        chain->sentinel.next = &chain->sentinel;
    }
}

/* 02888A40 */
void MixElapsedTime::clear() {
    gabi::AutoFrame frame{0x02888A40};
    low = 0;
    high = 0;
}

/* 02888A50 */
AudioMixRuntime* AudioMixRuntime::create(AudioMixRuntime* runtime) {
    gabi::AutoFrame frame{0x02888A50};
    if (!runtime) {
        runtime = (AudioMixRuntime*)sndAllocate(sizeof(AudioMixRuntime));
        if (!runtime) return runtime;
    }
    runtime->defaultMode = 2;
    runtime->enabled = 0;
    initializeMixMove(&runtime->mainVolume);
    initializeMixMove(&runtime->drcVolume);
    __construct_array(runtime->auxVolume, 9, sizeof(MoveValue), constructAuxMove);
    __construct_array(runtime->auxReturn, 9, sizeof(MoveValue), constructAuxMove);
    __construct_array(runtime->auxEffects, 9, sizeof(MixEffectChain), constructMixChain);
    initializeMixChain(&runtime->tvEffects);
    initializeMixChain(&runtime->drcEffects);
    __construct_array(runtime->auxTimes, 9, sizeof(MixElapsedTime), constructMixTime);
    runtime->drcVolume.counter = 0;
    runtime->drcVolume.frames = 0;
    runtime->mainVolume.frames = 0;
    runtime->mainVolume.origin = 1.0f;
    runtime->mainVolume.target = 1.0f;
    runtime->drcVolume.target = 1.0f;
    runtime->mainVolume.counter = 0;
    runtime->drcVolume.origin = 1.0f;
    for (u32 i = 0; i < 3; ++i) {
        runtime->outputMode[i] = 4;
        runtime->requestedMode[i] = 4;
    }
    for (u32 group = 0; group < 3; ++group) {
        for (u32 bus = 0; bus < 3; ++bus) {
            u32 index = group * 3 + bus;
            runtime->auxVolume[index].origin = 1.0f;
            runtime->auxReturn[index].target = 1.0f;
            runtime->auxVolume[index].target = 1.0f;
            runtime->auxReturn[index].origin = 1.0f;
            runtime->auxReturn[index].frames = 0;
            runtime->auxVolume[index].frames = 0;
            runtime->auxVolume[index].counter = 0;
            runtime->auxReturn[index].counter = 0;
            runtime->auxPending[index] = 0;
            runtime->auxHandles[index] = 0;
        }
    }
    return runtime;
}

/* 02888C88 */
void AudioMixRuntime::processTv(FinalMixFrame* info) {
    gabi::AutoFrame frame{0x02888C88};
    u32 previous = previousCallbacks[0];
    if (previous) gabi::call_ptr<void>(previous, info);
    gabi::FrameLocal<MixBufferSet> buffers(8);
    u32 channels = info->channelCount;
    buffers->channels[3] = 0;
    buffers->channels[1] = 0;
    buffers->channels[0] = 0;
    buffers->channels[5] = 0;
    buffers->channels[4] = 0;
    buffers->channels[2] = 0;
    if (channels) {
        be<u32>* input = info->buffers;
        buffers->sampleCount = 0;
        buffers->channelCount = 0;
        buffers->channels[0] = input[0];
        buffers->channels[1] = channels > 1 ? (u32)input[1] : 0;
        buffers->channels[2] = channels > 2 ? (u32)input[2] : 0;
        buffers->channels[3] = channels > 3 ? (u32)input[3] : 0;
        buffers->channels[4] = channels > 4 ? (u32)input[4] : 0;
        buffers->channels[5] = channels > 5 ? (u32)input[5] : 0;
    }
    u16 samples = info->sampleCount;
    MixEffect* effect = (MixEffect*)tvEffects.sentinel.next.get();
    buffers->sampleCount = samples;
    buffers->channelCount = channels;
    while (effect != (MixEffect*)&tvEffects.sentinel) {
        u16 outputs = info->outputCount;
        gabi::call_ptr<void>(effect->vtable->processTv.function, effect, buffers.get(), outputs);
        effect = (MixEffect*)effect->link.next.get();
    }
    effect = (MixEffect*)drcEffects.sentinel.next.get();
    while (effect != (MixEffect*)&drcEffects.sentinel) {
        u16 outputs = info->outputCount;
        gabi::call_ptr<void>(effect->vtable->processTv.function, effect, buffers.get(), outputs);
        effect = (MixEffect*)effect->link.next.get();
    }
}

/* 02888E00 */
void forwardTvMix(FinalMixFrame* info) {
    gabi::AutoFrame frame{0x02888E00};
    if (!sMixRuntimeGuard.value) {
        sMixRuntimeGuard.value = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    sMixRuntime.processTv(info);
}

struct BufferPairs {
    MixBufferSet outputs[2];
};

/* 02888E60 */
void AudioMixRuntime::processDrc(FinalMixFrame* info) {
    gabi::AutoFrame frame{0x02888E60};
    u32 previous = previousCallbacks[1];
    if (previous) gabi::call_ptr<void>(previous, info);
    gabi::FrameLocal<BufferPairs> buffers(8);
    __construct_array(buffers->outputs, 2, sizeof(MixBufferSet), constructMixBuffers);
    u32 channels = info->channelCount;
    if (channels) {
        u32 outputs = info->outputCount;
        if (outputs) {
            u16 samples = info->sampleCount;
            for (u32 output = 0; output < outputs; ++output) {
                MixBufferSet* current = &buffers->outputs[output];
                u32 start = channels * output;
                be<u32>* input = info->buffers;
                current->channels[0] = channels ? (u32)input[start] : 0;
                u32 right = 0;
                if (channels > 1) {
                    input = info->buffers;
                    right = input[start + 1];
                }
                current->channels[1] = right;
                u32 rear = 0;
                if (channels > 2) {
                    input = info->buffers;
                    rear = input[start + 2];
                }
                current->channels[2] = rear;
                u32 rearRight = 0;
                if (channels > 3) {
                    input = info->buffers;
                    rearRight = input[start + 3];
                }
                current->channelCount = channels;
                current->channels[3] = rearRight;
                current->sampleCount = samples;
            }
        }
    }
    MixEffect* effect = (MixEffect*)tvEffects.sentinel.next.get();
    while (effect != (MixEffect*)&tvEffects.sentinel) {
        u16 outputs = info->outputCount;
        gabi::call_ptr<void>(effect->vtable->processDrc.function, effect, buffers.get(), outputs);
        effect = (MixEffect*)effect->link.next.get();
    }
    effect = (MixEffect*)drcEffects.sentinel.next.get();
    while (effect != (MixEffect*)&drcEffects.sentinel) {
        u16 outputs = info->outputCount;
        gabi::call_ptr<void>(effect->vtable->processDrc.function, effect, buffers.get(), outputs);
        effect = (MixEffect*)effect->link.next.get();
    }
}

/* 02888FEC */
void forwardDrcMix(FinalMixFrame* info) {
    gabi::AutoFrame frame{0x02888FEC};
    if (!sMixRuntimeGuard.value) {
        sMixRuntimeGuard.value = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    sMixRuntime.processDrc(info);
}

/* 0288904C */
void AudioMixRuntime::setOutputPointer(s32 output, void* pointer) {
    gabi::AutoFrame frame{0x0288904C};
    // unchecked index: 32-bit element address as the original computes it
    if (output > 0) *gabi::at<be<u32>>(gabi::ea(outputPointers) + (u32)output * 4u) = gabi::ea(pointer);
}

struct ModeCommand : DriverCommand {
    be<u32> changed;
};

/* 02889064 */
void AudioMixRuntime::setOutputMode(u32 mode, u32 output) {
    gabi::AutoFrame frame{0x02889064};
    if (outputMode[output] == mode) return;
    outputMode[output] = mode;
    if (output == 1) AXSetDRCVSMode(mode <= 2 ? (s32)sDrcVirtualModes[mode] : -1);
    DriverCommandManager* manager = DriverCommandManager::instance();
    ModeCommand* command = (ModeCommand*)manager->allocCommand(sizeof(ModeCommand));
    command->id = 75;
    command->changed = 8;
    manager->pushCommand(command);
    for (u32 group = 0; group < 3; ++group) {
        for (u32 bus = 0; bus < 3; ++bus) {
            MixEffectChain* chain = &auxEffects[group * 3 + bus];
            AuxEffect* effect = (AuxEffect*)chain->sentinel.next.get();
            while (effect != (AuxEffect*)&chain->sentinel) {
                gabi::call_ptr<void>(effect->vtable->onModeChange.function, effect);
                effect = (AuxEffect*)effect->link.next.get();
            }
        }
    }
}

/* 02889148 */
void AudioMixRuntime::enable() {
    gabi::AutoFrame frame{0x02889148};
    if (enabled) return;
    for (u32 i = 0; i < 128; ++i) outputPointers[i] = 0;
    setOutputPointer(1, &sOutputPointers.slots[0]);
    setOutputPointer(2, &sOutputPointers.slots[1]);
    setOutputPointer(3, &sOutputPointers.slots[2]);
    setOutputPointer(4, &sOutputPointers.slots[3]);
    setOutputPointer(5, &sOutputPointers.slots[4]);
    if (!AXGetDeviceFinalMixCallback(0, &previousCallbacks[0]))
        AXRegisterDeviceFinalMixCallback(0, gabi::guest_fn((const void*)forwardTvMix));
    if (!AXGetDeviceFinalMixCallback(1, &previousCallbacks[1]))
        AXRegisterDeviceFinalMixCallback(1, gabi::guest_fn((const void*)forwardDrcMix));
    gabi::FrameLocal<be<u32>> tvMode(8);
    gabi::FrameLocal<be<u32>> drcMode(12);
    AXGetDeviceMode(0, tvMode.get());
    u32 mode = 1;
    if (*tvMode == 5)
        mode = 0;
    else if (*tvMode == 3)
        mode = 2;
    requestedMode[0] = mode;
    setOutputMode(mode, 0);
    AXGetDeviceMode(1, drcMode.get());
    mode = 1;
    if (*drcMode == 1)
        mode = 2;
    else if (*drcMode == 5)
        mode = 0;
    requestedMode[1] = mode;
    setOutputMode(mode, 1);
    for (u32 i = 0; i < 32; ++i) outputBits[i] = i < 7 ? (u8)(1u << i) : 0;
    enabled = 1;
}

/* 028893F8 */
void AudioMixRuntime::disable() {
    gabi::AutoFrame frame{0x028893F8};
    if (!enabled) return;
    u32 callback = previousCallbacks[0];
    AXRegisterDeviceFinalMixCallback(0, callback);
    callback = previousCallbacks[1];
    previousCallbacks[0] = 0;
    AXRegisterDeviceFinalMixCallback(1, callback);
    previousCallbacks[1] = 0;
    enabled = 0;
}

/* 0288945C */
u32 nw::snd::selectMixDevice(u32 output, MixOutputDevice* selected) {
    gabi::AutoFrame frame{0x0288945C};
    if (output == 0) {
        selected->index = 0;
        selected->device = 0;
        return 1;
    }
    if (output == 1) {
        selected->device = 1;
        selected->index = 0;
        return 1;
    }
    return 0;
}

/* 0288949C */
u32 nw::snd::auxDeviceBus(u32 bus) {
    gabi::AutoFrame frame{0x0288949C};
    // The caller observes the unchanged incoming register; the source-level ABI is inferred.
    return bus;
}

/* 028894A0 */
void AudioMixRuntime::resetAuxChain(u32 bus, u32 output) {
    gabi::AutoFrame frame{0x028894A0};
    u32 index = output * 3 + bus;
    MixEffectChain* chain = &auxEffects[index];
    if (!chain->count) return;
    gabi::FrameLocal<MixOutputDevice> device(16);
    if (!selectMixDevice(output, device.get())) return;
    u32 sdkBus = auxDeviceBus(bus);
    u32 type = device->device;
    u32 deviceIndex = device->index;
    if (AXRegisterAuxCallback(type, deviceIndex, sdkBus, 0, 0) < 0) return;
    auxTimes[index].high = 0;
    auxTimes[index].low = 0;
    AuxEffect* effect = (AuxEffect*)chain->sentinel.next.get();
    while (effect != (AuxEffect*)&chain->sentinel) {
        gabi::call_ptr<void>(effect->vtable->onStop.function, effect);
        effect = (AuxEffect*)effect->link.next.get();
    }
    SoundListNode* first = chain->sentinel.next;
    gabi::FrameLocal<SoundListIterator> begin(8);
    gabi::FrameLocal<SoundListIterator> end(12);
    begin->node = first;
    end->node = &chain->sentinel;
    eraseSoundListRange(chain, begin.get(), end.get());
}
struct MixConversionScratch {
    be<u32> high;
    be<u32> low;
};

/* inlined into AudioMixRuntime::updateVolumes */
static f32 interpolateMixValue(MoveValue* value, s32 count, s32 frames, f32 target, MixConversionScratch* scratch) {
    if (count >= frames) return target;
    scratch->high = 0x43300000;
    scratch->low = (u32)count ^ 0x80000000;
    f32 origin = value->origin;
    scratch->low = (u32)frames ^ 0x80000000;
    f32 difference = gabi::fsubs_ppc(target, origin);
    f32 scaled = gabi::fmuls_ppc(difference, (f32)count);
    return gabi::fadds_ppc(origin, gabi::fdivs_ppc(scaled, (f32)frames));
}

/* 02889594 */
void AudioMixRuntime::updateVolumes() {
    gabi::AutoFrame frame{0x02889594};
    gabi::FrameLocal<MixConversionScratch> scratch(8);
    gabi::FrameLocal<MixOutputDevice> device(16);
    for (u32 output = 0; output < 3; ++output) {
        for (u32 bus = 0; bus < 3; ++bus) {
            u32 index = output * 3 + bus;
            MoveValue* fade = &auxReturn[index];
            MoveValue* volume = &auxVolume[index];
            s32 fadeCount = fade->counter;
            s32 fadeFrames = fade->frames;
            bool fadeActive = fadeCount < fadeFrames;
            if (fadeActive) {
                s32 current = fade->counter;
                s32 frames = fade->frames;
                if (current < frames) fade->counter = current + 1;
            }
            s32 volumeCount = volume->counter;
            s32 volumeFrames = volume->frames;
            if (volumeCount < volumeFrames) {
                s32 current = volume->counter;
                s32 frames = volume->frames;
                if (current < frames) volume->counter = current + 1;
                volumeCount = volume->counter;
                volumeFrames = volume->frames;
                if (!(volumeCount < volumeFrames)) resetAuxChain(bus, output);
            } else if (!fadeActive)
                continue;
            fadeFrames = fade->frames;
            fadeCount = fade->counter;
            volumeFrames = volume->frames;
            volumeCount = volume->counter;
            f32 fadeTarget = fade->target;
            f32 fadeValue = interpolateMixValue(fade, fadeCount, fadeFrames, fadeTarget, scratch.get());
            if (fadeValue > 1.0f)
                fadeValue = 1.0f;
            else
                fadeValue = fadeValue >= 0.0f ? fadeValue : 0.0f;
            f32 target = volume->target;
            f32 level = interpolateMixValue(volume, volumeCount, volumeFrames, target, scratch.get());
            if (level > 1.0f)
                level = 1.0f;
            else
                level = level >= 0.0f ? level : 0.0f;
            f32 product = gabi::fmuls_ppc(fadeValue, level);
            f32 scaled = gabi::fmuls_ppc(product, 32768.0f);
            scratch->high = (u32)gabi::ftoi(scaled);
            u32 rawVolume = (u16)(u32)scratch->high;
            if (!selectMixDevice(output, device.get())) continue;
            u32 sdkBus = auxDeviceBus(bus);
            u32 deviceIndex = device->index;
            u32 type = device->device;
            AXSetAuxReturnVolume(type, deviceIndex, sdkBus, rawVolume);
        }
    }
    s32 current = mainVolume.counter;
    s32 frames = mainVolume.frames;
    if (current < frames) {
        mainVolume.counter = current + 1;
        PlaybackVoicePool* voices = PlaybackVoicePool::instance();
        voices->markDirty(PlaybackVoice::DirtyVolume);
    }
    current = drcVolume.counter;
    frames = drcVolume.frames;
    if (current < frames) {
        drcVolume.counter = current + 1;
        PlaybackVoicePool* voices = PlaybackVoicePool::instance();
        voices->markDirty(PlaybackVoice::DirtyVolume);
    }
}

/* 028898F0 */
void nw::snd::flushMixBuffer(void* data, u32 size) {
    gabi::AutoFrame frame{0x028898F0};
    DCFlushRange(data, size);
}

/* 028898F4 */
u32 nw::snd::flattenAuxBus(u32 bus, u32 output) {
    gabi::AutoFrame frame{0x028898F4};
    if (output == 0) return bus;
    if (output == 1) return bus + 3;
    return 0xFFFFFFFF;
}

/* 0288991C */
s32 nw::snd::auxBusIndex(s32 flattened) {
    gabi::AutoFrame frame{0x0288991C};
    return flattened % 3;
}

/* 0288993C */
s32 nw::snd::auxOutputIndex(s32 flattened) {
    gabi::AutoFrame frame{0x0288993C};
    return flattened / 3;
}

struct AuxBufferArray {
    be<u32> channels[6];
};

/* 02889954 */
void processAuxMix(be<u32>* inputs, s32 flattened, AuxMixFrame* info) {
    gabi::AutoFrame frame{0x02889954};
    auto start = OSGetSystemTime();
    s32 bus = auxBusIndex(flattened);
    s32 output = auxOutputIndex(flattened);
    gabi::FrameLocal<AuxBufferArray> buffers(8);
    buffers->channels[3] = 0;
    buffers->channels[1] = 0;
    buffers->channels[4] = 0;
    buffers->channels[5] = 0;
    u32 channels = info->channelCount;
    buffers->channels[2] = 0;
    buffers->channels[0] = 0;
    for (u32 i = 0; i < channels; ++i) buffers->channels[i] = inputs[i];
    u32 samples = info->sampleCount;
    u32 bytes = channels * samples * 4;
    u32 guard = sMixRuntimeGuard.value;
    if (!guard) {
        sMixRuntimeGuard.value = 1;
        AudioMixRuntime::create(&sMixRuntime);
        guard = sMixRuntimeGuard.value;
    }
    u32 mode = sMixRuntime.outputMode[(u32)output];
    if (!guard) {
        sMixRuntimeGuard.value = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    u32 index = (u32)output * 3 + (u32)bus;
    MixEffectChain* chain = &sMixRuntime.auxEffects[index];
    AuxEffect* effect = (AuxEffect*)chain->sentinel.next.get();
    for (;;) {
        if (!sMixRuntimeGuard.value) {
            sMixRuntimeGuard.value = 1;
            AudioMixRuntime::create(&sMixRuntime);
        }
        if (effect == (AuxEffect*)&chain->sentinel) break;
        u32 count = info->channelCount;
        gabi::call_ptr<void>(effect->vtable->processAux.function, effect, count, buffers.get(), bytes, 3, mode,
                             32000.0f);
        effect = (AuxEffect*)effect->link.next.get();
    }
    if (!sMixRuntimeGuard.value) {
        sMixRuntimeGuard.value = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    auto end = OSGetSystemTime();
    u64 elapsed = (((u64)end.r3 << 32) | end.r4) - (((u64)start.r3 << 32) | start.r4);
    sMixRuntime.auxTimes[index].low = (u32)elapsed;
    sMixRuntime.auxTimes[index].high = (u32)(elapsed >> 32);
}

/* inlined into dispatchAudioCommandList */
AudioMixRuntime* commandMixRuntime() {
    if (!sMixRuntimeGuard.value) {
        sMixRuntimeGuard.value = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    return &sMixRuntime;
}

/* WWHD glue
data sMixRuntime 101FCE00
data sMixRuntimeGuard 101FD8C8
data sOutputPointers 104B71D8
data sDrcVirtualModes 10181DB8
ext 0288A0EC void* constructAuxMove(void*) callback
ext 0288A13C void* constructMixChain(void*) callback
ext 0288A094 void* constructMixBuffers(void*) callback
ext 02888A40 void* constructMixTime(void*) callback
ext 02885F10 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::instance()
ext C0006F50 void AXSetDRCVSMode(s32)
ext C0006D38 s32 AXGetDeviceFinalMixCallback(u32,be<u32>*)
ext C0006ED8 s32 AXRegisterDeviceFinalMixCallback(u32,u32)
ext C0006D48 s32 AXGetDeviceMode(u32,be<u32>*)
ext C0006EB8 s32 AXRegisterAuxCallback(u32,u32,u32,u32,u32)
ext C0006F20 s32 AXSetAuxReturnVolume(u32,u32,u32,u32)
ext 0289F364 nw::snd::PlaybackVoicePool* nw::snd::PlaybackVoicePool::instance()
ext 0289F880 void nw::snd::PlaybackVoicePool::markDirty(u32)
ext C0009C98 Pair32 OSGetSystemTime()
ext C0008898 void DCFlushRange(void*,u32)
*/
