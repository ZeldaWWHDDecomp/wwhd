/* Audio output and effect-bus helpers, reconstructed from WWHD. */
#pragma once
#include "nw/snd/ExternalSoundQueue.h"
namespace nw { namespace snd {
struct MixElapsedTime {
    be<u32> high;
    be<u32> low;
    void clear();
};
struct MixOutputDevice {
    be<u32> device;
    be<u32> index;
};
WWHD_SIZE(MixElapsedTime, 8);
WWHD_SIZE(MixOutputDevice, 8);
struct AuxMixFrame {
    be<u32> channelCount;
    be<u32> sampleCount;
};
WWHD_SIZE(AuxMixFrame, 8);
struct FinalMixFrame {
    gptr<be<u32>> buffers;
    be<u16> channelCount;
    be<u16> sampleCount;
    be<u16> outputCount;
    u8 padding[2];
};
struct MixBufferSet {
    be<u32> channels[6];
    be<u16> sampleCount;
    be<u16> channelCount;
};
struct MixEffectVtable {
    VtableEntry header, destroy, processTv, processDrc, processAux, onModeChange;
};
struct MixEffect {
    SoundListNode link;
    gptr<MixEffectVtable> vtable;
};
struct AuxEffectVtable {
    VtableEntry header, destroy, onStart, onStop, processAux, onModeChange;
};
struct AuxEffect {
    SoundListNode link;
    gptr<AuxEffectVtable> vtable;
};
struct MixEffectChain {
    be<u32> count;
    SoundListNode sentinel;
};
struct AudioMixRuntime {
    be<u8> enabled;
    u8 padding[3];
    be<u32> outputMode[3];
    be<u32> requestedMode[3];
    be<u32> defaultMode;
    MoveValue mainVolume;
    MoveValue drcVolume;
    MoveValue auxVolume[9];
    MoveValue auxReturn[9];
    MixEffectChain auxEffects[9];
    be<u32> auxPending[9];
    MixEffectChain tvEffects;
    MixEffectChain drcEffects;
    be<u32> previousCallbacks[2];
    be<u32> outputPointers[128];
    be<u32> auxHandles[9];
    be<u32> timerPadding;
    MixElapsedTime auxTimes[9];
    u8 flags[9];
    be<u8> outputBits[32];
    u8 tail[7];
    static AudioMixRuntime* create(AudioMixRuntime*);
    void processTv(FinalMixFrame*);
    void processDrc(FinalMixFrame*);
    void setOutputPointer(s32, void*);
    void setOutputMode(u32, u32);
    void enable();
    void disable();
    void resetAuxChain(u32, u32);
    void updateVolumes();
};
WWHD_SIZE(FinalMixFrame, 12);
WWHD_SIZE(MixBufferSet, 28);
WWHD_SIZE(MixEffectChain, 12);
WWHD_OFFSET(AudioMixRuntime, mainVolume, 0x20);
WWHD_OFFSET(AudioMixRuntime, auxVolume, 0x40);
WWHD_OFFSET(AudioMixRuntime, auxReturn, 0xD0);
WWHD_OFFSET(AudioMixRuntime, auxEffects, 0x160);
WWHD_OFFSET(AudioMixRuntime, auxPending, 0x1CC);
WWHD_OFFSET(AudioMixRuntime, tvEffects, 0x1F0);
WWHD_OFFSET(AudioMixRuntime, previousCallbacks, 0x208);
WWHD_OFFSET(AudioMixRuntime, outputPointers, 0x210);
WWHD_OFFSET(AudioMixRuntime, auxHandles, 0x410);
WWHD_OFFSET(AudioMixRuntime, auxTimes, 0x438);
WWHD_OFFSET(AudioMixRuntime, outputBits, 0x489);
WWHD_SIZE(AudioMixRuntime, 0x4B0);
u32 selectMixDevice(u32 output, MixOutputDevice* selected);
u32 auxDeviceBus(u32);
void flushMixBuffer(void* data, u32 size);
u32 flattenAuxBus(u32 bus, u32 output);
s32 auxBusIndex(s32 flattened);
s32 auxOutputIndex(s32 flattened);
}}
void DCFlushRange(void*, u32);

void forwardTvMix(nw::snd::FinalMixFrame*);
void forwardDrcMix(nw::snd::FinalMixFrame*);
void* constructAuxMove(void*);
void* constructMixChain(void*);
void* constructMixBuffers(void*);
void* constructMixTime(void*);
void AXSetDRCVSMode(s32);
s32 AXGetDeviceFinalMixCallback(u32, be<u32>*);
s32 AXRegisterDeviceFinalMixCallback(u32, u32);
s32 AXGetDeviceMode(u32, be<u32>*);

s32 AXRegisterAuxCallback(u32, u32, u32, u32, u32);
s32 AXSetAuxReturnVolume(u32, u32, u32, u32);

void processAuxMix(be<u32>*, s32, nw::snd::AuxMixFrame*);
Pair32 OSGetSystemTime();

nw::snd::AudioMixRuntime* commandMixRuntime();
