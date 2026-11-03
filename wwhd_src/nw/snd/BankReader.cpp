/* Bank header attachment and borrowed references. From WWHD's instructions only. */
#include "nw/snd/BankReader.h"
using namespace nw::snd;

void* sndBankAllocate(u32 size);
const BankInfoBlock* bankInfoBlock(const BankFileHeader* header);
const void* bankWaveReferences(const void* info);
const be<s32>* bankInstrumentCount(const void* info);
const void* bankInstrumentAt(const void* info, s32 instrument);
const void* bankKeyRegion(const void* instrument, s32 key);
const BankRegion_l* bankVelocityRegion(const void* region, s32 velocity);
const BankPackedRegion_l* bankPackedRegion(const BankRegion_l* region);
u32 bankRegionBaseKey(const BankRegion_l* region);
u32 bankRegionVolume(const BankRegion_l* region);
u32 bankRegionPan(const BankRegion_l* region);
f64 bankRegionPitch(const BankRegion_l* region);
u32 bankRegionIgnoreRelease(const BankRegion_l* region);
u32 bankRegionInterpolation(const BankRegion_l* region);
u32 bankRegionOutputMode(const BankRegion_l* region);
const BankEnvelopeParameters* bankRegionEnvelope(const BankRegion_l* region);

enum { BankSignature = 0x46424E4B, BankVersion = 0x00010000, InfoSignature = 0x494E464F };

/* 028A31DC */
BankReader* BankReader::construct(BankReader* reader) {
    gabi::AutoFrame frame{0x028A31DC};
    if (!reader) reader = (BankReader*)sndBankAllocate(sizeof(BankReader));
    if (reader) {
        reader->file = nullptr;
        reader->attached = 0;
        reader->info = nullptr;
    }
    return reader;
}

/* 028A3220 */
u32 BankReader::validHeader(const BankFileHeader* header) {
    if (header->signature != BankSignature) return 0;
    return (u32)header->version == BankVersion;
}

/* 028A3258 */
void BankReader::attach(const BankFileHeader* header) {
    gabi::AutoFrame frame{0x028A3258};
    if (!header || !validHeader(header)) return;
    file = header;
    const BankInfoBlock* block = bankInfoBlock(header);
    if (block->signature != InfoSignature) return;
    info = block->data;
    attached = 1;
}

/* 028A32C8 */
BankReader* BankReader::constructFrom(BankReader* reader,const BankFileHeader* header) {
    gabi::AutoFrame frame{0x028A32C8};
    if (!reader) reader = (BankReader*)sndBankAllocate(sizeof(BankReader));
    if (reader) {
        reader->attached = 0;
        reader->file = nullptr;
        reader->info = nullptr;
        reader->attach(header);
    }
    return reader;
}

/* 028A3330 */
void BankReader::detach() {
    if (!attached) return;
    file = nullptr;
    attached = 0;
    info = nullptr;
}

/* 028A3350 */
u32 BankReader::readInstrument(BankInstrumentInfo* output, s32 instrument, s32 key, s32 velocity) {
    gabi::AutoFrame frame{0x028A3350};
    if (!attached || instrument < 0) return 0;
    const be<s32>* count = bankInstrumentCount(info);
    if (instrument >= (s32)*count) return 0;
    const void* definition = bankInstrumentAt(info, instrument);
    if (!definition) return 0;
    const void* keyRegion = bankKeyRegion(definition, key);
    if (!keyRegion) return 0;
    const BankRegion_l* region = bankVelocityRegion(keyRegion, velocity);
    if (!region) return 0;
    const void* regionInfo = info;
    u32 waveIndex = region->waveReferenceIndex;
    const BankWaveReferences* references = (const BankWaveReferences*)bankWaveReferences(regionInfo);
    const BankWaveReference* wave = nullptr;
    if (waveIndex < (u32)references->count) wave = &references->entries[waveIndex];
    if (!wave || wave->waveIndex == 0xFFFFFFFFu) return 0;
    output->wave.archiveIndex = wave->archiveIndex;
    output->wave.waveIndex = wave->waveIndex;
    const BankPackedRegion_l* packed = bankPackedRegion(region);
    if (!packed) {
        output->baseKey = bankRegionBaseKey(region);
        output->volume = bankRegionVolume(region);
        output->pan = bankRegionPan(region);
        f64 pitch = bankRegionPitch(region);
        output->pitch = gabi::f32_from_bits(ppc_stfs_bits(pitch));
        output->ignoreRelease = bankRegionIgnoreRelease(region);
        output->interpolation = bankRegionInterpolation(region);
        output->outputMode = bankRegionOutputMode(region);
        const BankEnvelopeParameters* envelope = bankRegionEnvelope(region);
        output->envelope = envelope->envelope;
        output->release = envelope->release;
    } else {
        output->baseKey = packed->baseKey;
        output->volume = packed->volume;
        output->pan = packed->pan;
        output->pitch = packed->pitch;
        output->ignoreRelease = packed->ignoreRelease;
        output->interpolation = packed->interpolation;
        output->outputMode = packed->outputMode;
        output->envelope = packed->envelope;
        output->release = packed->release;
    }
    return 1;
}

/* 028A3518 */
const void* BankReader::waveReferences() {
    gabi::AutoFrame frame{0x028A3518};
    if (!attached) return nullptr;
    return bankWaveReferences(info);
}

/* WWHD glue
 ext 028B15F0 const be<s32>* bankInstrumentCount(const void*)
 ext 028B15FC const void* bankInstrumentAt(const void*,s32)
 ext 028B176C const void* bankKeyRegion(const void*,s32)
 ext 028B177C const nw::snd::BankRegion_l* bankVelocityRegion(const void*,s32)
 ext 028B1AB4 const nw::snd::BankPackedRegion_l* bankPackedRegion(const nw::snd::BankRegion_l*)
 ext 028B178C u32 bankRegionBaseKey(const nw::snd::BankRegion_l*)
 ext 028B17AC u32 bankRegionVolume(const nw::snd::BankRegion_l*)
 ext 028B1818 u32 bankRegionPan(const nw::snd::BankRegion_l*)
 ext 028B1884 f64 bankRegionPitch(const nw::snd::BankRegion_l*)
 ext 028B18F0 u32 bankRegionIgnoreRelease(const nw::snd::BankRegion_l*)
 ext 028B1964 u32 bankRegionInterpolation(const nw::snd::BankRegion_l*)
 ext 028B19D0 u32 bankRegionOutputMode(const nw::snd::BankRegion_l*)
 ext 028B1A3C const nw::snd::BankEnvelopeParameters* bankRegionEnvelope(const nw::snd::BankRegion_l*)
 ext 0273AD10 void* sndBankAllocate(u32)
 ext 028B1588 const nw::snd::BankInfoBlock* bankInfoBlock(const nw::snd::BankFileHeader*)
 ext 028B15E4 const void* bankWaveReferences(const void*)
*/
