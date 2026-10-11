/* Link range 04b: creation, sight/blur rendering and particle callbacks.
 * HD expands the emitter tail and render packets; GameCube callback member names retained.
 * Canonical Link layout stays in d_a_player_main.h. */
#pragma once
#include "SSystem/SComponent/c_cc_d.h"
#include "d/d_save.h"
#include "SSystem/SComponent/c_m2d_g_box.h"
#include "d/actor/d_a_player_main.h"
#include "gc_api/d/d_com_inf_game.h"
#include "JSystem/JParticle/JPAParticle.h"
#include "gc_api/JSystem/JPAMath.h"

namespace player04b {
// HD: complete callback-facing tail beyond the shared emitter's dynamics prefix.
struct Emitter {
    /* 0x000 */ u8 _000[0x28];
    /* 0x028 */ cXyz mEmitterDir;
    /* 0x034 */ u8 _034[0x5C - 0x34];
    /* 0x05C */ be<s32> mMaxFrame;
    /* 0x060 */ u8 _060[0x70 - 0x60];
    /* 0x070 */ be<f32> mInitialVelDir;
    /* 0x074 */ u8 _074[0x1AC - 0x74];
    /* 0x1AC */ be<u32> mpFirstParticle;
    /* 0x1B0 */ u8 _1B0[4];
    /* 0x1B4 */ be<s32> mActiveParticleCount;
    /* 0x1B8 */ u8 _1B8[8];
    /* 0x1C0 */ be<s32> mChildParticleCount;
    /* 0x1C4 */ u8 _1C4[0x1E4 - 0x1C4];
    /* 0x1E4 */ be<u32> mpEmitterCallBack;
    /* 0x1E8 */ be<u32> mpParticleCallBack;
    /* 0x1EC */ be<u32> mRandomSeed;
    /* 0x1F0 */ Mtx34 mGlobalRotation;
    /* 0x220 */ cXyz mGlobalDynamicsScale;
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ u8 _238[0x244 - 0x238];
    /* 0x244 */ GXColor mGlobalPrmColor;
    /* 0x248 */ u8 _248[0x254 - 0x248];
    /* 0x254 */ be<u32> mFlags;
    /* 0x258 */ u8 _258[0x262 - 0x258];
    /* 0x262 */ be<u8> mResourceVersion;
    /* 0x263 */ u8 _263;
    bool isEnableDeleteEmitter() const {
        return (mFlags & 8) && (s32)mActiveParticleCount + (s32)mChildParticleCount == 0;
    }
};
WWHD_OFFSET(Emitter, mInitialVelDir, 0x70);
WWHD_OFFSET(Emitter, mpFirstParticle, 0x1AC);
WWHD_OFFSET(Emitter, mActiveParticleCount, 0x1B4);
WWHD_OFFSET(Emitter, mChildParticleCount, 0x1C0);
WWHD_OFFSET(Emitter, mpEmitterCallBack, 0x1E4);
WWHD_OFFSET(Emitter, mpParticleCallBack, 0x1E8);
WWHD_OFFSET(Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Emitter, mGlobalPrmColor, 0x244);
WWHD_OFFSET(Emitter, mFlags, 0x254);
WWHD_OFFSET(Emitter, mResourceVersion, 0x262);
WWHD_SIZE(Emitter, 0x264);
inline Emitter* emitterView(JPABaseEmitter* emitter) { return gabi::at<Emitter>(gabi::ea(emitter)); }
struct ParticleLink {
    gptr<JPABaseParticle> mpParticle;
    u8 _04[8];
    be<u32> mpNext;
};
WWHD_OFFSET(ParticleLink, mpNext, 0xC);
WWHD_SIZE(ParticleLink, 0x10);
}

struct daPy_followEcallBack_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u32> mpEmitter;
    /* 0x08 */ cXyz mPos;
    /* 0x14 */ csXyz mAngle;
    /* 0x1A */ u8 _1A[2];
    void execute(JPABaseEmitter*);
    void setup(JPABaseEmitter*, const cXyz*, const csXyz*, s8);
};
WWHD_SIZE(daPy_followEcallBack_c, 0x1C);
WWHD_OFFSET(daPy_followEcallBack_c, __vtbl, 0x00);
WWHD_OFFSET(daPy_followEcallBack_c, mpEmitter, 0x04);
WWHD_OFFSET(daPy_followEcallBack_c, mPos, 0x08);
WWHD_OFFSET(daPy_followEcallBack_c, mAngle, 0x14);
WWHD_OFFSET(daPy_followEcallBack_c, _1A, 0x1A);
struct daPy_waterDropEcallBack_c : daPy_followEcallBack_c {
    /* 0x1C */ be<u32> field_0x1C;
    void execute(JPABaseEmitter*);
    void setup(JPABaseEmitter*, const cXyz*, const csXyz*, s8);
    void end() { gabi::call(0x0240CA4C, this); }
};
WWHD_SIZE(daPy_waterDropEcallBack_c, 0x20);
WWHD_OFFSET(daPy_waterDropEcallBack_c, field_0x1C, 0x1C);
struct daPy_swimTailEcallBack_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> mbEnd;
    /* 0x05 */ be<u8> field_0x05;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<f32> mSpeedRate;
    /* 0x0C */ be<f32> mWaterY;
    /* 0x10 */ be<f32> mWaterFlatY;
    /* 0x14 */ cXyz mPos;
    /* 0x20 */ be<u32> field_0x20; /* const csXyz* */
    /* 0x24 */ be<u32> mpEmitter;
    void remove();
    void getMaxWaterY(cXyz*);
    void execute(JPABaseEmitter*);
    void draw(JPABaseEmitter*);
    void setup(JPABaseEmitter*, const cXyz*, const csXyz*, s8);
};
WWHD_SIZE(daPy_swimTailEcallBack_c, 0x28);
WWHD_OFFSET(daPy_swimTailEcallBack_c, __vtbl, 0x00);
WWHD_OFFSET(daPy_swimTailEcallBack_c, mbEnd, 0x04);
WWHD_OFFSET(daPy_swimTailEcallBack_c, field_0x05, 0x05);
WWHD_OFFSET(daPy_swimTailEcallBack_c, _06, 0x06);
WWHD_OFFSET(daPy_swimTailEcallBack_c, mSpeedRate, 0x08);
WWHD_OFFSET(daPy_swimTailEcallBack_c, mWaterY, 0x0C);
WWHD_OFFSET(daPy_swimTailEcallBack_c, mWaterFlatY, 0x10);
WWHD_OFFSET(daPy_swimTailEcallBack_c, mPos, 0x14);
WWHD_OFFSET(daPy_swimTailEcallBack_c, field_0x20, 0x20);
WWHD_OFFSET(daPy_swimTailEcallBack_c, mpEmitter, 0x24);
struct daPy_fanSwingEcallBack_c {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<s32> mAlphaOutFlg;
    /* 0x8 */ be<u32> mpEmitter;
    void execute(JPABaseEmitter*);
};
WWHD_SIZE(daPy_fanSwingEcallBack_c, 0xC);
WWHD_OFFSET(daPy_fanSwingEcallBack_c, __vtbl, 0x0);
WWHD_OFFSET(daPy_fanSwingEcallBack_c, mAlphaOutFlg, 0x4);
WWHD_OFFSET(daPy_fanSwingEcallBack_c, mpEmitter, 0x8);
struct daPy_mtxPosFollowEcallBack_c {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<u32> mpEmitter;
    /* 0x8 */ be<u32> mpMtx;
    /* 0xC */ be<u32> mpAngle;
    void execute(JPABaseEmitter*);
};
WWHD_SIZE(daPy_mtxPosFollowEcallBack_c, 0x10);
WWHD_OFFSET(daPy_mtxPosFollowEcallBack_c, __vtbl, 0x0);
WWHD_OFFSET(daPy_mtxPosFollowEcallBack_c, mpEmitter, 0x4);
WWHD_OFFSET(daPy_mtxPosFollowEcallBack_c, mpMtx, 0x8);
WWHD_OFFSET(daPy_mtxPosFollowEcallBack_c, mpAngle, 0xC);
struct daPy_waterDropPcallBack_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s32> mbWaterMark;
    /* 0x08 */ dBgS_GndChk mGndChk;
    void execute(JPABaseEmitter*, void*);
};
WWHD_SIZE(daPy_waterDropPcallBack_c, 0x5C);
WWHD_OFFSET(daPy_waterDropPcallBack_c, __vtbl, 0x00);
WWHD_OFFSET(daPy_waterDropPcallBack_c, mbWaterMark, 0x04);
WWHD_OFFSET(daPy_waterDropPcallBack_c, mGndChk, 0x08);
struct daPy_dmEcallBack_c {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<u32> mpEmitter;
    /* 0x8 */ be<u32> mpMtx;
    void execute(JPABaseEmitter*);
};
WWHD_SIZE(daPy_dmEcallBack_c, 0xC);
WWHD_OFFSET(daPy_dmEcallBack_c, __vtbl, 0x0);
WWHD_OFFSET(daPy_dmEcallBack_c, mpEmitter, 0x4);
WWHD_OFFSET(daPy_dmEcallBack_c, mpMtx, 0x8);

