/* Resolving the wave archive behind a bank or a wave sound, reconstructed from WWHD. */
#pragma once
#include "nw/snd/SoundArchive.h"
#include "nw/snd/ArchiveLoading.h"
#include "nw/snd/WaveArchiveReader.h"
#include "nw/snd/WaveFileReader.h"
namespace nw { namespace snd {
// Where a bank's waves are: archive id and loaded image (NULL and -1 until resolved).
struct WaveArchiveLocation { be<u32> mArchiveId; gptr<const WaveArchiveHeader> mImage; };
WWHD_SIZE(WaveArchiveLocation, 8);
enum WaveResolveResult { ResolveNoWaves = -2, ResolveNotLoaded = -1, ResolveReady = 0, ResolveEmpty = 1, ResolveWaveMissing = 2 };
s32 resolveBankWaveArchive(WaveArchiveLocation* location, be<u8>* individualLoad, const BankFileHeader* bank, SoundArchive* archive, ArchiveFileLoader* loader);
const void* resolveWaveSoundWave(const void* waveSound, u32 index, SoundArchive* archive, ArchiveFileLoader* loader);
} }
