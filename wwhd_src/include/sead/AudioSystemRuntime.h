/* sead audio system, second part of its translation unit (the first is sead/AudioSystem.cpp, SEAD#17): device and
 * sound-system setup, the launch-thread start and stop paths, mixer/compressor/upsampler options and the runtime
 * type. Layouts are the canonical sead/AudioSystem.h ones; the methods below are this unit's companion API on those
 * classes. */
#pragma once
#include "sead/AudioSystem.h"
#include "sead/TabletAudioRuntime.h"

namespace sead {

/* The sound-system setup record inside the audio-system state (AudioSystemState::mSoundState, 0x24 bytes). It is
 * built and consumed by the parked nw::snd sound-system code (028983xx); only the option byte this unit writes is
 * named here. */
struct SoundSystemSetup_l {
    u8 mOpaque[0x1D];
    be<u8> mOption;
};

/* The AudioSystemState and AudioSystem methods of this unit are declared on the canonical classes in
 * sead/AudioSystem.h (folded at the SEAD#18 merge). */

/* A SafeText with this translation unit's vtable copy (literal text: termination is already assured). */
struct AudioSystemText : SafeText {
    void destroy(u32 flags);
    void assureTermination();
};

WWHD_OFFSET(SoundSystemSetup_l, mOption, 0x1D);
} // namespace sead