namespace player04b {
struct SafeString {
    /* 0x000 */ be<u32> mpString;
    /* 0x004 */ be<u32> __vtbl;
};
WWHD_OFFSET(SafeString, mpString, 0x0);
WWHD_OFFSET(SafeString, __vtbl, 0x4);
WWHD_SIZE(SafeString, 0x8);
}

namespace player04b {
struct Texture {
    /* 0x000 */ u8 _000[0x4];
    /* 0x004 */ be<u32> mDimension;
    /* 0x008 */ be<u32> mWidth;
    /* 0x00C */ be<u32> mHeight;
    /* 0x010 */ be<u32> mDepth;
    /* 0x014 */ be<u32> mMipCount;
    /* 0x018 */ be<u32> mFormat;
    /* 0x01C */ be<u32> mAaMode;
    /* 0x020 */ u8 _020[0x8];
    /* 0x028 */ be<u32> mpImage;
    /* 0x02C */ u8 _02C[0x4];
    /* 0x030 */ be<u32> mpMipmaps;
    /* 0x034 */ be<u32> mTileMode;
    /* 0x038 */ be<u32> mSwizzle;
    /* 0x03C */ u8 _03C[0x54];
};
WWHD_OFFSET(Texture, mDimension, 0x4);
WWHD_OFFSET(Texture, mWidth, 0x8);
WWHD_OFFSET(Texture, mHeight, 0xC);
WWHD_OFFSET(Texture, mDepth, 0x10);
WWHD_OFFSET(Texture, mMipCount, 0x14);
WWHD_OFFSET(Texture, mFormat, 0x18);
WWHD_OFFSET(Texture, mAaMode, 0x1C);
WWHD_OFFSET(Texture, mpImage, 0x28);
WWHD_OFFSET(Texture, mpMipmaps, 0x30);
WWHD_OFFSET(Texture, mTileMode, 0x34);
WWHD_OFFSET(Texture, mSwizzle, 0x38);
WWHD_SIZE(Texture, 0x90);
}

namespace player04b {
struct Sampler {
    /* 0x000 */ Texture mTexture;
    /* 0x090 */ u8 _090[0x44];
    /* 0x0D4 */ be<u32> mpBoundImage;
    /* 0x0D8 */ u8 _0D8[0x4];
    /* 0x0DC */ be<u32> mpBoundMipmaps;
    /* 0x0E0 */ u8 _0E0[0x7C];
    /* 0x15C */ be<u32> mWrapX;
    /* 0x160 */ be<u32> mWrapY;
    /* 0x164 */ be<u32> mWrapZ;
    /* 0x168 */ u8 _168[0x28];
    /* 0x190 */ be<u8> mDirtyFlags;
    /* 0x191 */ u8 _191[0x7];
};
WWHD_OFFSET(Sampler, mTexture, 0x0);
WWHD_OFFSET(Sampler, mpBoundImage, 0xD4);
WWHD_OFFSET(Sampler, mpBoundMipmaps, 0xDC);
WWHD_OFFSET(Sampler, mWrapX, 0x15C);
WWHD_OFFSET(Sampler, mWrapY, 0x160);
WWHD_OFFSET(Sampler, mWrapZ, 0x164);
WWHD_OFFSET(Sampler, mDirtyFlags, 0x190);
WWHD_SIZE(Sampler, 0x198);
}

namespace player04b {
struct Shape {
    /* 0x000 */ be<u32> mCount;
    /* 0x004 */ be<u32> mpShapeData;
    /* 0x008 */ u8 _008[0x10];
    /* 0x018 */ be<u32> mVertexMaterialVtable;
    /* 0x01C */ u8 _01C[0x64];
    /* 0x080 */ Mtx34 mModelMtx;
    /* 0x0B0 */ u8 _0B0[0x10];
    /* 0x0C0 */ be<u32> mPixelMaterialVtable;
    /* 0x0C4 */ u8 _0C4[0x64];
    /* 0x128 */ be<f32> mUniformValues[44];
    /* 0x1D8 */ u8 mUniformBuffers[96];
    /* 0x238 */ u8 _238[0x1E0];
};
WWHD_OFFSET(Shape, mCount, 0x0);
WWHD_OFFSET(Shape, mpShapeData, 0x4);
WWHD_OFFSET(Shape, mVertexMaterialVtable, 0x18);
WWHD_OFFSET(Shape, mModelMtx, 0x80);
WWHD_OFFSET(Shape, mPixelMaterialVtable, 0xC0);
WWHD_OFFSET(Shape, mUniformValues, 0x128);
WWHD_OFFSET(Shape, mUniformBuffers, 0x1D8);
WWHD_SIZE(Shape, 0x418);
}

namespace player04b {
struct DrawCall {
    /* 0x000 */ be<u32> mIndexType;
    /* 0x004 */ be<u32> mPrimitiveType;
    /* 0x008 */ be<u32> mpIndices;
    /* 0x00C */ be<u32> mIndexCount;
    /* 0x010 */ u8 _010[0x8];
};
WWHD_OFFSET(DrawCall, mIndexType, 0x0);
WWHD_OFFSET(DrawCall, mPrimitiveType, 0x4);
WWHD_OFFSET(DrawCall, mpIndices, 0x8);
WWHD_OFFSET(DrawCall, mIndexCount, 0xC);
WWHD_SIZE(DrawCall, 0x18);
}

namespace player04b {
struct VertexBuffer {
    /* 0x000 */ u8 _000[0x14C];
    /* 0x14C */ be<u32> mSize;
    /* 0x150 */ u8 _150[0x4];
};
WWHD_OFFSET(VertexBuffer, mSize, 0x14C);
WWHD_SIZE(VertexBuffer, 0x154);
}

namespace player04b {
struct VertexSlot {
    /* 0x000 */ be<u32> mpVertices;
    /* 0x004 */ VertexBuffer mBuffer;
    /* 0x158 */ u8 mVertexArray[244];
    /* 0x24C */ be<u32> mAllocationType;
    /* 0x250 */ be<u32> mpAllocation;
};
WWHD_OFFSET(VertexSlot, mpVertices, 0x0);
WWHD_OFFSET(VertexSlot, mBuffer, 0x4);
WWHD_OFFSET(VertexSlot, mVertexArray, 0x158);
WWHD_OFFSET(VertexSlot, mAllocationType, 0x24C);
WWHD_OFFSET(VertexSlot, mpAllocation, 0x250);
WWHD_SIZE(VertexSlot, 0x254);
}

namespace player04b {
struct VertexPair {
    /* 0x000 */ VertexSlot mSlots[2];
    /* 0x4A8 */ be<u32> mCurrent;
    /* 0x4AC */ be<u32> mFormat;
    /* 0x4B0 */ be<u32> mStride;
    /* 0x4B4 */ be<u32> mpFormatData;
    /* 0x4B8 */ be<u32> mpMaterial;
    /* 0x4BC */ be<u8> mReady;
    /* 0x4BD */ u8 _4BD[0x3];
};
WWHD_OFFSET(VertexPair, mSlots, 0x0);
WWHD_OFFSET(VertexPair, mCurrent, 0x4A8);
WWHD_OFFSET(VertexPair, mFormat, 0x4AC);
WWHD_OFFSET(VertexPair, mStride, 0x4B0);
WWHD_OFFSET(VertexPair, mpFormatData, 0x4B4);
WWHD_OFFSET(VertexPair, mpMaterial, 0x4B8);
WWHD_OFFSET(VertexPair, mReady, 0x4BC);
WWHD_SIZE(VertexPair, 0x4C0);
}

