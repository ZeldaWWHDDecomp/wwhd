// Fixed-block allocation layout reconstructed from WWHD instructions only.
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct AudioBlockPool {
    gptr<u8> storage;
    be<u32> storageBytes;
    be<u32> blockBytes;
    be<s32> blockCount;
    be<s32> allocatedCount;
    be<u8> occupied[4];
    void initialize(void* memory, u32 bytes, u32 count);
    void reset();
    void* allocate();
    void release(void* memory);
};
static_assert(offsetof(AudioBlockPool, occupied) == 0x14);
static_assert(sizeof(AudioBlockPool) == 0x18);
}}
