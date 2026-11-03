// HD software mixing ramps and software voice queues, reconstructed from the binary.
#include "nw/snd/SoftwareMixer.h"
using namespace nw::snd;
struct SoftwareMutex { u8 opaque[0x2C]; be<u8> initialized; };
static SoftwareMutex sSoftwareMutex;
struct DecodedSamples { be<s16> items[2052]; };
static DecodedSamples sDecodedSamples;
struct ResampleCoefficients { be<s16> tables[3][512]; };
static ResampleCoefficients sResampleCoefficients;
struct OutputModes { be<u32> reserved; be<u32> modes[3]; };
static OutputModes sSoftwareOutputModes;
static u32 sSoftwareOutputGuard;
struct MixScratch { be<f32> values[144]; };
static MixScratch sMixScratch;
static u8 sSoftwareStaticRegistration;
void softwareInitializeMutex(void*);
void softwareRegisterStatic(void*);
/* 028A8B40 */
SoftwareVoice* SoftwareVoice::create(SoftwareVoice* object) {
    if(!object) object=static_cast<SoftwareVoice*>(voiceAllocate(sizeof(SoftwareVoice)));
    if(object) {
        object->callbackContext=0; object->owner=nullptr; object->callback=0;
        for(u32 i=0;i<4;++i) { object->remoteVolume[i]=0.0f; object->remoteSend[i]=0.0f; }
        object->status=0; object->remoteEnabled=0;
        object->priorityLink.next=nullptr; object->priorityLink.previous=nullptr; object->linked=0;
    }
    return object;
}
/* 028A8C60 */
void SoftwareVoice::clearRequests() {
    voiceLock(&sSoftwareMutex);
    VoiceRequest* request=firstRequest;
    while(request) { request->state=3; request=request->next; }
    firstRequest=nullptr; lastRequest=nullptr;
    voiceUnlock(&sSoftwareMutex);
}
/* 028A8CD0 */
void SoftwareVoice::reset() {
    void* manager=owner;
    if(manager) softwareRemoveVoice(manager,this);
    clearRequests();
    u32 target=callback;
    if(target) { gabi::call_ptr<void>(target,this,static_cast<u32>(callbackContext)); callback=0; }
    status=0;
}
/* 028A8D44 */
void SoftwareVoice::release() { reset(); }
/* 028A8D48 */
void SoftwareVoice::appendRequest(VoiceRequest* request) {
    voiceLock(&sSoftwareMutex);
    request->next=nullptr; request->state=1;
    VoiceRequest* last=lastRequest;
    if(!last) { firstRequest=request; lastRequest=request; }
    else { last->next=request; lastRequest=request; }
    voiceUnlock(&sSoftwareMutex);
}
/* 028A8EB0 */
void SoftwareVoice::setPriority(u32 value) {
    void* manager=owner; priority=value;
    if(manager) softwareReorderVoice(manager,this);
}
/* 028A8ECC */
void SoftwareVoice::initialize(u32 handler,u32 context) {
    callback=handler; callbackContext=context; currentOffset=0; firstRequest=nullptr;
    sampleFormat=1; sampleRate=32000; playbackMode=1; paused=0; outputOffset=0; lastRequest=nullptr;
}
/* 028A8F04 */
u16 nw::snd::softwareVolumeToAx(f32 volume) {
    f32 scaled=gabi::fmuls_ppc(volume,32768.0f);
    f32 clamped=65535.0f;
    if(!(scaled>clamped)) clamped=scaled>=0.0f?scaled:0.0f;
    return static_cast<u16>(gabi::ftoi(clamped));
}
/* 028A8F48 */
u32 nw::snd::updateVolumeRamp(u32 target,VolumeRamp* ramp) {
    s32 delta=ramp->delta;
    u32 current=ramp->current;
    if(delta==0 && target==current) return 0;
    u32 endpoint=current+static_cast<u32>(delta*144);
    s32 difference=static_cast<s32>(target-endpoint);
    s32 step=difference>0?static_cast<s32>(static_cast<u32>(difference)/144):-static_cast<s32>(static_cast<u32>(-difference)/144);
    if(step==0) { ramp->current=target; ramp->delta=0; }
    else {
        ramp->current=endpoint;
        if(step>32767) step=32767;
        else if(step< -32768) step= -32768;
        ramp->delta=step;
    }
    return 1;
}
/* 028A8FF0 */
void SoftwareVoice::setVolume(MixLevels* levels,u32 initialize) {
    u16 value=softwareVolumeToAx(levels->volume);
    if(initialize) { volume.current=value; volume.delta=0; }
    else updateVolumeRamp(value,&volume);
}
/* 028A9044 */
u32 SoftwareVoice::updateChannels(VolumeRamp* destination,const be<f32>* levels,u32 surround) {
    u16 value=softwareVolumeToAx(levels[0]);
    u32 changed=updateVolumeRamp(value,&destination[0]);
    value=softwareVolumeToAx(levels[1]);
    changed|=updateVolumeRamp(value,&destination[1]);
    if(surround) {
        value=softwareVolumeToAx(levels[2]); changed|=updateVolumeRamp(value,&destination[2]);
        value=softwareVolumeToAx(levels[3]); changed|=updateVolumeRamp(value,&destination[3]);
    }
    return changed!=0;
}
/* 028A90C8 */
u32 SoftwareVoice::updateMainMix(MixLevels* levels,u32 outputMode) { return updateChannels(mainMix,levels->main,outputMode==2); }
/* 028A90E0 */
u32 SoftwareVoice::updateRemoteMix(MixLevels* levels) { return updateChannels(remoteMix,levels->remote,0); }
/* 028A90F0 */
void SoftwareVoice::updateMix(MixLevels* levels,u32 outputMode) {
    setVolume(levels,0); updateMainMix(levels,outputMode); updateRemoteMix(levels);
}
/* 028A914C */
void SoftwareVoice::initializeChannels(VolumeRamp* destination,const be<f32>* levels,u32 surround) {
    destination[0].current=softwareVolumeToAx(levels[0]); destination[0].delta=0;
    destination[1].current=softwareVolumeToAx(levels[1]); destination[1].delta=0;
    if(surround) {
        destination[2].current=softwareVolumeToAx(levels[2]); destination[2].delta=0;
        u16 value=softwareVolumeToAx(levels[3]); destination[3].delta=0; destination[3].current=value;
    }
}
/* 028A91B4 */
void SoftwareVoice::initializeMainMix(MixLevels* levels,u32 outputMode) { initializeChannels(mainMix,levels->main,outputMode==2); }
/* 028A91CC */
void SoftwareVoice::initializeRemoteMix(MixLevels* levels) { initializeChannels(remoteMix,levels->remote,0); }
/* 028A91DC */
void SoftwareVoice::initializeMix(MixLevels* levels,u32 outputMode) {
    resamplePhase=0;
    for(u32 i=0;i<4;++i) resampleHistory[i]=0;
    outputOffset=0;
    setVolume(levels,1); initializeMainMix(levels,outputMode); initializeRemoteMix(levels);
}
/* 028A925C */
void SoftwareVoice::applyMix(u32 outputMode) {
    VoiceRequest* request=firstRequest;
    bool active=request && static_cast<u32>(request->state)==2;
    switch(static_cast<u32>(playbackMode)) {
    case 0: if(active) updateMix(&mixLevels,outputMode); else initializeMix(&mixLevels,outputMode); break;
    case 1: paused=0; break;
    case 2: paused=1; break;
    }
}
/* 028A92CC */
u32 SoftwareVoice::nextRequest() {
    VoiceRequest* request=firstRequest;
    if(!request->active) {
        firstRequest=request->next; request->state=3;
        request=firstRequest;
        if(!request) { lastRequest=nullptr; return 0; }
        request->state=2;
        request=firstRequest;
    }
    outputOffset=request->loopStart;
    return 1;
}
/* 028A9328 */
u32 SoftwareVoice::decodePcm16(be<s16>* output,u32 count) {
    u32 remaining=count;
    while(remaining) {
        VoiceRequest* request=firstRequest;
        u32 position=outputOffset;
        u32 available=static_cast<u32>(request->sampleCount)-position;
        u32 amount=remaining<available?remaining:available;
        const be<s16>* source=static_cast<const be<s16>*>(request->samples.get())+position;
        softwareCopy(output,source,amount*2);
        u32 next=static_cast<u32>(outputOffset)+amount;
        request=firstRequest; outputOffset=next;
        u32 end=request->sampleCount;
        output+=amount; remaining-=amount;
        if(next>=end && !nextRequest()) {
            if(remaining) softwareBlockSet(output,0,remaining*2);
            break;
        }
    }
    return count-remaining;
}
/* 028A93FC */
u32 SoftwareVoice::decodePcm8(be<s16>* output,u32 count) {
    u32 remaining=count;
    while(remaining) {
        VoiceRequest* request=firstRequest;
        u32 position=outputOffset;
        u32 available=static_cast<u32>(request->sampleCount)-position;
        u32 amount=remaining<available?remaining:available;
        const be<s8>* source=static_cast<const be<s8>*>(request->samples.get())+position;
        for(u32 i=0;i<amount;++i) *output++=static_cast<s32>(source[i])*256;
        if(amount) position=outputOffset;
        request=firstRequest; position+=amount; outputOffset=position;
        u32 end=request->sampleCount; remaining-=amount;
        if(position>=end && !nextRequest()) {
            if(remaining) softwareBlockSet(output,0,remaining*2);
            break;
        }
    }
    return count-remaining;
}
/* 028A94E8 */
u32 SoftwareVoice::decodeAdpcm(be<s16>* output,u32 count) {
    u32 remaining=count;
    while(remaining) {
        VoiceRequest* request=firstRequest;
        u32 position=outputOffset;
        if(position==static_cast<u32>(request->loopStart)) {
            const AdpcmLoop* loop=request->adpcm.get();
            if(loop) {
                decoderLoop.predictor=loop->predictor;
                decoderLoop.history[0]=loop->history[0];
                request=firstRequest;
                decoderLoop.history[1]=loop->history[1];
                position=outputOffset;
            }
        }
        u32 available=static_cast<u32>(request->sampleCount)-position;
        u32 amount=remaining<available?remaining:available;
        const void* source=request->samples.get();
        softwareDecodeAdpcm(position,&decoderLoop,adpcmCoefficients,source,amount,output);
        u32 next=static_cast<u32>(outputOffset)+amount;
        request=firstRequest; outputOffset=next;
        u32 end=request->sampleCount; remaining-=amount; output+=amount;
        if(next>=end && !nextRequest()) {
            if(remaining) softwareBlockSet(output,0,remaining*2);
            break;
        }
    }
    return count-remaining;
}
/* 028A89E4 */
MixBlock* MixBlock::create(MixBlock* block) {
    if(!block) block=static_cast<MixBlock*>(voiceAllocate(sizeof(MixBlock)));
    if(block) {
        block->activeChannels=0; block->sampleCount=0;
        softwareEventInitialize(&block->mainReady,1,1); softwareEventInitialize(&block->remoteReady,1,1);
        block->remote[1]=nullptr; block->remoteChannelCount=0; block->main[2]=nullptr;
        block->remoteReserved=0; block->main[5]=nullptr; block->mainSampleCapacity=0;
        block->remoteState=0; block->main[1]=nullptr; block->remote[2]=nullptr;
        block->main[3]=nullptr; block->remoteSampleCapacity=0; block->main[4]=nullptr; block->mainChannelCount=0; block->remote[0]=nullptr;
        block->remote[3]=nullptr; block->main[0]=nullptr;
    }
    return block;
}
/* 028A8A98 */
void MixBlock::initialize(be<f32>* buffer,u32) {
    for(u32 i=1;i<6;++i) main[i]=buffer+144*i;
    remote[0]=buffer+144*6; remote[1]=buffer+144*7;
    main[0]=buffer;
    remote[2]=buffer+144*8; remote[3]=buffer+144*9;
    mainSampleCapacity=144; mainChannelCount=6; remoteSampleCapacity=144; remoteChannelCount=4;
    softwareEventSignal(&mainReady); softwareEventSignal(&remoteReady); sampleCount=0;
}
/* 028A8BB8 */
void SoftwareVoiceManager::unlink(SoftwareVoice* voice) {
    if(voice->linked) { voiceListRemove(&active,&voice->priorityLink); voice->linked=0; }
}
/* 028A8BFC */
void SoftwareVoiceManager::recycle(SoftwareVoice* voice) {
    if(voice->linked) {
        unlink(voice);
        gabi::Local<gptr<VoiceLink>> iterator;
        *iterator=&free.root;
        voiceListInsert(&free,iterator.get(),&voice->priorityLink);
    }
}
/* 028A8DD0 */
void SoftwareVoiceManager::link(SoftwareVoice* voice) {
    if(!voice->linked) {
        VoiceLink* position=&active.root;
        VoiceLink* first=active.root.next;
        while(position!=first) {
            VoiceLink* before=position->previous;
            SoftwareVoice* object=reinterpret_cast<SoftwareVoice*>(reinterpret_cast<u8*>(before)-offsetof(SoftwareVoice,priorityLink));
            if(static_cast<u32>(object->priority)<=static_cast<u32>(voice->priority)) break;
            position=position->previous;
        }
        gabi::Local<gptr<VoiceLink>> iterator;
        *iterator=position;
        voiceListInsert(&active,iterator.get(),&voice->priorityLink);
        voice->linked=1;
    }
}
/* 028A8E58 */
void SoftwareVoiceManager::reorder(SoftwareVoice* voice) {
    if(voice->linked) { unlink(voice); link(voice); }
}
/* 028A9D20 */
void SoftwareVoiceManager::initialize() {
    for(u32 i=0;i<96;++i) {
        gabi::Local<gptr<VoiceLink>> iterator;
        *iterator=&free.root;
        voiceListInsert(&free,iterator.get(),&voices[i].priorityLink);
    }
}
/* 028A9D8C */
void SoftwareVoiceManager::releaseAll() {
    for(u32 i=0;i<96;++i) if(voices[i].linked) voices[i].release();
}
/* 028A9DE0 */
SoftwareVoice* SoftwareVoiceManager::acquire(u32 priority,u32 callback,u32 context) {
    if(!static_cast<u32>(free.count)) {
        if(!static_cast<u32>(active.count)) return nullptr;
        VoiceLink* link=active.root.previous;
        SoftwareVoice* object=reinterpret_cast<SoftwareVoice*>(reinterpret_cast<u8*>(link)-offsetof(SoftwareVoice,priorityLink));
        if(priority<static_cast<u32>(object->priority)) return nullptr;
        object->reset();
    }
    VoiceLink* selected=free.root.previous;
    SoftwareVoice* voice=reinterpret_cast<SoftwareVoice*>(reinterpret_cast<u8*>(selected)-offsetof(SoftwareVoice,priorityLink));
    gabi::Local<gptr<VoiceLink>> iterator;
    *iterator=selected;
    softwareListErase(&free,iterator.get());
    voice->initialize(callback,context); voice->priority=priority; voice->owner=this; link(voice);
    return voice;
}
/* 028AA028 */
u32 SoftwareVoiceManager::count() { return active.count; }
/* 028A95F4 */
u32 SoftwareVoice::accumulate(const be<f32>* input,be<f32>* output,u32 count,VolumeRamp* ramp) {
    u32 current=ramp->current;
    s32 delta=ramp->delta;
    if(!current && !delta) return 0;
    f32 gain[2];
    gain[0]=gabi::fmuls_ppc(static_cast<f32>(current),0.000030517578125f);
    f32 step=gabi::fmuls_ppc(static_cast<f32>(delta),0.000030517578125f);
    gain[1]=gabi::fadds_ppc(gain[0],step);
    f32 pairStep=gabi::fadds_ppc(step,step);
    for(u32 block=0;block<count/16;++block) {
        f32 destination[2]={output[0],output[1]};
        for(u32 pair=0;pair<8;++pair) {
            f32 source[2]={input[0],input[1]};
            f32 next[2]={destination[0],destination[1]};
            if(pair!=7) { next[0]=output[2]; next[1]=output[3]; }
            f32 first=gabi::fmadds(source[0],gain[0],destination[0]);
            f32 second=gabi::fmadds(source[1],gain[1],destination[1]);
            output[0]=first; output[1]=second;
            if(delta) { gain[0]=gabi::fadds_ppc(gain[0],pairStep); gain[1]=gabi::fadds_ppc(gain[1],pairStep); }
            destination[0]=next[0]; destination[1]=next[1];
            input+=2; output+=2;
        }
    }
    return 1;
}
/* 028A9818 */
void SoftwareVoice::render(MixBlock* block,be<f32>* output,u32 count,f32 outputRate) {
    VoiceRequest* request=firstRequest;
    if(!request) return;
    if(static_cast<u32>(request->state)!=2) request->state=2;
    f32 rate=static_cast<f32>(static_cast<u32>(sampleRate));
    f32 ratio=gabi::fdivs_ppc(gabi::fmuls_ppc(mixLevels.pitch,rate),outputRate);
    u32 table=0;
    if(!(ratio>1.3333333730697632f)) table=ratio>1.0f?1:2;
    f32 fixed=gabi::fmuls_ppc(65536.0f,ratio);
    u32 phase=resamplePhase;
    u32 step;
    if(fixed<2147483648.0f) step=static_cast<u32>(gabi::ftoi(fixed));
    else step=static_cast<u32>(gabi::ftoi(gabi::fsubs_ppc(fixed,2147483648.0f)))+0x80000000u;
    u32 needed=(phase+step*count)>>16;
    if(needed>2048) { step=(0x08000000u-phase)/count; needed=(phase+step*count)>>16; }
    be<s16>* samples=sDecodedSamples.items;
    for(u32 i=0;i<4;++i) samples[i]=resampleHistory[i];
    switch(static_cast<u32>(sampleFormat)) {
    case 0:decodePcm8(samples+4,needed);break;
    case 1:decodePcm16(samples+4,needed);break;
    case 2:decodeAdpcm(samples+4,needed);break;
    default:return;
    }
    u32 level=volume.current;
    be<f32>* destination=output;
    const be<s16>* source=samples;
    const be<s16>* coefficients=sResampleCoefficients.tables[table];
    for(u32 pair=0;pair<count/2;++pair) {
        phase+=step;
        u32 advance=phase>>16; phase&=65535;
        u32 firstIndex=((phase>>6)&0x3F8)/2;
        phase+=step;
        u32 secondAdvance=phase>>16; phase&=65535;
        u32 secondIndex=((phase>>6)&0x3F8)/2;
        source+=advance;
        f32 first[4]; for(u32 i=0;i<4;++i) first[i]=source[i];
        source+=secondAdvance;
        f32 second[4]; for(u32 i=0;i<4;++i) second[i]=source[i];
        f32 left[2],right[2];
        for(u32 slot=0;slot<2;++slot) {
            f32 product=gabi::fmuls_ppc(coefficients[firstIndex+slot],first[slot]);
            left[slot]=gabi::fmadds(coefficients[firstIndex+slot+2],first[slot+2],product);
            product=gabi::fmuls_ppc(coefficients[secondIndex+slot],second[slot]);
            right[slot]=gabi::fmadds(coefficients[secondIndex+slot+2],second[slot+2],product);
        }
        f32 leftSum=gabi::fadds_ppc(left[0],left[1]);
        f32 rightSum=gabi::fadds_ppc(right[0],right[1]);
        s32 delta=volume.delta;
        u32 nextLevel=(level+delta)&65535;
        f32 firstGain=gabi::fmuls_ppc(static_cast<f32>(level),0.000000000931322574615478515625f);
        f32 secondGain=gabi::fmuls_ppc(static_cast<f32>(nextLevel),0.000000000931322574615478515625f);
        level=(nextLevel+delta)&65535;
        destination[0]=gabi::fmuls_ppc(leftSum,firstGain);
        destination[1]=gabi::fmuls_ppc(rightSum,secondGain);
        destination+=2;
    }
    resamplePhase=phase;
    for(u32 i=0;i<4;++i) resampleHistory[i]=source[i];
    for(u32 channel=0;channel<6;++channel) {
        u32 result=accumulate(output,block->main[channel],count,&mainMix[channel]);
        u32 mask=block->activeChannels;
        mask|=channel==0?result:(result?(1u<<channel):0);
        block->activeChannels=mask;
    }
    for(u32 channel=0;channel<4;++channel) {
        u32 result=accumulate(output,block->remote[channel],count,&remoteMix[channel]);
        u32 mask=block->activeChannels;
        if(channel==3) block->sampleCount=count;
        block->activeChannels=mask|(result?(1u<<(channel+6)):0);
    }
}
/* 028A9EE0 */
void SoftwareVoiceManager::updateAll() {
    voiceLock(&sSoftwareMutex);
    if(!sSoftwareOutputGuard) { sSoftwareOutputGuard=1; voiceInitializeOutputModes(&sSoftwareOutputModes); }
    u32 outputMode=sSoftwareOutputModes.modes[0];
    for(u32 i=0;i<96;++i) if(voices[i].linked) voices[i].applyMix(outputMode);
    voiceUnlock(&sSoftwareMutex);
}
/* 028A9F74 */
void SoftwareVoiceManager::render(MixBlock* block,u32 count,f32 rate) {
    softwareZeroRange(&sMixScratch,count*4);
    voiceLock(&sSoftwareMutex);
    VoiceLink* node=active.root.next;
    while(node!=&active.root) {
        SoftwareVoice* voice=reinterpret_cast<SoftwareVoice*>(reinterpret_cast<u8*>(node)-offsetof(SoftwareVoice,priorityLink));
        if(!static_cast<u32>(voice->playbackMode)) voice->render(block,sMixScratch.values,count,rate);
        node=node->next;
    }
    voiceUnlock(&sSoftwareMutex);
}
/* 028AA030 */
void SoftwareMixer::setOutputRate(u32 mode) {
    if(mode==0) { sampleCount=144; sampleRate=48000.0f; }
    else if(mode==1) { sampleCount=96; sampleRate=32000.0f; }
}
/* 028AA080 */
void SoftwareMixer::initialize(be<f32>* buffer,u32 bytes) {
    u32 count=bytes/5760;
    if(count>16) count=16;
    for(u32 i=0;i<count;++i) { queue.blocks[i].initialize(buffer,5760); buffer+=1440; }
    queue.blockCount=count;
    softwareMessageInitialize(&queue.main,queue.mainMessages,16);
    softwareMessageInitialize(&queue.remote,queue.remoteMessages,16);
    manager.initialize(); setOutputRate(0);
    if(!sSoftwareOutputGuard) { sSoftwareOutputGuard=1; voiceInitializeOutputModes(&sSoftwareOutputModes); }
    softwareRegisterMixer(&sSoftwareOutputModes,this);
    softwareSetActiveManager(&manager);
}
/* 028AA174 */
void SoftwareMixer::finalize() {
    softwareSetActiveManager(nullptr);
    if(!sSoftwareOutputGuard) { sSoftwareOutputGuard=1; voiceInitializeOutputModes(&sSoftwareOutputModes); }
    softwareUnregisterMixer(&sSoftwareOutputModes,this);
    manager.releaseAll();
    gabi::Local<gptr<MixBlock>> message;
    while(softwareMessageReceive(&queue.main,message.get(),0)) {}
    while(softwareMessageReceive(&queue.remote,message.get(),0)) {}
    softwareMessageFinalize(&queue.main); softwareMessageFinalize(&queue.remote);
}
/* 028AA234 */
void SoftwareMixer::update() { manager.updateAll(); }
/* 028AA23C */
u32 SoftwareMixer::render() {
    u32 active=manager.count();
    if(!active) return active;
    MixBlock* selected=nullptr;
    for(u32 i=0;i<queue.blockCount;++i) {
        MixBlock* block=&queue.blocks[i];
        if(softwareEventTryWait(&block->mainReady) && softwareEventTryWait(&block->remoteReady)) {
            softwareEventReset(&block->mainReady); softwareEventReset(&block->remoteReady);
            selected=block; break;
        }
    }
    if(!selected) return 0;
    softwareZeroRange(selected->main[0],5760);
    selected->activeChannels=0;
    f32 rate=sampleRate;
    u32 samples=sampleCount;
    manager.render(selected,samples,rate);
    softwareMessageSend(&queue.main,selected,1); softwareMessageSend(&queue.remote,selected,1);
    return active;
}
/* 028AA374 */
void SoftwareMixer::addSamples(const be<f32>* input,be<s32>* output,u32 count) {
    for(u32 i=0;i<(count/8)*8;++i) {
        f32 sample=input[i];
        u32 old=output[i];
        output[i]=old+static_cast<u32>(gabi::ftoi(sample));
    }
}
/* 028AA490 */
void SoftwareMixer::mixMain(MixerOutput* output) {
    gabi::Local<gptr<MixBlock>> message;
    if(softwareMessageReceive(&queue.main,message.get(),0)) {
        MixBlock* block=*message;
        u32 count=block->sampleCount;
        if(static_cast<u16>(output->sampleCount)==count) {
            u32 channels=output->channelCount;
            u32 mask=block->activeChannels;
            u32 used=channels>=6?6:channels>=4?4:channels>=2?2:0;
            for(u32 i=0;i<used;++i) if(mask&(1u<<i)) {
                be<s32>* destination=output->channels[i];
                const be<f32>* source=block->main[i];
                addSamples(source,destination,count);
            }
        }
        softwareEventSignal(&block->mainReady);
    }
}
/* 028AA64C */
void SoftwareMixer::mixRemote(MixerOutput* output) {
    gabi::Local<gptr<MixBlock>> message;
    if(softwareMessageReceive(&queue.remote,message.get(),0)) {
        MixBlock* block=*message;
        u32 count=block->sampleCount;
        if(static_cast<u16>(output->sampleCount)==count) {
            u32 channels=output->channelCount;
            u32 mask=block->activeChannels;
            u32 used=channels>=4?4:channels>=2?2:0;
            for(u32 i=0;i<used;++i) if(mask&(1u<<(i+6))) {
                be<s32>* destination=output->channels[i];
                const be<f32>* source=block->remote[i];
                addSamples(source,destination,count);
            }
        }
        softwareEventSignal(&block->remoteReady);
    }
}
/* 028AA764 */
static void initializeSoftwareVoiceMutex() {
    sSoftwareMutex.initialized=1; softwareInitializeMutex(&sSoftwareMutex); softwareRegisterStatic(&sSoftwareStaticRegistration);
}
/* WWHD glue
data sSoftwareOutputModes 101FCE00
data sSoftwareOutputGuard 101FD8C8
data sMixScratch 104D3660
data sSoftwareStaticRegistration 101FCB64
ext C0009D68 void softwareInitializeMutex(void*)
ext 028F026C void softwareRegisterStatic(void*)
ext 028F6C2C void softwareMessageInitialize(nw::snd::VoiceMessageQueue*,void*,u32)
ext 028F6C94 u32 softwareMessageReceive(nw::snd::VoiceMessageQueue*,gptr<nw::snd::MixBlock>*,u32)
ext 028F6C30 u32 softwareMessageSend(nw::snd::VoiceMessageQueue*,nw::snd::MixBlock*,u32)
ext 028F6CD4 void softwareMessageFinalize(nw::snd::VoiceMessageQueue*)
ext 028A5D70 void softwareSetActiveManager(nw::snd::SoftwareVoiceManager*)
ext 0288A004 void softwareRegisterMixer(void*,nw::snd::SoftwareMixer*)
ext 0288A038 void softwareUnregisterMixer(void*,nw::snd::SoftwareMixer*)
data sDecodedSamples 104D38A8
data sResampleCoefficients 1018724C
ext 028B1AF0 void softwareDecodeAdpcm(u32,nw::snd::AdpcmLoop*,be<u32>*,const void*,u32,be<s16>*)
ext 028F69A8 void softwareEventInitialize(nw::snd::VoiceEvent*,u32,u32)
ext 028F6A24 void softwareEventSignal(nw::snd::VoiceEvent*)
ext 028F6A30 u32 softwareEventTryWait(nw::snd::VoiceEvent*)
ext 028F6A28 void softwareEventReset(nw::snd::VoiceEvent*)
ext 0286DB18 void softwareListErase(nw::snd::VoiceList*,const gptr<nw::snd::VoiceLink>*)
ext C00088C8 void softwareZeroRange(void*,u32)
ext C0009990 void softwareBlockSet(void*,u32,u32)
ext C000A848 void* softwareCopy(void*,const void*,u32)
data sSoftwareMutex 104D48B0
ext 028A8BFC void softwareRemoveVoice(void*,nw::snd::SoftwareVoice*)
ext 028A8E58 void softwareReorderVoice(void*,nw::snd::SoftwareVoice*)
*/