namespace player04b {
struct VertexArray {
    /* 0x000 */ VertexSlot mSlots[120];
    /* 0x11760 */ be<u32> mCurrent;
    /* 0x11764 */ be<u32> mFormat;
    /* 0x11768 */ be<u32> mStride;
    /* 0x1176C */ be<u32> mpFormatData;
    /* 0x11770 */ be<u32> mpMaterial;
    /* 0x11774 */ be<u8> mReady;
    /* 0x11775 */ u8 _11775[0x3];
};
WWHD_OFFSET(VertexArray, mSlots, 0x0);
WWHD_OFFSET(VertexArray, mCurrent, 0x11760);
WWHD_OFFSET(VertexArray, mFormat, 0x11764);
WWHD_OFFSET(VertexArray, mStride, 0x11768);
WWHD_OFFSET(VertexArray, mpFormatData, 0x1176C);
WWHD_OFFSET(VertexArray, mpMaterial, 0x11770);
WWHD_OFFSET(VertexArray, mReady, 0x11774);
WWHD_SIZE(VertexArray, 0x11778);
}

namespace player04b {
struct daPy_sightPacket_c {
    void draw();
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<u8> mDrawFlag;
    /* 0x005 */ be<u8> mLockFlag;
    /* 0x006 */ be<u8> mFrame;
    /* 0x007 */ be<u8> mLockAlpha;
    /* 0x008 */ cXyz mPos;
    /* 0x014 */ Mtx34 mMtx;
    /* 0x044 */ be<u32> mpImg;
    /* 0x048 */ be<u32> mpLockTex;
    /* 0x04C */ be<u32> mpSightTex;
    /* 0x050 */ be<u32> mpMaterial;
    /* 0x054 */ VertexPair mVertices;
    /* 0x514 */ Shape mShape;
    /* 0x92C */ be<u8> mShapeReady;
    /* 0x92D */ u8 _92D[0x3];
    /* 0x930 */ DrawCall mDrawCall;
    /* 0x948 */ be<s16> mIndices[4];
    /* 0x950 */ Sampler mSampler;
    /* 0xAE8 */ Texture mLockTexture;
    /* 0xB78 */ Texture mSightTexture;
};
WWHD_OFFSET(daPy_sightPacket_c, __vtbl, 0x0);
WWHD_OFFSET(daPy_sightPacket_c, mDrawFlag, 0x4);
WWHD_OFFSET(daPy_sightPacket_c, mLockFlag, 0x5);
WWHD_OFFSET(daPy_sightPacket_c, mFrame, 0x6);
WWHD_OFFSET(daPy_sightPacket_c, mLockAlpha, 0x7);
WWHD_OFFSET(daPy_sightPacket_c, mPos, 0x8);
WWHD_OFFSET(daPy_sightPacket_c, mMtx, 0x14);
WWHD_OFFSET(daPy_sightPacket_c, mpImg, 0x44);
WWHD_OFFSET(daPy_sightPacket_c, mpLockTex, 0x48);
WWHD_OFFSET(daPy_sightPacket_c, mpSightTex, 0x4C);
WWHD_OFFSET(daPy_sightPacket_c, mpMaterial, 0x50);
WWHD_OFFSET(daPy_sightPacket_c, mVertices, 0x54);
WWHD_OFFSET(daPy_sightPacket_c, mShape, 0x514);
WWHD_OFFSET(daPy_sightPacket_c, mShapeReady, 0x92C);
WWHD_OFFSET(daPy_sightPacket_c, mDrawCall, 0x930);
WWHD_OFFSET(daPy_sightPacket_c, mIndices, 0x948);
WWHD_OFFSET(daPy_sightPacket_c, mSampler, 0x950);
WWHD_OFFSET(daPy_sightPacket_c, mLockTexture, 0xAE8);
WWHD_OFFSET(daPy_sightPacket_c, mSightTexture, 0xB78);
WWHD_SIZE(daPy_sightPacket_c, 0xC08);
}

namespace player04b {
struct daPy_swBlur_c {
    enum BlurColorType { NORMAL_SLASH_BLUR, ELIXIR_SOUP_SLASH_BLUR, PARRYING_SLASH_BLUR };
    void draw();
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ be<u32> __vtbl;
    /* 0x010 */ u8 _010[0x88];
    /* 0x098 */ be<u32> mpTex;
    /* 0x09C */ be<s32> mPointCount;
    /* 0x0A0 */ be<u32> mSampleIndex;
    /* 0x0A4 */ be<u32> mBlurColorType;
    /* 0x0A8 */ be<f32> mBlurTopRate;
    /* 0x0AC */ be<u32> mpPosBuffer;
    /* 0x0B0 */ u8 _0B0[0xC];
    /* 0x0BC */ cXyz mTopPos[60];
    /* 0x38C */ cXyz mBottomPos[60];
    /* 0x65C */ be<u32> mpMaterial;
    /* 0x660 */ VertexArray mVertices;
    /* 0x11DD8 */ Shape mShape;
    /* 0x121F0 */ be<u8> mShapeReady;
    /* 0x121F1 */ u8 _121F1[0x3];
    /* 0x121F4 */ DrawCall mDrawCall;
    /* 0x1220C */ be<s16> mIndices[4];
    /* 0x12214 */ Sampler mSampler;
    /* 0x123AC */ Texture mTexture;
};
WWHD_OFFSET(daPy_swBlur_c, __vtbl, 0xC);
WWHD_OFFSET(daPy_swBlur_c, mpTex, 0x98);
WWHD_OFFSET(daPy_swBlur_c, mPointCount, 0x9C);
WWHD_OFFSET(daPy_swBlur_c, mSampleIndex, 0xA0);
WWHD_OFFSET(daPy_swBlur_c, mBlurColorType, 0xA4);
WWHD_OFFSET(daPy_swBlur_c, mBlurTopRate, 0xA8);
WWHD_OFFSET(daPy_swBlur_c, mpPosBuffer, 0xAC);
WWHD_OFFSET(daPy_swBlur_c, mTopPos, 0xBC);
WWHD_OFFSET(daPy_swBlur_c, mBottomPos, 0x38C);
WWHD_OFFSET(daPy_swBlur_c, mpMaterial, 0x65C);
WWHD_OFFSET(daPy_swBlur_c, mVertices, 0x660);
WWHD_OFFSET(daPy_swBlur_c, mShape, 0x11DD8);
WWHD_OFFSET(daPy_swBlur_c, mShapeReady, 0x121F0);
WWHD_OFFSET(daPy_swBlur_c, mDrawCall, 0x121F4);
WWHD_OFFSET(daPy_swBlur_c, mIndices, 0x1220C);
WWHD_OFFSET(daPy_swBlur_c, mSampler, 0x12214);
WWHD_OFFSET(daPy_swBlur_c, mTexture, 0x123AC);
WWHD_SIZE(daPy_swBlur_c, 0x1243C);
}

namespace player04b {
struct RenderState {
    /* 0x000 */ be<u8> mDepthTest;
    /* 0x001 */ be<u8> mDepthWrite;
    /* 0x002 */ u8 _002[0x6];
    /* 0x008 */ be<u32> mDepthFunc;
    /* 0x00C */ be<u8> mBlendEnable;
    /* 0x00D */ u8 _00D[0x2B];
    /* 0x038 */ be<u8> mCullEnable;
    /* 0x039 */ u8 _039[0xB];
    /* 0x044 */ be<u8> mWriteRed;
    /* 0x045 */ be<u8> mWriteGreen;
    /* 0x046 */ be<u8> mWriteBlue;
    /* 0x047 */ be<u8> mWriteAlpha;
    /* 0x048 */ u8 _048[0x2C];
};
WWHD_OFFSET(RenderState, mDepthTest, 0x0);
WWHD_OFFSET(RenderState, mDepthWrite, 0x1);
WWHD_OFFSET(RenderState, mDepthFunc, 0x8);
WWHD_OFFSET(RenderState, mBlendEnable, 0xC);
WWHD_OFFSET(RenderState, mCullEnable, 0x38);
WWHD_OFFSET(RenderState, mWriteRed, 0x44);
WWHD_OFFSET(RenderState, mWriteGreen, 0x45);
WWHD_OFFSET(RenderState, mWriteBlue, 0x46);
WWHD_OFFSET(RenderState, mWriteAlpha, 0x47);
WWHD_SIZE(RenderState, 0x74);
}

