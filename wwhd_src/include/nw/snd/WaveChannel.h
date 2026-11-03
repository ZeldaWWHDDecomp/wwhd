/* Wave-channel state and pool, reconstructed from WWHD constructors and call sites. */
#pragma once
#include "nw/snd/Envelope.h"
#include "nw/snd/Modulator.h"
#include "nw/snd/ChannelParameters.h"
#include "nw/snd/SoundRangeListeners.h"
#include "nw/snd/WaveInfo.h"
#include "nw/snd/PlaybackVoice.h"
#include "nw/snd/WaveArchiveReader.h"
namespace nw { namespace snd {
// One queued block of wave samples for a playback channel. It has the first 0x20 bytes of the hardware
// voice's VoiceRequest (HardwareVoice.h, which adds 8 private bytes): the voice queues it through
// HardwareVoice/SoftwareVoice::appendRequest and moves it through the WaveRequestState values.
enum WaveRequestState { RequestIdle, RequestQueued, RequestPlaying, RequestDone };
struct WaveSampleData {
    gptr<const void> samples;
    be<u32> sampleCount;
    be<u32> loopStart;
    gptr<const WaveAdpcmState> adpcm;
    be<u32> context;
    be<u8> active;
    u8 padding[3];
    be<u32> state;
    gptr<WaveSampleData> next;
    static WaveSampleData* construct(WaveSampleData* data);
    static void destroy(WaveSampleData* data, u32 flags);
};
WWHD_OFFSET(WaveSampleData, adpcm, 0xC);
WWHD_OFFSET(WaveSampleData, active, 0x14);
WWHD_OFFSET(WaveSampleData, state, 0x18);
WWHD_OFFSET(WaveSampleData, next, 0x1C);
WWHD_SIZE(WaveSampleData, 0x20);
struct WaveChannelMix {
    be<f32> gain, pitch, pan, surroundPan;
    be<u32> mode;
    ChannelMixGroup groups[3];
    ChannelOutputGain outputs[4];
};
WWHD_OFFSET(WaveChannelMix, groups, 0x14);
WWHD_OFFSET(WaveChannelMix, outputs, 0x104);
WWHD_SIZE(WaveChannelMix, 0x134);
struct WaveChannelControl {
    be<u8> index, bus;
    be<u16> value, flags;
};
WWHD_SIZE(WaveChannelControl, 6);
struct WaveChannelPool;
struct BankReader;
struct WaveChannel : SoundRangeListener {
    gptr<WaveChannel> owner;
    Envelope envelope;
    Modulator modulators[4];
    be<u8> modulationTargets[4];
    be<u8> active, started, allocated, autoRelease, hold, ignoreRelease, panEnabled;
    u8 mixPadding;
    WaveChannelMix mix;
    be<f32> streamValue;
    be<f32> releaseTarget;
    be<s32> releaseElapsed, releaseDuration;
    be<f32> volume, pan, send, pitch;
    WaveChannelControl control;
    u8 filterPadding[2];
    be<f32> filter, scale;
    be<u32> sourceType;
    gptr<void> track;
    be<u32> flags;
    be<u8> key, baseKey, interpolation, outputMode;
    be<u32> callback, callbackArgument;
    gptr<PlaybackVoice> voice;
    gptr<void> context;
    WaveSampleData sampleData[4];
    WaveAdpcmState initialStates[2];
    WaveAdpcmState loopStates[2];
    u8 positionPadding[4];
    be<u32> position;
    be<u8> looping, ending;
    u8 listPadding[2];
    SoundRangeLink poolLink;
    static WaveChannel* construct(WaveChannel* channel);
    static void destroy(WaveChannel* channel,u32 flags);
    void notify(u32 event);
    static void voiceEvent(PlaybackVoice* voice, u32 event, WaveChannel* channel);
    static WaveChannel* playBankInstrument(void* unused, BankReader* bank, WaveArchiveReader* archive, const BankNote* note);
    static u32 currentOutputMode();
    void initialize(u32 callback, u32 argument);
    static WaveChannel* allocate(u32 channelCount,u32 priority,u32 callback,u32 argument,u32 mode);
    void releaseToPool();
    void invalidateRange(const void* start,const void* end);
    void clearCallback();
    f32 releaseValue();
    void stop();
    void advanceRelease(u32 ticks);
    void beginRelease();
    void releaseUnlessIgnored();
    void setPan(u32 enabled, f64 amount);
    void setReleaseTarget(f64 amount, u32 duration, u32 automatic);
    void start(const WaveDescription* wave, u32 startFrame, u32 sampleOffset);
    void prepareWave(const WaveDescription* wave, u32 sampleOffset);
    void update(u32 ticks);
};
WWHD_OFFSET(WaveChannel, owner, 0xC);
WWHD_OFFSET(WaveChannel, envelope, 0x10);
WWHD_OFFSET(WaveChannel, modulators, 0x2C);
WWHD_OFFSET(WaveChannel, active, 0xB0);
WWHD_OFFSET(WaveChannel, mix, 0xB8);
WWHD_OFFSET(WaveChannel, streamValue, 0x1EC);
WWHD_OFFSET(WaveChannel, releaseTarget, 0x1F0);
WWHD_OFFSET(WaveChannel, control, 0x20C);
WWHD_OFFSET(WaveChannel, key, 0x228);
WWHD_OFFSET(WaveChannel, callback, 0x22C);
WWHD_OFFSET(WaveChannel, voice, 0x234);
WWHD_OFFSET(WaveChannel, sampleData, 0x23C);
WWHD_OFFSET(WaveChannel, position, 0x2D8);
WWHD_OFFSET(WaveChannel, poolLink, 0x2E0);
WWHD_SIZE(WaveChannel, 0x2E8);
struct WaveChannelPool {
    gptr<void> storage;
    be<u32> capacity;
    be<u32> freeCount;
    SoundRangeListeners channels;
    be<u8> initialized;
    u8 padding[3];
    be<u32> count;
    static WaveChannelPool* construct(WaveChannelPool* pool);
    static WaveChannelPool* instance();
    static u32 requiredBytes(void* unused, u32 count);
    void initialize(void* storage, u32 size);
    void release(WaveChannel* channel);
    void finish();
    WaveChannel* allocate();
    void update();
};
WWHD_OFFSET(WaveChannelPool, channels, 0xC);
WWHD_OFFSET(WaveChannelPool, initialized, 0x18);
WWHD_OFFSET(WaveChannelPool, count, 0x1C);
WWHD_SIZE(WaveChannelPool, 0x20);
} }
