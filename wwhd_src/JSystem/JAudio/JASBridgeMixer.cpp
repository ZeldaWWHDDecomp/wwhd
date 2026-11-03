/* Virtual DSP buses, gain ramps and modulated delay, reconstructed from WWHD. */
#include "JSystem/JAudio/JASBridgeMixer.h"
static void* voiceMixerHeap;
static JASBridge::MixerBusMap mixerBusMap;
static JASBridge::MixerPanTable mixerPanTable;
static JASBridge::MixerBusCodes mixerBusCodes;
static JASBridge::MixerStatics voiceMixerStatics;
static JASBridge::MixerDestructorRecord voiceMixerDestructorRecord;
/* 0281B81C */
void JASBridge::initializeMixerDelay(MixerDelayState* state) {
    state->direction = -1;
    state->writeHalf = 0;
    state->step = 0x7F00;
    state->excursion = 0x100;
    state->readPosition = 150;
    state->fraction = 0;
    state->forwardThreshold = 16;
    state->reverseThreshold = 48;
}
/* 0281B85C */
JASBridge::VoiceMixer* JASBridge::constructVoiceMixer(VoiceMixer* object) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!object) object = static_cast<VoiceMixer*>(allocateVoiceMixer(sizeof(VoiceMixer)));
    if (object) {
        object->masterGain = 0x4000;
        object->outputBlock = 0;
        object->blockCount = 7;
        object->delayStorage = nullptr;
        initializeMixerDelay(&object->delay);
        for (u32 bus = 0; bus < 13; ++bus) {
            void* samples = allocateVoiceMixerSamples(0xC0, voiceMixerHeap, 0x40);
            object->buses[bus] = static_cast<be<s16>*>(samples);
            clearVoiceMixerStorage(samples, 0, 0xC0);
        }
        void* storage = allocateVoiceMixerSamples(0x140, voiceMixerHeap, 0x40);
        object->delayStorage = static_cast<be<s16>*>(storage);
        clearVoiceMixerStorage(storage, 0, 0x140);
        initializeMixerDelay(&object->delay);
    }
    return object;
}
/* 0281B934 */
be<s16>* JASBridge::VoiceMixer::bus(u32 index) {
    u32 physical = mixerBusMap.physical[index];
    return buses[physical];
}
/* 0281B94C */
void JASBridge::VoiceMixer::scaleGains(be<f32>* destination, be<f32>* source, f32 gain) {
    for (u32 channel = 0; channel < 4; ++channel) {
        f32 value = source[channel];
        destination[channel] = gabi::fmuls_ppc(value, gain);
    }
}
/* 0281B970 */
f32 JASBridge::VoiceMixer::ramp(be<s16>* destination, be<s16>* source, f32 current, f32 target) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (current == 0.0f && target == 0.0f) return 0.0f;
    f32 increment = gabi::fdivs_ppc(gabi::fsubs_ppc(target, current), 80.0f);
    for (u32 sample = 0; sample < 80; ++sample) {
        s32 output = destination[sample];
        s32 input = source[sample];
        f32 value = gabi::fmadds((f32)input, current, (f32)output);
        if (value < -32768.0f) value = -32768.0f;
        else if (value > 32767.0f) value = 32767.0f;
        s32 mixed = gabi::ftoi(value);
        current = gabi::fadds_ppc(current, increment);
        destination[sample] = (s16)mixed;
    }
    return current;
}
/* 0281BA60 */
void JASBridge::VoiceMixer::mixAutomaticVoice(DSPBuffer_l* voice, be<s16>* samples) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<MixerPanLocals> gains(0x10);
    if (voice->fade) {
        u32 volume = (u32)voice->volume >> 1;
        voice->targetVolume = volume;
        if (!volume) voice->stopped = 1;
    }
    u32 volume = voice->volume;
    f32 current = gabi::fdivs_ppc((f32)volume, 65535.0f);
    u32 pan = voice->pan;
    f32 front = mixerPanTable.gains[((pan >> 8) ^ 127)];
    f32 left = mixerPanTable.gains[(pan & 127) ^ 127];
    f32 right = mixerPanTable.gains[pan & 127];
    u32 surround = voice->surround;
    f32 rear = mixerPanTable.gains[pan >> 8];
    gains->base[0] = gabi::fmuls_ppc(front, left);
    gains->base[1] = gabi::fmuls_ppc(rear, left);
    gains->base[2] = gabi::fmuls_ppc(front, right);
    gains->base[3] = gabi::fmuls_ppc(rear, right);
    f32 ratio = gabi::fdivs_ppc((f32)surround, 32767.0f);
    scaleGains(gains->current, gains->base, current);
    current = gabi::fmuls_ppc(ratio, current);
    scaleGains(gains->current + 4, gains->base, current);
    u32 target = voice->targetVolume;
    f32 next = gabi::fdivs_ppc((f32)target, 65535.0f);
    voice->volume = target;
    scaleGains(gains->target, gains->base, next);
    next = gabi::fmuls_ppc(next, ratio);
    scaleGains(gains->target + 4, gains->base, next);
    for (u32 channel = 0; channel < 8; ++channel) {
        be<s16>* destination = bus(channel);
        f32 initial = gains->current[channel];
        f32 final = gains->target[channel];
        ramp(destination, samples, initial, final);
    }
}
/* 0281BC44 */
void JASBridge::VoiceMixer::mixExplicitVoice(DSPBuffer_l* voice, be<s16>* samples) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (u32 history = 0; history < (u32)voice->historyCount; ++history) {
        samples[-1 - (s32)history] = voice->mixingHistory[15 - (s32)history];
    }
    if (voice->fade) {
        s32 sum = 0;
        for (u32 bus = 0; bus < 6; ++bus) {
            s32 gain = (s32)voice->buses[bus].currentGain >> 1;
            voice->buses[bus].targetGain = gain;
            sum += gain;
        }
        if (!sum) voice->stopped = 1;
    }
    for (u32 busIndex = 0; busIndex < 6; ++busIndex) {
        MixerVoiceBus_l* descriptor = &voice->buses[busIndex];
        u32 code = descriptor->code;
        be<s16>* destination = nullptr;
        if (code) {
            s32 physical = physicalBus(code);
            destination = buses[physical];
        }
        s32 targetGain = descriptor->targetGain;
        s32 currentGain = descriptor->currentGain;
        f32 target = gabi::fdivs_ppc((f32)targetGain, 32767.0f);
        f32 current = gabi::fdivs_ppc((f32)currentGain, 32767.0f);
        u32 delay = descriptor->delay;
        u32 desired = delay >> 8;
        u32 offset = delay & 255;
        if (desired > offset) descriptor->delay = delay + 1;
        else if (desired < offset) descriptor->delay = delay - 1;
        be<s16>* delayed = samples - offset;
        if (code) {
            f32 gain = ramp(destination, delayed, current, target);
            descriptor->currentGain = (s16)gabi::ftoi(gabi::fmuls_ppc(gain, 32767.0f));
        }
    }
    for (u32 history = 0; history < (u32)voice->historyCount; ++history) {
        voice->mixingHistory[15 - (s32)history] = samples[79 - (s32)history];
    }
}
/* 0281BC14 */
s32 JASBridge::VoiceMixer::physicalBus(u32 index) {
    for (u32 physical = 0; physical < 13; ++physical) {
        if ((u32)mixerBusCodes.codes[physical] == index) return physical;
    }
    return -1;
}
/* 0281BEA4 */
void JASBridge::VoiceMixer::mixVoice(DSPBuffer_l* voice, be<s16>* samples) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (voice->automaticMode) mixAutomaticVoice(voice, samples);
    else mixExplicitVoice(voice, samples);
}
/* 0281BEB8 */
void JASBridge::VoiceMixer::commitDelay() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 half = delay.writeHalf;
    be<s16>* source = buses[9];
    be<s16>* storage = delayStorage;
    delay.writeHalf = half ? 0 : 80;
    copyVoiceMixerStorage(storage + half, source, 160);
}
/* 0281BEEC */
void JASBridge::VoiceMixer::renderDelay() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    commitDelay();
    s32 position = delay.readPosition;
    s32 remaining = 160 - position;
    if (remaining >= 80) remaining -= 80;
    if (delay.direction >= 0) {
        if (remaining - (s32)(u32)delay.forwardThreshold <= 0) {
            delay.step = 0x8000 - (u32)delay.excursion;
            delay.direction = -(s32)delay.direction;
        }
    } else {
        if ((s32)(u32)delay.reverseThreshold - remaining <= 0) {
            delay.step = 0x8000 + (u32)delay.excursion;
            delay.direction = -(s32)delay.direction;
        }
    }
    u32 step = (u32)delay.step * 2;
    be<s16>* destination = buses[9];
    u32 fraction = delay.fraction;
    for (u32 sample = 0; sample < 80; ++sample) {
        fraction += step;
        s32 oldPosition = position;
        position = (s32)((u32)position + (u32)((s32)fraction >> 16));
        be<s16>* storage = delayStorage;
        s16 value = storage[oldPosition];
        destination[sample] = value;
        fraction &= 0xFFFF;
        position %= 160;
    }
    delay.readPosition = position;
    delay.fraction = fraction;
}
/* 0281BFF4 */
void JASBridge::VoiceMixer::blendBlock(be<s16>* destination, be<s16>* source, s32 count, s32 gain) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (s32 sample = 0; sample < count; ++sample) {
        s32 value = source[sample];
        s32 product = (s32)((u32)value * (u32)gain);
        s32 scaled = clampBridgeSample(product >> 15);
        destination[sample] = (s32)destination[sample] + scaled;
    }
}
/* 0281C070 */
void JASBridge::VoiceMixer::scaleBlock(be<s16>* destination, be<s16>* source, s32 count) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (s32 sample = 0; sample < count; ++sample) {
        s32 value = source[sample];
        s32 gain = masterGain;
        s32 product = (s32)((u32)value * (u32)gain);
        destination[sample] = clampBridgeSample(product >> 12);
    }
}
/* 0281C0E8 */
void JASBridge::VoiceMixer::render(be<s16>* first, be<s16>* second, be<s16>* third, be<s16>* fourth) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    s32 limit = blockCount;
    s32 block = outputBlock;
    if (block >= limit) return;
    renderDelay();
    be<s16>* auxiliary = buses[9];
    blendBlock(buses[1], auxiliary, 80, 0x3FFF);
    blendBlock(buses[2], auxiliary, 80, 0x3FFF);
    auxiliary = buses[10];
    blendBlock(buses[1], auxiliary, 80, 0x5A82);
    blendBlock(buses[2], auxiliary, 80, 0x5A82);
    blendBlock(buses[2], buses[8], 80, 0x8000);
    s32 offset = (s32)((u32)(s32)outputBlock * 80);
    first += offset;
    second += offset;
    third += offset;
    fourth += offset;
    be<s16>* samples = bus(0);
    scaleBlock(first, samples, 80);
    samples = bus(1);
    scaleBlock(second, samples, 80);
    samples = bus(2);
    scaleBlock(third, samples, 80);
    samples = bus(3);
    scaleBlock(fourth, samples, 80);
    outputBlock = (s32)((u32)(s32)outputBlock + 1);
}
/* 0281C260 */
void JASBridge::VoiceMixer::restartOutput() { outputBlock = 0; }
/* 0281C26C */
void JASBridge::VoiceMixer::clearBuses() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (u32 bus = 0; bus < 13; ++bus) clearVoiceMixerStorage(buses[bus], 0, 160);
}
/* 0281C2B8 */
void JASBridge::VoiceMixer::addBus(u32 index, be<s16>* samples, s32 gain) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    s32 physical = physicalBus(index);
    blendBlock(buses[physical], samples, 80, gain);
}
/* 0281C2F4 */
void JASBridge::initializeVoiceMixerStatics() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    voiceMixerStatics.words[2] = 0;
    voiceMixerStatics.words[0] = 0;
    voiceMixerStatics.words[3] = 0;
    voiceMixerStatics.words[1] = 0;
    registerVoiceMixerStatics(&voiceMixerDestructorRecord);
}
/* WWHD glue
data voiceMixerHeap 101F9DA8
data mixerBusMap 1017086C
data mixerPanTable 10170640
data mixerBusCodes 1017088C
data voiceMixerStatics 104B56F0
data voiceMixerDestructorRecord 101FA028
ext 0273AD10 void* allocateVoiceMixer(u32)
ext 0273B0D4 void* allocateVoiceMixerSamples(u32,void*,s32)
ext 028F026C void registerVoiceMixerStatics(JASBridge::MixerDestructorRecord*)
ext C000A858 void clearVoiceMixerStorage(void*,u32,u32)
ext C000A848 void copyVoiceMixerStorage(void*,const void*,u32)
*/
