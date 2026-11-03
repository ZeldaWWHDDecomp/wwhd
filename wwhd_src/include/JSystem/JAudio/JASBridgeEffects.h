#pragma once
#include "JSystem/JAudio/JASBridgeMixer.h"
namespace JASBridge {
/* One effects line (0x20 bytes): local named copy of the merged JASystem::DSPInterface::FXBuffer. */
struct EffectLine_l {
    be<u16> flags;
    be<u16> blockCount;
    gptr<be<s16>> storage;
    be<u16> firstBus;
    be<s16> firstGain;
    be<u16> secondBus;
    be<s16> secondGain;
    be<s16> coefficients[8];
};
class EffectsMixer {
public:
    void filter(be<s16>* samples, be<s16>* coefficients);
    void processLine(EffectLine_l* effect, u32 index);
    void beginBlock();
    void endBlock();
    void assignLine(u32 index, EffectLine_l* line);
    gptr<EffectLine_l> lines[4];
    be<u32> positions[4];
    gptr<VoiceMixer> mixer;
};
struct EffectStatics { be<u32> words[4]; };
struct EffectDestructorRecord { be<u32> words[3]; };
EffectsMixer* constructEffectsMixer(EffectsMixer* object);
void initializeEffectsMixerStatics();
s16 clampBridgeSample(s32 sample);
WWHD_SIZE(EffectLine_l, 0x20);
WWHD_OFFSET(EffectsMixer, positions, 0x10);
WWHD_OFFSET(EffectsMixer, mixer, 0x20);
WWHD_SIZE(EffectsMixer, 0x24);
}
static void* allocateEffectsMixer(u32 size);
static void registerEffectsMixerStatics(JASBridge::EffectDestructorRecord* record);
static void copyEffectsSamples(void* destination, const void* source, u32 size);
