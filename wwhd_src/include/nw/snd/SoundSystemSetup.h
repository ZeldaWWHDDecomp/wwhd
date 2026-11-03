/* NW-SND#6: the sound system's setup (sizes, storage carving, start-up and shut-down order) and the
 * audio frame driver singleton it starts (nw/snd/AudioFrameDriver.h, constructed here by 02898574).
 * Layouts from WWHD's own constructors (0289806C, 028984F0). */
#pragma once
#include "nw/snd/AudioFrameDriver.h"
#include "nw/snd/AudioMixRuntime.h"
#include "nw/snd/AudioWorkQueue.h"
#include "nw/snd/SoftwareMixer.h"
#include "nw/snd/WaveChannel.h"
#include "nw/snd/Modulator.h"
#include "nw/snd/SequenceRuntimeSupport.h"
#include "nw/snd/SoundStartStorage.h"
#include "nw/snd/PlaybackVoice.h"
#include "nw/snd/RemoteSpeaker.h"
namespace nw { namespace snd {

/* The setup parameters the game passes in (sead's audio system builds one with SoundSystemConfig::create). */
struct SoundSystemConfig {
    /* 0x00 */ be<u32> driverPriority;
    /* 0x04 */ be<u32> driverStackBytes;
    /* 0x08 */ be<u32> driverCommandBytes;
    /* 0x0C */ be<u32> reserved0C;
    /* 0x10 */ be<u32> workerPriority;
    /* 0x14 */ be<u32> workerStackBytes;
    /* 0x18 */ be<u32> taskCommandBytes;
    /* 0x1C */ be<u8> driverOption;
    /* 0x1D */ be<u8> softwareMixEnabled;
    /* 0x1E */ u8 padding1E[2];
    /* 0x20 */ be<u32> softwareChannelCount;

    static SoundSystemConfig* create(SoundSystemConfig* storage);
    u32 commandStorageBytes();
    void initializeCommandStorage(u32 buffer);
    u32 requiredStorageBytes();
    void initialize(u32 buffer);
};
WWHD_OFFSET(SoundSystemConfig, workerPriority, 0x10);
WWHD_OFFSET(SoundSystemConfig, driverOption, 0x1C);
WWHD_SIZE(SoundSystemConfig, 0x24);
} }

void finalizeSoundSystem();
u32 isSoftwareMixReady();

/* SoftwareMixer::initialize / finalize, bound as free functions: initialize() may pass a NULL mixer. */
void initializeSoftwareMixer(nw::snd::SoftwareMixer*, be<f32>*, u32);
void finalizeSoftwareMixer(nw::snd::SoftwareMixer*);
