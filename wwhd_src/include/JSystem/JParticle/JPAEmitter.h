// Companion emitter methods extend the already-merged canonical calc layout.
#pragma once
#include "JSystem/JParticle/JPA_emitter_calc.h"
#include "JSystem/JParticle/JPAEmitter_gpu.h"
#include "JSystem/JParticle/JPA_particle.h"
namespace jpa_emitter_extension {
struct JPAExtraShapePivotVtable {
    u8 mPrefix[0xA4];
    be<u32> getPivotX;
    u8 mBeforeY[4];
    be<u32> getPivotY;
};
struct JPAExtraShapePivot { gptr<JPAExtraShapePivotVtable> __vtbl; };
struct JPABaseShapeToneVtable {
    u8 mPrefix[0x7C]; be<u32> getTevMode;
    u8 mBeforeTone[0x84]; be<u32> isEnableAnmTone;
    u8 mBeforeProjection[4]; be<u32> isEnableProjection;
    u8 mBeforeTexture[0x2C]; be<u32> getTextureIndex;
};
struct JPABaseShapeToneView { gptr<JPABaseShapeToneVtable> __vtbl; };
struct JPAExTexShapeVtable {
    u8 mPrefix[0x14]; be<u32> getIndTexMode; u8 mBeforeID[4];
    be<u32> getIndTexMtxID; u8 mBeforeMatrix[4];
    be<u32> getIndTexMtx; u8 mBeforeScale[4]; be<u32> getExpScale;
    u8 mBeforeSecond[0x14]; be<u32> isEnableSecondTex;
};
struct JPAExTexShapeView { gptr<JPAExTexShapeVtable> __vtbl; };
struct JPAShaderMaterial;
struct JPAShaderVariantTable { be<u32> mCount; gptr<JPAShaderMaterial> mEntries; };
struct JPAShaderMaterial {
    union {
        u8 mPrefix[0x1C];
        struct {
            gptr<void> mShader;
            u8 mPadding4[8];
            be<u32> mBindingCount;
            gptr<void> mBindings;
        };
    };
    gptr<JPAShaderVariantTable> mVariants;
    be<u8> mReady;
    u8 mPadding[3];
};
struct JPAShaderResource { u8 mData[0x84]; };
struct JPAShaderProgram {
    u8 mPrefix[0x1C]; be<u32> mResourceCount; gptr<JPAShaderResource> mResources;
};
struct JPAShaderMaterialSet {
    u8 mPrefix[4]; gptr<JPAShaderProgram> mProgram;
    be<u32> mCount; gptr<JPAShaderMaterial> mMaterials;
};
struct JPAEmitterRenderInfo {
    be<u32> mName, mVtable;
    be<u32> mColorBits[4];
    be<u8> mEnabled, mPass; u8 mPadding[2];
};
struct JPADynamicsVtable {
    u8 mPrefix[0x14];
    be<u32> getDataFlag; u8 mAfterDataFlag[4];
    be<u32> getUseKeyFlag; u8 mAfterUseKeyFlag[4];
    be<u32> getEmitterScl; u8 mAfterEmitterScl[4];
    be<u32> getEmitterRot; u8 mAfterEmitterRot[4];
    be<u32> getEmitterTrs; u8 mAfterEmitterTrs[4];
    be<u32> getEmitterDir; u8 mAfterEmitterDir[4];
    be<u32> getInitVelOmni; u8 mAfterInitVelOmni[4];
    be<u32> getInitVelAxis; u8 mAfterInitVelAxis[4];
    be<u32> getInitVelRndm; u8 mAfterInitVelRndm[4];
    be<u32> getInitVelDir; u8 mAfterInitVelDir[4];
    be<u32> getSpread; u8 mAfterSpread[4];
    be<u32> getInitVelRatio; u8 mAfterInitVelRatio[4];
    be<u32> getVolumeType; u8 mAfterVolumeType[4];
    be<u32> getVolumeSize; u8 mAfterVolumeSize[4];
    be<u32> getVolumeSweep; u8 mAfterVolumeSweep[4];
    be<u32> getVolumeMinRad; u8 mAfterVolumeMinRad[4];
    be<u32> getDivNumber; u8 mAfterDivNumber[4];
    be<u32> getRate; u8 mAfterRate[4];
    be<u32> getRateRndm; u8 mAfterRateRndm[4];
    be<u32> getRateStep; u8 mAfterRateStep[4];
    be<u32> getMaxFrame; u8 mAfterMaxFrame[4];
    be<u32> getStartFrame; u8 mAfterStartFrame[4];
    be<u32> getLifeTime; u8 mAfterLifeTime[4];
    be<u32> getLifeTimeRndm; u8 mAfterLifeTimeRndm[4];
    be<u32> getAccel; u8 mAfterAccel[4];
    be<u32> getAccelRndm; u8 mAfterAccelRndm[4];
    be<u32> getAirResist; u8 mAfterAirResist[4];
    be<u32> getAirResistRndm; u8 mAfterAirResistRndm[4];
    be<u32> getMoment; u8 mAfterMoment[4];
    be<u32> getMomentRndm;
};
struct JPADynamicsView { gptr<JPADynamicsVtable> __vtbl; };
struct ParticleLists {
    ::JPAFieldManager mFields;
    ::JPAParticleList mActive, mChild;
};
WWHD_SIZE(ParticleLists, 0x28);
struct JPABaseEmitter : jpa_emitter_calc::JPABaseEmitter {
    void create(jpa_emitter_calc::JPADataBlockLinkInfo*);
    void initGpuState(void*);
    void setRenderMatrix(const Mtx34*);
    void updateViewPosition(s32);
    void requestDelete();
    JPAEmitterScaleAndColors& scaleAndColors() { return *reinterpret_cast<JPAEmitterScaleAndColors*>(pad238); }
    ParticleLists& particles() { return *reinterpret_cast<ParticleLists*>(mFieldManager); }
    gptr<void>& vacantParticlePool() { return *reinterpret_cast<gptr<void>*>(_1C4 + 24); }
    ::JPAParticleVector* deferredParticles() { return reinterpret_cast<::JPAParticleVector*>(_1C4); }
    u8 getPivotX();
    u8 getPivotY();
    void initializeMaterialBindings(JPAShaderMaterialSet*);
    void applyRenderInfo(JPAEmitterRenderInfo*);
    void selectTextureVariant(u32);
    void acquireMediumGpuPool();
    void acquireLargeGpuPool();
    void acquireSmallGpuPool();
    void releaseGpuPools();
    bool acquireParticleGpuPool(u32);
    u32 getViewGroup();
    void drawView(void*, JPADrawParameters*);
    JPAEmitterGpuState& gpu() {
        return mGpuState;
    }
};
WWHD_SIZE(JPABaseEmitter, 0x3FC);
WWHD_SIZE(JPAExtraShapePivotVtable, 0xB0);
WWHD_OFFSET(JPAExtraShapePivotVtable, getPivotY, 0xAC);
WWHD_SIZE(JPAExtraShapePivot, 4);
WWHD_SIZE(JPADynamicsVtable, 0x100);
WWHD_OFFSET(JPADynamicsVtable, getMomentRndm, 0xFC);
WWHD_OFFSET(JPABaseShapeToneVtable, isEnableAnmTone, 0x104);
WWHD_OFFSET(JPABaseShapeToneVtable, getTextureIndex, 0x13C);
WWHD_SIZE(JPAEmitterRenderInfo, 0x1C);
WWHD_OFFSET(JPAEmitterRenderInfo, mColorBits, 8);
WWHD_SIZE(JPAShaderVariantTable, 8);
WWHD_SIZE(JPAShaderMaterialSet, 0x10);
WWHD_OFFSET(JPAShaderMaterialSet, mMaterials, 0xC);
WWHD_SIZE(JPAShaderProgram, 0x24);
WWHD_OFFSET(JPAShaderProgram, mResources, 0x20);
WWHD_OFFSET(JPAShaderMaterial, mShader, 0);
WWHD_OFFSET(JPAShaderMaterial, mBindingCount, 0xC);
WWHD_OFFSET(JPAShaderMaterial, mBindings, 0x10);
WWHD_OFFSET(JPAShaderMaterial, mVariants, 0x1C);
WWHD_OFFSET(JPAShaderMaterial, mReady, 0x20);
WWHD_SIZE(JPAExTexShapeVtable, 0x48);
WWHD_OFFSET(JPAExTexShapeVtable, isEnableSecondTex, 0x44);
WWHD_SIZE(JPAShaderMaterial, 0x24);
WWHD_SIZE(JPAShaderResource, 0x84);
}
u32 JPAGroupToView(u32);
