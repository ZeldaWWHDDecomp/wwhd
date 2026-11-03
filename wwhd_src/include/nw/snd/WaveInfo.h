/* Bank note inputs and wave metadata consumed by this unit. From WWHD call sites. */
#pragma once
#include "nw/snd/HardwareVoice.h"
#include "nw/snd/SoundResourceDirectories.h"   // WaveDescription: the wave metadata the wave channel plays
namespace nw { namespace snd {
using WaveAdpcmState = AdpcmLoop;
// Sample encodings of a wave (WaveDescription::mEncoding); ADPCM packs 14 samples into each 8-byte frame.
enum WaveFormat { WaveFormatPcm8, WaveFormatPcm16, WaveFormatAdpcm };
enum { AdpcmFrameSamples = 14 };
WWHD_SIZE(WaveAdpcmState, 6);
struct BankNote {
    be<s32> instrument, key, velocity;
    be<u32> startFrame;
    be<s32> panOffset;
    be<u32> priority, callback, argument, mode;
};
WWHD_OFFSET(BankNote, startFrame, 0xC);
WWHD_OFFSET(BankNote, priority, 0x14);
WWHD_SIZE(BankNote, 0x24);
} }
