/* Block directories and relative-reference prefixes recovered from the WWHD sound readers. */
#pragma once
#include "nw/snd/GroupFileReader.h"
#include "nw/snd/HardwareVoice.h"
namespace nw { namespace snd {
enum SoundBlockKind : u32 { StreamInfoBlock=0x4000, StreamSeekBlock=0x4001, StreamDataBlock=0x4002, StreamRegionBlock=0x4003, SequenceDataBlock=0x5000, SequenceLabelBlock=0x5001, WaveInfoBlock=0x7000, WaveDataBlock=0x7001, AuxiliaryWavePrimaryBlock=0x7800, AuxiliaryWaveSecondaryBlock=0x7801, AuxiliaryWaveAdditionalBlock=0x7802 };
enum SoundReferenceKind : u32 { ReferenceTableKind=0x101, AdpcmInfoKind=0x300, StreamInfoKind=0x4100, StreamTrackKind=0x4101, StreamChannelKind=0x4102 };
struct SoundBlockLocation { be<u16> mKind,mReserved; be<u32> mOffset,mSize; };
struct StreamFileDirectory : GroupFileHeader {
    be<u32> mFileSize;
    be<u16> mBlockCount,mReserved;
    SoundBlockLocation mBlocks[1];
    const SoundBlockLocation* findBlock(u32 kind) const;
    u32 hasSeekBlock() const;
    u32 hasRegionBlock() const;
    u32 infoSize() const;
    u32 infoOffset() const;
    u32 seekOffset() const;
    u32 dataOffset() const;
    u32 regionOffset() const;
};
struct StreamInfoReferences {
    GroupReference mStream,mTracks,mChannels;
    const void* streamInfo() const;
    const void* trackTable() const;
    const void* channelTable() const;
};
struct StreamReferenceTable : GroupReferenceTable {
    const void* track(u32 index) const;
    const void* channel(u32 index) const;
};
struct StreamChannelReference {
    GroupReference mAdpcm;
    const void* adpcmInfo() const;
};
struct StreamInfoData { be<u32> mWords[17]; };
struct StreamTrackInfo { be<u8> mParameters[4]; be<u8> mChannelCount,mChannels[2]; };
struct InitialAdpcmState { be<s16> mCoefficients[16]; AdpcmLoop mContext; };
struct StreamMetadataReader {
    gptr<const StreamFileDirectory> mHeader;
    gptr<const StreamInfoReferences> mInfo;
    static StreamMetadataReader* create(StreamMetadataReader*);
    u32 accepts(const GroupFileHeader* header) const;
    void bind(const StreamFileDirectory* header);
    u32 legacyLayout() const;
    u32 readInfo(StreamInfoData* output) const;
    u32 readTrack(StreamTrackInfo* output,s32 index) const;
    u32 readChannel(InitialAdpcmState* initial,AdpcmLoop* loop,u32 index) const;
};
struct WaveSoundArchiveDirectory : StreamFileDirectory { const void* infoBlock() const; };
struct WaveSoundSetReferences {
    GroupReference mWaveTable,mSoundTable;
    const void* waveTable() const;
    const GroupReferenceTable* soundTable() const;
    const void* sound(u32 index) const;
};
struct WaveSoundNoteReferences {
    GroupReference mParameters,mTracks,mNotes;
    const void* parameters() const;
    const GroupReferenceTable* noteTable() const;
    const void* note(u32 index) const;
};
struct WaveSoundDataDirectory : StreamFileDirectory { const void* infoBlock() const; };
struct WaveSoundDefinition {
    GroupReference mParameters,mEvents;
    const void* parameters() const;
    const GroupReferenceTable* eventTable() const;
    const void* event(u32 index) const;
};
struct WaveDescription;
struct AuxiliaryWaveReader {
    gptr<const void> mInfo;
    static AuxiliaryWaveReader* create(AuxiliaryWaveReader*);
    u32 readDescription(WaveDescription* output) const;
};
u32 sampleEncodingFromReferenceKind(u32 kind);
const void* resolveSoundReference(const void* base,u32 kind,u32 offset,u32 index);
void decodeAdpcmSamples(u32 sampleIndex,AdpcmLoop* context,const be<s16>* coefficients,const be<u8>* data,u32 count,be<s16>* output);
struct SoundReferenceSelector : GroupReference {
    const void* resolve(u32 index) const;
    const void* resolveAlternate(u32 index) const;
};
WWHD_SIZE(StreamTrackInfo,7);
WWHD_SIZE(InitialAdpcmState,38);
WWHD_SIZE(StreamInfoData,0x44);
WWHD_SIZE(StreamMetadataReader,8);
WWHD_SIZE(WaveSoundSetReferences,16);
WWHD_OFFSET(WaveSoundNoteReferences,mNotes,0x10);
WWHD_SIZE(WaveSoundDefinition,16);
WWHD_SIZE(AuxiliaryWaveReader,4);
u32 decodeSampleEncoding(u32 encoding);
u32 isSupportedWaveFile(const GroupFileHeader* header);
struct AuxiliaryWaveDirectory : StreamFileDirectory {
    const void* primaryBlock() const;
    const void* secondaryBlock() const;
    const void* additionalBlock() const;
};
struct SequenceFileDirectory : StreamFileDirectory {
    const void* sequenceBlock() const;
    const void* labelBlock() const;
};
struct WaveFileDirectory : StreamFileDirectory {
    const void* infoBlock() const;
    const void* dataBlock() const;
};
struct SequenceLabelTable : GroupReferenceTable {
    const void* entry(u32 index) const;
    u32 findPrefix(const char* name,be<u32>* identifier) const;
};
struct WaveChannelInfo {
    be<u32> mReserved,mDataOffset,mReservedData,mAdpcmOffset;
    const void* sampleData(const void* base) const;
    const void* adpcmInfo() const;
};
struct WaveChannelTable {
    be<u8> mEncoding,mLooping;
    u8 mReserved2[2];
    be<u32> mSampleRate,mLoopStart,mLoopEnd,mReserved10;
    GroupReferenceTable mChannels;
    const WaveChannelInfo* channel(u32 index) const;
};
WWHD_SIZE(SoundBlockLocation,12);
WWHD_SIZE(StreamFileDirectory,0x20);
WWHD_OFFSET(StreamFileDirectory,mFileSize,0xC);
WWHD_OFFSET(StreamFileDirectory,mBlockCount,0x10);
WWHD_OFFSET(StreamFileDirectory,mBlocks,0x14);
WWHD_SIZE(StreamInfoReferences,24);
WWHD_OFFSET(StreamInfoReferences,mTracks,8);
WWHD_OFFSET(StreamInfoReferences,mChannels,0x10);
WWHD_SIZE(StreamChannelReference,8);
WWHD_SIZE(WaveChannelInfo,16);
WWHD_OFFSET(WaveChannelTable,mChannels,0x14);
} }

namespace nw { namespace snd {
struct WaveChannelDescription {
    gptr<const void> mSamples;
    InitialAdpcmState mInitial;
    AdpcmLoop mLoop;
};
struct WaveDescription {
    be<u32> mEncoding;
    be<u8> mLooping; u8 mReserved5[3];
    be<u32> mChannelCount,mSampleRate,mLoopStart,mLoopEnd;
    WaveChannelDescription mChannels[2];
};
struct WaveDataReader {
    gptr<const WaveChannelTable> mInfo;
    gptr<const void> mDataBase;
    AuxiliaryWaveReader mAuxiliary;
    be<u8> mSourceKind; u8 mReservedD[3];
    static WaveDataReader* create(WaveDataReader*,const WaveFileDirectory* resource,u32 sourceKind);
    const void* dataForChannel(const WaveChannelInfo* channel,const void* base) const;
    u32 readDescription(WaveDescription* output,const void* base) const;
};
WWHD_SIZE(WaveChannelDescription,0x30);
WWHD_SIZE(WaveDescription,0x78);
WWHD_SIZE(WaveDataReader,16);
WWHD_OFFSET(WaveChannelTable,mChannels,0x14);
WWHD_OFFSET(WaveChannelDescription,mLoop,0x2A);
}}
