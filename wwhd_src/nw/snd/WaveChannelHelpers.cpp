/* Compiler-kept construction helpers for wave-channel members. From WWHD only. */
#include "nw/snd/WaveChannel.h"
using namespace nw::snd;
void* allocateWaveMember(u32 size);
void deleteWaveMember(void* member);
void constructWaveMembers(void* storage, u32 count, u32 stride, u32 callback);

/* 028A4BC8 */
WaveSampleData* WaveSampleData::construct(WaveSampleData* data) {
    gabi::AutoFrame frame{0x028A4BC8};
    if (!data) data = (WaveSampleData*)allocateWaveMember(sizeof(WaveSampleData));
    if (data) {
        data->context = 0;
        data->active = 0;
        data->state = RequestIdle;
        data->next = nullptr;
        data->samples = nullptr;
        data->sampleCount = 0;
        data->loopStart = 0;
        data->adpcm = nullptr;
    }
    return data;
}

/* 028A4C20 */
void WaveSampleData::destroy(WaveSampleData* data, u32 flags) {
    gabi::AutoFrame frame{0x028A4C20};
    if (data && (flags & 1)) deleteWaveMember(data);
}

/* 028A4C34 */
ChannelMixState* ChannelMixState::constructForWave(ChannelMixState* state) {
    gabi::AutoFrame frame{0x028A4C34};
    if (!state) state = (ChannelMixState*)allocateWaveMember(sizeof(ChannelMixState));
    if (state) {
        state->pitch = 1.0f;
        state->auxiliarySend = 0.0f;
        state->pan = 0.0f;
        state->mainSend = 0.0f;
        state->gain = 1.0f;
        state->surroundPan = 0.0f;
    }
    return state;
}

/* 028A4C90 */
ChannelMixGroup* ChannelMixGroup::constructForWave(ChannelMixGroup* group) {
    gabi::AutoFrame frame{0x028A4C90};
    if (!group) group = (ChannelMixGroup*)allocateWaveMember(sizeof(ChannelMixGroup));
    if (group) constructWaveMembers(group->channels, 2, sizeof(ChannelMixState),
        gabi::guest_fn((const void*)&ChannelMixState::constructForWave));
    return group;
}

/* 028A4CE8 */
Modulator* Modulator::construct(Modulator* modulator) {
    gabi::AutoFrame frame{0x028A4CE8};
    if (!modulator) modulator = (Modulator*)allocateWaveMember(sizeof(Modulator));
    if (modulator) {
        modulator->initialize();
        modulator->started = 0;
        modulator->elapsed = 0;
        modulator->phase = 0.0f;
        modulator->wrapped = 0;
        modulator->cached = 1.0f;
    }
    return modulator;
}

/* WWHD glue
 ext 0273AD10 void* allocateWaveMember(u32)
 ext 0273AF40 void deleteWaveMember(void*)
 ext 028EFFD0 void constructWaveMembers(void*,u32,u32,u32)
*/
