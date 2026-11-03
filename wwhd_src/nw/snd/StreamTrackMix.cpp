// Stream track mixing, reconstructed from the original game's arithmetic and call order.
#include "nw/snd/StreamPlayback.h"
#include "nw/snd/DriverCommandDispatch.h"
using namespace nw::snd;
using TrackMixConstructor=ChannelMixState*(*)(ChannelMixState*);
using TrackOutputConstructor=ChannelMixGroup*(*)(ChannelMixGroup*);
void constructTrackMixArray(void*,u32,u32,TrackMixConstructor);
void constructTrackOutputArray(void*,u32,u32,TrackOutputConstructor);
static f32 byteVolume(u32 value) { return gabi::fdivs_ppc(static_cast<f32>(value),127.0f); }
static void copyMix(ChannelMixState* out,const ChannelMixState* input,u32 count) {
    auto* to=reinterpret_cast<be<f32>*>(out);
    const auto* from=reinterpret_cast<const be<f32>*>(input);
    for(u32 index=0;index<count*6;++index) to[index]=static_cast<f32>(from[index]);
}
static void copyOutputWords(ChannelMixGroup* out,const DriverOutputWords* input) {
    auto* words=reinterpret_cast<be<u32>*>(out);
    for(u32 index=0;index<20;index+=4) {
        words[index]=static_cast<u32>(input->words[index]);
        words[index+1]=static_cast<u32>(input->words[index+1]);
        words[index+2]=static_cast<u32>(input->words[index+2]);
        words[index+3]=static_cast<u32>(input->words[index+3]);
    }
}
/* 02899EC4 */
void StreamPlayback::updateTrack(StreamTrackParam* track) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!track->active) return;
    auto* base=reinterpret_cast<DriverPlayerParameters*>(this);
    f32 pitch=gabi::fmuls_ppc(gabi::f32_from_bits(base->pitchBits),sourcePitch);
    f32 volume=gabi::fmuls_ppc(gabi::fmuls_ppc(gabi::f32_from_bits(base->volumeBits),byteVolume(track->volumeByte)),track->volume);
    if(codec==1 && pitch>3.0f) pitch=3.0f;
    f32 lpf=gabi::fmadds(static_cast<f32>(static_cast<u8>(track->lpfByte)),0.015625f,gabi::f32_from_bits(base->lpfBits));
    f32 biquad=byteVolume(track->biquadByte);
    s32 biquadType=track->biquadType;
    s8 baseType=gabi::load<s8>(gabi::ea(base)+0x3C);
    if(baseType!=-1) {
        biquad=gabi::load<f32>(gabi::ea(base)+0x38);
        biquadType=baseType;
    }
    u32 outputLine=track->outputLine;
    if(outputLine==0xFFFFFFFF) outputLine=base->outputLine;
    u32 panByte=track->panByte;
    f32 pan=gabi::fdivs_ppc(static_cast<f32>(static_cast<s32>(panByte)-(panByte<=1?63:64)),63.0f);
    u32 surroundByte=track->surroundByte;
    f32 surround=surroundByte>63 ? gabi::fmuls_ppc(static_cast<f32>(surroundByte+1),0.015625f) : gabi::fdivs_ppc(static_cast<f32>(surroundByte),63.0f);
    f32 mainOffset=gabi::fsubs_ppc(byteVolume(track->mainSendByte),1.0f);
    gabi::FrameLocal<be<f32>[3]> sends(0x98);
    for(u32 bus=0;bus<3;++bus) (*sends)[bus]=byteVolume(track->fxSendBytes[bus]);
    gabi::FrameLocal<ChannelMixGroup> tv(0x18);
    constructTrackMixArray(tv->channels,2,0x18,&constructStreamMixState);
    copyOutputWords(tv.get(),&base->outputs[0]);
    tv->gain=gabi::fmuls_ppc(tv->gain,track->tvVolume);
    copyMix(tv->channels,track->mix,2);
    f32 sourceMainOffset=gabi::fsubs_ppc(byteVolume(sourceMainSend),1.0f);
    tv->pan=gabi::fadds_ppc(tv->pan,gabi::fadds_ppc(pan,track->pan));
    tv->surroundPan=gabi::fadds_ppc(tv->surroundPan,gabi::fadds_ppc(surround,track->surroundPan));
    f32 combinedMain=gabi::fadds_ppc(mainOffset,sourceMainOffset);
    tv->mainSend=gabi::fadds_ppc(gabi::fadds_ppc(tv->mainSend,track->mainSend),combinedMain);
    for(u32 bus=0;bus<3;++bus) {
        f32 sourceSend=byteVolume(sourceFxSend[bus]);
        f32 trackSend=gabi::fadds_ppc((*sends)[bus],sourceSend);
        f32 mixed=gabi::fadds_ppc(tv->auxiliarySend[bus],track->fxSend[bus]);
        tv->auxiliarySend[bus]=gabi::fadds_ppc(mixed,trackSend);
    }
    gabi::FrameLocal<ChannelMixGroup[2]> drc(0x104);
    constructTrackOutputArray(&(*drc)[0],2,0x50,&constructStreamMixGroup);
    f32 drcMain=gabi::fadds_ppc(mainOffset,gabi::fsubs_ppc(byteVolume(sourceMainSend),1.0f));
    for(u32 output=0;output<2;++output) {
        auto& params=(*drc)[output];
        copyOutputWords(&params,&base->outputs[output+1]);
        params.gain=gabi::fmuls_ppc(params.gain,track->outputs[output].gain);
        copyMix(params.channels,track->outputs[output].channels,2);
        params.pan=gabi::fadds_ppc(params.pan,gabi::fadds_ppc(pan,track->outputs[output].pan));
        params.surroundPan=gabi::fadds_ppc(params.surroundPan,gabi::fadds_ppc(surround,track->outputs[output].surroundPan));
        params.mainSend=gabi::fadds_ppc(gabi::fadds_ppc(params.mainSend,track->outputs[output].mainSend),drcMain);
        for(u32 bus=0;bus<3;++bus) {
            f32 send=gabi::fadds_ppc((*sends)[bus],byteVolume(sourceFxSend[bus]));
            f32 mixed=gabi::fadds_ppc(params.auxiliarySend[bus],track->outputs[output].auxiliarySend[bus]);
            params.auxiliarySend[bus]=gabi::fadds_ppc(mixed,send);
        }
    }
    gabi::FrameLocal<be<f32>[2]> savedPan(0x10);
    gabi::FrameLocal<ChannelMixState[2]> savedTv(0x68);
    gabi::FrameLocal<ChannelMixState[4]> savedDrc(0xA4);
    u32 count=track->channelCount;
    for(u32 channel=0;channel<count;++channel) {
        StreamPlaybackChannel* source=track->channels[channel];
        PlaybackVoice* voice=source->voice;
        if(!voice) continue;
        // the f32 values go to the f64 parameters as lfs would load them (signalling NaNs kept)
        voice->setVolume(gabi::fpr_from_f32(volume));
        voice->setPitch(gabi::fpr_from_f32(pitch));
        voice->setLowPass(gabi::fpr_from_f32(lpf));
        voice->setFilter(biquadType, gabi::fpr_from_f32(biquad));
        voice->setGamepadSurround(track->frontBypass!=0);
        voice->setFilterPreset(base->frontBypass);
        voice->setOutputMask(outputLine);
        u32 channelsNow=track->channelCount;
        if(channelsNow==1) {
            voice->setMainMix(tv.get());
            for(u32 output=0;output<2;++output) voice->setGamepadMix(output, &(*drc)[output]);
        } else if(channelsNow==2) {
            f32 tvPan=tv->pan;
            for(u32 output=0;output<2;++output) (*savedPan)[output]=static_cast<f32>((*drc)[output].pan);
            constructTrackMixArray(&(*savedTv)[0],2,0x18,&constructStreamMixState);
            copyMix(&(*savedTv)[0],tv->channels,2);
            constructTrackMixArray(&(*savedDrc)[0],4,0x18,&constructStreamMixState);
            for(u32 output=0;output<2;++output) copyMix(&(*savedDrc)[output*2],(*drc)[output].channels,2);
            if(tv->mode==0) {
                if(channel==0) {
                    tv->pan=gabi::fsubs_ppc(tv->pan,1.0f);
                    for(u32 output=0;output<2;++output) (*drc)[output].pan=gabi::fsubs_ppc((*drc)[output].pan,1.0f);
                } else if(channel==1) {
                    tv->pan=gabi::fadds_ppc(tv->pan,1.0f);
                    for(u32 output=0;output<2;++output) (*drc)[output].pan=gabi::fadds_ppc((*drc)[output].pan,1.0f);
                }
            } else if(channel==1) {
                auto* to=reinterpret_cast<be<f32>*>(tv->channels);
                for(u32 component=0;component<6;++component) { f32 value=to[component+6]; to[component+6]=0.0f; to[component]=value; }
                for(u32 output=0;output<2;++output) {
                    auto* mix=reinterpret_cast<be<f32>*>((*drc)[output].channels);
                    for(u32 component=0;component<6;++component) { mix[component]=static_cast<f32>(mix[component+6]); mix[component+6]=0.0f; }
                }
            }
            voice->setMainMix(tv.get());
            for(u32 output=0;output<2;++output) voice->setGamepadMix(output, &(*drc)[output]);
            tv->pan=tvPan;
            for(u32 output=0;output<2;++output) (*drc)[output].pan=static_cast<f32>((*savedPan)[output]);
            copyMix(tv->channels,&(*savedTv)[0],2);
            for(u32 output=0;output<2;++output) copyMix((*drc)[output].channels,&(*savedDrc)[output*2],2);
        }
        for(u32 remote=0;remote<4;++remote) voice->setRemoteMix(remote, reinterpret_cast<const ChannelOutputGain*>(&base->remotes[remote]));
        count=track->channelCount;
    }
}
/* WWHD glue
ext 028EFFD0 void constructTrackMixArray(void*,u32,u32,TrackMixConstructor)
ext 028EFFD0 void constructTrackOutputArray(void*,u32,u32,TrackOutputConstructor)
ext 0289EB18 void nw::snd::PlaybackVoice::setVolume(f64)
ext 0289EB44 void nw::snd::PlaybackVoice::setPitch(f64)
ext 0289EBA4 void nw::snd::PlaybackVoice::setLowPass(f64)
ext 0289EBC4 void nw::snd::PlaybackVoice::setFilter(u32,f64)
ext 0289ECC4 void nw::snd::PlaybackVoice::setGamepadSurround(u32)
ext 0289ED28 void nw::snd::PlaybackVoice::setFilterPreset(u32)
ext 0289ED6C void nw::snd::PlaybackVoice::setOutputMask(u32)
ext 0289EEE0 void nw::snd::PlaybackVoice::setMainMix(const nw::snd::ChannelMixGroup*)
ext 0289EEE8 void nw::snd::PlaybackVoice::setGamepadMix(u32,const nw::snd::ChannelMixGroup*)
ext 0289EEFC void nw::snd::PlaybackVoice::setRemoteMix(u32,const nw::snd::ChannelOutputGain*)
*/
