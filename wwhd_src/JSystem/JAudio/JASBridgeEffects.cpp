/* Virtual DSP effect mixer operations, reconstructed from WWHD. */
#include "JSystem/JAudio/JASBridgeEffects.h"
static JASBridge::EffectStatics effectStatics;
static JASBridge::EffectDestructorRecord effectDestructorRecord;
/* 0281B470 */
s16 JASBridge::clampBridgeSample(s32 sample) {
    if (sample <= -32768) return -32768;
    if (sample >= 32767) return 32767;
    return (s16)sample;
}


/* 0281B498 */
JASBridge::EffectsMixer* JASBridge::constructEffectsMixer(EffectsMixer* object) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!object) object = static_cast<EffectsMixer*>(allocateEffectsMixer(sizeof(EffectsMixer)));
    if (object) {
        object->mixer = nullptr;
        for (u32 line = 0; line < 4; ++line) {
            object->lines[line] = nullptr;
            object->positions[line] = 0;
        }
    }
    return object;
}
/* 0281B4EC */
void JASBridge::EffectsMixer::filter(be<s16>* samples, be<s16>* coefficients) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (u32 sample = 0; sample < 80; ++sample) {
        f32 sum = 0.0f;
        for (u32 tap = 0; tap < 8; ++tap) {
            s32 value = samples[sample + tap];
            s32 weight = coefficients[tap];
            sum = gabi::fmadds((f32)value, (f32)weight, sum);
        }
        f32 value = gabi::fmuls_ppc(sum, 1.0f / 32768.0f);
        if (value < -32768.0f) value = -32768.0f;
        else if (value > 32767.0f) value = 32767.0f;
        samples[sample] = (s16)gabi::ftoi(value);
    }
}
/* 0281B5D4 */
void JASBridge::EffectsMixer::processLine(EffectLine_l* effect, u32 index) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    be<s16>* samples = mixer->bus(index + 4);
    u32 position = positions[index];
    be<s16>* storage = effect->storage;
    copyEffectsSamples(samples + 8, storage + position * 80, 160);
    if (effect->flags & 1) filter(samples, effect->coefficients);
    u32 first = effect->firstBus;
    if (first) mixer->addBus(first, samples, effect->firstGain);
    u32 second = effect->secondBus;
    if (second) mixer->addBus(second, samples, effect->secondGain);
    if (effect->flags & 2) filter(samples, effect->coefficients);
}
/* 0281B6B8 */
void JASBridge::EffectsMixer::beginBlock() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (u32 index = 0; index < 4; ++index) {
        EffectLine_l* effect = lines[index];
        if (effect && effect->flags) {
            be<s16>* samples = mixer->bus(index + 4);
            copyEffectsSamples(samples, samples + 80, 16);
            processLine(effect, index);
        }
    }
}
/* 0281B73C */
void JASBridge::EffectsMixer::endBlock() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (u32 index = 0; index < 4; ++index) {
        EffectLine_l* effect = lines[index];
        if (effect && effect->flags) {
            u32 position = positions[index];
            be<s16>* destination = effect->storage.get() + position * 80;
            be<s16>* samples = mixer->bus(index + 4);
            copyEffectsSamples(destination, samples, 160);
            u32 next = (u32)positions[index] + 1;
            positions[index] = next;
            if (next >= (u32)effect->blockCount) positions[index] = 0;
        }
    }
}
/* 0281B7E8 */
void JASBridge::EffectsMixer::assignLine(u32 index, EffectLine_l* line) {
    lines[index] = line;
}
/* 0281B7F4 */
void JASBridge::initializeEffectsMixerStatics() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    effectStatics.words[2] = 0;
    effectStatics.words[0] = 0;
    effectStatics.words[3] = 0;
    effectStatics.words[1] = 0;
    registerEffectsMixerStatics(&effectDestructorRecord);
}
/* WWHD glue
data effectStatics 104B56E0
data effectDestructorRecord 101FA01C
ext 0273AD10 void* allocateEffectsMixer(u32)
ext 028F026C void registerEffectsMixerStatics(JASBridge::EffectDestructorRecord*)
ext C000A848 void copyEffectsSamples(void*,const void*,u32)
*/
