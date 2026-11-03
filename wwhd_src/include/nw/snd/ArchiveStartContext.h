/* NW-SND#6: the records of the archive player's sound start (ArchivePlayback::startSound and its
 * helpers, declared on ArchivePlayback). */
#pragma once
#include "nw/snd/ArchiveAddressCache.h"
#include "nw/snd/SoundPlayerControl.h"
namespace nw { namespace snd {
/* What a caller may override when it starts a sound (flags select the fields). */
enum SoundStartFlag {
    SoundStart_Offset = 0x1,
    SoundStart_Player = 0x2,
    SoundStart_Priority = 0x4,
    SoundStart_ActorPlayer = 0x8,
    SoundStart_Sequence = 0x10,
    SoundStart_Stream = 0x20,
    SoundStart_Wave = 0x40,
    SoundStart_OutputMode = 0x80,
    SoundStart_FadeOut = 0x100,
    SoundStart_FinalizeCallback = 0x200,
};
struct WaveStartOverride { gptr<const void> waveFile; u8 reserved4[5]; be<s8> waveType; u8 padding[2]; };
WWHD_SIZE(WaveStartOverride, 12);
struct SoundStartInfo {
    /* 0x00 */ be<u32> flags;
    /* 0x04 */ be<u32> offsetKind;
    /* 0x08 */ be<u32> offset;
    /* 0x0C */ be<u32> playerId;
    /* 0x10 */ be<s32> priority;
    /* 0x14 */ be<u32> actorPlayerId;
    /* 0x18 */ PlaybackSequenceOptions_l sequence;
    /* 0x30 */ PlaybackStreamFile_l stream;
    /* 0x3C */ WaveStartOverride wave;
    /* 0x48 */ be<u8> outputMode;
    /* 0x49 */ u8 padding49[3];
    /* 0x4C */ be<s32> fadeFrames;
    /* 0x50 */ be<u32> finalizeCallback;
};
WWHD_OFFSET(SoundStartInfo, sequence, 0x18);
WWHD_OFFSET(SoundStartInfo, stream, 0x30);
WWHD_OFFSET(SoundStartInfo, wave, 0x3C);
WWHD_OFFSET(SoundStartInfo, outputMode, 0x48);
WWHD_OFFSET(SoundStartInfo, finalizeCallback, 0x50);

} }