namespace player04b {
// HD: ResTIMG gains a trailing renderer pointer; retain all nine words in copies.
struct TextureHeader {
    u8 _00[0xC]; be<u32> paletteOffset; u8 _10[0xC];
    be<u32> imageOffset, mHDTexture;
};
WWHD_OFFSET(TextureHeader, paletteOffset, 0xC);
WWHD_OFFSET(TextureHeader, imageOffset, 0x1C);
WWHD_OFFSET(TextureHeader, mHDTexture, 0x20);
WWHD_SIZE(TextureHeader, 0x24);
struct TextureOriginals {
    be<s32> mCount;
    be<s32> mCapacity;
    be<u32> mpHeaders;
    u8 mStorage[0x28];
};
WWHD_OFFSET(TextureOriginals, mpHeaders, 8);
WWHD_SIZE(TextureOriginals, 0x34);
struct AnmHeap {
    be<s16> mIdx, field_0x2, field_0x4, field_0x6;
    be<u32> m_buffer, mpAnimeHeap;
};
WWHD_OFFSET(AnmHeap, m_buffer, 8);
WWHD_OFFSET(AnmHeap, mpAnimeHeap, 0xC);
WWHD_SIZE(AnmHeap, 0x10);
struct AnmRatio {
    be<f32> mRatio;
    be<u32> mpAnmTransform;
    u8 _08[8];
};
WWHD_SIZE(AnmRatio, 0x10);
struct AnimationValue {
    be<f32> mFrame, mStart, mEnd;
    u8 _0C[4];
    be<u32> mpInterpolate, mpContext;
};
WWHD_OFFSET(AnimationValue, mpInterpolate, 0x10);
WWHD_SIZE(AnimationValue, 0x18);
struct AnimationChannel {
    be<u32> mpAnimation;
    u8 _04[0x30];
    be<u32> __vtbl;
    u8 _38[0xC];
    be<u32> mFlags;
    u8 _48[0x10];
};
WWHD_OFFSET(AnimationChannel, __vtbl, 0x34);
WWHD_OFFSET(AnimationChannel, mFlags, 0x44);
WWHD_SIZE(AnimationChannel, 0x58);
struct MirrorEntries {
    be<s32> mCount, mCapacity;
    be<u32> mpEntries;
    u8 mStorage[0x80];
    be<u8> mEnabled;
};
WWHD_OFFSET(MirrorEntries, mEnabled, 0x8C);
WWHD_SIZE(MirrorEntries, 0x8D);
struct MirrorObject {
    u8 _00[0x9C];
    be<u32> mMirrorValue, mMirrorMask;
};
WWHD_OFFSET(MirrorObject, mMirrorMask, 0xA0);
WWHD_SIZE(MirrorObject, 0xA4);
struct Material;
struct daPy_matAnm_c {
    void calc(Material*);
    u8 _00[0x6C];
    cXy mOldOffset, mNowOffset;
};
WWHD_OFFSET(daPy_matAnm_c, mOldOffset, 0x6C);
WWHD_OFFSET(daPy_matAnm_c, mNowOffset, 0x74);
WWHD_SIZE(daPy_matAnm_c, 0x7C);
}

namespace player04b {
// HD: sead material resources use relative SRT and name-table offsets.
struct MaterialResource {
    u8 _00[0x34];
    be<s32> mSrtTableOffset, mNameTableOffset;
};
WWHD_OFFSET(MaterialResource, mSrtTableOffset, 0x34);
WWHD_SIZE(MaterialResource, 0x3C);
struct Material {
    be<u32> mpResource;
    be<u16> mDirtyFlags;
    u8 _06[6];
    be<u32> mpDirtySrtBits;
    u8 _10[0x18];
    be<u32> mpSrtValues;
};
WWHD_OFFSET(Material, mpDirtySrtBits, 0xC);
WWHD_OFFSET(Material, mpSrtValues, 0x28);
WWHD_SIZE(Material, 0x2C);
struct MaterialSrtEntry {
    u8 _00[2];
    be<u16> mValueOffset;
    be<s32> mUniformIndex;
    u8 _08[4];
    be<u16> mParentIndex;
    u8 _0E[6];
};
WWHD_OFFSET(MaterialSrtEntry, mParentIndex, 0xC);
WWHD_SIZE(MaterialSrtEntry, 0x14);
struct MaterialSrt {
    u8 _00[0x10];
    be<f32> mOffsetX, mOffsetY;
};
WWHD_SIZE(MaterialSrt, 0x18);
}

namespace player04b {
// HD: the shared Link header reserves this block for the complete background-check objects.
struct BackgroundChecks {
    dBgS_LinChk mLinkLinChk, mRopeLinChk, mBoomerangLinChk;
    dBgS_GndChk mGndChk;
    dBgS_RoofChk mRoofChk;
    dBgS_LinChk mArrowLinChk, mMirLightLinChk;
    dBgS_GndChk mLavaGndChk;
    cBgS_PolyInfo mPolyInfo;
};
WWHD_OFFSET(BackgroundChecks, mGndChk, 0x144);
WWHD_OFFSET(BackgroundChecks, mRoofChk, 0x198);
WWHD_OFFSET(BackgroundChecks, mLavaGndChk, 0x2BC);
WWHD_OFFSET(BackgroundChecks, mPolyInfo, 0x310);
WWHD_SIZE(BackgroundChecks, 0x320);
inline BackgroundChecks* backgroundChecks(daPy_lk_c* player) {
    return gabi::at<BackgroundChecks>(gabi::ea(player->mLinkLinChk));
}
}

namespace player04b {
struct ProjectionMatrix { be<f32> m[4][4]; };
WWHD_SIZE(ProjectionMatrix, 0x40);
}

