#pragma once
#include "JSystem/JAudio/JASBridgeDecoder.h"
namespace JASBridge {
class ChannelFilters {
public:
    void distance(DSPBuffer_l* channel, be<s16>* samples, s32 coefficient);
    void fir(be<s16>* samples, be<s16>* coefficients, s32 count);
    be<s16>* processFir(DSPBuffer_l* channel, be<s16>* samples);
    void biquad(be<s16>* samples, be<s16>* coefficients, be<s16>* history);
    void processBiquad(DSPBuffer_l* channel, be<s16>* samples);
    u8 storage;
};
struct FilterStatics { be<u32> words[4]; };
struct FilterDestructorRecord { be<u32> words[3]; };
ChannelFilters* constructChannelFilters(ChannelFilters* object);
void initializeChannelFilterStatics();
s16 clampBridgeSample(s32 sample);
WWHD_SIZE(ChannelFilters, 1);
}
static void* allocateChannelFilters(u32 size);
static void registerChannelFilterStatics(JASBridge::FilterDestructorRecord* record);
static void copyChannelFilterSamples(void* destination, const void* source, u32 size);
