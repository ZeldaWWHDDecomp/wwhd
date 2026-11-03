/* Sound-resource directory readers, reconstructed from WWHD comparisons and relative offsets. */
#include "nw/snd/SoundResourceDirectories.h"
using namespace nw::snd;
static void* allocateSoundReader(u32);
static void* copySoundResource(void*,const void*,u32);
struct SampleEncodingBytes { be<u8> mValues[3]; };
static SampleEncodingBytes sSampleEncodingBytes;
template<class T> static const T* soundResourceAt(const void* base,u32 offset) { return gabi::at<T>(gabi::ea(base)+offset); }
/* 028B0334 */
const SoundBlockLocation* StreamFileDirectory::findBlock(u32 kind) const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==kind) return block;
    }
    return nullptr;
}
/* 028B036C */
u32 StreamFileDirectory::hasSeekBlock() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return findBlock(StreamSeekBlock)!=nullptr;
}
/* 028B0398 */
u32 StreamFileDirectory::hasRegionBlock() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return findBlock(StreamRegionBlock)!=nullptr;
}
/* 028B03C4 */
u32 StreamFileDirectory::infoSize() const {
    gabi::AutoFrame frame{gabi::current_function()};
    const SoundBlockLocation* block=findBlock(StreamInfoBlock);
    return *soundResourceAt<be<u32>>(block,offsetof(SoundBlockLocation,mSize));
}
/* 028B03EC */
u32 StreamFileDirectory::infoOffset() const {
    gabi::AutoFrame frame{gabi::current_function()};
    const SoundBlockLocation* block=findBlock(StreamInfoBlock);
    return *soundResourceAt<be<u32>>(block,offsetof(SoundBlockLocation,mOffset));
}
/* 028B0414 */
u32 StreamFileDirectory::seekOffset() const {
    gabi::AutoFrame frame{gabi::current_function()};
    const SoundBlockLocation* block=findBlock(StreamSeekBlock);
    return *soundResourceAt<be<u32>>(block,offsetof(SoundBlockLocation,mOffset));
}
/* 028B043C */
u32 StreamFileDirectory::dataOffset() const {
    gabi::AutoFrame frame{gabi::current_function()};
    const SoundBlockLocation* block=findBlock(StreamDataBlock);
    return *soundResourceAt<be<u32>>(block,offsetof(SoundBlockLocation,mOffset));
}
/* 028B0464 */
u32 StreamFileDirectory::regionOffset() const {
    gabi::AutoFrame frame{gabi::current_function()};
    const SoundBlockLocation* block=findBlock(StreamRegionBlock);
    return *soundResourceAt<be<u32>>(block,offsetof(SoundBlockLocation,mOffset));
}
/* 028B048C */
const void* StreamInfoReferences::streamInfo() const {
    if (static_cast<u32>(mStream.mKind)!=StreamInfoKind) return nullptr;
    return soundResourceAt<be<u8>>(this,static_cast<u32>(mStream.mOffset));
}
/* 028B04AC */
const void* StreamInfoReferences::trackTable() const {
    if (static_cast<u32>(mTracks.mKind)!=ReferenceTableKind) return nullptr;
    return soundResourceAt<be<u8>>(this,static_cast<u32>(mTracks.mOffset));
}
/* 028B04CC */
const void* StreamInfoReferences::channelTable() const {
    if (static_cast<u32>(mChannels.mKind)!=ReferenceTableKind) return nullptr;
    return soundResourceAt<be<u8>>(this,static_cast<u32>(mChannels.mOffset));
}
/* 028B04EC */
const void* StreamReferenceTable::track(u32 index) const {
    if (index>=static_cast<u32>(mCount)) return nullptr;
    const GroupReference* reference=soundResourceAt<GroupReference>(this,sizeof(be<u32>)+index*sizeof(GroupReference));
    if (static_cast<u32>(reference->mKind)!=StreamTrackKind) return nullptr;
    return soundResourceAt<be<u8>>(this,static_cast<u32>(reference->mOffset));
}
/* 028B0520 */
const void* StreamReferenceTable::channel(u32 index) const {
    if (index>=static_cast<u32>(mCount)) return nullptr;
    const GroupReference* reference=soundResourceAt<GroupReference>(this,sizeof(be<u32>)+index*sizeof(GroupReference));
    if (static_cast<u32>(reference->mKind)!=StreamChannelKind) return nullptr;
    return soundResourceAt<be<u8>>(this,static_cast<u32>(reference->mOffset));
}
/* 028B0554 */
const void* StreamChannelReference::adpcmInfo() const {
    if (static_cast<u32>(mAdpcm.mKind)!=AdpcmInfoKind) return nullptr;
    return soundResourceAt<be<u8>>(this,static_cast<u32>(mAdpcm.mOffset));
}
/* 028B094C */
u32 StreamMetadataReader::accepts(const GroupFileHeader* header) const {
    if (static_cast<u32>(header->mMagic)!=0x4653544D) return 0;
    u32 relativeVersion=static_cast<u32>(header->mVersion)-0x10000;
    return relativeVersion<0x20001;
}
/* 028B0C00 */
u32 nw::snd::decodeSampleEncoding(u32 encoding) {
    if (encoding<1) return 0;
    if (encoding==1) return 1;
    return 2;
}
/* 028B0C24 */
u32 nw::snd::isSupportedWaveFile(const GroupFileHeader* header) {
    if (static_cast<u32>(header->mMagic)!=0x46574156) return 0;
    u32 relativeVersion=static_cast<u32>(header->mVersion)-0x10000;
    return relativeVersion<ReferenceTableKind;
}
/* 028B1D44 */
const void* AuxiliaryWaveDirectory::primaryBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==AuxiliaryWavePrimaryBlock) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B1DA0 */
const void* AuxiliaryWaveDirectory::secondaryBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==AuxiliaryWaveSecondaryBlock) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B1DFC */
const void* AuxiliaryWaveDirectory::additionalBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==AuxiliaryWaveAdditionalBlock) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B1F14 */
const void* SequenceFileDirectory::sequenceBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==SequenceDataBlock) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B1F70 */
const void* SequenceFileDirectory::labelBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==SequenceLabelBlock) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B210C */
const void* WaveFileDirectory::infoBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==WaveInfoBlock) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B2168 */
const void* WaveFileDirectory::dataBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==WaveDataBlock) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B1FCC */
const void* SequenceLabelTable::entry(u32 index) const {
    if (index>=static_cast<u32>(mCount)) return nullptr;
    const GroupReference* reference=soundResourceAt<GroupReference>(this,sizeof(be<u32>)+index*sizeof(GroupReference));
    return soundResourceAt<be<u8>>(this,static_cast<u32>(reference->mOffset));
}
/* 028B21C4 */
const WaveChannelInfo* WaveChannelTable::channel(u32 index) const {
    if (index>=static_cast<u32>(mChannels.mCount)) return nullptr;
    const GroupReference* reference=soundResourceAt<GroupReference>(&mChannels,sizeof(be<u32>)+index*sizeof(GroupReference));
    return soundResourceAt<WaveChannelInfo>(&mChannels,static_cast<u32>(reference->mOffset));
}
/* 028B21EC */
const void* WaveChannelInfo::sampleData(const void* base) const {
    return soundResourceAt<be<u8>>(base,mDataOffset);
}
/* 028B21F8 */
const void* WaveChannelInfo::adpcmInfo() const {
    return soundResourceAt<be<u8>>(this,mAdpcmOffset);
}