namespace player04b {
// HD: the GameCube animation pointers became inline animation objects. Field names follow tww.
// The mirror rendering and model-instance spans remain opaque here.
struct ItemModels {
    /* 0x0000 */ mDoExt_brkAnm mpTswgripmsabBrk;
    /* 0x0078 */ mDoExt_btkAnm mpTswgripmsBtk;
    /* 0x00EC */ be<u32> mpPodmsModel;
    /* 0x00F0 */ be<u32> mpEquippedShieldModel;
    /* 0x00F4 */ be<u32> mpShaModel;
    /* 0x00F8 */ be<u32> mpShmsModel;
    /* 0x00FC */ mDoExt_bckAnm mAtngshaBck;
    /* 0x0188 */ mDoExt_btkAnm mpTshmsBtk;
    /* 0x01FC */ u8 _01FC[0x3428];
    /* 0x3624 */ be<u32> mpYmsls00Model;
    /* 0x3628 */ mDoExt_btkAnm mpYmsls00Btk;
    /* 0x369C */ be<u32> mpHbootsModels[2];
    /* 0x36A4 */ be<u32> mpPringModel;
    /* 0x36A8 */ be<u32> mpItemHeaps[2];
    /* 0x36B0 */ be<u32> mpEquipItemModel;
    /* 0x36B4 */ mDoExt_bckAnm mSwordAnim;
    /* 0x3740 */ be<u32> mpParachuteFanMorf;
    /* 0x3744 */ mDoExt_brkAnm mpBombBrk;
    /* 0x37BC */ mDoExt_brkAnm mpGwp00BrkData;
    /* 0x3834 */ mDoExt_btkAnm mpGwp00BtkData;
    /* 0x38A8 */ mDoExt_btkAnm mpGicer00Btk;
    /* 0x391C */ mDoExt_btkAnm mpIceArrowBtk;
    /* 0x3990 */ mDoExt_btkAnm mpLightArrowBtk;
    /* 0x3A04 */ mDoExt_btkAnm mpGicer01Btk;
    /* 0x3A78 */ be<u32> mpItemAnimeHeap;
    /* 0x3A7C */ be<u32> m_item_bck_buffer;
    /* 0x3A80 */ mDoExt_btkAnm mpEquipItemBtk;
    /* 0x3AF4 */ mDoExt_btkAnm mpSwordBtk;
    /* 0x3B68 */ mDoExt_brkAnm mpEquipItemBrk;
    /* 0x3BE0 */ be<u32> mpBottleContentsModel;
    /* 0x3BE4 */ be<u32> mpBottleCapModel;
    /* 0x3BE8 */ be<u32> mpSwordModel1;
    /* 0x3BEC */ be<u32> mpSwordTipStabModel;
    /* 0x3BF0 */ mDoExt_bpkAnm mpCutfBpk;
    /* 0x3C64 */ mDoExt_btkAnm mpCutfBtk;
    /* 0x3CD8 */ mDoExt_brkAnm mpCutfBrk;
    /* 0x3D50 */ mDoExt_brkAnm m2EFC;
    /* 0x3DC8 */ MirrorEntries mMirrorEntries;
    /* 0x3E55 */ u8 _3E55[0x793];
    /* 0x45E8 */ be<u32> mpSuimenMunyaModel;
    /* 0x45EC */ mDoExt_btkAnm mpSuimenMunyaBtk;
    /* 0x4660 */ be<u32> mpYuchw00Model;
    /* 0x4664 */ mDoExt_bckAnm mYuchw00Bck;
    /* 0x46F0 */ mDoExt_btkAnm mpYuchw00Btk;
    /* 0x4764 */ mDoExt_brkAnm mpYuchw00Brk;
    /* 0x47DC */ be<u32> mpYbafo00Model;
    /* 0x47E0 */ mDoExt_btkAnm mpYbafo00Btk;
    /* 0x4854 */ u8 _4854[0x30];
    /* 0x4884 */ mDoExt_brkAnm mYaura00rBrk;
    /* 0x48FC */ mDoExt_btkAnm mpYaura00Btk;
    /* 0x4970 */ be<u32> mpYmgcs00Model;
    /* 0x4974 */ mDoExt_brkAnm mYmgcs00Brk;
    /* 0x49EC */ mDoExt_btkAnm mpYmgcs00Btk;
};
WWHD_OFFSET(ItemModels, mpTswgripmsabBrk, 0x0);
WWHD_OFFSET(ItemModels, mpTswgripmsBtk, 0x78);
WWHD_OFFSET(ItemModels, mpPodmsModel, 0xEC);
WWHD_OFFSET(ItemModels, mpEquippedShieldModel, 0xF0);
WWHD_OFFSET(ItemModels, mpShaModel, 0xF4);
WWHD_OFFSET(ItemModels, mpShmsModel, 0xF8);
WWHD_OFFSET(ItemModels, mAtngshaBck, 0xFC);
WWHD_OFFSET(ItemModels, mpTshmsBtk, 0x188);
WWHD_OFFSET(ItemModels, mpYmsls00Model, 0x3624);
WWHD_OFFSET(ItemModels, mpYmsls00Btk, 0x3628);
WWHD_OFFSET(ItemModels, mpHbootsModels, 0x369C);
WWHD_OFFSET(ItemModels, mpPringModel, 0x36A4);
WWHD_OFFSET(ItemModels, mpItemHeaps, 0x36A8);
WWHD_OFFSET(ItemModels, mpEquipItemModel, 0x36B0);
WWHD_OFFSET(ItemModels, mSwordAnim, 0x36B4);
WWHD_OFFSET(ItemModels, mpParachuteFanMorf, 0x3740);
WWHD_OFFSET(ItemModels, mpBombBrk, 0x3744);
WWHD_OFFSET(ItemModels, mpGwp00BrkData, 0x37BC);
WWHD_OFFSET(ItemModels, mpGwp00BtkData, 0x3834);
WWHD_OFFSET(ItemModels, mpGicer00Btk, 0x38A8);
WWHD_OFFSET(ItemModels, mpIceArrowBtk, 0x391C);
WWHD_OFFSET(ItemModels, mpLightArrowBtk, 0x3990);
WWHD_OFFSET(ItemModels, mpGicer01Btk, 0x3A04);
WWHD_OFFSET(ItemModels, mpItemAnimeHeap, 0x3A78);
WWHD_OFFSET(ItemModels, m_item_bck_buffer, 0x3A7C);
WWHD_OFFSET(ItemModels, mpEquipItemBtk, 0x3A80);
WWHD_OFFSET(ItemModels, mpSwordBtk, 0x3AF4);
WWHD_OFFSET(ItemModels, mpEquipItemBrk, 0x3B68);
WWHD_OFFSET(ItemModels, mpBottleContentsModel, 0x3BE0);
WWHD_OFFSET(ItemModels, mpBottleCapModel, 0x3BE4);
WWHD_OFFSET(ItemModels, mpSwordModel1, 0x3BE8);
WWHD_OFFSET(ItemModels, mpSwordTipStabModel, 0x3BEC);
WWHD_OFFSET(ItemModels, mpCutfBpk, 0x3BF0);
WWHD_OFFSET(ItemModels, mpCutfBtk, 0x3C64);
WWHD_OFFSET(ItemModels, mpCutfBrk, 0x3CD8);
WWHD_OFFSET(ItemModels, m2EFC, 0x3D50);
WWHD_OFFSET(ItemModels, mMirrorEntries, 0x3DC8);
WWHD_OFFSET(ItemModels, mpSuimenMunyaModel, 0x45E8);
WWHD_OFFSET(ItemModels, mpSuimenMunyaBtk, 0x45EC);
WWHD_OFFSET(ItemModels, mpYuchw00Model, 0x4660);
WWHD_OFFSET(ItemModels, mYuchw00Bck, 0x4664);
WWHD_OFFSET(ItemModels, mpYuchw00Btk, 0x46F0);
WWHD_OFFSET(ItemModels, mpYuchw00Brk, 0x4764);
WWHD_OFFSET(ItemModels, mpYbafo00Model, 0x47DC);
WWHD_OFFSET(ItemModels, mpYbafo00Btk, 0x47E0);
WWHD_OFFSET(ItemModels, mYaura00rBrk, 0x4884);
WWHD_OFFSET(ItemModels, mpYaura00Btk, 0x48FC);
WWHD_OFFSET(ItemModels, mpYmgcs00Model, 0x4970);
WWHD_OFFSET(ItemModels, mYmgcs00Brk, 0x4974);
WWHD_OFFSET(ItemModels, mpYmgcs00Btk, 0x49EC);
WWHD_SIZE(ItemModels, 0x4A60);
inline ItemModels* itemModels(daPy_lk_c* player) { return gabi::at<ItemModels>(gabi::ea(player->mD90)); }
}

namespace player04b {
struct SelfiePose { be<u32> mFace; be<f32> mFrame; be<u8> mKeep; u8 _09[3]; };
WWHD_OFFSET(SelfiePose, mFrame, 4);
WWHD_OFFSET(SelfiePose, mKeep, 8);
WWHD_SIZE(SelfiePose, 0xC);
}

namespace player04b {
struct EmitterMaterialParam {
    /* 0x00 */ be<u32> mName; /* sead::SafeString */
    /* 0x04 */ be<u32> __vtbl;
    /* 0x08 */ be<f32> mValues[4];
    /* 0x18 */ be<u8> mOptions[4];
};
WWHD_OFFSET(EmitterMaterialParam, mValues, 8);
WWHD_OFFSET(EmitterMaterialParam, mOptions, 0x18);
WWHD_SIZE(EmitterMaterialParam, 0x1C);
}

// GameCube API bindings used only by this range; autoglue keeps their guest ABI.
JKRSolidHeap* mDoExt_createSolidHeapFromGameToCurrent(u32, u32);
void mDoExt_restoreCurrentHeap();
void mDoAud_bgmStart(u32);
void mDoAud_subBgmStop();
void mDoAud_seStart(u32);
BOOL cLib_chaseS(be<s16>*, s16, s16);

// HD: fixed guest ABI bindings for GX2 and the partially decoded sead renderer.
u32 player04bGetActiveShader(u32);
void player04bSetMaterialView(u32, s32, const Mtx34*, const void*, u32);
void player04bUpdateShape(u32, s32);
void player04bFlushMaterialUniforms(u32);
void player04bBindTexture(u32, u32, s32, s32);
void player04bConstructRenderState(u32);
void player04bApplyRenderState(u32);
void player04bAssignTexture(u32, u32);
void player04bUpdateShaderProgram(u32, s32);
void player04bBindShaderGroup(u32);
void player04bBindShaderProgram(u32);
void GX2CallDisplayList(u32, u32);
void GX2DrawIndexedEx(u32, u32, u32, u32, u32, u32);
void GX2SetGeometryUniformBlock(s32, u32, u32);
void GX2SetPixelUniformBlock(s32, u32, u32);
void GX2SetVertexUniformBlock(s32, u32, u32);

