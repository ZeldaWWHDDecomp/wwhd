// Stream buffer progression and playback scheduling, reconstructed from WWHD.
#include "nw/snd/StreamPlayback.h"
#include "nw/snd/AudioFrameDriver.h"
using namespace nw::snd;
Pair32 divideStreamPosition(u32,u32,u32,u32);
void requestStreamRead(void*,gptr<void>*,u32,u32,u32);
u32 streamReaderBusy(void*);
void updateStreamReader(void*);
void stopStreamReader(void*);
static u8 sStreamForcePause;
/* inlined into StreamPlayback::queueRead and StreamPlayback::headerLoaded */
u32 streamPlaybackSampleOffset(StreamPlayback* player) {
    if(player->positionMode==0) return static_cast<u32>(player->startOffset);
    if(player->positionMode!=1) return 0;
    s64 product=static_cast<s64>(static_cast<s32>(player->startOffset))*static_cast<u32>(player->sampleRate);
    u64 bits=static_cast<u64>(product);
    auto result=divideStreamPosition(static_cast<u32>(bits>>32),static_cast<u32>(bits),0,1000);
    return result.r3 ? 0xFFFFFFFF : result.r4;
}
/* 02899BB4 */
void StreamPlayback::queueRead() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<void>[16]> buffers(8);
    u32 count=channelCount;
    if(count) {
        u32 stride=static_cast<u32>(blockPayloadBytes)+0x900;
        u32 offset=stride*static_cast<u32>(readBlock);
        for(u32 index=0;index<count;++index) (*buffers)[index]=static_cast<u8*>(channels[index].storage.get())+offset;
    }
    u32 offset=streamPlaybackSampleOffset(this);
    u32 flags=playing ? 2 : 1;
    startOffset=0;
    u32 block=readBlock;
    requestStreamRead(&sourceState,&(*buffers)[0],block,offset,flags);
    u32 next=static_cast<u32>(readBlock)+1;
    readBlock=next;
    if(next>=static_cast<u32>(blockCount)) readBlock=0;
}
/* 02899CE8 */
u32 StreamPlayback::allBlocksIdle() {
    u32 count=blockCount;
    for(u32 index=0;index<count;++index) {
        u32 state=channels[0].adpcm[index].state;
        if(state>=1 && state<=2) return 0;
    }
    return 1;
}
/* 02899D28 */
void StreamPlayback::retireBlocks() {
    if(!playing || !tracks[0].active) return;
    if(sStreamForcePause) { loadingPaused=1; updatePause(); }
    for(;;) {
        u32 block=writeBlock;
        u32 count=channelCount;
        bool complete=true;
        for(u32 channel=0;channel<count;++channel) {
            if(static_cast<u32>(channels[channel].adpcm[block].state)!=3) { complete=false; break; }
        }
        if(!complete) break;
        count=channelCount;
        for(u32 channel=0;channel<count;++channel) {
            u32 current=writeBlock;
            StreamAdpcmState& state=channels[channel].adpcm[current];
            state.loaded=0; state.position=0; state.reserved=0; state.state=0;
            state.coefficients[1]=0; state.coefficients[0]=0; state.coefficients[3]=0; state.coefficients[2]=0;
            count=channelCount;
        }
        block=writeBlock;
        consumedSamples=static_cast<u32>(consumedSamples)+static_cast<u32>(blocks[block].consumed);
        queueRead();
        u32 previous=writeBlock;
        u32 next=previous+1;
        completedBlock=previous; writeBlock=next;
        if(next>=static_cast<u32>(blockCount)) writeBlock=0;
    }
    if(!loadingPaused && streamReaderBusy(&sourceState) && allBlocksIdle()) {
        starved=1; loadingPaused=1; updatePause();
    }
}
/* 0289A984 */
void StreamPlayback::update() {
    updateStreamReader(&sourceState);
    retireBlocks();
    if(playing) {
        u32 count=channelCount;
        for(u32 channel=0;channel<count;++channel) {
            if(!channels[channel].voice) { gabi::call_ptr<void>(vtable->hook,this); finished=1; return; }
        }
        count=trackCount;
        for(u32 index=0;index<count;++index) { updateTrack(&tracks[index]); count=trackCount; }
    }
    if(loadingPaused && !streamReaderBusy(&sourceState) && !sStreamForcePause) {
        loadingPaused=0; updatePause();
    }
    if(starved) starved=0;
}
/* 0289AACC */
void StreamPlayback::listenerUpdate() {
    reinterpret_cast<StreamPlayback*>(reinterpret_cast<u8*>(this)-0x178)->finishHook();
}
/* 0289AAD4 */
void StreamPlayback::listenerHook() {
    reinterpret_cast<StreamPlayback*>(reinterpret_cast<u8*>(this)-0x178)->forwardHook();
}
/* 0289AF6C */
void StreamPlayback::beginPlayback() {
    if(playing) return;
    u32 count=trackCount;
    for(u32 track=0;track<count;++track) {
        StreamTrackParam& parameters=tracks[track];
        if(parameters.active) {
            u32 voices=parameters.channelCount;
            for(u32 channel=0;channel<voices;++channel) {
                StreamPlaybackChannel* source=parameters.channels[channel];
                if(!source) continue;
                PlaybackVoice* voice=source->voice;
                if(!voice) continue;
                voice->setSampleFormat(sampleFormat);
                voice->setSampleRate(sampleRate);
                voice->requestStart();
                voices=parameters.channelCount;
            }
            count=trackCount;
        }
    }
    updatePause();
    AudioFrameDriver::instance()->addListener(&link);
    playing=1;
}
/* 0289B048 */
void StreamPlayback::stopPlayback() {
    stopStreamReader(&sourceState);
    u32 count=channelCount;
    for(u32 channel=0;channel<count;++channel) {
        PlaybackVoice* voice=channels[channel].voice;
        if(voice) { voice->stop(); count=channelCount; }
    }
    if(playing) { AudioFrameDriver::instance()->removeListener(&link); playing=0; }
}
/* 0289BF00 */
void StreamPlayback::finishHook() { update(); }
/* WWHD glue
data sStreamForcePause 101FC8F1
ext 028F5A1C Pair32 divideStreamPosition(u32,u32,u32,u32)
ext 028AE7D8 void requestStreamRead(void*,gptr<void>*,u32,u32,u32)
ext 028AE768 u32 streamReaderBusy(void*)
ext 028AE660 void updateStreamReader(void*)
ext 028AE574 void stopStreamReader(void*)
ext 0289EAB0 void nw::snd::PlaybackVoice::setSampleFormat(u32)
ext 0289EAE8 void nw::snd::PlaybackVoice::setSampleRate(u32)
ext 0289D624 void nw::snd::PlaybackVoice::requestStart()
ext 0289D6B0 void nw::snd::PlaybackVoice::stop()
*/
