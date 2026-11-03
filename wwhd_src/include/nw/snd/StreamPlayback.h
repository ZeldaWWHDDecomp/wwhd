// Stream player views recovered from WWHD accesses. Names are our own.
#pragma once
#include "nw/snd/AudioBlockPool.h"
#include "nw/snd/AudioWorkQueue.h"
#include "nw/snd/ChannelParameters.h"
#include "nw/snd/PlaybackVoice.h"
#include "nw/snd/SequenceManager.h"
namespace nw { namespace snd {
// This translation unit's own copies of the inline ChannelMixState/ChannelMixGroup constructors.
ChannelMixState* constructStreamMixState(ChannelMixState* state);
ChannelMixGroup* constructStreamMixGroup(ChannelMixGroup* group);
struct StreamPlaybackChannel;
struct StreamTrackParam {
    be<u8> active;
    u8 padding1[3];
    gptr<StreamPlaybackChannel> channels[2];
    be<u8> channelCount,volumeByte,panByte,surroundByte,mainSendByte;
    be<u8> fxSendBytes[3];
    be<u8> lpfByte;
    be<s8> biquadType;
    be<u8> biquadByte,frontBypass;
    be<f32> volume;
    be<u32> outputLine;
    be<f32> tvVolume;
    be<u32> reserved24;
    ChannelMixState mix[2];
    be<f32> pan,surroundPan,mainSend,fxSend[3];
    ChannelMixGroup outputs[2];
    static StreamTrackParam* construct(StreamTrackParam*);
};
struct StreamAdpcmState {
    be<u32> coefficients[4];
    be<u32> position;
    be<u8> loaded;
    u8 padding15[3];
    be<u32> state,reserved;
    static StreamAdpcmState* construct(StreamAdpcmState*);
};
struct StreamPlaybackChannel {
    gptr<void> storage;
    gptr<PlaybackVoice> voice;
    StreamAdpcmState adpcm[32];
    be<u16> coefficients[32][3];
    static StreamPlaybackChannel* construct(StreamPlaybackChannel*);
    void queueSamples(StreamAdpcmState*,u32 flag);
    static void destroy(StreamPlaybackChannel*,u32 flags);
};
struct StreamPlaybackVtable { u8 prefix[0x2C]; be<u32> hook; };
struct StreamPlayback {
    nw::ut::Event event;
    be<u8> started;
    be<u8> playing;
    be<u8> paused;
    be<u8> finished;
    be<u8> stopRequested;
    u8 reserved29[0x14B];
    gptr<StreamPlaybackVtable> vtable;
    nw::ut::HeapLink link;
    be<u32> listenerVtable;
    be<u8> active;
    be<u8> prepared;
    be<u8> cancelRequested;
    u8 reserved187;
    be<u8> effectivePaused,loadingPaused;
    be<u8> lastBlock,starved;
    be<u32> consumedSamples,pendingLoads;
    SequenceManager sourceState;
    gptr<AudioBlockPool> pool;
    be<u32> blockCount,readBlock,writeBlock,completedBlock;
    be<u32> positionMode;
    be<s32> startOffset;
    be<u16> generation;
    be<u8> codec,sourceFlag;
    be<u32> sampleFormat,sampleRate;
    be<u8> looped;
    u8 reserved1A11[0xF];
    be<u32> totalSamples,reserved1A24;
    be<u32> blockPayloadBytes;
    u8 reserved1A2C[8];
    be<u32> fileChannelCount;
    u8 reserved1A38[4];
    be<u8> trackMetadata[8][14];
    be<u32> firstSource,secondSource;
    be<f32> sourcePitch;
    be<u8> sourceMainSend,sourceFxSend[3];
    be<u32> channelCount,trackCount;
    StreamPlaybackChannel channels[16];
    StreamTrackParam tracks[8];
    struct BlockInfo { be<u32> first,sampleCount,consumed; };
    BlockInfo blocks[32];
    static StreamPlayback* construct(StreamPlayback*);
    void releaseChannels();
    void releaseVoices();
    void finalize();
    void destroy(u32 flags);
    void updatePause();
    void setPaused(u32 paused);
    void setTrackVolume(u32 mask,f64 value);
    void setTrackVolumeByte(u32 mask,u8 value);
    void setTrackOutputLine(u32 mask,u32 value);
    void resetTrackOutputLine(u32 mask);
    void setTrackTvVolume(u32 mask,f64 value);
    void setTrackMix(u32 mask,u32 set,const ChannelMixState* value);
    void setTrackPan(u32 mask,f64 value);
    void setTrackSurroundPan(u32 mask,f64 value);
    void setTrackMainSend(u32 mask,f64 value);
    void setTrackFxSend(u32 mask,u32 bus,f64 value);
    void setTrackOutputVolume(u32 output,u32 mask,f64 value);
    void setTrackOutputMix(u32 output,u32 mask,u32 set,const ChannelMixState* value);
    void setTrackOutputPan(u32 output,u32 mask,f64 value);
    void setTrackOutputSurroundPan(u32 output,u32 mask,f64 value);
    void setTrackOutputMainSend(u32 output,u32 mask,f64 value);
    void setTrackOutputFxSend(u32 output,u32 mask,u32 bus,f64 value);
    u32 allocateChannels();
    u32 configureBlocks();
    void forwardHook();
    void queueRead();
    u32 allBlocksIdle();
    void retireBlocks();
    void update();
    void updateTrack(StreamTrackParam*);
    void listenerUpdate();
    void listenerHook();
    void finishHook();
    void initialize();
    u32 configure(const struct StreamPlaybackConfig*);
    u32 open(const struct StreamPlaybackOpen*);
    void beginPlayback();
    void stopPlayback();
    u32 headerLoaded(u32 success,const gptr<void>* channelContexts,u32 generation);
    u32 blockLoaded(u32 success,u32 block,u32 samples,u32 first,u32 state,u32 haveAdpcm,const be<u16>* coefficients,u32 consumed,u32 last);
};
WWHD_SIZE(StreamTrackParam,0x110); WWHD_SIZE(StreamAdpcmState,0x20);
WWHD_SIZE(StreamPlaybackChannel,0x4C8); WWHD_SIZE(StreamPlayback,0x7144);
WWHD_OFFSET(StreamPlayback,pool,0x19E8); WWHD_OFFSET(StreamPlayback,channelCount,0x1ABC);
WWHD_OFFSET(StreamPlayback,tracks,0x6744); WWHD_OFFSET(StreamTrackParam,outputs,0x70);
}}

u32 streamPlaybackSampleOffset(nw::snd::StreamPlayback*);
u32 allocatePlaybackVoices(nw::snd::StreamPlayback*);
