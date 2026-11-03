/* NW-SND#6: the sequence note-on bridge of the archive player, the actor-less start, and the label
 * lookup. Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/SequenceBankPreparation.h"
using namespace nw::snd;

/* 02895E24 */
WaveChannel* playArchiveBankNote(ArchiveNoteCallback* callback, SequenceRuntimeSupport* sequence, u32 index, const BankNote* note) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!callback->playback->hasArchiveData()) return NULL;
    gabi::FrameLocal<BankNotePlayer> player(8);
    BankNotePlayer::construct(player.get());
    WaveChannel* channel = WaveChannel::playBankInstrument(player.get(), &sequence->banks[index], &sequence->waveArchives[index], note);
    BankNotePlayer::destroy(player.get(), 2);
    return channel;
}

/* 02895DF4 */
u32 ArchivePlayback::startSoundWithoutActor(SoundHandle* handle, u32 id, u32 hold, SoundStartInfo* info) {
    gabi::AutoFrame frame{gabi::current_function()};
    return startSound(handle, id, NULL, NULL, hold, info);
}

/* 02896084 */
u32 ArchivePlayback::findLabel(const char* label) {
    gabi::AutoFrame frame{gabi::current_function()};
    return archive->findItem(label);
}

/* WWHD glue
 ext 02893C88 u32 nw::snd::ArchivePlayback::hasArchiveData()
 ext 028A2FA0 nw::snd::BankNotePlayer* nw::snd::BankNotePlayer::construct(nw::snd::BankNotePlayer*)
 ext 028A2FCC void nw::snd::BankNotePlayer::destroy(nw::snd::BankNotePlayer*,u32)
 ext 028A2FE0 nw::snd::WaveChannel* nw::snd::WaveChannel::playBankInstrument(void*,nw::snd::BankReader*,nw::snd::WaveArchiveReader*,const nw::snd::BankNote*)
 ext 0288E91C u32 nw::snd::SoundArchive::findItem(const char*)
*/
