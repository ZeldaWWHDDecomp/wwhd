// Cafe draw-buffer methods extend the canonical manager GPU pools.
#pragma once
#include "JSystem/JParticle/JPAEmitterManager.h"
#include "JSystem/JKRHeap_adapters.h"
namespace JPADrawGpu {
using JPAFieldPool = JPAGpuPool<4>;
using JPAParticlePool = JPAGpuPool<2>;
using JPAMediumPool = JPAGpuPool<10>;
using JPALargePool = JPAGpuPool<300>;
struct JPAManagerGpuPools_l : jpa_emitter_extension::JPAManagerGpuPools {
    void initializeBuffers(heap_adapters::Heap*);
    JPAFieldPool* acquireFieldPool(be<u32>&);
    JPAParticlePool* acquireParticlePool(be<u32>&);
    JPAMediumPool* acquireMediumPool(be<u32>&);
    JPALargePool* acquireLargePool(be<u32>&);
    void releaseFieldPool(u32);
    void releaseParticlePool(u32);
    void releaseMediumPool(u32);
    void releaseLargePool(u32);
};
WWHD_SIZE(JPAManagerGpuPools_l,0x57E80C);
}