namespace player04b {
// HD: shader cache and draw-material records used by the GX2 sight and blur paths.
struct ShaderCache { be<u32> mpProgram, mpShader; };
WWHD_SIZE(ShaderCache, 8);
struct ShaderResource { u8 _00[0x28]; be<u32> mpProgram; };
WWHD_OFFSET(ShaderResource, mpProgram, 0x28);
WWHD_SIZE(ShaderResource, 0x2C);
struct Shader {
    be<u8> mFlags;
    u8 _01[3];
    be<u32> mpDisplayList;
    u8 _08[4];
    be<u32> mDisplayListSize;
    u8 _10[0x6C];
    be<u32> mpResource;
};
WWHD_OFFSET(Shader, mpDisplayList, 4);
WWHD_OFFSET(Shader, mDisplayListSize, 0xC);
WWHD_OFFSET(Shader, mpResource, 0x7C);
WWHD_SIZE(Shader, 0x80);
struct DrawMaterial {
    be<u32> mpShader;
    u8 _04[8];
    be<u32> mUniformBlockCount, mpUniformBlocks, mSamplerCount, mpSamplerBindings;
};
WWHD_OFFSET(DrawMaterial, mUniformBlockCount, 0xC);
WWHD_OFFSET(DrawMaterial, mpUniformBlocks, 0x10);
WWHD_OFFSET(DrawMaterial, mSamplerCount, 0x14);
WWHD_OFFSET(DrawMaterial, mpSamplerBindings, 0x18);
WWHD_SIZE(DrawMaterial, 0x1C);
struct UniformBinding {
    u8 _00[0xC];
    be<s16> mVertexLocation, mPixelLocation, mGeometryLocation;
};
WWHD_OFFSET(UniformBinding, mVertexLocation, 0xC);
WWHD_OFFSET(UniformBinding, mPixelLocation, 0xE);
WWHD_OFFSET(UniformBinding, mGeometryLocation, 0x10);
WWHD_SIZE(UniformBinding, 0x12);
struct UniformBlock {
    u8 _00[4];
    be<u32> mpData;
    u8 _08[4];
    be<u32> mSize;
    u8 _10[0xC];
};
WWHD_OFFSET(UniformBlock, mpData, 4);
WWHD_OFFSET(UniformBlock, mSize, 0xC);
WWHD_SIZE(UniformBlock, 0x1C);
struct ShapeResource {
    u8 _00[0x10];
    u8 mUniformBlockData[0x3C];
    be<u32> mUniformBlockIndex;
};
WWHD_OFFSET(ShapeResource, mUniformBlockData, 0x10);
WWHD_OFFSET(ShapeResource, mUniformBlockIndex, 0x4C);
WWHD_SIZE(ShapeResource, 0x50);
}

namespace player04b {
struct MaterialManager {
    u8 _00[4];
    be<u32> mpArchive, mCount, mpMaterials;
};
WWHD_OFFSET(MaterialManager, mpArchive, 4);
WWHD_OFFSET(MaterialManager, mCount, 8);
WWHD_OFFSET(MaterialManager, mpMaterials, 0xC);
WWHD_SIZE(MaterialManager, 0x10);
struct MaterialArchive {
    u8 _00[0x1C];
    be<u32> mCount, mpMaterials;
};
WWHD_OFFSET(MaterialArchive, mCount, 0x1C);
WWHD_OFFSET(MaterialArchive, mpMaterials, 0x20);
WWHD_SIZE(MaterialArchive, 0x24);
struct MaterialEntry { u8 _00[0x20]; be<u8> mReady; u8 _21[3]; };
WWHD_OFFSET(MaterialEntry, mReady, 0x20);
WWHD_SIZE(MaterialEntry, 0x24);
struct MaterialSource { u8 mData[0x84]; };
WWHD_SIZE(MaterialSource, 0x84);
}
u32 player04bGetMaterialManager();
s32 player04bFindMaterial(u32, player04b::SafeString*);
void player04bLoadMaterial(u32, u32, s32);

namespace player04b {
// HD: address-only views of flags owned by the preceding Link source range.
inline be<u8>& eyeMoveFlag() { return gabi::object_at<be<u8>>(0x101CEF1A); }
inline be<u8>& morphFrame() { return gabi::object_at<be<u8>>(0x101CEF1B); }
inline be<u16>& dmType() { return gabi::object_at<be<u16>>(0x101CEF16); }
}

namespace player04b {
// GameCube constants used by this range; the shared Link layout remains unchanged.
enum daPy_HEAP_TYPE {
    HEAP_TYPE_UNDER_UPPER_e = 0, HEAP_TYPE_TEXTURE_ANIME_e = 1,
    HEAP_TYPE_TEXTURE_SCROLL_e = 2, HEAP_TYPE_ITEM_ANIME_e = 3,
};
enum ProcID {
    daPyProc_CRAWL_MOVE_e = 0x10, daPyProc_SMALL_JUMP_e = 0x29,
    daPyProc_SWIM_MOVE_e = 0x37, daPyProc_SHIP_RESTART_e = 0x91,
    daPyProc_TACT_PLAY_END_e = 0x9C,
};
}

namespace player04b {
// HD: the primary-player slots precede the shared Link/partner/ship registry.
struct PlayerRegistry {
    gptr<fopAc_ac_c> mpPlayer[2];
    gptr<fopAc_ac_c> mpLinkPlayer;
};
WWHD_OFFSET(PlayerRegistry, mpLinkPlayer, 8);
WWHD_SIZE(PlayerRegistry, 0xC);
inline PlayerRegistry& playerRegistry() {
    return gabi::object_at<PlayerRegistry>(dComIfGp_ea() + 0x5B2C);
}
}

// HD: explicit GHS constructor/destructor entry points keep null-allocation and array-cookie ABI.
void player04bConstructArray(u32, u32, u32, u32);
void player04bDestroyArray(u32, u32, u32, u32, u32, u32);
void player04bConstructPlayerBase(u32);
void player04bDestroyPlayerBase(u32, s32);
void player04bConstructLineCheck(u32);
void player04bConstructGroundCheck(u32);
void player04bDestroyBackgroundCheck(u32, s32);
void player04bConstructAcch(u32);
void player04bDestroyAcch(u32, s32);
void player04bConstructRoofCheck(u32);
void player04bConstructMirrorPacket(u32);
void player04bConstructAnimeSound(u32);
void player04bConstructCollisionGlobalStatus(u32);
void player04bConstructCollisionObjectInfo(u32);
void player04bDestroyCollisionObjectInfo(u32, s32);
void player04bConstructDrawCall(u32);
void player04bConstructSampler(u32);
void player04bConstructTexture(u32);
void player04bCreateShape(u32, s32, s32);
void player04bSetDrawCallIndices(u32, u32, s32, s32);
void player04bSetVertexBuffer(u32, u32, s32, s32);
void player04bBindVertexArray(u32, u32, u32, s32, s32);
void player04bLoadTextureImage(u32, u32);
void player04bFlushVertexBuffer(u32, u32, u32);

u32 player04bConstructCollisionStatus(u32);
u32 player04bConstructCylinder(u32);
u32 player04bConstructCapsule(u32);
u32 player04bConstructSphere(u32);
u32 player04bConstructRippleCallback(u32);
u32 player04bConstructModelPacket(u32);

// HD: backend base allocates on NULL, initializes its inline data, and returns the object.
u32 player04bConstructAnimationBackend(u32);
// HD: pointer-array storage is embedded after its count/capacity/pointer header.
void player04bSetPointerArrayBuffer(u32, s32, u32);

void player04bSetEmitterMaterial(JPABaseEmitter*, player04b::EmitterMaterialParam*);
void player04bBeginNamedHeap(void*, const char*, u32, u32);
void player04bEndNamedHeap(void*, s32);

namespace player04b {
template<class Collider>
inline CcDataHD::cCcD_ShapeAttr& shapeAttribute(Collider& collider) {
    return gabi::object_at<CcDataHD::cCcD_ShapeAttr>(gabi::ea(collider.mShapeAttr));
}
}

BOOL player04bCreateHeap(fopAc_ac_c*);

