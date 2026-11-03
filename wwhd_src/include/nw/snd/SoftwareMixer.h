#pragma once
#include "nw/snd/SoftwareVoice.h"
#include "nw/snd/AudioMixRuntime.h"
namespace nw { namespace snd {
struct MixQueue {
    VoiceMessageQueue main;
    VoiceMessageQueue remote;
    u8 mainMessages[0x100];
    u8 remoteMessages[0x100];
    MixBlock blocks[16];
    be<u32> blockCount;
};
// Signed-sample pointer view of the canonical driver's address-word descriptor.
struct MixerOutput {
    gptr<be<s32>> channels[6];
    be<u16> sampleCount;
    be<u16> channelCount;
};
static_assert(sizeof(MixerOutput)==sizeof(MixBufferSet));
static_assert(offsetof(MixerOutput,channels)==offsetof(MixBufferSet,channels));
static_assert(offsetof(MixerOutput,sampleCount)==offsetof(MixBufferSet,sampleCount));
static_assert(offsetof(MixerOutput,channelCount)==offsetof(MixBufferSet,channelCount));
struct SoftwareMixer {
    u8 interfaceState[12];
    SoftwareVoiceManager manager;
    be<u32> sampleCount;
    be<f32> sampleRate;
    MixQueue queue;
    void setOutputRate(u32);
    void initialize(be<f32>*,u32);
    void finalize();
    void update();
    u32 render();
    void addSamples(const be<f32>*,be<s32>*,u32);
    void mixMain(MixerOutput*);
    void mixRemote(MixerOutput*);
};
WWHD_OFFSET(MixQueue,blocks,0x278); WWHD_OFFSET(MixQueue,blockCount,0xB78);
WWHD_OFFSET(SoftwareMixer,manager,0xC); WWHD_OFFSET(SoftwareMixer,sampleCount,0xC024);
WWHD_OFFSET(SoftwareMixer,queue,0xC02C);
WWHD_OFFSET(MixerOutput,sampleCount,0x18);
}}
void softwareMessageInitialize(nw::snd::VoiceMessageQueue*,void*,u32);
u32 softwareMessageReceive(nw::snd::VoiceMessageQueue*,gptr<nw::snd::MixBlock>*,u32);
u32 softwareMessageSend(nw::snd::VoiceMessageQueue*,nw::snd::MixBlock*,u32);
void softwareMessageFinalize(nw::snd::VoiceMessageQueue*);
void softwareSetActiveManager(nw::snd::SoftwareVoiceManager*);
void softwareRegisterMixer(void*,nw::snd::SoftwareMixer*);
void softwareUnregisterMixer(void*,nw::snd::SoftwareMixer*);
