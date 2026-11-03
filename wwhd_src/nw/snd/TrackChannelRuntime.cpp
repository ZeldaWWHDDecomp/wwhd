/* Track-to-channel parameter composition and channel unlinking, reconstructed from WWHD. */
#include "nw/snd/TrackChannelRuntime.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;

/* 0288CE6C */
void ChannelRuntime_l::detach(u32,TrackRuntimeControl* track) {
    SequenceRuntimeSupport* sequence = track->owner;
    if (sequence) gabi::call_ptr<void>(sequence->vtable->detachChannel,sequence,this);
    ChannelRuntime_l* previous = track->channels;
    if (previous == this) {
        track->channels = next.get();
        return;
    }
    ChannelRuntime_l* current = previous->next;
    while (current) {
        if (current == this) {
            previous->next = next.get();
            return;
        }
        previous = current;
        current = previous->next;
    }
}
static inline u8 unsignedRampValue(const ByteRamp_l& ramp) {
    s32 counter = ramp.counter;
    s32 frames = ramp.frames;
    if (counter >= frames) return ramp.target;
    s32 origin = ramp.origin;
    s32 difference = (s32)(u8)ramp.target - origin;
    return (u8)(origin + (s32)ppc_divw(difference * counter,frames));
}
static inline s8 signedRampValue(const ByteRamp_l& ramp) {
    s32 counter = ramp.counter;
    s32 frames = ramp.frames;
    if (counter >= frames) return (s8)(u8)ramp.target;
    s32 origin = (s8)(u8)ramp.origin;
    s32 difference = (s32)(s8)(u8)ramp.target - origin;
    return (s8)(origin + (s32)ppc_divw(difference * counter,frames));
}
static inline void copyTrackOutput(TrackOutputControl* destination,const TrackOutputControl* source) {
    be<u32>* to = reinterpret_cast<be<u32>*>(destination);
    const be<u32>* from = reinterpret_cast<const be<u32>*>(source);
    for (u32 i = 0; i < sizeof(TrackOutputControl) / 4; ++i) to[i] = (u32)from[i];
}
/* 0288D2E8 */
void TrackRuntimeControl::updateChannels() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!activeFlag || !channels) return;
    u32 volumeLevel = unsignedRampValue(volumeRamp);
    u32 expressionLevel = unsignedRampValue(expressionRamp);
    SequenceRuntimeSupport* sequence = owner;
    ByteRamp_l* playerExpression = reinterpret_cast<ByteRamp_l*>(&sequence->modulationStart);
    u32 playerLevel = unsignedRampValue(*playerExpression);
    f32 scale = gabi::fdivs_ppc((f32)(s32)(volumeLevel * expressionLevel * playerLevel),2048383.0f);
    f32 channelVolume = gabi::fmuls_ppc(scale,scale);
    channelVolume = gabi::fmuls_ppc(channelVolume,volume);
    channelVolume = gabi::fmuls_ppc(channelVolume,sequence->volume);
    f32 bend = (f32)signedRampValue(pitchBendRamp);
    f32 channelPitch = gabi::fmuls_ppc(sequence->pitch,pitch);
    bend = gabi::fmuls_ppc(bend,0.0078125f);
    f32 channelLpf = gabi::fadds_ppc(lpfOffset,sequence->lpfOffset);
    f32 filterValue = biquadValue;
    s32 filterType = biquadType;
    u32 line = outputLine;
    f32 pitchBend = gabi::fmuls_ppc(bend,(f32)(u8)pitchBendRange);
    if (sequence->biquadType != -1) {
        filterType = sequence->biquadType;
        filterValue = sequence->biquadValue;
    }
    u32 channelLine = sequence->defaultOutputLine;
    if (line != 0xFFFFFFFFu) channelLine = line;
    f32 pan = gabi::fdivs_ppc((f32)signedRampValue(panOffsetRamp),63.0f);
    if (pan > 1.0f) pan = 1.0f;
    else if (!(pan >= -1.0f)) pan = -1.0f;
    pan = gabi::fmuls_ppc(pan,panRange);
    pan = gabi::fmuls_ppc(pan,sequence->panRange);
    f32 surround = gabi::fdivs_ppc((f32)signedRampValue(surroundOffsetRamp),63.0f);
    if (surround > 2.0f) surround = 2.0f;
    else if (!(surround >= 0.0f)) surround = 0.0f;
    f32 mainSendOffset = gabi::fsubs_ppc(gabi::fdivs_ppc((f32)(u8)priority,127.0f),1.0f);
    struct EffectOffsets { be<f32> value[3]; };
    gabi::FrameLocal<EffectOffsets> effects(0x60);
    for (u32 i = 0; i < 3; ++i) effects->value[i] = gabi::fdivs_ppc((f32)(u8)envelopeFlags[i],127.0f);
    gabi::FrameLocal<TrackOutputControl> tv(0x10);
    copyTrackOutput(tv.get(),&sequence->tvOutput);
    tv->volume = gabi::fmuls_ppc(sequence->tvOutput.volume,tvVolume);
    tv->pan = gabi::fadds_ppc(gabi::fadds_ppc(sequence->tvOutput.pan,pan),tvPan);
    tv->surroundPan = gabi::fadds_ppc(gabi::fadds_ppc(sequence->tvOutput.surroundPan,surround),tvSurroundPan);
    tv->mainSend = gabi::fadds_ppc(gabi::fadds_ppc(sequence->tvOutput.mainSend,mainSendOffset),tvMainSend);
    for (u32 i = 0; i < 3; ++i) {
        f32 send = gabi::fadds_ppc(tv->effectSend[i],effects->value[i]);
        tv->effectSend[i] = gabi::fadds_ppc(send,tvEffectSend[i]);
    }
    struct TrackOutputs { TrackOutputControl line[2]; };
    gabi::FrameLocal<TrackOutputs> outputs(0x6C);
    __construct_array(outputs->line,2,0x50,runtimeTrackOutputConstruct);
    sequence = owner;
    for (u32 i = 0; i < 2; ++i) {
        TrackOutputControl* out = &outputs->line[i];
        TrackOutputControl* track = &output[i];
        copyTrackOutput(out,&sequence->output[i]);
        f32 panOffset = gabi::fadds_ppc(out->pan,pan);
        f32 sendOffset = gabi::fadds_ppc(out->mainSend,mainSendOffset);
        f32 volumeScale = gabi::fmuls_ppc(out->volume,track->volume);
        f32 finalPan = gabi::fadds_ppc(panOffset,track->pan);
        f32 surroundOffset = gabi::fadds_ppc(out->surroundPan,surround);
        f32 finalSurround = gabi::fadds_ppc(surroundOffset,track->surroundPan);
        f32 finalSend = gabi::fadds_ppc(sendOffset,track->mainSend);
        out->volume = volumeScale;
        out->pan = finalPan;
        out->surroundPan = finalSurround;
        out->mainSend = finalSend;
        for (u32 effect = 0; effect < 3; ++effect) {
            f32 send = gabi::fadds_ppc(out->effectSend[effect],effects->value[effect]);
            out->effectSend[effect] = gabi::fadds_ppc(send,track->effectSend[effect]);
        }
    }
    ChannelRuntime_l* channel = channels;
    while (channel) {
        channel->volume = channelVolume;
        channel->pitch = channelPitch;
        channel->lpfOffset = channelLpf;
        channel->pitchBend = pitchBend;
        runtimeChannelBiquad(channel,filterType,filterValue);
        channel->outputLine = channelLine;
        for (u32 i = 0; i < 4; ++i) {
            const be<u32>* state = reinterpret_cast<const be<u32>*>(modulationState[i]);
            for (u32 word = 0; word < 4; ++word) channel->modulation[i].state[word] = (u32)state[word];
            channel->modulationType[i] = (u8)modulationType[i];
        }
        copyTrackOutput(&channel->tvOutput,tv.get());
        for (u32 i = 0; i < 2; ++i) copyTrackOutput(&channel->output[i],&outputs->line[i]);
        for (u32 i = 0; i < 4; ++i) {
            sequence = owner;
            for (u32 word = 0; word < 3; ++word) channel->remoteOutput[i][word] = (u32)sequence->remoteOutput[i][word];
        }
        channel = channel->next;
    }
}
/* WWHD glue
ext 028A4AA4 void runtimeChannelBiquad(nw::snd::ChannelRuntime_l*,s32,f32)
*/