/* 028B0ED4 */
const void* WaveSoundArchiveDirectory::infoBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==0x6800) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B1588 */
const void* WaveSoundDataDirectory::infoBlock() const {
    u32 count=mBlockCount;
    for (u32 i=0;i<count;++i) {
        const SoundBlockLocation* block=soundResourceAt<SoundBlockLocation>(this,offsetof(StreamFileDirectory,mBlocks)+i*sizeof(SoundBlockLocation));
        if (static_cast<u32>(block->mKind)==0x5800) {
            u32 offset=block->mOffset;
            if (!offset) return nullptr;
            return soundResourceAt<be<u8>>(this,offset);
        }
    }
    return nullptr;
}
/* 028B0F30 */
const void* WaveSoundSetReferences::waveTable() const {
    return soundResourceAt<be<u8>>(this,static_cast<u32>(mWaveTable.mOffset));
}
/* 028B0F98 */
const void* WaveSoundNoteReferences::parameters() const {
    return soundResourceAt<be<u8>>(this,static_cast<u32>(mParameters.mOffset));
}
/* 028B15E4 */
const void* WaveSoundDefinition::parameters() const {
    return soundResourceAt<be<u8>>(this,static_cast<u32>(mParameters.mOffset));
}
/* 028B0F3C */
const GroupReferenceTable* WaveSoundSetReferences::soundTable() const {
    return soundResourceAt<GroupReferenceTable>(this,static_cast<u32>(mSoundTable.mOffset));
}
/* 028B0FA4 */
const GroupReferenceTable* WaveSoundNoteReferences::noteTable() const {
    return soundResourceAt<GroupReferenceTable>(this,static_cast<u32>(mNotes.mOffset));
}
/* 028B15F0 */
const GroupReferenceTable* WaveSoundDefinition::eventTable() const {
    return soundResourceAt<GroupReferenceTable>(this,static_cast<u32>(mEvents.mOffset));
}
/* 028B0F48 */
const void* WaveSoundSetReferences::sound(u32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const GroupReferenceTable* table=soundTable();
    if (index>=static_cast<u32>(table->mCount)) return nullptr;
    const GroupReference* reference=soundResourceAt<GroupReference>(table,4+index*sizeof(GroupReference));
    if (static_cast<u32>(reference->mKind)!=0x4900) return nullptr;
    return soundResourceAt<be<u8>>(table,static_cast<u32>(reference->mOffset));
}
/* 028B0FB0 */
const void* WaveSoundNoteReferences::note(u32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const GroupReferenceTable* table=noteTable();
    if (index>=static_cast<u32>(table->mCount)) return nullptr;
    const GroupReference* reference=soundResourceAt<GroupReference>(table,4+index*sizeof(GroupReference));
    if (static_cast<u32>(reference->mKind)!=0x4902) return nullptr;
    return soundResourceAt<be<u8>>(table,static_cast<u32>(reference->mOffset));
}
/* 028B15FC */
const void* WaveSoundDefinition::event(u32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const GroupReferenceTable* table=eventTable();
    const GroupReference* reference=soundResourceAt<GroupReference>(table,4+index*sizeof(GroupReference));
    if (static_cast<u32>(reference->mKind)!=0x5900) return nullptr;
    if (index>=static_cast<u32>(table->mCount)) return nullptr;
    return soundResourceAt<be<u8>>(table,static_cast<u32>(reference->mOffset));
}
/* 028B166C */
u32 nw::snd::sampleEncodingFromReferenceKind(u32 kind) {
    if (kind<0x6000 || kind>0x6002) return 3;
    return sSampleEncodingBytes.mValues[kind-0x6000];
}
/* 028B090C */
StreamMetadataReader* StreamMetadataReader::create(StreamMetadataReader* reader) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!reader) reader=static_cast<StreamMetadataReader*>(allocateSoundReader(sizeof(StreamMetadataReader)));
    if (reader) { reader->mInfo=nullptr; reader->mHeader=nullptr; }
    return reader;
}
/* 028B098C */
void StreamMetadataReader::bind(const StreamFileDirectory* header) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!accepts(header)) return;
    mHeader=header;
    u32 offset=header->infoOffset();
    const void* block=soundResourceAt<be<u8>>(header,offset);
    if (*soundResourceAt<be<u32>>(block,0)==0x494E464F) mInfo=soundResourceAt<StreamInfoReferences>(block,8);
}
/* 028B09F8 */
u32 StreamMetadataReader::legacyLayout() const {
    const StreamFileDirectory* header=mHeader;
    return static_cast<u32>(header->mVersion)<=0x20000;
}
/* 028B0A14 */
u32 StreamMetadataReader::readInfo(StreamInfoData* output) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const StreamInfoReferences* info=mInfo;
    const void* source=info->streamInfo();
    for (u32 i=0;i<17;++i) output->mWords[i]=*soundResourceAt<be<u32>>(source,i*sizeof(be<u32>));
    return 1;
}
/* 028B1C70 */
AuxiliaryWaveReader* AuxiliaryWaveReader::create(AuxiliaryWaveReader* reader) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!reader) reader=static_cast<AuxiliaryWaveReader*>(allocateSoundReader(sizeof(AuxiliaryWaveReader)));
    if (reader) reader->mInfo=nullptr;
    return reader;
}
/* 028B0A64 */
u32 StreamMetadataReader::readTrack(StreamTrackInfo* output,s32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const StreamInfoReferences* info=mInfo;
    const StreamReferenceTable* table=static_cast<const StreamReferenceTable*>(info->trackTable());
    if (!table || index>=static_cast<s32>(table->mCount)) return 0;
    const void* track=table->track(static_cast<u32>(index));
    for (u32 i=0;i<4;++i) output->mParameters[i]=*soundResourceAt<be<u8>>(track,i);
    u32 offset=*soundResourceAt<be<u32>>(track,8);
    u32 count=static_cast<u32>(*soundResourceAt<be<u32>>(track,offset))&255;
    output->mChannelCount=count;
    u32 copied=count>2?2:count;
    for (u32 i=0;i<copied;++i) {
        offset=*soundResourceAt<be<u32>>(track,8);
        output->mChannels[i]=*soundResourceAt<be<u8>>(track,offset+4+i);
    }
    return 1;
}
/* 028B0B4C */
u32 StreamMetadataReader::readChannel(InitialAdpcmState* initial,AdpcmLoop* loop,u32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const StreamInfoReferences* info=mInfo;
    const StreamReferenceTable* table=static_cast<const StreamReferenceTable*>(info->channelTable());
    const StreamChannelReference* channel=static_cast<const StreamChannelReference*>(table->channel(index));
    const void* adpcm=channel->adpcmInfo();
    if (!adpcm) return 0;
    for (u32 i=0;i<16;++i) initial->mCoefficients[i]=*soundResourceAt<be<s16>>(adpcm,i*2);
    initial->mContext.predictor=*soundResourceAt<be<u16>>(adpcm,0x20);
    initial->mContext.history[0]=*soundResourceAt<be<s16>>(adpcm,0x22);
    initial->mContext.history[1]=*soundResourceAt<be<s16>>(adpcm,0x24);
    loop->predictor=*soundResourceAt<be<u16>>(adpcm,0x26);
    loop->history[0]=*soundResourceAt<be<s16>>(adpcm,0x28);
    loop->history[1]=*soundResourceAt<be<s16>>(adpcm,0x2A);
    return 1;
}
/* 028B1690 */
const void* nw::snd::resolveSoundReference(const void* base,u32 kind,u32 offset,u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    const void* record=soundResourceAt<be<u8>>(base,offset);
    u32 selector=sampleEncodingFromReferenceKind(kind);
    if (selector==0) return soundResourceAt<be<u8>>(record,*soundResourceAt<be<u32>>(record,4));
    if (selector==1) {
        u32 count=*soundResourceAt<be<u32>>(record,0);
        u32 position=0;
        while (position<count) {
            if (index<=static_cast<u32>(*soundResourceAt<be<u8>>(record,4+position))) {
                count=*soundResourceAt<be<u32>>(record,0);
                u32 entryOffset=((count+3)&~3u)+position*8;
                return soundResourceAt<be<u8>>(record,*soundResourceAt<be<u32>>(record,entryOffset+8));
            }
            ++position;
        }
        return nullptr;
    }
    if (selector==2) {
        u32 first=*soundResourceAt<be<u8>>(record,0);
        if (index<first || index>static_cast<u32>(*soundResourceAt<be<u8>>(record,1))) return nullptr;
        return soundResourceAt<be<u8>>(record,*soundResourceAt<be<u32>>(record,(index-first)*8+8));
    }
    return nullptr;
}
/* 028B176C */
const void* SoundReferenceSelector::resolve(u32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    return resolveSoundReference(this,mKind,mOffset,index);
}
/* 028B177C */
const void* SoundReferenceSelector::resolveAlternate(u32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    return resolveSoundReference(this,mKind,mOffset,index);
}
/* 028B1AF0 */
void nw::snd::decodeAdpcmSamples(u32 sampleIndex,AdpcmLoop* context,const be<s16>* coefficients,const be<u8>* data,u32 count,be<s16>* output) {
    u32 frameIndex=sampleIndex/14;
    u32 position=sampleIndex-frameIndex*14;
    const be<u8>* block=data+frameIndex*8;
    u32 header=context->predictor;
    u32 shift=header&15;
    u32 predictor=header>>4;
    for (u32 i=0;i<count;++i) {
        if (position==0) {
            header=block[0];
            predictor=header>>4;
            shift=header&15;
            context->predictor=header;
        }
        s32 previous=context->history[0];
        u32 packed=block[1+position/2];
        u32 nibble=(position&1)?(packed&15):(packed>>4);
        s32 residual=static_cast<s16>(nibble<<12)>>1;
        s32 scale=static_cast<s16>(1u<<shift);
        s32 coefficient0=coefficients[predictor*2];
        s32 coefficient1=coefficients[predictor*2+1];
        s32 older=context->history[1];
        u32 sum=static_cast<u32>(coefficient0*previous)+static_cast<u32>(coefficient1*older)+static_cast<u32>(scale*residual);
        s32 rounded=((static_cast<s32>(sum)>>10)+1)>>1;
        if (rounded>32767) {
            context->history[1]=previous;
            context->history[0]=32767;
            output[i]=32767;
        } else {
            if (rounded<-32768) rounded=-32768;
            context->history[0]=rounded;
            context->history[1]=previous;
            output[i]=rounded;
        }
        ++position;
        if (position==14) { block+=8; position=0; }
    }
}
/* 028B1FF4 */
u32 SequenceLabelTable::findPrefix(const char* name,be<u32>* identifier) const {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 length=0;
    while (*soundResourceAt<be<u8>>(name,length)) ++length;
    if (length==0) {
        if (static_cast<s32>(mCount)<=0) return 0;
        const void* label=entry(0);
        *identifier=*soundResourceAt<be<u32>>(label,4);
        return 1;
    }
    s32 count=mCount;
    for (s32 i=0;i<count;++i) {
        const void* label=entry(static_cast<u32>(i));
        u32 j=0;
        for (;j<length;++j) {
            u32 character=*soundResourceAt<be<u8>>(name,j);
            if (character!=static_cast<u32>(*soundResourceAt<be<u8>>(label,12+j))) break;
            if (character==0) break;
        }
        if (j==length || !*soundResourceAt<be<u8>>(name,j)) {
            *identifier=*soundResourceAt<be<u32>>(label,4);
            return 1;
        }
    }
    return 0;
}
/* 028B0C5C */
WaveDataReader* WaveDataReader::create(WaveDataReader* reader,const WaveFileDirectory* resource,u32 sourceKind) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!reader) reader=static_cast<WaveDataReader*>(allocateSoundReader(sizeof(WaveDataReader)));
    if (!reader) return reader;
    reader->mInfo=nullptr;
    reader->mDataBase=nullptr;
    AuxiliaryWaveReader::create(&reader->mAuxiliary);
    reader->mSourceKind=sourceKind;
    if (sourceKind==0) {
        if (!isSupportedWaveFile(resource)) return reader;
        const void* info=resource->infoBlock();
        const void* data=resource->dataBlock();
        if (!info || !data) return reader;
        if (*soundResourceAt<be<u32>>(info,0)!=0x494E464F || *soundResourceAt<be<u32>>(data,0)!=0x44415441) return reader;
        reader->mInfo=soundResourceAt<WaveChannelTable>(info,8);
        reader->mDataBase=soundResourceAt<be<u8>>(data,8);
    } else if (sourceKind==1) reader->mAuxiliary.mInfo=resource;
    return reader;
}
/* 028B0D58 */
const void* WaveDataReader::dataForChannel(const WaveChannelInfo* channel,const void*) const {
    gabi::AutoFrame frame{gabi::current_function()};
    return channel->sampleData(mDataBase);
}
/* 028B1CAC */
u32 AuxiliaryWaveReader::readDescription(WaveDescription* output) const {
    gabi::AutoFrame frame{gabi::current_function()};
    const void* info=mInfo;
    output->mEncoding=2;
    output->mLooping=0;
    output->mChannelCount=1;
    output->mLoopStart=0;
    output->mSampleRate=*soundResourceAt<be<u32>>(info,8);
    output->mLoopEnd=*soundResourceAt<be<u32>>(info,0);
    output->mChannels[0].mSamples=soundResourceAt<be<u8>>(info,0x60);
    copySoundResource(&output->mChannels[0].mInitial,soundResourceAt<be<u8>>(info,0x1C),32);
    copySoundResource(&output->mChannels[0].mInitial.mContext,soundResourceAt<be<u8>>(info,0x3E),6);
    return 1;
}
/* 028B0D68 */
u32 WaveDataReader::readDescription(WaveDescription* output,const void* base) const {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 kind=static_cast<s8>(mSourceKind);
    if (kind==1) { mAuxiliary.readDescription(output); return 1; }
    if (kind!=0) return 1;
    const WaveChannelTable* info=mInfo;
    output->mEncoding=decodeSampleEncoding(info->mEncoding);
    info=mInfo; output->mChannelCount=info->mChannels.mCount;
    info=mInfo; output->mSampleRate=info->mSampleRate;
    info=mInfo; output->mLooping=static_cast<u32>(info->mLooping)==1;
    info=mInfo; output->mLoopStart=info->mLoopStart;
    info=mInfo; output->mLoopEnd=info->mLoopEnd;
    info=mInfo;
    for (s32 i=0;i<static_cast<s32>(info->mChannels.mCount);++i) {
        if (i<2) {
            const WaveChannelInfo* channel=info->channel(i);
            WaveChannelDescription* target=&output->mChannels[i];
            if (static_cast<u32>(channel->mAdpcmOffset)!=0) {
                const void* adpcm=channel->adpcmInfo();
                for (u32 c=0;c<16;++c) target->mInitial.mCoefficients[c]=*soundResourceAt<be<s16>>(adpcm,c*2);
                target->mInitial.mContext.predictor=*soundResourceAt<be<u16>>(adpcm,0x20);
                target->mInitial.mContext.history[0]=*soundResourceAt<be<s16>>(adpcm,0x22);
                target->mInitial.mContext.history[1]=*soundResourceAt<be<s16>>(adpcm,0x24);
                target->mLoop.predictor=*soundResourceAt<be<u16>>(adpcm,0x26);
                target->mLoop.history[0]=*soundResourceAt<be<s16>>(adpcm,0x28);
                target->mLoop.history[1]=*soundResourceAt<be<s16>>(adpcm,0x2A);
            }
            target->mSamples=dataForChannel(channel,base);
            info=mInfo;
        }
    }
    return 1;
}
/* WWHD glue
data sSampleEncodingBytes 1018808C
ext 0273AD10 void* allocateSoundReader(u32)
ext C000A848 void* copySoundResource(void*,const void*,u32)

*/