void player04bDestroyPolyCheck(u32, s32);
void player04bDestroyModelPacket(u32, s32);
void player04bClearActorKeep(u32);
void player04bTerminateSafeString(player04b::SafeString*);
void player04bDestroyCollisionStatus(u32, s32);
void player04bDestroyCollisionCylinder(u32, s32);
void player04bDestroyCollisionSphere(u32, s32);
void player04bSetCapsule(u32, u32);
void player04bConstructDrawListBase(u32);
void player04bDestroyDrawListBase(u32, u32);
void player04bInitMirrorPacket(u32, u32);
u32 player04bSetStartDemo(u32, u32);
void player04bRegisterPointLight(u32);
void player04bConstructSmokeCallback(u32, u32);
void player04bFreeHeapAllocation(u32);
u32 player04bGetResource(u32, player04b::SafeString*, u32);
void player04bNotifySystemChild(u32);
void player04bCopyTextureList(u32, u32);
u32 player04bAllocateAligned(u32, s32);
u32 player04bGetCameraProjection(u32);
void player04bLockGraphics(u32);
void player04bUnlockGraphics(u32);
u32 player04bFindContainingHeap(u32, u32);
u32 player04bGetCurrentHeap(u32);
void player04bDestroyIndexBuffer(u32, s32);
void player04bDestroyTextureSampler(u32, s32);
void player04bReleaseUniformBuffer(u32);
void player04bReleaseAttributeBuffer(u32);
void player04bBindAttributeBuffer(u32);
u32 player04bGetName(u32, u32);
void player04bConstructPacket(u32);
void player04bDestroyPacket(u32, u32);
void player04bDestroyAnimationTransform(u32, s32);
void player04bConstructUniformBlock(u32);
void player04bDestroyUniformBlock(u32, s32);
void player04bConstructShaderParameters(u32);
void player04bDestroyShaderParameters(u32, s32);
void player04bCopyProjection(u32, void*);
void player04bClearMemory(u32, u32);

namespace player04b {
// HD: extensions of shared J3D prefixes used by this range's material classification.
struct Joint : J3DJoint { u8 _0C[4]; be<u32> mMesh; u8 _14[8]; };
WWHD_OFFSET(Joint, mMesh, 0x10);
WWHD_SIZE(Joint, 0x1C);
struct MaterialNode { be<u32> mpName; be<u32> mNext; gptr<J3DShape> mShape; };
WWHD_OFFSET(MaterialNode, mNext, 4);
WWHD_OFFSET(MaterialNode, mShape, 8);
struct NameEntry { u8 _00[4]; be<u32> mNameOffset; };
WWHD_OFFSET(NameEntry, mNameOffset, 4);
struct ModelData : J3DModelData {
    u8 _14[0x30-0x14]; be<u32> mTexture, mTextureName;
};
WWHD_OFFSET(ModelData, mTexture, 0x30);
WWHD_OFFSET(ModelData, mTextureName, 0x34);
struct MatrixCalculator {
    u8 _00[0xA8]; be<u32> mUserArea; u8 _AC[4];
    be<u32> mJointBeforeCallback, mJointAfterCallback;
};
WWHD_OFFSET(MatrixCalculator, mUserArea, 0xA8);
WWHD_OFFSET(MatrixCalculator, mJointBeforeCallback, 0xB0);
WWHD_OFFSET(MatrixCalculator, mJointAfterCallback, 0xB4);
// HD: four dirty-mask words and two seeds follow the J3DTexture resource table.
struct TextureTable {
    be<u16> mNum; u8 _02[2]; be<u32> mpRes;
    be<u32> mDirtyTexMask[4], mDirtyMaskSeed[2];
};
WWHD_OFFSET(TextureTable, mpRes, 4);
WWHD_OFFSET(TextureTable, mDirtyTexMask, 8);
WWHD_OFFSET(TextureTable, mDirtyMaskSeed, 0x18);
WWHD_SIZE(TextureTable, 0x20);
// HD: additional actor state; unresolved bytes retain their measured field names.
struct ActorState {
    be<u8> mTingleDemoOrdered, mBottleThrowPending; u8 _02[2];
    be<u32> mThrowActor; u8 _0C[0x827C-0x826C];
    be<u8> field_0x827C; u8 _827D[3]; be<u16> field_0x8280; be<u8> field_0x8282;
};
WWHD_OFFSET(ActorState, mThrowActor, 4);
WWHD_OFFSET(ActorState, field_0x827C, 0x18);
WWHD_OFFSET(ActorState, field_0x8280, 0x1C);
WWHD_OFFSET(ActorState, field_0x8282, 0x1E);
inline ActorState& actorState(daPy_lk_c* p) { return gabi::object_at<ActorState>(gabi::ea(p)+0x8264); }
// HD: templates remain image-backed; this adds no second definition or GC initializer.
inline const dCcD_SrcCyl& l_cyl_src() { return gabi::object_at<dCcD_SrcCyl>(0x101CECDC); }
inline const dCcD_SrcCyl& l_wind_cyl_src() { return gabi::object_at<dCcD_SrcCyl>(0x101CED20); }
inline const dCcD_SrcCyl& l_at_cyl_src() { return gabi::object_at<dCcD_SrcCyl>(0x101CED64); }
inline const dCcD_SrcSph& l_fan_wind_sph_src() { return gabi::object_at<dCcD_SrcSph>(0x101CEB50); }
inline const u8* l_at_cps_src() { return gabi::at<const u8>(0x101CEDA8); }
inline const u8* l_fan_wind_cps_src() { return gabi::at<const u8>(0x101CEDF4); }
}

WWHD_OFFSET(player04b::AnmHeap, mIdx, 0);
WWHD_OFFSET(player04b::AnmHeap, field_0x2, 2);
WWHD_OFFSET(player04b::AnmHeap, field_0x4, 4);
WWHD_OFFSET(player04b::AnmHeap, field_0x6, 6);


namespace player04b {
// HD: the player stores the original 0x20-byte light prefix, without newer environment tails.
struct LightInfluence {
    cXyz mPos; GXColorS10 mColor; be<f32> mPower, mFluctuation; be<s32> mIdx;
};
WWHD_OFFSET(LightInfluence, mColor, 0xC);
WWHD_OFFSET(LightInfluence, mPower, 0x14);
WWHD_OFFSET(LightInfluence, mFluctuation, 0x18);
WWHD_SIZE(LightInfluence, 0x20);
}

namespace player04b {
// HD: cM3dGCps prefix isolated here because the ordinary capsule header redeclares shared Cyl.
struct CapsuleGeometry { cXyz mStart, mEnd; be<u32> __vtbl; be<f32> mRadius; };
WWHD_OFFSET(CapsuleGeometry, mRadius, 0x1C);
WWHD_SIZE(CapsuleGeometry, 0x20);
// HD: the enlarged mirror packet occupies the gap between two canonical item animations.
inline u8* mirrorPacket(daPy_lk_c* player) { return player->mD90 + 0x1FC; }
inline char* l_tact_event_label() { return gabi::at<char>(0x101CEC98); }
inline char* l_tact_night_event_label() { return gabi::at<char>(0x101CECC0); }
}

namespace player04b {
struct NamedHeap { u8 _00[0x10]; be<u32> mpName; };
WWHD_OFFSET(NamedHeap, mpName, 0x10);
}

namespace player04b {
// HD: bck animation embeds an expanded backend, followed by unresolved renderer state.
struct BckAnmHD {
    J3DFrameCtrl mFrameCtrl; be<u32> __vtbl; AnimationChannel mBackend;
    be<u32> field_0x6C, field_0x70, field_0x74, field_0x78;
    be<u32> field_0x7C, field_0x80, field_0x84, field_0x88;
};
WWHD_OFFSET(BckAnmHD, __vtbl, 0x10);
WWHD_OFFSET(BckAnmHD, mBackend, 0x14);
WWHD_OFFSET(BckAnmHD, field_0x7C, 0x7C);
WWHD_OFFSET(BckAnmHD, field_0x80, 0x80);
WWHD_OFFSET(BckAnmHD, field_0x84, 0x84);
WWHD_OFFSET(BckAnmHD, field_0x88, 0x88);
WWHD_SIZE(BckAnmHD, 0x8C);
inline BckAnmHD* bckAnm(mDoExt_bckAnm* a) { return gabi::at<BckAnmHD>(gabi::ea(a)); }
// HD: the model-packet objects have no established GameCube member identities.
struct HDModelPacket { u8 field_0x0[0xB0]; };
WWHD_SIZE(HDModelPacket, 0xB0);
struct ModelPackets {
    u8 _0000[0x4BE8]; HDModelPacket mBeforeArray[6];
    HDModelPacket mArray[2]; HDModelPacket mAfterArray[3];
};
WWHD_OFFSET(ModelPackets, mBeforeArray, 0x4BE8);
WWHD_OFFSET(ModelPackets, mArray, 0x5008);
WWHD_OFFSET(ModelPackets, mAfterArray, 0x5168);
WWHD_SIZE(ModelPackets, 0x5378);
inline ModelPackets* modelPackets(daPy_lk_c* p) { return gabi::at<ModelPackets>(gabi::ea(p)); }
}

