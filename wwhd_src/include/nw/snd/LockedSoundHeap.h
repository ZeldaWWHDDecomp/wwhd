/* NW-SND#6: a sound heap behind a mutex (sead::SoundHeap derives from it). Layout from 02896DBC. */
#pragma once
#include "nw/snd/SoundStartStorage.h"
namespace nw { namespace snd {
struct LockedSoundHeap {
    be<u32> vtable;
    u8 mutex[0x2C];
    be<u8> alive;
    u8 padding[3];
    SoundHeap heap;
    static LockedSoundHeap* create(LockedSoundHeap* storage);
    static void destroy(LockedSoundHeap* storage, u32 flags);
    void reset();
    void finalize();
    u32 initialize(u32 buffer, u32 size);
    void* allocate(u32 size, u32 callback, u32 argument);
    void* allocateWithRelease(u32 size);
};
enum { HeapReleaseCommand_Id = 0x46 };
/* Sent to the sound thread when a block allocated with allocateWithRelease is released. */
struct HeapReleaseCommand : DriverCommand {
    gptr<LockedSoundHeap> owner;
    gptr<void> data;
};
WWHD_OFFSET(LockedSoundHeap, mutex, 4);
WWHD_OFFSET(LockedSoundHeap, alive, 0x30);
WWHD_OFFSET(LockedSoundHeap, heap, 0x34);
WWHD_SIZE(LockedSoundHeap, 0x44);
WWHD_SIZE(HeapReleaseCommand, 0x18);
} }

