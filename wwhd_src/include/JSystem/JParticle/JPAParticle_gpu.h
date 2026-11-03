// Cafe GPU/deferred state extends the merged GameCube-compatible particle prefix.
#pragma once
#include "JSystem/JParticle/JPA_particle.h"
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JParticle/JPA_gpu_pool.h"
struct JPAFloatColor {
    union {
        struct { be<f32> r, g, b, a; };
        be<u32> bits[4];
    };
};
struct JPAEmitterColors { JPAFloatColor mPrm, mEnv; };
struct JPAIndTexMatrix { be<u32> elements[2][3]; };
namespace jpa_particle_extension {
struct GpuContextState {
    be<u32> mInitialized;
    be<u32> mDrawContext;
    gptr<JPAGpuPool<4>> mParentPool;
};
struct GpuDrawState {
    be<u32> mTextureIndices[2];
    gptr<void> mParticleBuffer, mEmitterBuffer;
    be<u8> mDrawMode;
    u8 mPadding[3];
    gptr<void> mHeap;
};
struct GpuPoolTail {
    u8 mBeforeOwner[0xB];
    gptr<void> mOwner;
    be<u32> mIndex;
};
struct JPABaseParticle : ::JPABaseParticle {  // no own fields: the canonical particle covers 0x121..0x127 (merge resolution 2026-10-21)
    void initializeGpu(void*,void*);
    void setDrawMode(u32);
    void finishGpuBuffers(u32);
    void drawGpu(u32,u32,u32);
    void setPrmColor(const JPAFloatColor*);
    void setEnvColor(const JPAFloatColor*);
    void applyEmitterColors(jpa_emitter_extension::JPABaseEmitter*);
    void setIndirectMatrix(u32,const JPAIndTexMatrix*,s32);
    void applyEmitterTexture(jpa_emitter_extension::JPABaseEmitter*);
    GpuContextState& contextState() { return *reinterpret_cast<GpuContextState*>(mBeforeSlot); }
    void acquireGpuPool();
    void releaseGpuPool();
    gptr<void>& gpuPool() { return *reinterpret_cast<gptr<void>*>(mBeforeGpu); }
    GpuDrawState& drawState() { return *reinterpret_cast<GpuDrawState*>(mBeforeDelete); }
    GpuPoolTail& poolTail() { return *reinterpret_cast<GpuPoolTail*>(mBeforeHidden); }
};
WWHD_SIZE(GpuContextState, 0xC);
WWHD_OFFSET(GpuContextState, mParentPool, 8);
WWHD_OFFSET(GpuDrawState, mParticleBuffer, 8);
WWHD_OFFSET(GpuDrawState, mDrawMode, 0x10);
WWHD_OFFSET(GpuDrawState, mHeap, 0x14);
WWHD_OFFSET(GpuPoolTail, mOwner, 0xB);
WWHD_OFFSET(GpuPoolTail, mIndex, 0xF);
WWHD_SIZE(GpuDrawState, 0x18);
WWHD_SIZE(GpuPoolTail, 0x13);
WWHD_SIZE(JPABaseParticle, 0x128);
}

WWHD_SIZE(JPAFloatColor, 0x10);
WWHD_SIZE(JPAEmitterColors, 0x20);
WWHD_SIZE(JPAIndTexMatrix, 0x18);
