// HD hardware-voice ownership and AX parameter translation, reconstructed from the binary.
// AX boundary layouts cross-checked with wut and Cemu (facts only; no code copied).
#include "nw/snd/HardwareVoice.h"
#include "sead/ElementAt.h"
#include "nw/snd/AudioEventLog.h"
using namespace nw::snd;
struct HardwareVoicePool { HardwareVoice items[96]; };
static HardwareVoicePool sHardwareVoices;
struct VoiceMutex { u8 opaque[0x2C]; be<u8> initialized; };
static VoiceMutex sVoiceMutex;
static VoiceList sPriorityList;
struct AxSampleFormatTable { be<u8> items[3]; };
static AxSampleFormatTable sAxSampleFormats;
static AxAdpcm sPcm8Adpcm;
static AxAdpcm sPcm16Adpcm;
struct OutputModes { be<u32> reserved; be<u32> modes[3]; };
static OutputModes sOutputModes;
static u32 sOutputModesGuard;
static u8 sVoiceStaticRegistration;
using HardwareVoiceConstructor=HardwareVoice* (*)(HardwareVoice*);
void voiceConstructArray(void*,u32,u32,HardwareVoiceConstructor);
void voiceInitializeMutex(void*);
void voiceRegisterStatic(void*);
/* 028A62F8 */
static void destroyMainRequests() {
    voiceDestroyArray(sMainRequests.items,512,sizeof(VoiceRequest),&VoiceRequest::destroy,0,0);
}
/* 028A631C */
static void destroyRemoteRequests() {
    voiceDestroyArray(sRemoteRequests.items,48,sizeof(VoiceRequest),&VoiceRequest::destroy,0,0);
}
/* 028A6340 */
VoiceRequest* VoiceRequest::create(VoiceRequest* request) {
    if(!request) request=static_cast<VoiceRequest*>(voiceAllocate(sizeof(VoiceRequest)));
    if(request) {
        request->context=0; request->active=0; request->state=0; request->next=nullptr;
        request->samples=nullptr; request->sampleCount=0; request->loopStart=0; request->adpcm=nullptr;
    }
    return request;
}
/* 028A6398 */
void VoiceRequest::destroy(VoiceRequest* request,u32 flags) { if(request && (flags&1)) voiceDelete(request); }
/* 028A63AC */
HardwareVoice* HardwareVoice::create(HardwareVoice* object) {
    if(!object) object=static_cast<HardwareVoice*>(voiceAllocate(sizeof(HardwareVoice)));
    if(object) {
        object->callbackContext=0; object->voice=nullptr; object->callback=0;
        for(u32 i=0;i<4;++i) { object->remoteVolume[i]=0.0f; object->remoteSend[i]=0.0f; }
        object->status=0; object->remoteEnabled=0;
        object->priorityLink.next=nullptr; object->priorityLink.previous=nullptr; object->linked=0;
    }
    return object;
}
/* 028A6424 */
void HardwareVoice::dropped(AxVoice* voice) { sead::elementAt(sHardwareVoices.items, (u32)voice->index).voice=nullptr; }
/* 028A6440 */
void HardwareVoice::unlinkPriority() {
    if(linked) { voiceListRemove(&sPriorityList,&priorityLink); linked=0; }
}
/* 028A648C */
void HardwareVoice::clearRequests() {
    voiceLock(&sVoiceMutex);
    VoiceRequest* request=firstRequest;
    while(request) { request->state=3; request=request->next; }
    firstRequest=nullptr; lastRequest=nullptr;
    voiceUnlock(&sVoiceMutex);
}
/* 028A64FC */
void HardwareVoice::reset() {
    if(linked) unlinkPriority();
    clearRequests();
    u32 target=callback;
    if(target) { gabi::call_ptr<void>(target,this,static_cast<u32>(callbackContext)); callback=0; }
    voice=nullptr; status=0;
}
/* 028A6574 */
void HardwareVoice::initialize(AxVoice* axVoice) {
    voice=axVoice; resumePending=0; sampleRate=32000; completedLoops=0; sampleFormat=1;
    // New voices start stopped until a queued request is activated.
    playbackMode=1;
    firstRequest=nullptr; loopPending=0; firstLoop=0; samplePosition=0; lastRequest=nullptr; mixerSelect=0;
}
/* 028A65B4 */
void HardwareVoice::linkPriority() {
    if(!linked) {
        VoiceLink* position=&sPriorityList.root;
        VoiceLink* first=sPriorityList.root.next;
        while(position!=first) {
            VoiceLink* before=position->previous;
            HardwareVoice* owner=reinterpret_cast<HardwareVoice*>(reinterpret_cast<u8*>(before)-offsetof(HardwareVoice,priorityLink));
            if(static_cast<u32>(owner->priority)<=static_cast<u32>(priority)) break;
            position=position->previous;
        }
        gabi::Local<gptr<VoiceLink>> iterator;
        *iterator=position;
        voiceListInsert(&sPriorityList,iterator.get(),&priorityLink);
        linked=1;
    }
}
/* 028A6644 */
HardwareVoice* HardwareVoice::acquire(u32 priority,u32 callback,u32 context) {
    AxVoice* axVoice=voiceAcquire(priority==255?31:17,&HardwareVoice::dropped,0);
    if(!axVoice) return nullptr;
    axVoice->userContext=0;
    if(priority!=255) voiceSetPriority(axVoice,16);
    HardwareVoice* result=&sead::elementAt(sHardwareVoices.items, (u32)axVoice->index);
    result->reset(); result->initialize(axVoice);
    result->callbackContext=context; result->priority=priority; result->callback=callback;
    result->linkPriority();
    return result;
}
/* 028A6708 */
void HardwareVoice::release() {
    AxVoice* axVoice=voice;
    if(axVoice) { voiceFree(axVoice); voice=nullptr; }
    reset();
}
/* 028A6750 */
void HardwareVoice::appendRequest(VoiceRequest* request) {
    voiceLock(&sVoiceMutex);
    request->next=nullptr; request->state=1;
    VoiceRequest* last=lastRequest;
    if(!last) { firstRequest=request; lastRequest=request; }
    else { last->next=request; lastRequest=request; }
    voiceUnlock(&sVoiceMutex);
}
/* 028A67D8 */
void HardwareVoice::setPriority(u32 value) {
    u8 wasLinked=linked; priority=value;
    if(wasLinked) { unlinkPriority(); linkPriority(); }
}
/* 028A6820 */
u32 nw::snd::sampleToAxOffset(u32 sample,u32 format) {
    if(format!=2) return sample;
    return (sample/14)*16+sample%14+2;
}
/* 028A6854 */
u32 nw::snd::sampleFormatToAx(u32 format) {
    switch(format) { case 1:return 0; case 2:return 1; default:return 2; }
}
/* 028A6880 */
void HardwareVoice::setLoop(WaveLoop* wave) {
    const AdpcmLoop* source=wave->adpcm;
    if(source) {
        gabi::Local<AdpcmLoop> parameters;
        parameters->predictor=source->predictor;
        parameters->history[0]=source->history[0];
        parameters->history[1]=source->history[1];
        voiceSetType(voice,0);
        voiceSetAdpcmLoop(voice,parameters.get());
    } else {
        gabi::Local<AdpcmLoop> parameters;
        parameters->predictor=wave->data->get();
        voiceSetType(voice,1);
        voiceSetAdpcmLoop(voice,parameters.get());
    }
}
/* 028A690C */
void HardwareVoice::setOffsets(WaveOffsets* wave) {
    gabi::Local<AxOffsets> parameters;
    parameters->samples=wave->samples;
    u32 format=sampleFormat;
    if(format<=2) {
        parameters->format=sAxSampleFormats.items[format];
        u32 start=sampleToAxOffset(wave->loopStart,format);
        parameters->current=start;
        parameters->end=sampleToAxOffset(static_cast<u32>(wave->sampleCount)-1,format);
        resumeOffset=start;
        parameters->loopStart=sampleToAxOffset(0,sampleFormat);
        WaveLoop* loop=wave->loop;
        if(loop) {
            parameters->loop=1;
            setLoop(loop);
            loopPending=1; firstLoop=1;
            voiceSetOffsets(voice,parameters.get());
        } else {
            parameters->loop=wave->loopEnabled;
            firstLoop=0; loopPending=0;
            voiceSetOffsets(voice,parameters.get());
        }
    }
}
/* 028A69E8 */
void HardwareVoice::setAdpcm(WaveLoop* wave) {
    u32 format=sampleFormat;
    if(format>2) return;
    gabi::Local<AxAdpcm> parameters;
    if(format==2) {
        for(u32 i=0;i<8;++i) parameters->coefficients[i]=adpcmCoefficients[i];
        const AdpcmLoop* loop=wave->adpcm;
        parameters->gain=0;
        if(loop) {
            parameters->loop.predictor=loop->predictor;
            parameters->loop.history[0]=loop->history[0];
            parameters->loop.history[1]=loop->history[1];
        } else {
            parameters->loop.history[1]=0; parameters->loop.history[0]=0; parameters->loop.predictor=0;
        }
    } else {
        const AxAdpcm* source=format==1?&sPcm16Adpcm:&sPcm8Adpcm;
        const be<u32>* words=reinterpret_cast<const be<u32>*>(source);
        be<u32>* destination=reinterpret_cast<be<u32>*>(parameters.get());
        for(u32 i=0;i<10;++i) destination[i]=words[i];
    }
    voiceSetAdpcm(voice,parameters.get());
}
/* 028A6B24 */
u16 nw::snd::volumeToAx(f32 volume) {
    f32 scaled=gabi::fmuls_ppc(volume,32768.0f);
    f32 clamped=65535.0f;
    if(!(scaled>clamped)) clamped=scaled>=0.0f?scaled:0.0f;
    return static_cast<u16>(gabi::ftoi(clamped));
}
/* 028A6B68 */
void HardwareVoice::initializeMainChannels(DeviceMix* destination,MixInput* levels,u32 surround) {
    if(surround) {
        for(u32 bus=0;bus<4;++bus) for(u32 channel=2;channel<6;++channel) {
            u16 value=volumeToAx(levels->bus[bus][channel]);
            destination->channel[channel][bus].current=value;
            destination->channel[channel][bus].delta=0;
        }
    } else {
        for(u32 channel=2;channel<6;++channel) voiceClear(destination->channel[channel],0,16);
    }
    for(u32 bus=0;bus<4;++bus) for(u32 channel=0;channel<2;++channel) {
        u16 value=volumeToAx(levels->bus[bus][channel]);
        destination->channel[channel][bus].delta=0;
        destination->channel[channel][bus].current=value;
    }
}
/* 028A6DF4 */
void HardwareVoice::initializeMainMix(VoiceParameters* parameters,u32 outputMode) {
    initializeMainChannels(&mainMix,&parameters->mainMix,outputMode==2);
    voiceSetDeviceMix(voice,0,0,&mainMix);
}
/* 028A6E48 */
void HardwareVoice::initializeGamepadChannels(DeviceMix* destination,RemoteDeviceMix* secondary,MixInput* levels,u32 surround,u32 split) {
    for(u32 channel=0;channel<6;++channel) voiceClear(destination->channel[channel],0,16);
    if(surround) {
        for(u32 bus=0;bus<4;++bus) for(u32 channel=2;channel<4;++channel) {
            u16 value=volumeToAx(levels->bus[bus][channel]);
            destination->channel[channel][bus].delta=0;
            destination->channel[channel][bus].current=value;
        }
    }
    if(surround && split) {
        for(u32 channel=0;channel<4;++channel) voiceClear(secondary->channel[channel],0,16);
        for(u32 channel=0;channel<2;++channel) {
            u16 value=volumeToAx(levels->bus[0][channel]);
            secondary->channel[channel][0].delta=0; secondary->channel[channel][0].current=value;
        }
        for(u32 bus=1;bus<4;++bus) for(u32 channel=0;channel<2;++channel) {
            u16 value=volumeToAx(levels->bus[bus][channel]);
            destination->channel[channel][bus].delta=0; destination->channel[channel][bus].current=value;
        }
    } else {
        for(u32 bus=0;bus<4;++bus) for(u32 channel=0;channel<2;++channel) {
            u16 value=volumeToAx(levels->bus[bus][channel]);
            destination->channel[channel][bus].current=value; destination->channel[channel][bus].delta=0;
        }
    }
}
/* 028A70B8 */
void HardwareVoice::initializeGamepadMix(VoiceParameters* parameters,u32 outputMode) {
    initializeGamepadChannels(&gamepadMix.primary,&gamepadMix.secondary.mix,&parameters->gamepadMix,outputMode==2,parameters->gamepadSurround);
    voiceSetDeviceMix(voice,1,0,&gamepadMix.primary);
    if(outputMode==2 && parameters->gamepadSurround) voiceSetDeviceMix(voice,1,1,&gamepadMix.secondary.mix);
}
/* 028A7154 */
void HardwareVoice::initializeRemoteMix(VoiceParameters* parameters) {
    u8 enabled=parameters->remoteEnabled;
    remoteOutputEnabled=enabled;
    voiceSetRemoteEnabled(voice,enabled);
    if(parameters->remoteEnabled) {
        for(u32 channel=0;channel<4;++channel) {
            remoteMix.channel[channel][0].current=volumeToAx(parameters->remoteMix[channel]);
            remoteMix.channel[channel][0].delta=0;
        }
        for(u32 channel=0;channel<4;++channel) voiceSetDeviceMix(voice,2,channel,remoteMix.channel[channel]);
    }
}
/* 028A7948 */
u32 HardwareVoice::updateMainChannels(DeviceMix* destination,MixInput* levels,u32 surround) {
    u32 changed=0;
    for(u32 bus=0;bus<4;++bus) for(u32 channel=2;channel<6;++channel) {
        u16 value=0;
        if(surround) value=volumeToAx(levels->bus[bus][channel]);
        changed|=updateHardwareVolumeRamp(value,&destination->channel[channel][bus]);
    }
    for(u32 bus=0;bus<4;++bus) for(u32 channel=0;channel<2;++channel) {
        u16 value=volumeToAx(levels->bus[bus][channel]);
        changed|=updateHardwareVolumeRamp(value,&destination->channel[channel][bus]);
    }
    return changed!=0;
}
/* 028A7D78 */
void HardwareVoice::updateMainMix(VoiceParameters* parameters,u32 outputMode) {
    if(updateMainChannels(&mainMix,&parameters->mainMix,outputMode==2)) voiceSetDeviceMix(voice,0,0,&mainMix);
}
/* 028A7DD4 */
u32 HardwareVoice::updateGamepadChannels(DeviceMix* destination,RemoteDeviceMix* secondary,MixInput* levels,u32 surround,u32 split) {
    u32 changed=0;
    if(!surround) {
        for(u32 bus=0;bus<4;++bus) for(u32 channel=0;channel<2;++channel) {
            u16 value=volumeToAx(levels->bus[bus][channel]);
            changed|=updateHardwareVolumeRamp(value,&destination->channel[channel][bus]);
        }
        for(u32 bus=0;bus<4;++bus) for(u32 channel=2;channel<4;++channel) changed|=updateHardwareVolumeRamp(0,&destination->channel[channel][bus]);
    } else {
        for(u32 bus=1;bus<4;++bus) for(u32 channel=0;channel<2;++channel) {
            u16 value=volumeToAx(levels->bus[bus][channel]);
            changed|=updateHardwareVolumeRamp(value,&destination->channel[channel][bus]);
        }
        for(u32 bus=0;bus<4;++bus) for(u32 channel=2;channel<4;++channel) {
            u16 value=volumeToAx(levels->bus[bus][channel]);
            changed|=updateHardwareVolumeRamp(value,&destination->channel[channel][bus]);
        }
        for(u32 channel=0;channel<2;++channel) {
            u16 value=volumeToAx(levels->bus[0][channel]);
            VolumeRamp* ramp=split?&secondary->channel[channel][0]:&destination->channel[channel][0];
            changed|=updateHardwareVolumeRamp(value,ramp);
        }
    }
    return changed!=0;
}
/* 028A80C8 */
void HardwareVoice::updateGamepadMix(VoiceParameters* parameters,u32 outputMode) {
    if(updateGamepadChannels(&gamepadMix.primary,&gamepadMix.secondary.mix,&parameters->gamepadMix,outputMode==2,parameters->gamepadSurround)) {
        voiceSetDeviceMix(voice,1,0,&gamepadMix.primary);
        if(outputMode==2 && parameters->gamepadSurround) voiceSetDeviceMix(voice,1,1,&gamepadMix.secondary.mix);
    }
}
/* 028A816C */
void HardwareVoice::updateRemoteMix(VoiceParameters* parameters) {
    u16 old=remoteOutputEnabled; u8 enabled=parameters->remoteEnabled;
    if(enabled!=old) { voiceSetRemoteEnabled(voice,enabled); remoteOutputEnabled=enabled; }
    u32 changed=0;
    for(u32 channel=0;channel<4;++channel) {
        u16 value=volumeToAx(parameters->remoteMix[channel]);
        changed|=updateHardwareVolumeRamp(value,&remoteMix.channel[channel][0]);
    }
    if(changed) for(u32 channel=0;channel<4;++channel) voiceSetDeviceMix(voice,2,channel,remoteMix.channel[channel]);
}
/* 028A7844 */
void HardwareVoice::configure(VoiceParameters* parameters,WaveOffsets* wave,const be<u32>* outputModes) {
    u32 mixer=mixerSelect;
    if(mixer) { u32 mode=sampleFormatToAx(mixer); voiceSetMixer(voice,mode); }
    setOffsets(wave);
    setAdpcm(reinterpret_cast<WaveLoop*>(wave));
    initializeMainMix(parameters,outputModes[0]);
    initializeGamepadMix(parameters,outputModes[1]);
    initializeRemoteMix(parameters);
    setVolume(parameters,1); setSource(parameters,1); setLowPass(parameters,1); setBiquad(parameters,1); setRemoteFilter(parameters,1);
}
void HardwareVoice::updateParameters(VoiceParameters* parameters,const be<u32>* outputModes) {
    setVolume(parameters,0); setSource(parameters,0);
    updateMainMix(parameters,outputModes[0]); updateGamepadMix(parameters,outputModes[1]); updateRemoteMix(parameters);
    setLowPass(parameters,0); setBiquad(parameters,0); setRemoteFilter(parameters,0);
}
/* 028A826C */
void HardwareVoice::updateStream(VoiceParameters* parameters,u32 playing,const be<u32>* outputModes) {
    if(!resumePending) {
        if(!playing) {
            VoiceRequest* request=firstRequest;
            if(!request) return;
            u32 state=request->state;
            if(state==2) {
                VoiceRequest* last=lastRequest;
                firstRequest=request->next;
                if(last==request) lastRequest=nullptr;
                request->state=3;
                request=firstRequest;
                if(!request) return;
                state=request->state;
            }
            if(state==1) {
                configure(parameters,reinterpret_cast<WaveOffsets*>(request),outputModes);
                voiceSetState(voice,1);
                request->state=2;
            }
            return;
        }
    } else {
        gabi::Local<AxOffsets> offsets;
        voiceGetOffsets(voice,offsets.get());
        u32 format=sampleFormat;
        u32 position=resumeOffset;
        AxVoice* axVoice=voice;
        if(format==2) {
            u32 nibble=position&15;
            if(nibble<=1) { position-=nibble+1; resumeOffset=position; }
        }
        offsets->current=position;
        voiceSetOffsets(axVoice,offsets.get()); voiceSetState(voice,1); resumePending=0;
    }
    u8 waiting=loopPending;
    VoiceRequest* request=firstRequest;
    if(waiting) {
        u32 count=voiceGetLoopCount(voice);
        u32 oldCount=completedLoops;
        if(count==oldCount && !firstLoop) { updateParameters(parameters,outputModes); return; }
        u8 first=firstLoop; loopPending=0;
        if(!first) {
            firstRequest=request->next; request->state=3;
            request=firstRequest; request->state=2;
            if(!firstLoop) {
                u32 end=sampleToAxOffset(static_cast<u32>(request->sampleCount)-1,sampleFormat);
                const void* samples=request->samples.get();
                voiceSetEnd(voice,end,samples);
            }
        }
        firstLoop=0;
        VoiceRequest* next=request->next;
        u32 format=sampleFormat;
        if(next) {
            u32 position=sampleToAxOffset(next->loopStart,format);
            const void* samples=next->samples.get();
            voiceSetLoop(voice,position,samples); voiceSetLoopEnabled(voice,1);
            setLoop(reinterpret_cast<WaveLoop*>(next));
            loopPending=1; completedLoops=count;
        } else {
            u32 position=sampleToAxOffset(0,format);
            const void* samples=request->samples.get();
            voiceSetLoop(voice,position,samples); voiceSetLoopEnabled(voice,request->active);
            completedLoops=count;
        }
        updateParameters(parameters,outputModes);
        return;
    }
    VoiceRequest* next=request->next;
    if(next) {
        u32 position=next->loopStart;
        AxVoice* axVoice=voice;
        const void* samples=next->samples.get();
        voiceSetLoop(axVoice,position,samples); voiceSetLoopEnabled(voice,1);
        setLoop(reinterpret_cast<WaveLoop*>(next)); loopPending=1;
    }
    updateParameters(parameters,outputModes);
}
/* 028A8670 */
void HardwareVoice::stop(VoiceParameters*,u32 playing) {
    if(playing) { voiceSetState(voice,0); resumePending=0; }
}
/* 028A86B4 */
void HardwareVoice::pause(VoiceParameters*,u32 playing) {
    if(playing) {
        voiceSetState(voice,0);
        gabi::Local<AxOffsets> offsets;
        voiceGetOffsets(voice,offsets.get());
        u32 position=offsets->current; resumePending=1; resumeOffset=position;
    }
}
/* 028A870C */
void HardwareVoice::capturePosition() {
    VoiceRequest* request=firstRequest;
    if(request && static_cast<u32>(request->state)==2) {
        AxVoice* axVoice=voice;
        const void* samples=request->samples.get();
        u32 position=voiceGetCurrent(axVoice,samples);
        if(static_cast<u32>(sampleFormat)==2) {
            u32 remainder=position&15;
            if(remainder>=2) remainder-=2;
            position=(position>>4)*14+remainder;
        }
        samplePosition=position;
    }
}
/* 028A8784 */
void HardwareVoice::update(const be<u32>* outputModes) {
    AxVoice* axVoice=voice;
    bool playing=static_cast<u32>(axVoice->state)==1;
    u32 mode=playbackMode;
    VoiceParameters* parameters=reinterpret_cast<VoiceParameters*>(streamState);
    switch(mode) {
    case 0:updateStream(parameters,playing,outputModes);break;
    case 1:stop(parameters,playing);break;
    case 2:pause(parameters,playing);break;
    }
    capturePosition();
}
/* 028A882C */
void HardwareVoice::updateAll() {
    voiceLock(&sVoiceMutex);
    if(!voiceUserProtected()) {
        gabi::Local<be<u32>[3]> modes;
        for(u32 i=0;i<3;++i) {
            if(!sOutputModesGuard) { sOutputModesGuard=1; voiceInitializeOutputModes(&sOutputModes); }
            (*modes)[i]=sOutputModes.modes[i];
        }
        for(u32 i=0;i<96;++i) {
            HardwareVoice* object=&sHardwareVoices.items[i];
            if(object->voice) object->update(&(*modes)[0]);
        }
    }
    voiceUnlock(&sVoiceMutex);
}
/* 028A88EC */
void HardwareVoice::restorePriorities() {
    voiceUserBegin();
    VoiceLink* link=sPriorityList.root.next;
    while(link!=&sPriorityList.root) {
        HardwareVoice* object=reinterpret_cast<HardwareVoice*>(reinterpret_cast<u8*>(link)-offsetof(HardwareVoice,priorityLink));
        if(object->voice) voiceSetPriority(object->voice,16);
        link=link->next;
    }
    voiceUserEnd();
}
/* 028A7234 */
u32 nw::snd::updateHardwareVolumeRamp(u32 target,VolumeRamp* ramp) {
    s32 delta=ramp->delta;
    u32 current=ramp->current;
    if(delta==0 && target==current) return 0;
    u32 endpoint=current+static_cast<u32>(delta*96);
    s32 difference=static_cast<s32>(target-endpoint);
    s32 step=difference/96;
    if(step==0) { ramp->current=target; ramp->delta=0; }
    else {
        ramp->current=endpoint;
        if(step>32767) step=32767;
        else if(step< -32768) step= -32768;
        ramp->delta=step;
    }
    return 1;
}
/* 028A72E4 */
void HardwareVoice::setVolume(VoiceParameters* parameters,u32 initialize) {
    u16 value=volumeToAx(parameters->volume);
    if(initialize) {
        volume.current=value; volume.delta=0;
        voiceSetVolume(voice,&volume);
    } else {
        gabi::Local<VolumeRamp> ramp;
        ramp->current=volume.current; ramp->delta=volume.delta;
        if(updateHardwareVolumeRamp(value,ramp.get())) {
            u16 current=ramp->current; s16 delta=ramp->delta;
            AxVoice* axVoice=voice; volume.delta=delta; volume.current=current;
            voiceSetVolume(axVoice,&volume);
        }
    }
}
/* 028A736C */
u32 nw::snd::ratioInteger(u32 ratio) { return ratio>>16; }
/* 028A7374 */
u32 nw::snd::ratioFraction(u32 ratio) { return ratio&65535; }
/* 028A737C */
void HardwareVoice::setSource(VoiceParameters* parameters,u32 initialize) {
    f32 rate=static_cast<f32>(static_cast<u32>(sampleRate));
    f32 ratio=gabi::fdivs_ppc(gabi::fmuls_ppc(parameters->pitch,rate),32000.0f);
    if(initialize) {
        f32 fixed=gabi::fmuls_ppc(65536.0f,ratio);
        u32 bits;
        if(fixed<2147483648.0f) bits=static_cast<u32>(gabi::ftoi(fixed));
        else bits=static_cast<u32>(gabi::ftoi(gabi::fsubs_ppc(fixed,2147483648.0f)))+0x80000000u;
        u32 whole=ratioInteger(bits);
        u32 fraction=ratioFraction(bits);
        gabi::Local<AxSource> source;
        if(whole<8) { source->ratioInteger=whole; source->ratioFraction=fraction; }
        else { source->ratioFraction=0; source->ratioInteger=8; }
        source->history[0]=0; source->sampleOffset=0; source->history[2]=0; source->history[3]=0; source->history[1]=0;
        u8 mode=parameters->interpolationMode;
        u32 type=0;
        if(mode==0) {
            if(ratio>1.3333333730697632f) type=2;
            else if(ratio>1.0f) type=3;
            else type=4;
        } else if(mode==1) type=1;
        voiceSetSource(voice,source.get());
        voiceSetSourceType(voice,type);
        sourceRatio=ratio;
    } else if(ratio!=static_cast<f32>(sourceRatio)) {
        voiceSetSourceRatio(voice,ratio); sourceRatio=ratio;
    }
}
/* 028A7598 */
void HardwareVoice::setLowPass(VoiceParameters* parameters,u32 initialize) {
    u32 enabled=parameters->lowPassEnabled;
    if(initialize || enabled!=static_cast<u16>(lowPassEnabled)) {
        gabi::Local<AxLowPass> filter;
        filter->enabled=enabled;
        if(enabled==1) {
            u16 cutoff=parameters->lowPassCutoff;
            filter->history=0;
            voiceComputeLowPass(cutoff,&filter->coefficients[0],&filter->coefficients[1]);
        }
        voiceSetLowPass(voice,filter.get()); lowPassEnabled=enabled;
    } else if(enabled==1) {
        gabi::Local<be<u16>[2]> coefficients;
        voiceComputeLowPass(parameters->lowPassCutoff,&(*coefficients)[0],&(*coefficients)[1]);
        u16 first=(*coefficients)[0];
        AxVoice* axVoice=voice;
        u16 second=(*coefficients)[1];
        voiceSetLowPassCoefficients(axVoice,first,second);
    }
}
/* 028A7654 */
void HardwareVoice::setBiquad(VoiceParameters* parameters,u32 initialize) {
    u32 enabled=parameters->biquadEnabled?2:0;
    if(initialize || enabled!=static_cast<u16>(biquadEnabled)) {
        gabi::Local<AxBiquad> filter;
        filter->enabled=enabled;
        AxVoice* axVoice=voice;
        if(enabled==2) {
            s16 first=parameters->biquadCoefficients[0];
            s16 second=parameters->biquadCoefficients[1];
            filter->history[0]=0;
            s16 fifth=parameters->biquadCoefficients[4];
            s16 third=parameters->biquadCoefficients[2];
            s16 fourth=parameters->biquadCoefficients[3];
            filter->coefficients[4]=fifth; filter->coefficients[3]=fourth; filter->coefficients[2]=third;
            filter->coefficients[1]=second;
            filter->history[1]=0; filter->history[2]=0; filter->history[3]=0;
            filter->coefficients[0]=first;
        }
        voiceSetBiquad(axVoice,filter.get()); biquadEnabled=enabled;
    } else if(enabled==2) {
        AxVoice* axVoice=voice;
        u16 second=parameters->biquadCoefficients[1];
        u16 fourth=parameters->biquadCoefficients[3];
        u16 fifth=parameters->biquadCoefficients[4];
        u16 third=parameters->biquadCoefficients[2];
        u16 first=parameters->biquadCoefficients[0];
        voiceSetBiquadCoefficients(axVoice,first,second,third,fourth,fifth);
    }
}
/* 028A7748 */
void HardwareVoice::setRemoteFilter(VoiceParameters* parameters,u32 initialize) {
    u32 enabled=parameters->remoteFilterEnabled?2:0;
    if(initialize || enabled!=static_cast<u16>(remoteFilterEnabled)) {
        gabi::Local<AxBiquad> filter;
        filter->enabled=enabled;
        if(enabled==2) {
            u8 cutoff=parameters->remoteFilterCutoff;
            for(u32 i=0;i<4;++i) filter->history[i]=0;
            remoteFilterCoefficients(cutoff,&filter->coefficients[0],&filter->coefficients[1],&filter->coefficients[2],&filter->coefficients[3],&filter->coefficients[4]);
        }
        voiceSetRemoteFilter(voice,filter.get()); remoteFilterEnabled=enabled;
    } else {
        gabi::Local<be<s16>[5]> coefficients;
        remoteFilterCoefficients(parameters->remoteFilterCutoff,&(*coefficients)[0],&(*coefficients)[1],&(*coefficients)[2],&(*coefficients)[3],&(*coefficients)[4]);
        s16 third=(*coefficients)[2]; AxVoice* axVoice=voice;
        s16 first=(*coefficients)[0]; s16 second=(*coefficients)[1]; s16 fourth=(*coefficients)[3]; s16 fifth=(*coefficients)[4];
        gabi::cpu->cr[6]=0;
        voiceSetRemoteFilterCoefficients(axVoice,2,first,second,third,fourth,fifth);
    }
}
/* 028A8954 */
static void initializeHardwareVoicePool() {
    VoiceLink* root=&sPriorityList.root;
    root->previous=root; sPriorityList.count=0; root->next=root;
    voiceConstructArray(sHardwareVoices.items,96,sizeof(HardwareVoice),&HardwareVoice::create);
    sVoiceMutex.initialized=1; voiceInitializeMutex(&sVoiceMutex);
    voiceRegisterStatic(&sVoiceStaticRegistration);
}
/* 028A89C8 */
static void destroyVoiceMutex(VoiceMutex* mutex,u32 flags) {
    if(mutex) { mutex->initialized=0; if(flags&1) voiceDelete(mutex); }
}
/* WWHD glue
data sVoiceStaticRegistration 101FCB58
ext 028EFFD0 void voiceConstructArray(void*,u32,u32,HardwareVoiceConstructor)
ext 028F026C void voiceRegisterStatic(void*)
ext C0009D68 void voiceInitializeMutex(void*)
data sOutputModes 101FCE00
data sOutputModesGuard 101FD8C8
ext 02888A50 void voiceInitializeOutputModes(void*)
ext C0007140 void voiceUserEnd()
ext C0007138 void voiceUserBegin()
ext C0007148 u32 voiceUserProtected()
ext C0006DE8 u32 voiceGetCurrent(nw::snd::AxVoice*,const void*)
ext C0007070 void voiceSetLoopEnabled(nw::snd::AxVoice*,u32)
ext C0007080 void voiceSetLoop(nw::snd::AxVoice*,u32,const void*)
ext C0007058 void voiceSetEnd(nw::snd::AxVoice*,u32,const void*)
ext C0006DF0 u32 voiceGetLoopCount(nw::snd::AxVoice*)
ext C0006E00 void voiceGetOffsets(nw::snd::AxVoice*,nw::snd::AxOffsets*)
ext C0007108 void voiceSetState(nw::snd::AxVoice*,u32)
ext C00070A0 void voiceSetMixer(nw::snd::AxVoice*,u32)
ext C00070D8 void voiceSetRemoteEnabled(nw::snd::AxVoice*,u32)
ext C0007048 void voiceSetDeviceMix(nw::snd::AxVoice*,u32,u32,const void*)
ext C000A858 void* voiceClear(void*,u32,u32)
ext 0289D108 void nw::snd::remoteFilterCoefficients(s32,be<s16>*,be<s16>*,be<s16>*,be<s16>*,be<s16>*)
ext C00070C8 void voiceSetRemoteFilterCoefficients(nw::snd::AxVoice*,u32,s32,s32,s32,s32,s32)
ext C00070C0 void voiceSetRemoteFilter(nw::snd::AxVoice*,const nw::snd::AxBiquad*)
ext C0007028 void voiceSetBiquadCoefficients(nw::snd::AxVoice*,u32,u32,u32,u32,u32)
ext C0007020 void voiceSetBiquad(nw::snd::AxVoice*,const nw::snd::AxBiquad*)
ext C0007090 void voiceSetLowPassCoefficients(nw::snd::AxVoice*,u32,u32)
ext C0007088 void voiceSetLowPass(nw::snd::AxVoice*,const nw::snd::AxLowPass*)
ext C0006C90 void voiceComputeLowPass(u32,be<u16>*,be<u16>*)
ext C00070F8 void voiceSetSourceRatio(nw::snd::AxVoice*,f32)
ext C0007100 void voiceSetSourceType(nw::snd::AxVoice*,u32)
ext C00070F0 void voiceSetSource(nw::snd::AxVoice*,const nw::snd::AxSource*)
ext C0007118 void voiceSetVolume(nw::snd::AxVoice*,const nw::snd::VolumeRamp*)
data sHardwareVoices 104C1AC8
data sVoiceMutex 104C1A8C
data sPriorityList 104C1ABC
data sAxSampleFormats 10187194
data sPcm8Adpcm 101871BC
data sPcm16Adpcm 101871E4
ext C0007110 void voiceSetType(nw::snd::AxVoice*,u32)
ext C0007018 void voiceSetAdpcmLoop(nw::snd::AxVoice*,const nw::snd::AdpcmLoop*)
ext C00070A8 void voiceSetOffsets(nw::snd::AxVoice*,const nw::snd::AxOffsets*)
ext C0007010 void voiceSetAdpcm(nw::snd::AxVoice*,const nw::snd::AxAdpcm*)
ext 028F0164 void voiceDestroyArray(void*,u32,u32,VoiceDestroyCallback,u32,u32)
ext 0273AD10 void* voiceAllocate(u32)
ext 0273AF40 void voiceDelete(void*)
ext 0286DA9C void voiceListRemove(nw::snd::VoiceList*,nw::snd::VoiceLink*)
ext 0286DA70 void voiceListInsert(nw::snd::VoiceList*,const gptr<nw::snd::VoiceLink>*,nw::snd::VoiceLink*)
ext C0009E40 void voiceLock(void*)
ext C000A1C8 void voiceUnlock(void*)
ext C0006C80 nw::snd::AxVoice* voiceAcquire(u32,VoiceDropCallback,u32)
ext C00070B8 void voiceSetPriority(nw::snd::AxVoice*,u32)
ext C0006CA8 void voiceFree(nw::snd::AxVoice*)
*/
