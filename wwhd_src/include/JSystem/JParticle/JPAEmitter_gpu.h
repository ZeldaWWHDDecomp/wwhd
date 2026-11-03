// HD: named render-state component occupying emitter bytes 0x258..0x3FC.
#pragma once
#include "wwhd.h"
struct JPAEmitterVertexState {
    u8 mPrefix[0xC];
    gptr<const void> __vtbl;
    u8 mBeforeScratch[0x64];
    u8 mScratch[0x34];
};
WWHD_SIZE(JPAEmitterVertexState, 0xA8);
WWHD_OFFSET(JPAEmitterVertexState, mScratch, 0x74);
struct JPAEmitterScaleAndColors {
    be<f32> mGlobalParticleScale[3];
    be<u32> mGlobalPrmColor, mGlobalEnvColor;
};
WWHD_SIZE(JPAEmitterScaleAndColors, 0x14);
struct JPAEmitterGpuState {
    be<u32> mRenderFlags, mNoBloom;
    be<u16> mResourceIndex;
    be<u8> mGroupID, mResMgrID;
    gptr<void> mMaterialBindings[2];
    be<u32> mDrawContext;
    be<u8> mTextureVariant;
    u8 mBeforeName[3];
    gptr<const char> mResourceName;
    be<u32> mNameVtable;
    be<f32> mColorScale[4];
    be<u8> mRenderEnabled, mRenderPass, mDrawContextOverride, mApplyColorScale;
    be<u32> mColorBits[4][4];
    Mtx34 mRenderMatrix;
    gptr<void> mParticlePools[2];
    gptr<void> mSmallPool, mMediumPool, mLargePool;
    JPAEmitterVertexState mVertexState;
    u8 mDynamicBuffers[2][0xC];
    be<u8> mDeleteCountdown, mClearCountdown;
    be<u8> mDeleteRequested;
    union {
        u8 mBeforeContext[5];
        struct { u8 mBeforeTextureCopy; be<u8> mTextureCopyEnabled; u8 mAfterTextureCopy[3]; };  /* 0x180 (emitter 0x3D8): set by initialize 0282A9D0 from draw 0xF5, read by the stripe draw 02837B2C */
    };
    gptr<void> mContext, mTextureContext, mPoolOwner;
    be<u32> mParticlePoolIndices[2], mMediumPoolIndex, mLargePoolIndex, mSmallPoolIndex;
};
WWHD_SIZE(JPAEmitterGpuState, 0x1A4);
WWHD_OFFSET(JPAEmitterGpuState, mGroupID, 0xA);
WWHD_OFFSET(JPAEmitterGpuState, mRenderMatrix, 0x78);
WWHD_OFFSET(JPAEmitterGpuState, mParticlePools, 0xA8);
WWHD_OFFSET(JPAEmitterGpuState, mMediumPool, 0xB4);
WWHD_OFFSET(JPAEmitterGpuState, mPoolOwner, 0x18C);
WWHD_OFFSET(JPAEmitterGpuState, mParticlePoolIndices, 0x190);
WWHD_OFFSET(JPAEmitterGpuState, mTextureCopyEnabled, 0x180);

struct JPADrawParameters { be<u32> words[16]; };
WWHD_SIZE(JPADrawParameters, 0x40);
