/* Wave archive resolution for banks and wave sounds, reconstructed from WWHD; the bank and
 * wave-sound readers and the archive loader belong to other units. */
#include "nw/snd/WaveResourceResolver.h"
using namespace nw::snd;

/* 0289CEA0 */
s32 nw::snd::resolveBankWaveArchive(WaveArchiveLocation* location, be<u8>* individualLoad, const BankFileHeader* bank, SoundArchive* archive, ArchiveFileLoader* loader) {
    gabi::AutoFrame frame{0x0289CEA0u};
    location->mImage = nullptr;
    location->mArchiveId = 0xFFFFFFFFu;
    *individualLoad = 0;
    gabi::FrameLocal<BankReader> bankReader{0x24};
    BankReader::constructFrom(bankReader.get(), bank);
    const BankWaveReferences* references = static_cast<const BankWaveReferences*>(bankReader->waveReferences());
    const BankWaveReference* first = nullptr;
    if (references) {
        if (!references->count) return ResolveEmpty;
        // The original tests the entry address for NULL too; keep it a guest address.
        first = gabi::at<const BankWaveReference>(gabi::ea(references) + offsetof(BankWaveReferences, entries));
    }
    if (!first) return ResolveNoWaves;
    const WaveArchiveHeader* image = static_cast<const WaveArchiveHeader*>(loader->fileAddressForItem(first->archiveIndex));
    if (!image) return ResolveNotLoaded;
    gabi::FrameLocal<ArchiveWaveArchiveSummary> summary{0x18};
    summary->loadingFlags = 0;
    summary->fileId = 0xFFFFFFFFu;
    archive->readWaveArchiveInfo(first->archiveIndex, summary.get());
    const u32 archiveId = first->archiveIndex;
    const u8 individual = summary->loadingFlags;
    location->mArchiveId = archiveId;
    location->mImage = image;
    *individualLoad = individual;
    if (individual) {
        // An individually loaded archive: the bank is ready only once its wave is loaded.
        // (The original checks the first reference's wave on every pass.)
        gabi::FrameLocal<WaveArchiveReader> reader{8};
        WaveArchiveReader::construct(reader.get(), image, 1);
        for (u32 index = 0; index < references->count; ++index) {
            const u8 valid = reader->mValid;
            const u32 wave = first->waveIndex;
            if (!valid || !reader->loadedAddress(wave)) return ResolveWaveMissing;
        }
    }
    return ResolveReady;
}

/* 0289D018 */
const void* nw::snd::resolveWaveSoundWave(const void* waveSound, u32 index, SoundArchive* archive, ArchiveFileLoader* loader) {
    gabi::AutoFrame frame{0x0289D018u};
    gabi::FrameLocal<WaveSoundDataReader> waveSoundReader{0x2C};
    WaveSoundDataReader::construct(waveSoundReader.get(), static_cast<const WaveSoundArchiveDirectory*>(waveSound));
    gabi::FrameLocal<WaveSoundNoteParameters> note{8};
    note->mEnvelope.values[2] = 0;
    note->mEnvelope.values[4] = 0;
    note->mEnvelope.values[0] = 0;
    note->mEnvelope.values[3] = 0;
    note->mEnvelope.values[1] = 0;
    waveSoundReader->readNote(note.get(), index, 0);
    const WaveArchiveHeader* image = static_cast<const WaveArchiveHeader*>(loader->fileAddressForItem(note->mArchiveId));
    if (!image) return nullptr;
    gabi::FrameLocal<ArchiveWaveArchiveSummary> summary{0x20};
    summary->loadingFlags = 0;
    summary->fileId = 0xFFFFFFFFu;
    archive->readWaveArchiveInfo(note->mArchiveId, summary.get());
    gabi::FrameLocal<WaveArchiveReader> reader{0x34};
    WaveArchiveReader::construct(reader.get(), image, summary->loadingFlags);
    return reader->loadedAddress(note->mWaveIndex);
}

/* WWHD glue
ext 028A32C8 nw::snd::BankReader* nw::snd::BankReader::constructFrom(nw::snd::BankReader*,const nw::snd::BankFileHeader*)
ext 028A3518 const void* nw::snd::BankReader::waveReferences()
ext 028926BC void* nw::snd::ArchiveFileLoader::fileAddressForItem(u32)
ext 0288ED64 u32 nw::snd::SoundArchive::readWaveArchiveInfo(u32,nw::snd::ArchiveWaveArchiveSummary*)
ext 0289FD24 const void* nw::snd::WaveArchiveReader::loadedAddress(u32)
ext 028A02F0 nw::snd::WaveSoundDataReader* nw::snd::WaveSoundDataReader::construct(nw::snd::WaveSoundDataReader*,const nw::snd::WaveSoundArchiveDirectory*)
ext 028A0498 u32 nw::snd::WaveSoundDataReader::readNote(nw::snd::WaveSoundNoteParameters*,u32,u32)
*/
