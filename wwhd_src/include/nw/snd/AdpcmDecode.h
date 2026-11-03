/* Sample-count to byte-offset helpers and the ADPCM history seek, reconstructed from WWHD. */
#pragma once
#include "wwhd.h"
#include "nw/snd/HardwareVoice.h"
namespace nw { namespace snd {
// Sample formats of a wave channel.
enum SampleFormat { SamplePcm8 = 0, SamplePcm16 = 1, SampleAdpcm = 2 };
// The decoder state AXDecodeAdpcmData works on (nibble cursors, coefficients, predictor, history).
struct AdpcmDecodeParameters {
    be<u32> samplesPerFrame, nibblesPerFrame, reserved[2];
    be<u32> firstNibble, lastNibble, currentNibble;
    be<s16> coefficients[16];
    be<u16> reservedHalfword, predictor;
    be<s16> history[2];
    be<u32> tail[7];
};
WWHD_OFFSET(AdpcmDecodeParameters, coefficients, 0x1C);
WWHD_OFFSET(AdpcmDecodeParameters, predictor, 0x3E);
WWHD_OFFSET(AdpcmDecodeParameters, history, 0x40);
WWHD_SIZE(AdpcmDecodeParameters, 0x60);
u32 adpcmNibbleOffset(u32 samples);
u32 sampleByteOffset(u32 samples, u32 format);
void advanceAdpcmLoop(AdpcmLoop* loop, const be<s16>* coefficients, u32 samples, const be<u8>* data);
} }
