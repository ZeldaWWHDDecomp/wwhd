#pragma once
#include "JSystem/JAudio/JASBridgeDecoder.h"
namespace JASBridge {
struct MixerDelayState {
    be<u16> writeHalf;
    be<u16> readPosition;
    be<s16> direction;
    be<u16> reverseThreshold;
    be<u16> forwardThreshold;
    be<u16> step;
    be<u16> fraction;
    be<u16> excursion;
};
struct MixerPanLocals { be<f32> base[4]; be<f32> current[8]; be<f32> target[8]; };
struct MixerPanTable { be<f32> gains[128]; };
WWHD_SIZE(MixerPanLocals, 0x50);
class VoiceMixer {
public:
    be<s16>* bus(u32 index);
    s32 physicalBus(u32 index);
    void addBus(u32 index, be<s16>* samples, s32 gain);
    void scaleGains(be<f32>* destination, be<f32>* source, f32 gain);
    f32 ramp(be<s16>* destination, be<s16>* source, f32 current, f32 target);
    void mixVoice(DSPBuffer_l* voice, be<s16>* samples);
    void mixAutomaticVoice(DSPBuffer_l* voice, be<s16>* samples);
    void mixExplicitVoice(DSPBuffer_l* voice, be<s16>* samples);
    void commitDelay();
    void renderDelay();
    void blendBlock(be<s16>* destination, be<s16>* source, s32 count, s32 gain);
    void scaleBlock(be<s16>* destination, be<s16>* source, s32 count);
    void render(be<s16>* first, be<s16>* second, be<s16>* third, be<s16>* fourth);
    void restartOutput();
    void clearBuses();
    gptr<be<s16>> buses[13];
    be<s32> masterGain;
    be<s32> outputBlock;
    be<s32> blockCount;
    gptr<be<s16>> delayStorage;
    MixerDelayState delay;
};
struct MixerBusMap { be<u32> physical[8]; };
struct MixerBusCodes { be<u16> codes[13]; };
struct MixerStatics { be<u32> words[4]; };
struct MixerDestructorRecord { be<u32> words[3]; };
VoiceMixer* constructVoiceMixer(VoiceMixer* object);
void initializeMixerDelay(MixerDelayState* state);
void initializeVoiceMixerStatics();
s16 clampBridgeSample(s32 sample);
WWHD_SIZE(MixerDelayState, 0x10);
WWHD_OFFSET(VoiceMixer, masterGain, 0x34);
WWHD_OFFSET(VoiceMixer, outputBlock, 0x38);
WWHD_OFFSET(VoiceMixer, delayStorage, 0x40);
WWHD_OFFSET(VoiceMixer, delay, 0x44);
WWHD_SIZE(VoiceMixer, 0x54);
}
static void* allocateVoiceMixer(u32 size);
static void* allocateVoiceMixerSamples(u32 size, void* heap, s32 alignment);
static void registerVoiceMixerStatics(JASBridge::MixerDestructorRecord* record);
static void clearVoiceMixerStorage(void* destination, u32 value, u32 size);
static void copyVoiceMixerStorage(void* destination, const void* source, u32 size);
