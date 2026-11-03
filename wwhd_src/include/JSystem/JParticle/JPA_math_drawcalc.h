/** Qualified HD particle math and draw views.
 * HD: per-draw context/clipboard and GPU texture state extend the GameCube layout. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
struct JPABaseParticle;
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
// HD: the descriptor's GX2Surface starts at +4 (JPAMath_copyTextureState 027BDEB4 copies 0x74 =
// sizeof(GX2Surface) bytes from descriptor+4); this view holds its first 0x38 bytes.
struct JPATextureDescriptor {
    u8 padding_0[4];
    union {
        be<u32> mLayout[6];
        struct { be<u32> mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat; };
    };
    be<u32> mAA;
    be<u32> mUse;
    be<u32> mImageSize;
    be<u32> mImageAddress;
    be<u32> mMipSize;
    be<u32> mMipAddress;
    be<u32> mTileMode;
    be<u32> mSwizzle;
};
WWHD_OFFSET(JPABaseEmitterHD, mTextureGeneration, 0x260);
WWHD_OFFSET(JPABaseEmitterHD, mpTextureContext, 0x3DC);
WWHD_SIZE(JPATextureDescriptor, 0x3C);
WWHD_OFFSET(JPATextureDescriptor, mWidth, 8);
WWHD_OFFSET(JPATextureDescriptor, mFormat, 0x18);
WWHD_OFFSET(JPATextureDescriptor, mAA, 0x1C);
WWHD_OFFSET(JPATextureDescriptor, mImageAddress, 0x28);
WWHD_OFFSET(JPATextureDescriptor, mMipAddress, 0x30);
WWHD_OFFSET(JPATextureDescriptor, mSwizzle, 0x38);
WWHD_OFFSET(JPATextureDescriptor, mTileMode, 0x34);

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
    // HD: 0xB0 holds a GX2Surface copy (027BDEB4); mCachedImageAddress/mCachedMipAddress are its image and mip pointers.
    u8 padding_3c[0x98];
    be<u32> mCachedImageAddress;
    u8 padding_d8[4];
    be<u32> mCachedMipAddress;
    u8 padding_e0[0x70];
    be<u32> mMode[2];
    u8 padding_158[4];
    be<u32> mBinding[2];
    be<u32> mSecondaryBinding;
    be<u32> mExtraState[4];
    u8 padding_178[0x18];
    be<u8> mDirtyFlags;
};
WWHD_SIZE(JPATextureBinding, 16);
WWHD_OFFSET(JPATextureResource, mTextureBindings, 0x54);
WWHD_OFFSET(JPATextureState, mCachedImageAddress, 0xD4);
WWHD_OFFSET(JPATextureState, mCachedMipAddress, 0xDC);
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
    union { u8 padding_4[8]; gptr<void> mpAlternateClipBoards[2]; };  /* draw +0xB8/+0xBC banks (initialize 02829F40..4C) */
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
    /* execution-visitor aliases */
    union {
        u8 padding_0[0x24];
        struct { gptr<void> mpExecEmtrVis[1], mpExecEmtrPVis[5], mpExecEmtrCVis[3]; };
    };
    gptr<JPACalcVisitor> mpCalcEmtrVis[4];
    union { u8 padding_34[0x14]; gptr<void> mpExecPtclVis[5]; };
    gptr<JPACalcVisitor> mpCalcPtclVis[10];
    union { u8 padding_70[0x10]; gptr<void> mpExecChldVis[4]; };
    gptr<JPACalcVisitor> mpCalcChldVis[2];
    union {
        u8 padding_88[0x23];
        struct {
            gptr<void> mpHDExecEmtrPVis[2], mpHDExecEmtrCVis[2], mpHDExecPtclVis[2], mpHDExecChldVis[2];
            be<u8> execEmtrVisNum, execEmtrPVisNum, execEmtrCVisNum;
        };
    };
    be<u8> calcEmtrVisNum;
    union { u8 padding_ac; be<u8> execPtclVisNum; };
    be<u8> calcPtclVisNum;
    union { u8 padding_ae; be<u8> execChldVisNum; };
    be<u8> calcChldVisNum;
    union {
        u8 padding_b0[4];
        struct { be<u8> hdExecEmtrPVisNum, hdExecEmtrCVisNum, hdExecPtclVisNum, hdExecChldVisNum; };
    };
    JPADrawContext dc;
    be<f32> mScaleOut;
    be<u32> mPrmColor;
    be<u32> mEnvColor;
    be<u16> mTexIdx;
    be<u8> mDrawFlags, mUniformsUpdated[2], mTextureCopyEnabled;
    u8 mDrawTail[2];
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
WWHD_OFFSET(JPADraw, mpExecEmtrPVis, 0x4);
WWHD_OFFSET(JPADraw, mpExecEmtrCVis, 0x18);
WWHD_OFFSET(JPADraw, mpExecPtclVis, 0x34);
WWHD_OFFSET(JPADraw, mpExecChldVis, 0x70);
WWHD_OFFSET(JPADraw, mpHDExecEmtrPVis, 0x88);
WWHD_OFFSET(JPADraw, mpHDExecEmtrCVis, 0x90);
WWHD_OFFSET(JPADraw, mpHDExecPtclVis, 0x98);
WWHD_OFFSET(JPADraw, mpHDExecChldVis, 0xA0);
WWHD_OFFSET(JPADraw, execEmtrVisNum, 0xA8);
WWHD_OFFSET(JPADraw, execEmtrPVisNum, 0xA9);
WWHD_OFFSET(JPADraw, execEmtrCVisNum, 0xAA);
WWHD_OFFSET(JPADraw, execPtclVisNum, 0xAC);
WWHD_OFFSET(JPADraw, execChldVisNum, 0xAE);
WWHD_OFFSET(JPADraw, hdExecEmtrPVisNum, 0xB0);
WWHD_OFFSET(JPADraw, hdExecChldVisNum, 0xB3);
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

WWHD_OFFSET(JPAMathDraw::JPATextureState, mSecondaryBinding, 0x164);
WWHD_OFFSET(JPAMathDraw::JPADraw, mTexIdx, 0xF0);
WWHD_OFFSET(JPAMathDraw::JPADraw, mDrawFlags, 0xF2);
WWHD_OFFSET(JPAMathDraw::JPADraw, mUniformsUpdated, 0xF3);
WWHD_OFFSET(JPAMathDraw::JPADraw, mTextureCopyEnabled, 0xF5);
WWHD_SIZE(JPAMathDraw::JPADraw, 0xF8);
