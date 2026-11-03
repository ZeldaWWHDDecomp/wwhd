/* Wave archive address access and wave-sound data (FWSD) readers. Binary-derived; own names. */
#pragma once
#include "nw/snd/SoundArchive.h"
#include "nw/snd/SoundOptionalParameters.h"
#include "nw/snd/SoundResourceDirectories.h"
#include "nw/snd/WaveArchiveReader.h"   // loadedAddress / exchangeAddress are defined in WaveFileReader.cpp
namespace nw { namespace snd {
enum : u32 {
    WaveSoundFileMagic = 0x46575344,        // "FWSD"
    WaveSoundInfoMagic = 0x494E464F,        // "INFO"
    WaveSoundFirstVersion = 0x10000,
    WaveSoundVersionSpan = 0x101,           // accepted versions: 1.0.0 .. 1.1.0
    WaveSoundMixedLayoutVersion = 0x10100,  // from here on volume, release and mode are stored per sound
};
/* One wave of the file's wave table: which wave archive and which wave in it. */
struct WaveSoundWaveReference { be<u32> mArchiveId,mWaveIndex; };
struct WaveSoundWaveTable { be<u32> mCount; WaveSoundWaveReference mEntries[1]; };
WWHD_SIZE(WaveSoundWaveReference,8);
WWHD_OFFSET(WaveSoundWaveTable,mEntries,4);
/* Per-sound parameters as the player keeps them (read by readParameters). */
struct WaveSoundParameters {
    be<f32> mPitch;
    EnvelopeBytes mEnvelope;
    be<u8> mPan,mSurround,mFilter,mSend[3],mVolume,mRelease,mMode;
};
/* Per-note parameters (read by readNote). */
struct WaveSoundNoteParameters {
    be<u32> mArchiveId,mWaveIndex;
    EnvelopeBytes mEnvelope;
    be<u8> mPan,mSurround,mFilter,mVolume; u8 mAlignment[3]; be<f32> mPitch;
};
WWHD_OFFSET(WaveSoundParameters,mEnvelope,4); WWHD_OFFSET(WaveSoundParameters,mPan,9);
WWHD_OFFSET(WaveSoundParameters,mSend,12); WWHD_OFFSET(WaveSoundParameters,mVolume,15);
WWHD_SIZE(WaveSoundParameters,0x12);
WWHD_OFFSET(WaveSoundNoteParameters,mEnvelope,8); WWHD_OFFSET(WaveSoundNoteParameters,mPan,13);
WWHD_OFFSET(WaveSoundNoteParameters,mVolume,16); WWHD_OFFSET(WaveSoundNoteParameters,mPitch,20);
WWHD_SIZE(WaveSoundNoteParameters,0x18);
struct WaveSoundDataReader {
    gptr<const WaveSoundArchiveDirectory> mHeader;
    gptr<const WaveSoundSetReferences> mInfo;
    static u32 validate(const WaveSoundArchiveDirectory*);
    static WaveSoundDataReader* construct(WaveSoundDataReader*,const WaveSoundArchiveDirectory*);
    u32 hasModernLayout() const;
    u32 readParameters(WaveSoundParameters*,u32);
    u32 readNote(WaveSoundNoteParameters*,u32,u32);
};
WWHD_SIZE(WaveSoundDataReader,8);
}}
