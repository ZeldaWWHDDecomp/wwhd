#pragma once
#include "JSystem/JAudio/JASBridgeDecoder.h"

namespace JASBridge {
class Resampler {
public:
    void process(DSPBuffer_l* channel, be<s16>* input, be<s16>* output);
    gptr<be<u16>> coefficientIndices;
};
struct ResamplerStatics { be<u32> words[4]; };
struct ResamplerDestructorRecord { be<u32> words[3]; };
struct ResamplerCoefficients { be<s16> taps[256]; };
Resampler* constructResampler(Resampler* object);
void initializeResamplerStatics();
WWHD_SIZE(Resampler, 4);
}
static void* allocateBridgeResampler(u32 size);
static void* allocateResamplerIndices(u32 size, void* heap, s32 alignment);
static void registerResamplerStatics(JASBridge::ResamplerDestructorRecord* record);
