/* WWHD sead global random generator: the singleton (created in a heap, seeded from the tick counter) and
 * its disposer.
 * Reconstructed from this game's instructions. */
#include "sead/Random.h"

using namespace sead;

static void* allocateArrayFromHeap(u32 size, RegistryHeap* heap, s32 alignment);
static void releaseObject(void* object);
static void constructHeapDisposer(Disposer* disposer, RegistryHeap* heap, u32 mode);
static void destroyDisposer(Disposer* disposer, u32 flags);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kDisposerHeapMode = 3;

static GlobalRandom* sGlobalRandom;
static GlobalRandomDisposer* sGlobalRandomDisposer;
static DisposerVtable sGlobalRandomDisposerVtable;

/* 0275CD20 */
GlobalRandom* sead::GlobalRandom::createInstance(RegistryHeap* heap) {
    GlobalRandom* instance = sGlobalRandom;
    if (instance == NULL) {
        instance = static_cast<GlobalRandom*>(allocateArrayFromHeap(sizeof(GlobalRandom), heap, 4));
        /* GHS takes the member's address without checking the allocation */
        GlobalRandomDisposer* disposer =
            gabi::at<GlobalRandomDisposer>(gabi::ea(instance) + offsetof(GlobalRandom, mInstanceDisposer));
        if (gabi::ea(disposer) != 0) {
            constructHeapDisposer(disposer, heap, kDisposerHeapMode);
            disposer->mVtable = &sGlobalRandomDisposerVtable;
        }
        sGlobalRandomDisposer = disposer;
        if (instance != NULL) instance->initFromTick();
        sGlobalRandom = instance;
    }
    return instance;
}

/* 0275CDC8 */
void sead::GlobalRandomDisposer::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sGlobalRandomDisposerVtable;
    if (this == sGlobalRandomDisposer) {
        sGlobalRandomDisposer = NULL;
        sGlobalRandom = NULL;
    }
    destroyDisposer(this, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* WWHD glue
 data sGlobalRandom 101F8B60
 data sGlobalRandomDisposer 101F8B64
 data sGlobalRandomDisposerVtable 1015DE98 nocheck
 ext 0273B0D4 void* allocateArrayFromHeap(u32, sead::RegistryHeap*, s32)
 ext 0273AF40 void releaseObject(void*)
 ext 02752B0C void constructHeapDisposer(sead::Disposer*, sead::RegistryHeap*, u32)
 ext 02752BEC void destroyDisposer(sead::Disposer*, u32)
*/
