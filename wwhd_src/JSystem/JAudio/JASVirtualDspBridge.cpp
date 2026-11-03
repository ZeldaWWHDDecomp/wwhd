/* Virtual DSP worker: owns the decoder, resampler, filters, voice mixer and effects lines, renders
 * one 80-sample frame per sync message on its own thread, and the profile-name string it uses. */
#include "JSystem/JAudio/JASVirtualDspBridge.h"
static void* virtualDspHeap;
static JASBridge::VirtualDsp* virtualDspInstance;
static JASThreadVTable virtualDspThreadTable;
static JASBridge::VirtualDspProfileTable virtualDspProfileTable;
static JASBridge::VirtualDspProfileLabel virtualDspProfileLabel;
static JASBridge::VirtualDspStatics virtualDspStatics;
static JASBridge::VirtualDspDestructorRecord virtualDspDestructorRecord;
/* 0281B154 */
void JASBridge::VirtualDspProfileName::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this && (flags & 1)) freeVirtualDspProfileName(this);
}
/* 0281B168 */
void JASBridge::VirtualDspProfileName::assureTermination() {}
/* 0281AA98 */
JASBridge::VirtualDsp* JASBridge::constructVirtualDsp(VirtualDsp* object, s32 priority, u32 affinity,
                                                   u32 messages, u32 stackSize) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!object) object = static_cast<VirtualDsp*>(allocateVirtualDsp(sizeof(VirtualDsp)));
    if (!object) return object;
    constructVirtualDspThread(&object->thread, virtualDspHeap, stackSize, messages, priority, affinity);
    object->effectsMixer = nullptr;
    object->processing = 0;
    object->decodeStorage = nullptr;
    object->sampleStorage = nullptr;
    object->sampleScratch = nullptr;
    object->mixer = nullptr;
    object->filters = nullptr;
    object->channels = nullptr;
    object->currentSamples = nullptr;
    object->thread.mVtable = &virtualDspThreadTable;
    object->channelCount = 0;
    object->bootCallback = 0;
    object->decodeScratch = nullptr;
    object->resampler = nullptr;
    object->decoder = nullptr;
    object->effects = nullptr;
    for (u32 plane = 0; plane < 4; ++plane) {
        void* samples = allocateVirtualDspStorage(0x640, virtualDspHeap, 0x40);
        object->outputPlanes[plane] = static_cast<be<s16>*>(samples);
        clearVirtualDspStorage(samples, 0, 0x640);
    }
    auto* decoder = static_cast<SampleDecoder*>(allocateVirtualDspObject(sizeof(SampleDecoder), virtualDspHeap, 0x40));
    if (decoder) decoder = constructSampleDecoder(decoder);
    object->decoder = decoder;
    auto* resampler = static_cast<Resampler*>(allocateVirtualDspObject(sizeof(Resampler), virtualDspHeap, 0x40));
    if (resampler) resampler = constructResampler(resampler);
    object->resampler = resampler;
    auto* filters = static_cast<ChannelFilters*>(allocateVirtualDspObject(sizeof(ChannelFilters), virtualDspHeap, 0x40));
    if (filters) filters = constructChannelFilters(filters);
    object->filters = filters;
    auto* mixer = static_cast<VoiceMixer*>(allocateVirtualDspObject(sizeof(VoiceMixer), virtualDspHeap, 0x40));
    if (mixer) mixer = constructVoiceMixer(mixer);
    object->mixer = mixer;
    auto* effects = static_cast<EffectsMixer*>(allocateVirtualDspObject(sizeof(EffectsMixer), virtualDspHeap, 0x40));
    if (effects) effects = constructEffectsMixer(effects);
    object->effectsMixer = effects;
    u8* decodeStorage = static_cast<u8*>(allocateVirtualDspStorage(0x7C0, virtualDspHeap, 0x40));
    object->decodeStorage = decodeStorage;
    object->decodeScratch = reinterpret_cast<be<s16>*>(decodeStorage + 0x40);
    u8* sampleStorage = static_cast<u8*>(allocateVirtualDspStorage(0xE0, virtualDspHeap, 0x40));
    object->sampleStorage = sampleStorage;
    object->sampleScratch = reinterpret_cast<be<s16>*>(sampleStorage + 0x40);
    be<s16>* feedback = object->mixer->buses[0];
    object->decoder->feedback = reinterpret_cast<be<u16>*>(feedback);
    object->effectsMixer->mixer = object->mixer;
    return object;
}
/* 0281AC78 */
void JASBridge::createVirtualDsp(s32 priority, u32 affinity) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* object = static_cast<VirtualDsp*>(allocateVirtualDspObject(sizeof(VirtualDsp), virtualDspHeap, 4));
    if (object) object = constructVirtualDsp(object, priority, affinity, 0x40, 0x4000);
    virtualDspInstance = object;
    resumeVirtualDspThread(object->thread.mThread);
}
/* 0281ACEC */
void JASBridge::VirtualDsp::setBootCallback(u32 callback) { bootCallback = callback; }
/* 0281ACF4 */
void JASBridge::VirtualDsp::setChannels(u32 count, DSPBuffer_l* channelBuffer, EffectLine_l* effectBuffer) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    channelCount = count;
    channels = channelBuffer;
    effects = effectBuffer;
    EffectLine_l* current = effectBuffer;
    for (u32 line = 0; line < 4; ++line) {
        effectsMixer->assignLine(line, current + line);
        if (line != 3) current = effects;
    }
}
/* 0281AD74 */
void JASBridge::VirtualDsp::setMixerGain(f64 ratio) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    /* Only the integer conversion reaches memory, so no NaN payload is observable here. */
    f32 scaled = (f32)(ratio * 4096.0);
    mixer->masterGain = gabi::ftoi(scaled);
}
/* 0281ADA4 */
be<s16>* JASBridge::VirtualDsp::outputPlane(u32 index) { return outputPlanes[index]; }
/* 0281ADB4 */
void JASBridge::VirtualDsp::finishOutput() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    mixer->restartOutput();
}
/* 0281ADBC */
void JASBridge::VirtualDsp::processFrame() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    mixer->clearBuses();
    effectsMixer->beginBlock();
    decoder->updateNoise();
    clearVirtualDspStorage(decodeStorage, 0, 0xE0);
    clearVirtualDspStorage(sampleStorage, 0, 0xE0);
    for (u32 index = 0; index < (u32)channelCount; ++index) {
        DSPBuffer_l* channel = channels.get() + index;
        if (!channel->active || channel->stopped == 1) continue;
        if (channel->constant) {
            s16 value = channel->lastSample;
            for (u32 sample = 0; sample < 80; ++sample) {
                be<s16>* destination = sampleScratch;
                destination[sample] = value;
            }
        } else {
            u32 format = channel->format;
            bool resample = true;
            if (format == 5 || format == 9) decoder->renderAdpcm(channel, decodeScratch);
            else if (format == 8) decoder->renderPcm8(channel, decodeScratch);
            else if (format == 16) decoder->renderPcm16(channel, decodeScratch);
            else if (format == 32) { decoder->renderDirectStream(channel, sampleScratch); resample = false; }
            else if (format == 33) decoder->renderResampledStream(channel, decodeScratch);
            else { decoder->synthesize(channel, sampleScratch); resample = false; }
            if (resample) resampler->process(channel, decodeScratch, sampleScratch);
        }
        s32 coefficient = channel->parameters.distance;
        if (coefficient) filters->distance(channel, sampleScratch, coefficient);
        be<s16>* samples = sampleScratch;
        currentSamples = samples;
        u32 flags = channel->filterMode;
        if (flags & 31) {
            samples = filters->processFir(channel, samples);
            currentSamples = samples;
            flags = channel->filterMode;
        }
        if (flags & 32) {
            filters->processBiquad(channel, samples);
            samples = currentSamples;
        }
        mixer->mixVoice(channel, samples);
        if (!channel->constant) channel->reset = 0;
    }
    mixer->render(outputPlanes[0], outputPlanes[1], outputPlanes[2], outputPlanes[3]);
    effectsMixer->endBlock();
}
/* 0281B024 */
void* JASBridge::VirtualDsp::run() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<VirtualDspWorkerLocals> locals(8);
    for (;;) {
        // HD: frame-exact for cross-build save states (the thread waits here): the original's return
        // address and r29..r31 (0, 1, this) at the wait.
        gabi::call_site(0x0281B058, {{29, 0}, {30, 1}, {31, gabi::ea(this)}}, {}, 0xC0009ED0, &thread.mQueue,
                        &locals->message, 1u);
        u32 command = locals->message.words[0];
        if (command == 1) { exitVirtualDspThread(0); return nullptr; }
        if (command != 0) continue;
        if (!processing) {
            processing = 1;
            locals->name.vtable = &virtualDspProfileTable;
            locals->name.text = virtualDspProfileLabel.text;
            constructVirtualDspProfile(&locals->profile, &locals->name);
            processFrame();
            destroyVirtualDspProfile(&locals->profile, 2);
            u32 callback = bootCallback;
            if (callback) gabi::call_ptr<void>(callback, 0u);
        }
        processing = 0;
    }
}
/* 0281B0FC */
void JASBridge::VirtualDsp::sync() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<VirtualDspMessage> message(8);
    message->words[0] = 0;
    sendVirtualDspMessage(&thread.mQueue, message.get(), 0);
}
/* 0281B12C */
void JASBridge::initializeVirtualDspStatics() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    virtualDspStatics.words[2] = 0;
    virtualDspStatics.words[0] = 0;
    virtualDspStatics.words[3] = 0;
    virtualDspStatics.words[1] = 0;
    registerVirtualDspStatics(&virtualDspDestructorRecord);
}
/* WWHD glue
data virtualDspHeap 101F9DA8
data virtualDspInstance 101FA00C
data virtualDspThreadTable 101705FC
data virtualDspProfileTable 101705C0
data virtualDspProfileLabel 101705DC
data virtualDspStatics 104B56C0
data virtualDspDestructorRecord 101FA000
ext 0273AF40 void freeVirtualDspProfileName(void*)
ext 0273AD10 void* allocateVirtualDsp(u32)
ext 0273B050 void* allocateVirtualDspObject(u32,void*,s32)
ext 0273B0D4 void* allocateVirtualDspStorage(u32,void*,s32)
ext 02810D34 JASThread* constructVirtualDspThread(JASThread*,void*,u32,u32,s32,u32)
ext 02844088 void constructVirtualDspProfile(JASBridge::VirtualDspProfileState*,JASBridge::VirtualDspProfileName*)
ext 028440C4 void destroyVirtualDspProfile(JASBridge::VirtualDspProfileState*,u32)
ext 028F026C void registerVirtualDspStatics(JASBridge::VirtualDspDestructorRecord*)
ext C000A858 void clearVirtualDspStorage(void*,u32,u32)
ext C0009F38 void resumeVirtualDspThread(void*)
ext C0009ED0 void receiveVirtualDspMessage(JASMessageQueue*,JASBridge::VirtualDspMessage*,u32)
ext C0009FC8 void sendVirtualDspMessage(JASMessageQueue*,JASBridge::VirtualDspMessage*,u32)
ext C0009B30 void exitVirtualDspThread(u32)
*/
