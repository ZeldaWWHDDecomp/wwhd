/* Wave-channel initial values and output selection. From WWHD's instructions only. */
#include "nw/snd/WaveChannel.h"
#include "nw/snd/AudioMixRuntime.h"
using namespace nw::snd;
struct MixRuntimeGuard { be<u32> value; };
static AudioMixRuntime sMixRuntime;
static MixRuntimeGuard sMixRuntimeGuard;

/* 028A3B70 */
u32 WaveChannel::currentOutputMode() {
    gabi::AutoFrame frame{0x028A3B70};
    if (!sMixRuntimeGuard.value) {
        sMixRuntimeGuard.value = 1;
        AudioMixRuntime::create(&sMixRuntime);
    }
    u32 mode = sMixRuntime.defaultMode;
    if (mode == 0) return 2;
    if (mode == 1) return 1;
    return 0;
}

/* 028A3BEC */
void WaveChannel::initialize(u32 target, u32 argument) {
    gabi::AutoFrame frame{0x028A3BEC};
    volume = 1.0f;
    callback = target;
    pan = 0.0f;
    hold = 0;
    mix.surroundPan = 0.0f;
    sourceType = 0;
    send = 0.0f;
    callbackArgument = argument;
    position = 0;
    filter = 0.0f;
    mix.pitch = 1.0f;
    mix.groups[0].mainSend = 0.0f;
    mix.groups[0].surroundPan = 0.0f;
    baseKey = 60;
    mix.mode = 1;
    mix.groups[0].mode = 0;
    autoRelease = 1;
    mix.groups[0].pan = 0.0f;
    ignoreRelease = 0;
    panEnabled = 0;
    pitch = 1.0f;
    mix.gain = 1.0f;
    mix.pan = 0.0f;
    streamValue = 0.0f;
    looping = 0;
    active = 0;
    mix.groups[0].gain = 1.0f;
    context = nullptr;
    scale = 1.0f;
    key = 60;
    for (u32 i = 0; i < 3; ++i) mix.groups[0].auxiliarySend[i] = 0.0f;
    for (u32 group = 1; group < 3; ++group) {
        mix.groups[group].gain = 1.0f;
        mix.groups[group].mainSend = 0.0f;
        mix.groups[group].mode = 0;
        mix.groups[group].pan = 0.0f;
        mix.groups[group].surroundPan = 0.0f;
        for (u32 i = 0; i < 3; ++i) mix.groups[group].auxiliarySend[i] = 0.0f;
    }
    for (u32 i = 0; i < 4; ++i) {
        mix.outputs[i].gain = 1.0f;
        mix.outputs[i].pan = 0.0f;
        mix.outputs[i].surroundPan = 0.0f;
    }
    releaseTarget = 0.0f;
    releaseDuration = 0;
    control.bus = 0xFF;
    releaseElapsed = 0;
    control.flags = 0;
    control.value = 0;
    control.index = 0xFF;
    envelope.initialize(-90.40000152587890625);
    for (u32 i = 0; i < 4; ++i) {
        modulators[i].initialize();
        modulationTargets[i] = 0;
    }
    ending = 0;
    flags = 0;
    interpolation = 0;
    track = nullptr;
    outputMode = currentOutputMode();
}

/* WWHD glue
 data sMixRuntime 101FCE00
 data sMixRuntimeGuard 101FD8C8
 ext 02888A50 nw::snd::AudioMixRuntime* nw::snd::AudioMixRuntime::create(nw::snd::AudioMixRuntime*)
*/
