/* WWHD sead random numbers: a four-word xorshift generator seeded through a multiplicative hash, and the
 * global generator singleton (a generator plus its singleton disposer, 0x20 bytes).
 * Layout from this game's instructions. */
#pragma once
#include "sead/HeapRegistry.h"
#include "sead/Thread.h"

namespace sead {

struct Random {
    be<u32> mState0;
    be<u32> mState1;
    be<u32> mState2;
    be<u32> mState3;

    void init(u32 seed);
    void initFromTick();
    u32 getU32();
};

struct GlobalRandom : Random {
    Disposer mInstanceDisposer;
    static GlobalRandom* createInstance(RegistryHeap* heap);
};

/* the singleton's disposer: clears the singleton when it goes with its heap */
struct GlobalRandomDisposer : Disposer {
    void destroy(u32 flags);
};

}

WWHD_OFFSET(sead::Random, mState3, 0xC);
WWHD_SIZE(sead::Random, 0x10);
WWHD_OFFSET(sead::GlobalRandom, mInstanceDisposer, 0x10);
WWHD_SIZE(sead::GlobalRandom, 0x20);
