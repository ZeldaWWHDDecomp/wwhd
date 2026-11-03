/* Wave archive address access and wave-sound file validation, from WWHD. */
#include "nw/snd/WaveFileReader.h"
using namespace nw::snd;
static void* waveFileReaderAllocate(u32);

// Envelope bytes travel as one word plus one byte (GHS struct copy).
static void copyEnvelope(EnvelopeBytes* destination,const EnvelopeBytes* source) {
    *gabi::at<be<u32>>(gabi::ea(destination))=*gabi::at<be<u32>>(gabi::ea(source));
    destination->values[4]=source->values[4];
}

/* 0289FD24 */
const void* WaveArchiveReader::loadedAddress(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mValid || index>=count()) return nullptr;
    be<u32>* addresses=mCachedAddresses;
    if (addresses) return gabi::at<const void>(*gabi::at<be<u32>>(gabi::ea(addresses)+index*4u));
    // the file's entry in the table (unchecked index: 32-bit element address)
    const WaveArchiveBlockEntry* entry=gabi::at<const WaveArchiveBlockEntry>(gabi::ea(mTable->mEntries)+index*sizeof(WaveArchiveBlockEntry));
    u32 offset=entry->mOffset;
    const be<u8>* data=mHeader->dataAddress();
    return gabi::at<const void>(gabi::ea(data)+8u+offset);
}
/* 0289FDC8 */
const void* WaveArchiveReader::exchangeAddress(u32 index,const void* address) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mValid) return nullptr;
    be<u32>* addresses=mCachedAddresses;
    if (!addresses || index>=count()) return nullptr;
    be<u32>* slot=gabi::at<be<u32>>(gabi::ea(addresses)+index*4u);
    const void* previous=gabi::at<const void>(*slot);
    *slot=gabi::ea(address);
    return previous;
}
/* 028A02B8 */
u32 WaveSoundDataReader::validate(const WaveSoundArchiveDirectory* header) {
    if ((u32)header->mMagic!=WaveSoundFileMagic) return 0;
    return (u32)header->mVersion-WaveSoundFirstVersion<WaveSoundVersionSpan;
}
/* 028A02F0 */
WaveSoundDataReader* WaveSoundDataReader::construct(WaveSoundDataReader* object,const WaveSoundArchiveDirectory* header) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<WaveSoundDataReader*>(waveFileReaderAllocate(sizeof(WaveSoundDataReader)));
    if (object) {
        object->mHeader=nullptr;
        object->mInfo=nullptr;
        if (validate(header)) {
            object->mHeader=header;
            const GroupBlockHeader* block=static_cast<const GroupBlockHeader*>(header->infoBlock());
            if (block && (u32)block->mMagic==WaveSoundInfoMagic)
                object->mInfo=gabi::at<const WaveSoundSetReferences>(gabi::ea(block)+sizeof(GroupBlockHeader));
        }
    }
    return object;
}
/* 028A0388 */
u32 WaveSoundDataReader::hasModernLayout() const { return (u32)mHeader->mVersion>=WaveSoundMixedLayoutVersion; }
/* 028A03A8 */
u32 WaveSoundDataReader::readParameters(WaveSoundParameters* output,u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    const WaveSoundNoteReferences* sound=static_cast<const WaveSoundNoteReferences*>(mInfo->sound(index));
    const SoundOptionalParameters* parameters=static_cast<const SoundOptionalParameters*>(sound->parameters());
    output->mPitch=parameters->scalarValue();
    output->mPan=parameters->byteValue();
    output->mSurround=parameters->signedByteValue();
    parameters->readRouting(&output->mFilter,output->mSend,3);
    copyEnvelope(&output->mEnvelope,parameters->envelope());
    if (hasModernLayout()) {
        output->mVolume=parameters->alternateByteValue();
        output->mRelease=parameters->highByteValue();
        output->mMode=parameters->upperByteValue();
    } else { output->mVolume=0x40; output->mRelease=0; output->mMode=0; }
    return 1;
}
/* 028A0498 */
u32 WaveSoundDataReader::readNote(WaveSoundNoteParameters* output,u32 index,u32 noteIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    const WaveSoundNoteReferences* sound=static_cast<const WaveSoundNoteReferences*>(mInfo->sound(index));
    const TrackOptionalParameters* note=static_cast<const TrackOptionalParameters*>(sound->note(noteIndex));
    const WaveSoundWaveTable* table=static_cast<const WaveSoundWaveTable*>(mInfo->waveTable());
    u32 waveIndex=note->mIdentity;      // a note names its wave by index into the table
    if (waveIndex>=(u32)table->mCount) return 0;
    u32 entry=gabi::ea(table->mEntries)+waveIndex*sizeof(WaveSoundWaveReference);
    if (!entry) return 0;               // the original also rejects an entry address that wraps to 0
    const WaveSoundWaveReference* reference=gabi::at<const WaveSoundWaveReference>(entry);
    output->mArchiveId=reference->mArchiveId; output->mWaveIndex=reference->mWaveIndex;
    output->mPitch=note->scalarValue();
    copyEnvelope(&output->mEnvelope,note->envelope());
    output->mPan=note->byteValue(); output->mSurround=note->trailingByteValue();
    output->mFilter=note->highByteValue(); output->mVolume=note->alternateByteValue();
    return 1;
}
/* WWHD glue
ext 028B0F48 const void* nw::snd::WaveSoundSetReferences::sound(u32) const
ext 028B0F30 const void* nw::snd::WaveSoundSetReferences::waveTable() const
ext 028B0F98 const void* nw::snd::WaveSoundNoteReferences::parameters() const
ext 028B0FB0 const void* nw::snd::WaveSoundNoteReferences::note(u32) const
ext 028B1044 f32 nw::snd::SoundOptionalParameters::scalarValue() const
ext 028B1000 u32 nw::snd::SoundOptionalParameters::byteValue() const
ext 028B1020 s32 nw::snd::SoundOptionalParameters::signedByteValue() const
ext 028B10AC void nw::snd::SoundOptionalParameters::readRouting(be<u8>*,be<u8>*,s32) const
ext 028B1174 const nw::snd::EnvelopeBytes* nw::snd::SoundOptionalParameters::envelope() const
ext 028B11E8 u32 nw::snd::SoundOptionalParameters::alternateByteValue() const
ext 028B1250 u32 nw::snd::SoundOptionalParameters::highByteValue() const
ext 028B12B8 u32 nw::snd::SoundOptionalParameters::upperByteValue() const
ext 028B1484 f32 nw::snd::TrackOptionalParameters::scalarValue() const
ext 028B14F0 const nw::snd::EnvelopeBytes* nw::snd::TrackOptionalParameters::envelope() const
ext 028B1320 u32 nw::snd::TrackOptionalParameters::byteValue() const
ext 028B13AC u32 nw::snd::TrackOptionalParameters::trailingByteValue() const
ext 028B1418 u32 nw::snd::TrackOptionalParameters::highByteValue() const
ext 028B1340 u32 nw::snd::TrackOptionalParameters::alternateByteValue() const
ext 028B0ED4 const void* nw::snd::WaveSoundArchiveDirectory::infoBlock() const
ext 0289FC08 u32 nw::snd::WaveArchiveReader::count() const
ext 0289F9B0 const be<u8>* nw::snd::WaveArchiveHeader::dataAddress() const
ext 0273AD10 void* waveFileReaderAllocate(u32)
*/
