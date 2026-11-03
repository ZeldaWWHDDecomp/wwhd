// Companion manager state begins with the already-merged group-list prefix.
#pragma once
#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JParticle/JPA_particle.h"
#include "JSystem/JParticle/JPAResourceManager.h"
#include "JSystem/JParticle/JPA_gpu_pool.h"
namespace jpa_emitter_extension {
struct JPADrawInfo {
    gptr<void> mCameraMtx;
    JPADrawParameters mParameters;
    be<f32> mFovy, mAspect;
    gptr<void> mTextureContext;
};
WWHD_SIZE(JPADrawInfo, 0x50);
WWHD_OFFSET(JPADrawInfo, mFovy, 0x44);

struct JPAManagerDynamicBuffer { u8 mPrefix[4]; be<u32> mFlags; u8 mTail[0x10]; };
struct JPAManagerGpuDescriptor { be<u32> mWords[4]; };
struct JPAGpuQueue { gptr<be<u32>> mIndices; be<s32> mCapacity, mCursor, mCount; };  /* initialize 0282F050 */
struct JPAManagerGpuPools {
    JPAGpuPool<4> mFieldPools[100];
    JPAGpuPool<2> mParticlePools[1500];
    JPAGpuPool<10> mMediumPools[20];
    JPAGpuPool<300> mLargePools[20];
    union {
        JPAManagerGpuDescriptor mDescriptors[4];
        struct { JPAGpuQueue mFieldQueue, mParticleQueue, mMediumQueue, mLargeQueue; };
    };
    union {
        u8 mTail[12];
        struct { gptr<void> mFieldMaterial, mParticleMaterial, mLargeMaterial; };  /* 0282F290 / 0282F384 / 0282F46C */
    };
};
WWHD_SIZE(JPAManagerDynamicBuffer,0x18);
WWHD_OFFSET(JPAManagerGpuPools,mDescriptors,0x57E7C0);
WWHD_SIZE(JPAManagerGpuPools,0x57E80C);
WWHD_SIZE(JPAGpuQueue, 0x10);
WWHD_OFFSET(JPAManagerGpuPools,mParticleQueue,0x57E7D0);
WWHD_OFFSET(JPAManagerGpuPools,mLargeQueue,0x57E7F0);
WWHD_OFFSET(JPAManagerGpuPools,mFieldMaterial,0x57E800);
WWHD_OFFSET(JPAManagerGpuPools,mLargeMaterial,0x57E808);
struct JPATextureRegistration { be<u16> mID; u8 mPadding[2]; gptr<void> mTexture; };
WWHD_SIZE(JPATextureRegistration, 8);
struct JPAEmitterManager : jpa_emitter_calc::JPAEmitterManager {
    union {
        u8 mGpuPoolStorage[0x57E954 - 0x110];
        struct {
            gptr<JPAShaderMaterialSet> mMaterialSet;
            gptr<JPAShaderMaterial> mDefaultMaterial;
            JPAManagerDynamicBuffer mParticleBuffer, mEmitterBuffer;
            JPAManagerGpuPools mGpuPools;
        };
    };
    JPATextureRegistration mTextureRegistrations[1100];
    be<u16> mTextureRegistrationCount;
    u8 mEndPadding[2];
    JPABaseEmitter* createSimpleEmitterID(const cXyz*,u32,u32,u32,jpa_emitter_calc::EmitterCallback*,void*);
    void printStatus();
    void registerTexture(void*,u32);
    void deleteEmitter(JPABaseEmitter*);
    void forceDeleteEmitter(JPABaseEmitter*, u32);
    void clearResourceManager(u32);
    void forceDeleteAllEmitter();
    void drawView(JPADrawInfo*, u32);
    void drawIndirect(JPADrawInfo*, u32);
    void drawFirstView(JPADrawInfo*, u32);
    void drawSecondView(JPADrawInfo*, u32);
    void draw(JPADrawInfo*, u32);
    jpa_emitter_calc::JPAEmitterManager& pools() { return *this; }
};
WWHD_SIZE(JPAEmitterManager, 0x580BB8);
WWHD_OFFSET(JPAEmitterManager, mGpuPools, 0x148);
WWHD_OFFSET(JPAEmitterManager, mTextureRegistrations, 0x57E954);
WWHD_OFFSET(JPAEmitterManager, mTextureRegistrationCount, 0x580BB4);
}
