/* NW-SND#6: preparing a wave sound for a start (its wave file now, or a deferred load); the records
 * go to the WaveSound (WaveSound.h). */
#pragma once
#include "nw/snd/ArchiveStartContext.h"
#include "nw/snd/WaveResourceResolver.h"
#include "nw/snd/WaveSound.h"
namespace nw { namespace snd {
/* SoundStartInfo::offsetKind. A wave sound starts kind 0 as a millisecond offset (WaveSoundStart unit 1, which
 * the player converts with the sample rate, WaveSoundPlayer::prepareVoice 028A10AC) and kind 2 as a sample offset (unit 0); a tick offset (1)
 * starts it at 0. */
enum StartOffsetKind { StartOffset_Milliseconds = 0, StartOffset_Ticks = 1, StartOffset_Samples = 2 };
/* Start errors returned by the archive player. */
enum SoundStartResult {
    SoundStart_Success = 0,
    SoundStart_LowPriority = 1,
    SoundStart_NoLabel = 2,
    SoundStart_InvalidSound = 3,
    SoundStart_NoWaveFile = 7,
    SoundStart_NoWave = 8,
    SoundStart_NotReady = 11,
    SoundStart_NoSoundSlot = 13,
    SoundStart_InvalidActorPlayer = 14,
    SoundStart_NoSoftwareMix = 21,
    SoundStart_Rejected = 255,
};
} }
