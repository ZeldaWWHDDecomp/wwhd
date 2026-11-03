// Stream loader completion callbacks, reconstructed from original WWHD instructions.
#include "nw/snd/StreamPlayback.h"
using namespace nw::snd;
static void invokeStreamFinalize(StreamPlayback* player) {
    struct FinalizeTable { u8 prefix[0x1C]; be<u32> function; };
    auto* table=reinterpret_cast<FinalizeTable*>(player->vtable.get());
    gabi::call_ptr<void>(table->function,player);
}
/* 0289B86C */
u32 StreamPlayback::headerLoaded(u32 success,const gptr<void>* channelContexts,u32 loadedGeneration) {
    if(!active || loadedGeneration!=static_cast<u16>(generation)) return 0;
    if(!success) { finished=1; gabi::call_ptr<void>(vtable->hook,this); return 0; }
    u32 count=fileChannelCount;
    if(count>16) count=16;
    channelCount=count;
    if(!allocateChannels()) { invokeStreamFinalize(this); stopRequested=1; return 1; }
    if(!looped && streamPlaybackSampleOffset(this)>=static_cast<u32>(totalSamples)) {
        finished=1; gabi::call_ptr<void>(vtable->hook,this); return 0;
    }
    count=trackCount;
    for(u32 track=0;track<count;++track) {
        StreamTrackParam& out=tracks[track];
        be<u8>* in=trackMetadata[track];
        out.volumeByte=static_cast<u8>(in[0]); out.panByte=static_cast<u8>(in[1]);
        out.surroundByte=static_cast<u8>(in[2]); out.mainSendByte=static_cast<u8>(in[4]);
        for(u32 bus=0;bus<3;++bus) out.fxSendBytes[bus]=static_cast<u8>(in[bus+5]);
        out.lpfByte=static_cast<u8>(in[8]); out.biquadType=static_cast<s8>(static_cast<u8>(in[9]));
        out.biquadByte=static_cast<u8>(in[10]); out.frontBypass=static_cast<u8>(in[3]);
        out.channelCount=static_cast<u8>(in[11]);
        for(u32 channel=0;channel<static_cast<u8>(in[11]);++channel)
            out.channels[channel]=&channels[static_cast<u8>(in[12+channel])];
        count=trackCount;
    }
    if(!configureBlocks()) return 0;
    pendingLoads=0;
    count=blockCount;
    for(u32 block=0;block<count;++block) {
        queueRead();
        pendingLoads=static_cast<u32>(pendingLoads)+1;
        count=blockCount;
    }
    if(!allocatePlaybackVoices(this)) { releaseChannels(); return 0; }
    count=channelCount;
    for(u32 channel=0;channel<count;++channel) {
        void* context=channelContexts[channel];
        if(context) { channels[channel].voice->setAdpcmCoefficients(0,static_cast<const be<u16>*>(context)); count=channelCount; }
    }
    return 1;
}
/* 0289BC0C */
u32 StreamPlayback::blockLoaded(u32 success,u32 block,u32 samples,u32 first,u32 stateValue,u32 haveAdpcm,const be<u16>* inputCoefficients,u32 consumed,u32 last) {
    if(!active) return 0;
    if(!success) { finished=1; gabi::call_ptr<void>(vtable->hook,this); return 0; }
    if(samples) {
        u32 count=channelCount;
        for(u32 channel=0;channel<count;++channel) {
            StreamPlaybackChannel& output=channels[channel];
            u32 stride=static_cast<u32>(blockPayloadBytes)+0x900;
            void* buffer=static_cast<u8*>(output.storage.get())+stride*block;
            be<u16>* coefficients=nullptr;
            if(haveAdpcm) {
                coefficients=output.coefficients[block];
                coefficients[0]=static_cast<u16>(inputCoefficients[channel*3]);
                coefficients[1]=static_cast<u16>(inputCoefficients[channel*3+1]);
                coefficients[2]=static_cast<u16>(inputCoefficients[channel*3+2]);
            }
            StreamAdpcmState& state=output.adpcm[block];
            state.coefficients[2]=stateValue;
            state.reserved=0;
            state.coefficients[0]=gabi::ea(buffer);
            state.coefficients[3]=gabi::ea(coefficients);
            state.coefficients[1]=samples;
            state.loaded=0; state.position=0; state.state=0;
            output.queueSamples(&state,static_cast<u8>(last));
            count=channelCount;
        }
        blocks[block].first=first;
        blocks[block].consumed=consumed;
        blocks[block].sampleCount=samples;
    }
    u8 wasPrepared=prepared;
    if(static_cast<u8>(last)) lastBlock=1;
    if(!wasPrepared) {
        u32 remaining=static_cast<u32>(pendingLoads)-1;
        pendingLoads=remaining;
        if(remaining==0 || lastBlock) prepared=1;
    }
    return 1;
}
/* WWHD glue
ext 0289F1D0 void nw::snd::PlaybackVoice::setAdpcmCoefficients(u32,const be<u16>*)
*/
