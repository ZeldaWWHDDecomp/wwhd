// Stream setup and loader descriptors, reconstructed from WWHD accesses.
#include "nw/snd/StreamPlayback.h"
using namespace nw::snd;
namespace nw { namespace snd {
struct StreamPlaybackConfig {
    gptr<AudioBlockPool> pool;
    be<u32> reserved;
    be<u16> trackMask;
    be<u8> codec,sourceFlag;
    be<u8> tracks[8][14];
    be<u32> firstSource,secondSource;
    be<f32> pitch;
    be<u8> mainSend,fxSend[3];
};
struct StreamPlaybackOpen {
    be<u32> positionMode;
    be<s32> offset;
    be<u32> handle[2];
    be<u8> path[0x280];
    be<u32> settings[7];
    be<u32> file[4];
};
}}
struct StreamSourceConfiguration_l {
    u8 prefix[0x10];
    gptr<StreamPlayback> owner;
    be<u32> reserved;
    gptr<void> sampleInfo;
    be<u32> codec,settings[4];
    be<u32> reserved30;
    be<u16> generation;
    be<u8> sourceFlag,enabled;
    be<u32> firstSource,secondSource;
    u8 reserved40[8];
    be<u32> handle[2];
    be<u8> path[0x280];
    be<u32> file[4];
    be<u32> finalSettings[3];
};
static u16 sStreamGeneration;
void initializeStreamReader(void*);
u32 openStreamReader(void*);
/* 0289AB94 */
void StreamPlayback::initialize() {
    reinterpret_cast<ChannelParameters*>(this)->reset();
    initializeStreamReader(&sourceState);
    sourceMainSend=127; cancelRequested=0; sourcePitch=1.0f;
    prepared=0; active=0; loadingPaused=0; consumedSamples=0; lastBlock=0; effectivePaused=0;
    for(u32 bus=0;bus<3;++bus) sourceFxSend[bus]=0;
    for(u32 index=0;index<8;++index) {
        StreamTrackParam& track=tracks[index];
        track.active=0; track.volume=1.0f; track.outputLine=0xFFFFFFFF;
        track.mainSend=0.0f; track.tvVolume=1.0f; track.pan=0.0f; track.reserved24=0; track.surroundPan=0.0f;
        for(u32 bus=0;bus<3;++bus) track.fxSend[bus]=0.0f;
        for(u32 output=0;output<2;++output) {
            auto& params=track.outputs[output];
            params.gain=1.0f; params.mainSend=0.0f; params.mode=0;
            params.pan=0.0f; params.surroundPan=0.0f;
            for(u32 bus=0;bus<3;++bus) params.auxiliarySend[bus]=0.0f;
        }
    }
    for(u32 index=0;index<16;++index) { channels[index].storage=nullptr; channels[index].voice=nullptr; }
}
/* 0289ACD8 */
u32 StreamPlayback::configure(const StreamPlaybackConfig* config) {
    codec=config->codec; sourceFlag=config->sourceFlag;
    firstSource=config->firstSource; secondSource=config->secondSource;
    sourcePitch=static_cast<f32>(config->pitch); sourceMainSend=config->mainSend;
    for(u32 bus=0;bus<3;++bus) sourceFxSend[bus]=static_cast<u8>(config->fxSend[bus]);
    u32 mask=config->trackMask;
    u32 count=0;
    while(mask) {
        if(mask&1) { if(count>=8) break; tracks[count].active=1; }
        mask>>=1; ++count;
    }
    if(count>8) count=8;
    trackCount=count;
    if(!count) {
        struct FinalizeTable { u8 prefix[0x1C]; be<u32> finalize; };
        auto* table=reinterpret_cast<FinalizeTable*>(vtable.get());
        gabi::call_ptr<void>(table->finalize,this);
        return 2;
    }
    pool=config->pool;
    for(u32 track=0;track<8;++track) {
        // The original copies three full words followed by two bytes per record.
        const auto* input=reinterpret_cast<const be<u32>*>(config->tracks[track]);
        auto* output=reinterpret_cast<be<u32>*>(trackMetadata[track]);
        for(u32 word=0;word<3;++word) output[word]=static_cast<u32>(input[word]);
        trackMetadata[track][12]=static_cast<u8>(config->tracks[track][12]);
        trackMetadata[track][13]=static_cast<u8>(config->tracks[track][13]);
    }
    active=1;
    return 0;
}
/* 0289AE20 */
u32 StreamPlayback::open(const StreamPlaybackOpen* config) {
    if(!active) return 0;
    auto* loader=reinterpret_cast<StreamSourceConfiguration_l*>(&sourceState);
    loader->handle[0]=static_cast<u32>(config->handle[0]);
    loader->handle[1]=static_cast<u32>(config->handle[1]);
    positionMode=config->positionMode;
    startOffset=config->offset;
    starved=0;
    started=1;
    u16 generationValue=sStreamGeneration;
    sStreamGeneration=static_cast<u16>(generationValue+1);
    loader->codec=static_cast<u8>(codec);
    loader->secondSource=secondSource;
    loader->generation=generationValue;
    generation=generationValue;
    loader->sampleInfo=&sampleFormat;
    loader->owner=this;
    loader->enabled=1;
    loader->firstSource=firstSource;
    loader->sourceFlag=sourceFlag;
    u32 length=0;
    while(length<0x27E) {
        u8 character=config->path[length];
        if(!character) break;
        loader->path[length]=character; ++length;
    }
    loader->path[length]=0;
    loader->file[0]=static_cast<u32>(config->file[0]);
    loader->file[1]=static_cast<u32>(config->file[1]);
    loader->reserved=0;
    for(u32 index=0;index<4;++index) loader->settings[index]=static_cast<u32>(config->settings[index]);
    loader->file[2]=static_cast<u32>(config->file[2]);
    loader->file[3]=static_cast<u32>(config->file[3]);
    for(u32 index=0;index<3;++index) loader->finalSettings[index]=static_cast<u32>(config->settings[index+4]);
    openStreamReader(&sourceState);
    return 1;
}
/* WWHD glue
data sStreamGeneration 101FC904
ext 028A36C8 void nw::snd::ChannelParameters::reset()
ext 028AE510 void initializeStreamReader(void*)
ext 028AE790 u32 openStreamReader(void*)
*/