u32 player04bConstructSignedAngle(be<s16>*, s32);
s16 player04bInvertSignedAngle(u32);
u32 player04bGetBackgroundActor(dBgS*, u32);
f64 player04bGroundCrossWide(dBgS*, dBgS_GndChk*);
void player04bSetCylinderCenter(cM3dGCyl*, cXyz*);
void player04bConstructAngleVector(csXyz*, s32, s32, s32);
void player04bInitShipStartPosition(u32, cXyz*, s32);
BOOL player04bCheckMovingBackground(dBgS*, cBgS_PolyInfo*);
u32 player04bGetGroundCode(dBgS*, cBgS_PolyInfo*);
u32 player04bGetCameraBody();
void player04bEndEventCamera(u32, u32);
u32 player04bGetEventProcessId(u32, u32);
void player04bGetSeaColor(GXColor*, GXColor*);
void player04bSetRestartRoom(u32, cXyz*, s32, u32);
s32 player04bCreateChildActor(s32, u32, u32, cXyz*, s32, csXyz*, u32, s32, u32);
void player04bOrderNamedEvent(daPy_lk_c*, const char*, u32, u32);
void player04bOrderChangeEvent(daPy_lk_c*, u32, u32, u32, u32);
void player04bCreateEnvironmentActor(s32, u32, cXyz*, u32, u32);

void PSMTXIdentity(Mtx34*);

namespace player04b {
// HD: only the player's 0x98-byte JAIZelAnime prefix is present; do not copy the larger shared type.
struct AnimeSoundPrefix { u8 _00[0x94]; be<u32> mVtable; };
WWHD_OFFSET(AnimeSoundPrefix, mVtable, 0x94);
WWHD_SIZE(AnimeSoundPrefix, 0x98);
struct CutTurnCallback { be<u32> __vtbl; u8 _04[0xC]; };
WWHD_OFFSET(CutTurnCallback, __vtbl, 0);
WWHD_SIZE(CutTurnCallback, 0x10);
inline dBgS_LinChk* mirrorLineCheck(daPy_lk_c* p) { return gabi::at<dBgS_LinChk>(gabi::ea(mirrorPacket(p))+0x33BC); }
// These shared TU statics remain image-backed; subsequent Link ranges use the same storage.
inline cXyz& l_debug_keep_pos() { return gabi::object_at<cXyz>(0x1046CD48); }
inline csXyz& l_debug_shape_angle() { return gabi::object_at<csXyz>(0x1046CD10); }
inline csXyz& l_debug_current_angle() { return gabi::object_at<csXyz>(0x1046CD08); }
}

f32 player04bAddCalc(be<f32>*, f32, f32, f32, f32);
u32 player04bGetTrianglePlane(dBgS*, u32, u32);
s32 player04bGetSpecialCode(dBgS*, u32);
BOOL player04bAttentionLockonTruth(u32);
void player04bInitOldFrameMorf(u32, f32, u32, u32);
// HD: private helper bindings retain GHS word arguments, including zero-extended polygon indices.

// The second part uses the same guest objects without introducing additional TU data owners.
#include "f_op/f_op_camera.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"
namespace player04b {
// HD: the old-frame object prefix is isolated from animation backend tail variations.
struct OldFrame {
    u8 _00[0xC];
    be<f32> mOldFrameRate;
};
WWHD_OFFSET(OldFrame, mOldFrameRate, 0xC);
WWHD_SIZE(OldFrame, 0x10);
// These canonical TWW vectors remain backed by the original shared guest data.
inline const cXyz& l_crawl_start_front_offset() { return gabi::object_at<cXyz>(0x101CEBC0); }
inline const cXyz& l_crawl_top_offset() { return gabi::object_at<cXyz>(0x101CEBE4); }
// HD: the regional body-angle probe updates this mutable static's Z component.
inline cXyz& top_vec() { return gabi::object_at<cXyz>(0x101CEECC); }
}
BOOL player04bCalculateSubjectAngle(u32, be<s16>*, be<s16>*);
// HD: retain the complete GHS r3 result for the signed-angle equality comparison.
u32 player04bCheckBodyAngleRegister(daPy_lk_c*, s32);
f32 player04bSquareDistance(cXyz*, cXyz*);

namespace player04b {
// Canonical TWW names; resource indices retain the verified HD values for this range.
enum MovementConstants : u32 {
    ANM_DAMB = 0x58,
    ANM_DAMF = 0x57,
    ANM_DAML = 0x55,
    ANM_DAMR = 0x56,
    ANM_LADDERDWST = 0x87,
    ANM_LADDERLTOR = 0x8B,
    ANM_LADDERRTOL = 0x8A,
    ANM_LADDERUPEDL = 0x86,
    ANM_LADDERUPEDR = 0x85,
    ANM_LADDERUPST = 0x84,
    ANM_LIE = 0x40,
    ANM_VJMP = 0x4C,
    ANM_VJMPCHA = 0x4D,
    ANM_VJMPCHB = 0x4E,
    ANM_WAITATOB = 0x1D,
    CL_JNT_BODY_CHN_e = 2,
    DIR_BACKWARD = 1,
    DIR_FORWARD = 0,
    DIR_LEFT = 2,
    DIR_NONE = 4,
    DIR_RIGHT = 3,
    JA_SE_SUBJ_VIEW_OUT = 0x8FB,
    UPPER_MOVE2_e = 2,
    dRes_INDEX_LKANM_BCK_BOOMWAIT_e = 0x35,
    dRes_INDEX_LKANM_BCK_WAITB_e = 0x11F,
    dRes_INDEX_LKANM_BCK_WAITS_e = 0x126,
    daPyItem_NONE_e = 0x100,
    daPyProc_CLIMB_MOVE_UP_DOWN_e = 0x3F,
    daPyProc_CLIMB_UP_START_e = 0x3D,
    daPyProc_CRAWL_START_e = 0xF,
    daPyProc_FREE_WAIT_e = 5,
    daPyProc_HANG_START_e = 0x2B,
    daPyProc_HANG_WALL_CATCH_e = 0x31,
    daPyProc_ICE_SLIP_ALMOST_FALL_e = 0xA0,
    daPyProc_ICE_SLIP_FALL_e = 0x9E,
    daPyProc_LADDER_DOWN_START_e = 0x3A,
    daPyProc_LADDER_MOVE_e = 0x3C,
    daPyProc_LADDER_UP_END_e = 0x39,
    daPyProc_LADDER_UP_START_e = 0x38,
    daPyProc_MOVE_TURN_e = 0x18,
    daPyProc_MOVE_e = 6,
    daPyProc_ROPE_UP_HANG_e = 0x7F,
    daPyProc_ROPE_UP_e = 0x7A,
    daPyProc_VERTICAL_JUMP_e = 0x2A,
    daPyStts0_CRAWL_e = 0x8000000,
    daPyStts0_HANG_e = 0x100,
    daPyStts0_SHIP_RIDE_e = 0x10000,
    daPyStts0_UNK2000000_e = 0x2000000,
    daPyStts1_UNK10000_e = 0x10000,
};
}

// HD: getReadyItem is compared as a complete unsigned GHS return register.
u32 player04bGetReadyItemRegister(daPy_lk_c*);

namespace player04b {
enum AdditionalMovementConstants : u32 {
    ANM_FREEA = 0xE1,
    ANM_FREEB = 0xE2,
    ANM_FREED = 0xE3,
    ANM_DAMFL = 0x59,
    ANM_DAMFR = 0x5A,
    ANM_DAMFF = 0x5B,
    ModeFlg_00000001 = 1,
    ModeFlg_MIDAIR = 2,
    ModeFlg_SWIM = 0x40000,
};
}

namespace player04b {
enum MovementStateConstants : u32 {
    ANM_DAMFB = 0x5C,
    dCamAttnStts_00000010_e = 0x10,
    ModeFlg_00000100 = 0x100,
    ModeFlg_00000400 = 0x400,
};
}
