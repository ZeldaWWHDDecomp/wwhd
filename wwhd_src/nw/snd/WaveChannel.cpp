/* Wave-channel lifetime, callbacks and release controls. From WWHD's instructions only. */
#include "nw/snd/WaveChannel.h"
using namespace nw::snd;

void* allocateWaveChannel(u32 size);
void deleteWaveChannel(void* object);
void constructWaveArray(void* storage, u32 count, u32 stride, u32 callback);
void destroyWaveArray(void* storage, u32 count, u32 stride, u32 callback, u32 first, u32 second);
static SoundRangeListenerTable sWaveChannelTable;

/* 028A3A44 */
void WaveChannel::notify(u32 event) {
    gabi::AutoFrame frame{0x028A3A44};
    u32 target = callback;
    if (!target) return;
    u32 argument = callbackArgument;
    gabi::call_ptr<void>(target, this, event, argument);
    callbackArgument = 0;
    callback = 0;
}

/* 028A3A94 */
void WaveChannel::voiceEvent(PlaybackVoice* voice, u32 event, WaveChannel* channel) {
    gabi::AutoFrame frame{0x028A3A94};
    if (event == 0 || event == 1) {
        voice->release();
        channel->notify(event == 0 ? 2 : 3);
        channel->voice = nullptr;
        channel->allocated = 0;
        channel->active = 0;
        channel->started = 0;
    } else if (event == 2 || event == 3) {
        channel->notify(1);
        channel->allocated = 0;
        channel->voice = nullptr;
        channel->started = 0;
        channel->active = 0;
    } else {
        channel->notify(2);
        channel->voice = nullptr;
        channel->allocated = 0;
        channel->active = 0;
        channel->started = 0;
    }
}

/* 028A3EAC */
void WaveChannel::releaseToPool() {
    gabi::AutoFrame frame{0x028A3EAC};
    WaveChannelPool::instance()->release(this);
}

/* 028A3EE0 */
void WaveChannel::clearCallback() {
    callback = 0;
    callbackArgument = 0;
}

/* 028A3EF0 */
WaveChannel* WaveChannel::construct(WaveChannel* channel) {
    gabi::AutoFrame frame{0x028A3EF0};
    if (!channel) channel = (WaveChannel*)allocateWaveChannel(sizeof(WaveChannel));
    if (!channel) return channel;
    channel->owner = nullptr;
    channel->next = nullptr;
    channel->previous = nullptr;
    channel->table = &sWaveChannelTable;
    Envelope::construct(&channel->envelope);
    constructWaveArray(channel->modulators, 4, sizeof(Modulator),
        gabi::guest_fn((const void*)&Modulator::construct));
    channel->active = 0;
    channel->started = 0;
    channel->allocated = 0;
    constructWaveArray(channel->mix.groups[0].channels, 2, sizeof(ChannelMixState),
        gabi::guest_fn((const void*)&ChannelMixState::constructForWave));
    constructWaveArray(&channel->mix.groups[1], 2, sizeof(ChannelMixGroup),
        gabi::guest_fn((const void*)&ChannelMixGroup::constructForWave));
    // The inlined member constructor keeps GHS's null test of the member, so its address is formed by hand.
    WaveChannelControl* control = gabi::at<WaveChannelControl>(gabi::ea(channel) + 0x20C);
    if (!control) control = (WaveChannelControl*)allocateWaveChannel(sizeof(WaveChannelControl));
    if (control) {
        control->bus = 0;
        control->value = 0;
        control->index = 0;
        control->flags = 0;
    }
    channel->voice = nullptr;
    constructWaveArray(channel->sampleData, 4, sizeof(WaveSampleData),
        gabi::guest_fn((const void*)&WaveSampleData::construct));
    channel->poolLink.next = nullptr;
    channel->poolLink.previous = nullptr;
    channel->owner = channel;
    SoundRangeListeners::instance()->append(channel);
    return channel;
}

/* 028A400C */
void WaveChannel::destroy(WaveChannel* channel,u32 flags) {
    gabi::AutoFrame frame{0x028A400C};
    if (!channel) return;
    SoundRangeListeners::instance()->remove(channel);
    destroyWaveArray(channel->sampleData, 4, sizeof(WaveSampleData),
        gabi::guest_fn((const void*)&WaveSampleData::destroy), 0, 0);
    if (flags & 1) deleteWaveChannel(channel);
}

/* 028A4080 */
f32 WaveChannel::releaseValue() {
    f32 target = releaseTarget;
    if (target == 0.0f) return 0.0f;
    s32 elapsed = releaseElapsed;
    s32 duration = releaseDuration;
    if (elapsed >= duration) return 0.0f;
    f32 remaining = (f32)(s32)((u32)duration - (u32)elapsed);
    f32 total = (f32)duration;
    f32 scaled = gabi::fmuls_ppc(target, remaining);
    return gabi::fdivs_ppc(scaled, total);
}

/* 028A40F4 */
void WaveChannel::stop() {
    gabi::AutoFrame frame{0x028A40F4};
    PlaybackVoice* current = voice;
    if (!current) return;
    current->stop();
    voice->release();
    voice = nullptr;
    started = 0;
    active = 0;
}

/* 028A4144 */
void WaveChannel::advanceRelease(u32 ticks) {
    u32 current = releaseElapsed;
    s32 duration = releaseDuration;
    s32 next = (s32)(current + ticks);
    releaseElapsed = next;
    if (next > duration) releaseElapsed = duration;
}

/* 028A4A30 */
void WaveChannel::beginRelease() {
    gabi::AutoFrame frame{0x028A4A30};
    if (envelope.stage != Envelope::Release) {
        PlaybackVoice* current = voice;
        if (current && !hold) current->setPriority(1);
        envelope.stage = Envelope::Release;
    }
    active = 0;
}

/* 028A4A94 */
void WaveChannel::releaseUnlessIgnored() {
    gabi::AutoFrame frame{0x028A4A94};
    if (!ignoreRelease) beginRelease();
}

/* 028A4AA4 */
void WaveChannel::setPan(u32 enabled, f64 amount) {
    panEnabled = enabled;
    mix.surroundPan = gabi::f32_from_bits(ppc_stfs_bits(amount));
}

/* 028A4AB0 */
void WaveChannel::setReleaseTarget(f64 amount, u32 duration, u32 automatic) {
    releaseTarget = gabi::f32_from_bits(ppc_stfs_bits(amount));
    releaseDuration = duration;
    releaseElapsed = 0;
    autoRelease = automatic;
}

/* WWHD glue
 data sWaveChannelTable 10186D08
 ext 0273AD10 void* allocateWaveChannel(u32)
 ext 0273AF40 void deleteWaveChannel(void*)
 ext 028EFFD0 void constructWaveArray(void*,u32,u32,u32)
 ext 028F0164 void destroyWaveArray(void*,u32,u32,u32,u32,u32)
 ext 0289D598 void nw::snd::PlaybackVoice::release()
 ext 0289D6B0 void nw::snd::PlaybackVoice::stop()
 ext 0289EC48 void nw::snd::PlaybackVoice::setPriority(s32)
*/
