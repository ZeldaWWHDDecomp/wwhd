/* Fixed-point block resampling, reconstructed from the assigned WWHD code.
 * The guest coefficient table is referenced in place, without copying assets. */
#include "JSystem/JAudio/JASBridgeResampler.h"
static void* resamplerHeap;
static JASBridge::ResamplerCoefficients resamplerCoefficients;
static JASBridge::ResamplerStatics resamplerStatics;
static JASBridge::ResamplerDestructorRecord resamplerDestructorRecord;

/* 0281C31C */
JASBridge::Resampler* JASBridge::constructResampler(Resampler* object) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!object) object = static_cast<Resampler*>(allocateBridgeResampler(sizeof(Resampler)));
    if (object) {
        object->coefficientIndices = nullptr;
        object->coefficientIndices = static_cast<be<u16>*>(allocateResamplerIndices(0xA0, resamplerHeap, 0x40));
    }
    return object;
}
/* 0281C37C */
void JASBridge::Resampler::process(DSPBuffer_l* channel, be<s16>* input, be<s16>* output) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    be<s16>* window = input - 4;
    for (u32 tap = 0; tap < 4; ++tap) window[tap] = channel->resampleHistory[tap];
    u32 step = (u32)channel->pitch << 4;
    u32 phase = (u32)channel->phase << 4;
    if ((step >> 16) >= 4) {
        for (u32 sample = 0; sample < 80; ++sample) {
            s16 value = window[(phase >> 16) & 0xFFFF];
            phase += step;
            output[sample] = value;
        }
    } else {
        u32 position = phase;
        for (u32 sample = 0; sample < 80; ++sample) {
            be<u16>* indices = coefficientIndices;
            indices[sample] = (position >> 8) & 0xFC;
            position += step;
        }
        for (u32 sample = 0; sample < 80; ++sample) {
            be<u16>* indices = coefficientIndices;
            u32 coefficient = indices[sample];
            be<s16>* samples = window + ((phase >> 16) & 0xFFFF);
            s32 sum = 0;
            for (u32 tap = 0; tap < 4; ++tap) {
                s32 weight = resamplerCoefficients.taps[coefficient + tap];
                s32 value = samples[tap];
                sum = (s32)((u32)sum + (u32)value * (u32)weight);
            }
            phase += step;
            output[sample] = sum >> 16;
        }
    }
    s16 last = output[79];
    channel->phase = (phase >> 4) & 0xFFF;
    be<s16>* next = window + ((phase >> 16) & 0xFFFF);
    channel->lastSample = last;
    for (u32 tap = 0; tap < 4; ++tap) channel->resampleHistory[tap] = next[tap];
}
/* 0281C4DC */
void JASBridge::initializeResamplerStatics() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    resamplerStatics.words[2] = 0;
    resamplerStatics.words[0] = 0;
    resamplerStatics.words[3] = 0;
    resamplerStatics.words[1] = 0;
    registerResamplerStatics(&resamplerDestructorRecord);
}
/* WWHD glue
data resamplerHeap 101F9DA8
data resamplerCoefficients 101708A8
data resamplerStatics 104B5700
data resamplerDestructorRecord 101FA034
ext 0273AD10 void* allocateBridgeResampler(u32)
ext 0273B0D4 void* allocateResamplerIndices(u32,void*,s32)
ext 028F026C void registerResamplerStatics(JASBridge::ResamplerDestructorRecord*)
*/
