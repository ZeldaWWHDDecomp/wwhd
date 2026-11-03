/* NW-SND#6: the sound player (limits, priority order, output settings, player heaps). The class is the
 * canonical nw::snd::SoundPlayer (BasicSound.h); its heaps are SoundMemoryArena objects. */
#pragma once
#include "nw/snd/SoundStartStorage.h"
#include "nw/snd/SoundMemoryArena.h"
namespace nw { namespace snd {
/* The player priority a sound competes with: its own plus the ambient one, clamped to 0..127. */
/* inlined into the player and pool functions */
inline s32 clampedPriority(s32 priority) { return priority > 127 ? 127 : priority < 0 ? 0 : priority; }
inline s32 soundPriority(BasicSound* sound) {
    s32 ambient = sound->ambient.priority;
    u32 own = (u8)sound->playerPriority;
    return clampedPriority((s32)(own + (u32)ambient));
}
} }
