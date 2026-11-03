/* NW-SND#6: the archive player's note-on bridge for sequences, and two small archive-player entries. */
#pragma once
#include "nw/snd/ArchiveStartContext.h"
#include "nw/snd/WaveChannel.h"
#include "nw/snd/BankReader.h"
#include "nw/snd/WaveInfo.h"
#include "nw/snd/AudioFileStream.h"
nw::snd::WaveChannel* playArchiveBankNote(nw::snd::ArchiveNoteCallback* callback, nw::snd::SequenceRuntimeSupport* sequence, u32 index, const nw::snd::BankNote* note);
