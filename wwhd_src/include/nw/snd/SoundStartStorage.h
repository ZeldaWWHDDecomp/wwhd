/* NW-SND#6: the counted lists this unit walks, and the per-TU constructors of small archive start
 * records (built through __construct_array by the archive player). Layouts from WWHD's own code. */
#pragma once
#include "nw/snd/ArchivePlayback.h"
namespace nw { namespace snd {
/* The object that embeds list node NODE at OFFSET (offsetof the link member); no NULL check, as compiled. */
template <class Owner>
inline Owner* nodeOwner(const void* node, u32 offset) {
    return gabi::at<Owner>(gabi::ea(node) - offset);
}
/* A member's guest address as GHS computes it: 32 bits, so for an object near the top of memory it wraps
 * to 0, and the inlined constructors' NULL tests see that. */
template <class T>
inline T* wrappedAddress(T* member) {
    return gabi::at<T>(gabi::ea(member));
}
/* BasicSound's virtual initialize() (vtable slot 3). */
inline void initializeSoundVirtual(BasicSound* sound) {
    gabi::call_ptr<void>(sound->vtable->initialize.function, sound);
}
} }

void fillBytes(void* destination, s32 value, u32 size);   // Cafe memset
void OSInitMutex(void* mutex);
void OSLockMutex(void* mutex);
void OSUnlockMutex(void* mutex);
u32 OSGetCoreId();
/* BasicSound's byte setters, bound with the full word the archive player passes. */
void setSoundPanCurve(nw::snd::BasicSound* sound, u32 value);
void setSoundRemoteFilter(nw::snd::BasicSound* sound, u32 value);
void setSoundPanMode(nw::snd::BasicSound* sound, u32 value);
