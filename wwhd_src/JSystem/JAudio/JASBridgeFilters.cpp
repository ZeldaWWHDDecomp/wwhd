/* Virtual DSP distance, FIR and recursive filters, reconstructed from WWHD. */
#include "JSystem/JAudio/JASBridgeFilters.h"
static JASBridge::FilterStatics filterStatics;
static JASBridge::FilterDestructorRecord filterDestructorRecord;
/* 0281B16C */
JASBridge::ChannelFilters* JASBridge::constructChannelFilters(ChannelFilters* object) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (!object) object = static_cast<ChannelFilters*>(allocateChannelFilters(sizeof(ChannelFilters)));
    return object;
}
/* 0281B198 */
void JASBridge::ChannelFilters::distance(DSPBuffer_l* channel, be<s16>* samples, s32 coefficient) {
    s32 previousOutput;
    s32 previousInput;
    if (!channel->reset) {
        previousInput = channel->distanceInput;
        previousOutput = channel->distanceOutput;
    } else {
        previousOutput = 0;
        previousInput = 0;
        channel->distanceOutput = 0;
        channel->distanceInput = 0;
    }
    for (u32 sample = 0; sample < 80; ++sample) {
        s32 input = samples[sample];
        s32 product = (s32)((u32)(input - previousInput) * (u32)coefficient);
        s32 value = (s32)((u32)(product >> 8) + (u32)previousOutput);
        previousInput = input;
        if (value < -32768) previousOutput = -32768;
        else if (value > 32767) previousOutput = 32767;
        else previousOutput = (s16)value;
        samples[sample] = previousOutput;
    }
    channel->distanceOutput = previousOutput;
    channel->distanceInput = previousInput;
}
/* 0281B22C */
void JASBridge::ChannelFilters::fir(be<s16>* samples, be<s16>* coefficients, s32 count) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    for (u32 sample = 0; sample < 80; ++sample) {
        s32 sum = 0;
        for (s32 tap = 0; tap < count; ++tap) {
            s32 value = samples[sample + tap];
            s32 weight = coefficients[tap];
            sum = (s32)((u32)sum + (u32)value * (u32)weight);
        }
        samples[sample] = clampBridgeSample(sum >> 16);
    }
}
/* 0281B2BC */
be<s16>* JASBridge::ChannelFilters::processFir(DSPBuffer_l* channel, be<s16>* samples) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    u32 flags = channel->filterMode;
    u32 count = flags & 31;
    be<s16>* window = samples - count;
    copyChannelFilterSamples(window, channel->firHistory, count * 2);
    fir(window, channel->parameters.fir, count);
    be<s16>* tail = window + 80;
    copyChannelFilterSamples(channel->firHistory, tail, count * 2);
    return tail;
}
/* 0281B338 */
void JASBridge::ChannelFilters::biquad(be<s16>* samples, be<s16>* coefficients, be<s16>* history) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    s32 previousOutput = history[2];
    s32 previousInput = history[0];
    s32 lastWeight = coefficients[3];
    s32 olderOutput = history[3];
    s32 firstWeight = coefficients[0];
    s32 secondWeight = coefficients[1];
    s32 olderInput = history[1];
    s32 thirdWeight = coefficients[2];
    for (u32 sample = 0; sample < 80; ++sample) {
        u32 sum = (u32)previousInput * (u32)firstWeight + (u32)olderInput * (u32)secondWeight;
        sum += (u32)thirdWeight * (u32)previousOutput;
        sum += (u32)lastWeight * (u32)olderOutput;
        s32 input = samples[sample];
        s32 value = (s32)((u32)((s32)sum >> 16) + (u32)input);
        if (value < -32768) value = -32768;
        else if (value > 32767) value = 32767;
        olderInput = previousInput;
        previousInput = input;
        olderOutput = previousOutput;
        previousOutput = value;
        samples[sample] = value;
    }
    history[1] = olderInput;
    history[2] = previousOutput;
    history[0] = previousInput;
    history[3] = olderOutput;
}
/* 0281B404 */
void JASBridge::ChannelFilters::processBiquad(DSPBuffer_l* channel, be<s16>* samples) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    be<s16>* coefficients = channel->parameters.biquad;
    if (!coefficients[3] && !coefficients[2] && !coefficients[1] && coefficients[0] == 32767) return;
    biquad(samples, coefficients, channel->biquadHistory);
}
/* 0281B448 */
void JASBridge::initializeChannelFilterStatics() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    filterStatics.words[2] = 0;
    filterStatics.words[0] = 0;
    filterStatics.words[3] = 0;
    filterStatics.words[1] = 0;
    registerChannelFilterStatics(&filterDestructorRecord);
}
/* WWHD glue
data filterStatics 104B56D0
data filterDestructorRecord 101FA010
ext 0273AD10 void* allocateChannelFilters(u32)
ext 028F026C void registerChannelFilterStatics(JASBridge::FilterDestructorRecord*)
ext C000A848 void copyChannelFilterSamples(void*,const void*,u32)
*/
