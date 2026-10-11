/** Qualified HD particle math and draw views.
 * HD: per-draw context/clipboard and GPU texture state extend the GameCube layout. */
#pragma once
#include "JSystem/JParticle/JPAParticle.h"
namespace JPAMathDraw {
// HD: GPU texture slots and the default-texture cache sentinel.
enum {
    IND_TEXTURE_SINGLE = 1,
    IND_TEXTURE_SUB = 2,
    IND_TEXTURE_SLOT = 5,
    SUB_TEXTURE_SLOT = 6,
    SECOND_TEXTURE_SLOT = 7,
    DEFAULT_TEXTURE_CACHE_INDEX = 0x31
};
struct JPABaseShapeVtable {
    u8 padding_0[68];
    be<u32> getLoopOffset;
    u8 padding_48[220];
    be<u32> textureIsEmpty;
};
struct JPABaseShape {
    gptr<JPABaseShapeVtable> __vtbl;
    s32 getLoopOffset() { return gabi::call_ptr<s32>(__vtbl->getLoopOffset, this); }
    u32 textureIsEmpty() { return gabi::call_ptr<u32>(__vtbl->textureIsEmpty, this); }
};
struct JPAExtraShapeVtable {
    u8 padding_0[20];
    be<u32> isEnableScale;
    u8 padding_18[156];
    be<u32> getRandomScale;
    u8 padding_b8[4];
    be<u32> isEnableAlpha;
    u8 padding_c0[76];
    be<u32> getAlphaWaveRandom;
    u8 padding_110[20];
    be<u32> isEnableRotate;
    u8 padding_128[4];
    be<u32> getRotateAngle;
    u8 padding_130[4];
    be<u32> getRotateSpeed;
    u8 padding_138[4];
    be<u32> getRotateRandomAngle;
    u8 padding_140[4];
    be<u32> getRotateRandomSpeed;
    u8 padding_148[4];
    be<u32> getRotateDirection;
};
struct JPAExtraShape {
    gptr<JPAExtraShapeVtable> __vtbl;
    u32 isEnableScale() { return gabi::call_ptr<u32>(__vtbl->isEnableScale, this); }
    f32 getRandomScale() { return gabi::call_ptr<f32>(__vtbl->getRandomScale, this); }
    u32 isEnableAlpha() { return gabi::call_ptr<u32>(__vtbl->isEnableAlpha, this); }
    f32 getAlphaWaveRandom() { return gabi::call_ptr<f32>(__vtbl->getAlphaWaveRandom, this); }
    u32 isEnableRotate() { return gabi::call_ptr<u32>(__vtbl->isEnableRotate, this); }
    f32 getRotateAngle() { return gabi::call_ptr<f32>(__vtbl->getRotateAngle, this); }
    f32 getRotateSpeed() { return gabi::call_ptr<f32>(__vtbl->getRotateSpeed, this); }
    f32 getRotateRandomAngle() { return gabi::call_ptr<f32>(__vtbl->getRotateRandomAngle, this); }
    f32 getRotateRandomSpeed() { return gabi::call_ptr<f32>(__vtbl->getRotateRandomSpeed, this); }
    f32 getRotateDirection() { return gabi::call_ptr<f32>(__vtbl->getRotateDirection, this); }
};
struct JPASweepShapeVtable {
    u8 padding_0[164];
    be<u32> isEnableRotate;
    u8 padding_a8[4];
    be<u32> isInheritedScale;
    u8 padding_b0[4];
    be<u32> isInheritedAlpha;
    u8 padding_b8[4];
    be<u32> isInheritedRGB;
    u8 padding_c0[4];
    be<u32> getTextureIndex;
    u8 padding_c8[20];
    be<u32> getPrm;
    u8 padding_e0[4];
    be<u32> getEnv;
    u8 padding_e8[4];
    be<u32> getPrmAlpha;
    u8 padding_f0[4];
    be<u32> getEnvAlpha;
    u8 padding_f8[4];
    be<u32> getRotateSpeed;
    u8 padding_100[4];
    be<u32> getInheritScale;
    u8 padding_108[4];
    be<u32> getInheritAlpha;
    u8 padding_110[4];
    be<u32> getInheritRGB;
};
struct JPASweepShape {
    gptr<JPASweepShapeVtable> __vtbl;
    u32 isEnableRotate() { return gabi::call_ptr<u32>(__vtbl->isEnableRotate, this); }
    u32 isInheritedScale() { return gabi::call_ptr<u32>(__vtbl->isInheritedScale, this); }
    u32 isInheritedAlpha() { return gabi::call_ptr<u32>(__vtbl->isInheritedAlpha, this); }
    u32 isInheritedRGB() { return gabi::call_ptr<u32>(__vtbl->isInheritedRGB, this); }
    u32 getTextureIndex() { return gabi::call_ptr<u32>(__vtbl->getTextureIndex, this); }
    u32 getPrm() { return gabi::call_ptr<u32>(__vtbl->getPrm, this); }
    u32 getEnv() { return gabi::call_ptr<u32>(__vtbl->getEnv, this); }
    u32 getPrmAlpha() { return gabi::call_ptr<u32>(__vtbl->getPrmAlpha, this); }
    u32 getEnvAlpha() { return gabi::call_ptr<u32>(__vtbl->getEnvAlpha, this); }
    f32 getRotateSpeed() { return gabi::call_ptr<f32>(__vtbl->getRotateSpeed, this); }
    f32 getInheritScale() { return gabi::call_ptr<f32>(__vtbl->getInheritScale, this); }
    f32 getInheritAlpha() { return gabi::call_ptr<f32>(__vtbl->getInheritAlpha, this); }
    f32 getInheritRGB() { return gabi::call_ptr<f32>(__vtbl->getInheritRGB, this); }
};
struct JPAExTexShapeVtable {
    u8 padding_0[20];
    be<u32> getIndTexMode;
    u8 padding_18[28];
    be<u32> getIndTextureID;
    u8 padding_38[4];
    be<u32> getSubTextureID;
    u8 padding_40[4];
    be<u32> isEnableSecondTex;
    u8 padding_48[4];
    be<u32> getSecondTexIndex;
};
struct JPAExTexShape {
    gptr<JPAExTexShapeVtable> __vtbl;
    u32 getIndTexMode() { return gabi::call_ptr<u32>(__vtbl->getIndTexMode, this); }
    u32 getIndTextureID() { return gabi::call_ptr<u32>(__vtbl->getIndTextureID, this); }
    u32 getSubTextureID() { return gabi::call_ptr<u32>(__vtbl->getSubTextureID, this); }
    u32 isEnableSecondTex() { return gabi::call_ptr<u32>(__vtbl->isEnableSecondTex, this); }
    u32 getSecondTexIndex() { return gabi::call_ptr<u32>(__vtbl->getSecondTexIndex, this); }
};

struct JPABaseEmitterHD {
    u8 padding_0[0x1EC];
    be<u32> mRandomSeed;
    u8 padding_1f0[0x70];
    be<u16> mTextureGeneration;
    u8 padding_262[0x17A];
    gptr<void> mpTextureContext;
};
struct JPATextureDescriptor {
    u8 padding_0[4];
    be<u32> mLayout[6];
    be<u32> mSwizzle;
    u8 padding_20[8];
    be<u32> mAddress;
    u8 padding_2c[4];
    be<u32> mMipAddress;
    be<u32> mLayoutTail[2];
};
WWHD_OFFSET(JPABaseEmitterHD, mTextureGeneration, 0x260);
WWHD_OFFSET(JPABaseEmitterHD, mpTextureContext, 0x3DC);
WWHD_OFFSET(JPATextureDescriptor, mAddress, 0x28);
WWHD_OFFSET(JPATextureDescriptor, mLayoutTail, 0x34);

struct JPAParticleHD;
struct JPADrawContext;
struct JPATextureBinding {
    u8 padding[8];
    be<u32> mBinding[2];
};
struct JPATextureResource {
    u8 padding_0[0x50];
    gptr<gptr<JPATextureDescriptor>> pTexResArray;
    gptr<JPATextureBinding> mTextureBindings;
    u8 padding_58[4];
    be<u32> mDefaultTextureIndex;
};
enum JPATextureDirtyFlags { TEXTURE_BINDING_DIRTY = 2, TEXTURE_MODE_DIRTY = 4, TEXTURE_EXTRA_DIRTY = 0x20 };
struct JPATextureState {
    JPATextureDescriptor mDescriptor;
    u8 padding_3c[0x98];
    be<u32> mCachedAddress;
    u8 padding_d8[4];
    be<u32> mCachedMipAddress;
    u8 padding_e0[0x70];
    be<u32> mMode[2];
    u8 padding_158[4];
    be<u32> mBinding[2];
    u8 padding_164[4];
    be<u32> mExtraState[4];
    u8 padding_178[0x18];
    be<u8> mDirtyFlags;
};
WWHD_SIZE(JPATextureBinding, 16);
WWHD_OFFSET(JPATextureResource, mTextureBindings, 0x54);
WWHD_OFFSET(JPATextureState, mBinding, 0x15C);
WWHD_OFFSET(JPATextureState, mExtraState, 0x168);
WWHD_OFFSET(JPATextureState, mDirtyFlags, 0x190);

struct JPACalcVisitorVtable {
    u8 padding[0x14];
    be<u32> calc;
};
struct JPACalcVisitor {
    gptr<JPACalcVisitorVtable> __vtbl;
};
struct JPADrawContext {
    be<u32> pcb;
    u8 padding_4[8];
    gptr<JPABaseEmitterHD> pbe;
    gptr<JPABaseShape> pbsp;
    gptr<JPAExtraShape> pesp;
    gptr<JPASweepShape> pssp;
    gptr<JPAExTexShape> petx;
    be<u32> mpDraw;
    be<u32> mpActiveParticles;
    gptr<JPATextureResource> mpTextureResource;
    gptr<be<u16>> pTexIdx;
};
struct JPADraw {
    u8 padding_0[0x24];
    gptr<JPACalcVisitor> mpCalcEmtrVis[4];
    u8 padding_34[0x14];
    gptr<JPACalcVisitor> mpCalcPtclVis[10];
    u8 padding_70[0x10];
    gptr<JPACalcVisitor> mpCalcChldVis[2];
    u8 padding_88[0x23];
    be<u8> calcEmtrVisNum;
    u8 padding_ac;
    be<u8> calcPtclVisNum;
    u8 padding_ae;
    be<u8> calcChldVisNum;
    u8 padding_b0[4];
    JPADrawContext dc;
    be<f32> mScaleOut;
    be<u32> mPrmColor;
    be<u32> mEnvColor;
    void calc();
    void initParticle(JPAParticleHD *);
    void initChild(JPAParticleHD *, JPAParticleHD *);
    void calcParticle(JPABaseParticle *);
    void calcChild(JPABaseParticle *);
};
struct JPADrawParams {
    cXyz mAxis;
    be<f32> mScaleOut, mScaleX, mScaleY;
    u8 padding_18[8];
    be<f32> mAlphaOut, mAlphaWaveRandom;
    be<s32> mLoopOffset;
    be<u32> mPrmColor, mEnvColor;
    be<u16> mRotateAngle;
    be<s16> mRotateSpeed;
    u8 padding_38[2];
    be<u16> mTexIdx;
};
WWHD_OFFSET(JPADraw, mpCalcEmtrVis, 0x24);
WWHD_OFFSET(JPADraw, mpCalcPtclVis, 0x48);
WWHD_OFFSET(JPADraw, mpCalcChldVis, 0x80);
WWHD_OFFSET(JPADraw, calcEmtrVisNum, 0xAB);
WWHD_OFFSET(JPADraw, calcPtclVisNum, 0xAD);
WWHD_OFFSET(JPADraw, calcChldVisNum, 0xAF);
WWHD_OFFSET(JPADraw, dc, 0xB4);
WWHD_OFFSET(JPADraw, mScaleOut, 0xE4);
WWHD_SIZE(JPADrawContext, 0x30);
WWHD_SIZE(JPADrawParams, 0x3C);
WWHD_OFFSET(JPADrawParams, mRotateAngle, 0x34);

struct JPAParticleHD {
    u8 padding_0[0x8C];
    JPADrawParams mDrawParams;
    u8 padding_c8[0x28];
    gptr<void> mpTextureBindings;
    be<u32> mTextureIndex;
    be<u32> mTextureGeneration;
};
WWHD_OFFSET(JPAParticleHD, mpTextureBindings, 0xF0);
WWHD_OFFSET(JPAParticleHD, mTextureGeneration, 0xF8);

} // namespace JPAMathDraw
// HD: angle tables interleave sine and cosine at the native 13-bit index.
struct JPAMathSinCos {
    be<f32> sin, cos;
};
struct JPAMathSinCosTable {
    JPAMathSinCos entries[8192];
};
using JPAMtxP = be<f32> (*)[4];
WWHD_SIZE(JPAMathSinCos, 8);
WWHD_SIZE(JPAMathSinCosTable, 65536);
