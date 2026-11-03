/* Bank-reader prefixes and header checks reconstructed from WWHD's binary only. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct BankFileHeader {
    be<u32> signature;
    be<u32> byteOrderAndHeaderSize;
    be<u32> version;
};
WWHD_OFFSET(BankFileHeader, version, 8);
WWHD_SIZE(BankFileHeader, 12);
struct BankInfoBlock { be<u32> signature; be<u32> size; u8 data[1]; };
WWHD_OFFSET(BankInfoBlock, data, 8);
struct BankWaveReference { be<u32> archiveIndex, waveIndex; };
WWHD_SIZE(BankWaveReference, 8);
struct BankWaveReferences { be<u32> count; BankWaveReference entries[1]; };
WWHD_OFFSET(BankWaveReferences, entries, 4);
struct BankRegion_l { be<u32> waveReferenceIndex; };
struct BankPackedRegion_l {
    u8 levelPrefix[3]; be<u8> baseKey;
    u8 tuningPrefix[3]; be<u8> volume;
    u8 balancePrefix[3]; be<u8> pan;
    be<f32> pitch;
    u8 unused;
    be<u8> outputMode;
    be<u8> interpolation;
    be<u8> ignoreRelease;
    u8 reserved[12];
    be<u32> envelope;
    be<u8> release;
    u8 padding[3];
};
WWHD_OFFSET(BankPackedRegion_l, pitch, 0xC);
WWHD_OFFSET(BankPackedRegion_l, envelope, 0x20);
WWHD_OFFSET(BankPackedRegion_l, release, 0x24);
WWHD_SIZE(BankPackedRegion_l, 0x28);
struct BankEnvelopeParameters { be<u32> envelope; be<u8> release; u8 padding[3]; };
WWHD_SIZE(BankEnvelopeParameters, 8);
struct BankInstrumentInfo {
    BankWaveReference wave;
    be<f32> pitch;
    be<u32> envelope;
    be<u8> release;
    be<u8> baseKey;
    be<u8> volume;
    be<u8> pan;
    be<u8> ignoreRelease;
    be<u8> interpolation;
    be<u8> outputMode;
    u8 padding;
};
WWHD_OFFSET(BankInstrumentInfo, pitch, 8);
WWHD_OFFSET(BankInstrumentInfo, envelope, 0xC);
WWHD_OFFSET(BankInstrumentInfo, release, 0x10);
WWHD_OFFSET(BankInstrumentInfo, outputMode, 0x16);
WWHD_SIZE(BankInstrumentInfo, 0x18);
struct BankReader {
    gptr<const BankFileHeader> file;
    gptr<const void> info;
    be<u8> attached;
    u8 padding[3];
    static BankReader* construct(BankReader* reader);
    static u32 validHeader(const BankFileHeader* header);
    void attach(const BankFileHeader* header);
    static BankReader* constructFrom(BankReader* reader,const BankFileHeader* header);
    void detach();
    u32 readInstrument(BankInstrumentInfo* output, s32 instrument, s32 key, s32 velocity);
    const void* waveReferences();
};
WWHD_OFFSET(BankReader, info, 4);
WWHD_OFFSET(BankReader, attached, 8);
WWHD_SIZE(BankReader, 12);
} }
