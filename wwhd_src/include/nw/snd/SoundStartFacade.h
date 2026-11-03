/* NW-SND#6: the sound-start interface (the archive player implements it): start now, start by label,
 * hold (keep a sound running while called each frame), and prepare. Dispatch through its vtable. */
#pragma once
#include "nw/snd/ArchiveStartContext.h"
namespace nw { namespace snd {
struct SoundStarterVtable {
    VtableEntry header, destroy, startSound, findLabel;
};
WWHD_OFFSET(SoundStarterVtable, startSound, 0x10);
enum { InvalidSoundId = 0xFFFFFFFF };
struct SoundStarter {
    gptr<SoundStarterVtable> vtable;
    u32 startSoundVirtual(SoundHandle* handle, u32 id, u32 hold, SoundStartInfo* info) {
        return gabi::call_ptr<u32>(vtable->startSound.function, this, handle, id, hold, info);
    }
    u32 findLabelVirtual(const char* label) { return gabi::call_ptr<u32>(vtable->findLabel.function, this, label); }

    u32 startSound(SoundHandle* handle, u32 id, SoundStartInfo* info);
    u32 startSoundByLabel(SoundHandle* handle, const char* label, SoundStartInfo* info);
    u32 holdSound(SoundHandle* handle, u32 id, SoundStartInfo* info);
    u32 prepareSound(SoundHandle* handle, u32 id, SoundStartInfo* info);
    u32 prepareSoundByLabel(SoundHandle* handle, const char* label, SoundStartInfo* info);
};
} }
