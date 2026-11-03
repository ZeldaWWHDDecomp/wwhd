#pragma once
#include "bindings.h"
namespace JASBridge {
/* One DSP voice channel record (0x180 bytes), as the virtual DSP's decoder, resampler, filters and
 * mixer use it. Local named copy of the merged JASystem::DSPInterface::DSPBuffer, whose shared
 * declaration still has numbered fields; the field names here are derived from these functions. */
struct MixerVoiceBus_l {
    be<u16> code;
    be<s16> targetGain;
    be<s16> currentGain;
    be<u16> delay;
};
struct FilterParameters_l {
    union {
        be<s16> fir[32];
        struct {
            u8 firToBiquad[0x28];
            be<s16> biquad[4];
            be<s16> distance;
        };
    };
};
struct DSPBuffer_l {
    be<u16> active;
    be<u16> stopped;
    be<u16> pitch;
    u8 controlPadding[2];
    be<u16> reset;
    be<u16> loopPending;
    be<u16> constant;
    be<u16> historyCount;
    MixerVoiceBus_l buses[8];
    be<u16> pan;
    be<u16> surround;
    be<u16> volume;
    be<u16> targetVolume;
    be<u16> automaticMode;
    be<u16> decodeError;
    u8 decoderPadding[4];
    be<u16> phase;
    be<u16> blockOffset;
    be<u16> blockSize;
    be<s16> lastSample;
    be<u32> samplePosition;
    be<u32> frameIndex;
    gptr<be<u8>> frameData;
    be<u32> samplesLeft;
    be<s16> resampleHistory[4];
    be<s16> firHistory[20];
    be<s16> biquadHistory[4];
    be<s16> decodedBlock[16];
    be<s16> mixingHistory[16];
    be<s16> distanceOutput;
    be<s16> distanceInput;
    u8 distancePadding[0xC];
    be<u16> format;
    be<u16> loopEnabled;
    be<s16> loopPrevious;
    be<s16> loopOlder;
    be<u16> filterMode;
    be<u16> fade;
    u8 loopPadding[4];
    union {
        be<u32> loopStart;          /* sample formats: loop start sample */
        gptr<be<u8>> loopSource;    /* streams: loop start address */
    };
    be<u32> sampleEnd;
    gptr<be<u8>> source;
    u8 sourcePadding[4];
    FilterParameters_l parameters;
    u8 trailingControls[0x20];
};
struct DecoderWaveTable { be<s16> samples[64]; };
struct AdpcmPredictors { be<s16> values[32]; };
struct AdpcmScratch { be<u32> shift; be<u32> predictor; };
struct DecoderStatics { be<u32> words[4]; };
struct DecoderDestructorRecord { be<u32> words[3]; };
class SampleDecoder {
public:
    void processAdpcmSlow(DSPBuffer_l* channel);
    void processAdpcmBlocks(DSPBuffer_l* channel);
    void renderAdpcm(DSPBuffer_l* channel, be<s16>* destination);
    void renderPcm16(DSPBuffer_l* channel, be<s16>* destination);
    void renderPcm8(DSPBuffer_l* channel, be<s16>* destination);
    void renderStream(DSPBuffer_l* channel);
    void renderResampledStream(DSPBuffer_l* channel, be<s16>* destination);
    void renderDirectStream(DSPBuffer_l* channel, be<s16>* destination);
    void prepareAdpcmFrame(DSPBuffer_l* channel);
    void stopAdpcm(DSPBuffer_l* channel);
    void decodeAdpcmFrame(DSPBuffer_l* channel, be<s16>* destination, be<u8>* encoded,
                         s32 previous, s32 older, u32 format, be<s16>* unpacked);
    void decodeChannelFrame(DSPBuffer_l* channel);
    void advanceAdpcmFrame(DSPBuffer_l* channel);
    void copyDecodedSamples(be<s16>* source, s32 count);
    void appendDecodedSamples(DSPBuffer_l* channel, u32 limit);
    void appendDecodedLimited(DSPBuffer_l* channel);
    void appendDecodedFull(DSPBuffer_l* channel);
    void prepareAdpcmBlocks(DSPBuffer_l* channel);
    u32 handleAdpcmLoop(DSPBuffer_l* channel);
    void fillEndSamples(DSPBuffer_l* channel);
    void restartAdpcmBlocks(DSPBuffer_l* channel);
    void decodeAdpcmDirectBlock(DSPBuffer_l* channel);
    void fillStoppedSamples(DSPBuffer_l* channel);
    void preparePcm(DSPBuffer_l* channel);
    void restartPcm(DSPBuffer_l* channel);
    void copyPcm16(DSPBuffer_l* channel, be<s16>* source, s32 count);
    void copyPcm8(DSPBuffer_l* channel, be<u8>* source, s32 count);
    void copyStream(DSPBuffer_l* channel, u32 count);
    u32 outputCount(DSPBuffer_l* channel);
    u32 parseAdpcmHeader(u32 header, be<u32>* shift, be<u32>* predictor);
    void clearSamples(be<s16>* destination, s32 count);
    void unpackTwoBit(be<s16>* destination, be<u8>* source);
    void unpackFourBit(be<s16>* destination, be<u8>* source);
    void predictAdpcm(be<s16>* destination, be<s16>* source, s32 first, s32 second, s32 previous, s32 older, u32 shift);
    u32 square(u32 phase, u32 step);
    u32 saw(u32 phase, u32 step);
    u32 modulatedSquare(u32 phase, u32 step);
    u32 widePulse(u32 phase, u32 step);
    u32 wave(u32 phase, u32 step, be<s16>* table);
    u32 triangle(u32 phase, u32 step);
    u32 fillConstant(u32 phase, u32 value);
    u32 sine(u32 phase, u32 step);
    u32 modulatedSine(u32 phase, u32 step);
    u32 coupledNoiseWave(u32 phase, u32 step);
    u32 rampNoiseWave(u32 phase, u32 step);
    void synthesize(DSPBuffer_l* channel, be<s16>* destination);
    void updateNoise();
    be<u32> remainingOutput;
    gptr<be<s16>> output;
    gptr<be<s16>> unpackedFrame;
    gptr<be<u16>> feedback;
    gptr<be<s16>> coupledNoise;
    gptr<be<s16>> rampNoise;
};
SampleDecoder* constructSampleDecoder(SampleDecoder* object);
void initializeSampleDecoderStatics();
WWHD_OFFSET(DSPBuffer_l, phase, 0x60);
WWHD_OFFSET(DSPBuffer_l, frameData, 0x70);
WWHD_OFFSET(DSPBuffer_l, decodedBlock, 0xB0);
WWHD_OFFSET(DSPBuffer_l, format, 0x100);
WWHD_OFFSET(DSPBuffer_l, source, 0x118);
WWHD_OFFSET(DSPBuffer_l, reset, 8);
WWHD_OFFSET(DSPBuffer_l, buses, 0x10);
WWHD_OFFSET(DSPBuffer_l, automaticMode, 0x58);
WWHD_OFFSET(DSPBuffer_l, lastSample, 0x66);
WWHD_OFFSET(DSPBuffer_l, resampleHistory, 0x78);
WWHD_OFFSET(DSPBuffer_l, firHistory, 0x80);
WWHD_OFFSET(DSPBuffer_l, biquadHistory, 0xA8);
WWHD_OFFSET(DSPBuffer_l, mixingHistory, 0xD0);
WWHD_OFFSET(DSPBuffer_l, distanceOutput, 0xF0);
WWHD_OFFSET(DSPBuffer_l, filterMode, 0x108);
WWHD_OFFSET(DSPBuffer_l, fade, 0x10A);
WWHD_OFFSET(DSPBuffer_l, loopStart, 0x110);
WWHD_OFFSET(DSPBuffer_l, parameters, 0x120);
WWHD_OFFSET(FilterParameters_l, biquad, 0x28);
WWHD_SIZE(MixerVoiceBus_l, 8);
WWHD_SIZE(FilterParameters_l, 0x40);
WWHD_SIZE(DSPBuffer_l, 0x180);
WWHD_SIZE(SampleDecoder, 0x18);
}
static void* allocateSampleDecoder(u32 size);
static void* allocateDecoderStorage(u32 size, void* heap, s32 alignment);
static void registerSampleDecoderStatics(JASBridge::DecoderDestructorRecord* record);
static void copyDecoderStorage(void* destination, const void* source, u32 size);
