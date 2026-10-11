/* d_a_boomerang.h (WWHD) - WWHD layout of daBoomerang_c. */
#pragma once
struct boomerangQuad_c;
struct boomerangShaderMaterial_c;
#include "JSystem/J3DGraph.h"
#include "d/actor/d_a_player_main.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "d/d_com_inf_game.h"
#include "d/d_drawlist.h"
#include "f_op/f_op_actor.h"

// HD: Cafe render objects replace the GameCube GX immediate draw packets.
struct boomerangUniformVtable_c {
    u8 _prefix[0x2C];
    gfn mBind;
};
struct boomerangUniformBlock_c {
    be<u32> mWords[7];
};
struct boomerangRenderEntry_c {
    u8 _prefix[0x10];
    boomerangUniformBlock_c mUniforms[2];
    u8 _tail[0x23C - 0x48];
};
struct boomerangMatrixUniform_c {
    u8 _prefix[12];
    gptr<boomerangUniformVtable_c> mpVtable;
    boomerangUniformBlock_c mUniforms[2];
    u8 _state[0x2C];
    Mtx34 mMatrix;
    be<u32> mFlags;
};
struct boomerangVector4_c {
    union {
        be<f32> mValues[4];
        be<u32> mWords[4];
    };
};
struct boomerangLightingUniform_c {
    u8 _prefix[12];
    gptr<boomerangUniformVtable_c> mpVtable;
    boomerangUniformBlock_c mUniforms[2];
    u8 _state[0x2C];
    boomerangVector4_c mColors[11];
    boomerangVector4_c mVectors[3][2];
    Mtx34 mMtxArr[8];
    boomerangVector4_c mTailVectors[6];
    be<u8> mEnabled;
    u8 _padding[3];
};
struct boomerangRenderObject_c {
    be<s32> mCount;
    gptr<boomerangRenderEntry_c> mpEntries;
    be<u32> mFlags;
    boomerangMatrixUniform_c mTransform;
    boomerangLightingUniform_c mLighting;
};
WWHD_SIZE(boomerangMatrixUniform_c, 0xA8);
WWHD_OFFSET(boomerangMatrixUniform_c, mMatrix, 0x74);
WWHD_SIZE(boomerangLightingUniform_c, 0x368);
WWHD_SIZE(boomerangRenderObject_c, 0x41C);
WWHD_OFFSET(boomerangRenderObject_c, mLighting, 0xB4);
struct boomerangTextureSurface_c {
    union {
        struct {
            be<u32> mFlags;
            be<u32> mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat, mAntialias;
            be<u32> mAllocation[2];
            be<u32> mImage, mMipmapSize, mMipmap, mTileMode, mSwizzle;
            u8 _state[0x90 - 0x3C];
        };
        be<u32> mWords[0x90 / 4];
    };
};
struct boomerangSampler_c {
    boomerangTextureSurface_c mSurface;
    u8 _state[0x44];
    be<u32> mCachedImage;
    u8 _cacheGap[4];
    be<u32> mCachedMipmap;
    u8 _filterGap[0x15C - 0xE0];
    be<u32> mMinFilter, mMagFilter, mMipFilter;
    u8 _flagsGap[0x190 - 0x168];
    be<u8> mFlags;
    u8 _tail[7];
};
struct boomerangIndexBuffer_c {
    gptr<void> mpIndices;
    be<u32> mCount, mOffset, mPrimitive;
    u8 _state[8];
};
struct boomerangVertexBuffer_c {
    u8 _state[0x148];
    be<u32> mBufferSize;
    be<u32> mUploadSource;
    u8 _tail[4];
};
WWHD_SIZE(boomerangTextureSurface_c, 0x90);
WWHD_SIZE(boomerangSampler_c, 0x198);
WWHD_SIZE(boomerangIndexBuffer_c, 0x18);
WWHD_SIZE(boomerangVertexBuffer_c, 0x154);

struct boomerangRenderPoint_c {
    gptr<boomerangQuad_c> mVertices;
    boomerangVertexBuffer_c mVertexBuffer;
    u8 mDeclaration[0xF4];
    be<u32> mAllocationBytes;
    gptr<void> mAllocation;
};
static_assert(sizeof(boomerangRenderPoint_c) == 0x254);

struct boomerangBlurRing_c {
    boomerangRenderPoint_c mPoints[60][2];
    be<u32> mWriteBuffer;
    be<u32> mDeclarationWords[3];
    gptr<void> mPrevious;
    be<u8> mInitialized;
    u8 _pad[3];
};
static_assert(sizeof(boomerangBlurRing_c) == 0x11778);
WWHD_OFFSET(boomerangBlurRing_c, mWriteBuffer, 0x11760);

struct daBoomerang_sightPacket_c {
    void setup();
    void draw();
    void setSight(cXyz *position, s32 index);
    void play(s32 count);
    gptr<void> mVtable;
    Mtx34 mMtxArr[5];
    be<u8> mFrameArr[5];
    be<u8> mScaleArr[5];
    u8 _padFE[2];
    be<u32> mSightOnFlg;
    gptr<void> mTex, mImage, mResource;
    boomerangRenderPoint_c mPoints[2];
    be<u32> mWriteBuffer;
    be<u32> mVertexDeclaration[3];
    gptr<void> mPreviousResource;
    be<u8> mInitialized;
    u8 _indexGap[3];
    boomerangIndexBuffer_c mIndexBuffer;
    be<u16> mIndices[4];
    boomerangRenderObject_c mRenderObjects[5];
    boomerangTextureSurface_c mTextureSurface;
    boomerangSampler_c mSampler;
};
static_assert(sizeof(daBoomerang_sightPacket_c) == 0x1CA4);
WWHD_OFFSET(daBoomerang_sightPacket_c, mFrameArr, 0xF4);

struct daBoomerang_blur_c {
    void setup();
    void draw();
    void copyBlur(void *matrix, s16 angle);
    void initBlur(void *matrix, s16 angle);
    u8 _packetPrefix[12];
    be<u32> mVtable;
    u8 _packetBase[0x98 - 0x10];
    be<s32> numTrailSegments;
    cXyz pos;
    union {
        cXyz mTrail[4][60];
        struct {
            cXyz trail0_vtxArr0[60], trail0_vtxArr1[60], trail1_vtxArr0[60], trail1_vtxArr1[60];
        };
    };
    gptr<void> mResource;
    boomerangBlurRing_c mRings[2];
    boomerangRenderObject_c mRenderObject;
    boomerangIndexBuffer_c mIndexBuffer;
    be<u16> mIndices[4];
    boomerangSampler_c mSampler;
    boomerangTextureSurface_c mTextureSurface;
};
static_assert(sizeof(daBoomerang_blur_c) == 0x24140);
WWHD_OFFSET(daBoomerang_blur_c, mTrail, 0xA8);
WWHD_OFFSET(daBoomerang_blur_c, mRings, 0xBEC);

struct daBoomerang_c : fopAc_ac_c {
    enum Mode { Mode_Wait = 0, Mode_Throw = 1 };
    void resetLockActor();
    BOOL setLockActor(fopAc_ac_c *target, s32 sound);
    void setAimActor(fopAc_ac_c *target);
    void setRoomInfo();
    BOOL execute();
    void rockLineCallback(fopAc_ac_c *target);
    void setKeepMatrix();
    void setAimPos();
    f32 getFlyMax();
    BOOL createHeap();
    void checkBgHit(cXyz *from, cXyz *to);
    BOOL draw();
    BOOL procWait();
    BOOL procMove();
    BOOL create();

    u8 _toModel[0x3AC - sizeof(fopAc_ac_c)];
    gptr<J3DModel> mpModel;
    daBoomerang_sightPacket_c mSightPacket;
    daBoomerang_blur_c mBlur;
    u8 mKeep[0xB0];
    be<u32> mLockActorIDs[5];
    gptr<fopAc_ac_c> mLockActors[5];
    be<u8> mIsReturning, unused_0xF2D, mJustHit, mCatchAndDelete, unused_0xF30;
    be<u8> mLockCnt, mCurLockIdx, mThirdPerson, mCancelFlg;
    /* 0x26275 */ be<u8> mInWater;
    /* 0x26276 */ be<u8> mFreeFlyOut;
    u8 _padFlight;
    /* 0x26278 */ be<s16> mModelRotZ;
    /* 0x2627A */ be<s16> mModelRotY;
    /* 0x2627C */ be<f32> mGroundY;
    /* 0x26280 */ cXyz mTargetPos;
    /* 0x2628C */ dCcD_Stts mStts;
    /* 0x262C8 */ dCcD_Cps mCps;
    /* 0x26400 */ dBgS_LinChk mLinChk;
    /* 0x2646C */ dBgS_ObjGndChk mGndChk;
    // HD: GHS member pointers use two halfwords and a target word.
    /* 0x264C0 */ be<s16> mProcAdjust;
    /* 0x264C2 */ be<s16> mProcSelector;
    /* 0x264C4 */ union {
        be<u32> mProcTarget;
        struct {
            be<u16> mProcFunctionHigh;
            be<s16> mProcVtableOffset;
        };
    };
};
WWHD_OFFSET(daBoomerang_c, mpModel, 0x3AC);
WWHD_OFFSET(daBoomerang_c, mSightPacket, 0x3B0);
WWHD_OFFSET(daBoomerang_c, mBlur, 0x2054);
WWHD_OFFSET(daBoomerang_c, mLockActorIDs, 0x26244);
WWHD_OFFSET(daBoomerang_c, mLockActors, 0x26258);
WWHD_OFFSET(daBoomerang_c, mLockCnt, 0x26271);
static_assert(sizeof(daBoomerang_c) == 0x264C8);

WWHD_OFFSET(daBoomerang_c, mTargetPos, 0x26280);
WWHD_OFFSET(daBoomerang_c, mStts, 0x2628C);
WWHD_OFFSET(daBoomerang_c, mCps, 0x262C8);
WWHD_OFFSET(daBoomerang_c, mLinChk, 0x26400);
WWHD_OFFSET(daBoomerang_c, mGndChk, 0x2646C);
WWHD_OFFSET(daBoomerang_c, mProcTarget, 0x264C4);

// HD: private native renderer and verified register-width bindings.
void boomerang_startLockSound(u32 a0);
void boomerang_deallocate(void *a0);
void *boomerang_allocate(s32 a0);
void boomerang_constructVertexBuffer(void *a0);
void boomerang_constructVertexDeclaration(void *a0);
f32 boomerang_groundCross(void *a0, void *a1);
u8 boomerang_getRoomId(void *a0, void *a1);
u8 boomerang_getPolyColor(void *a0, void *a1);
u32 boomerang_judgeActor(s32 a0, gabi::be<unsigned int> *a1);
void boomerang_destroyUniformTransform(void *a0, s32 a1);
void boomerang_destroyRenderObject(void *a0, s32 a1);
void boomerang_destroyVertexDeclaration(void *a0, s32 a1);
void boomerang_destroyVertexBuffer(void *a0, s32 a1);
void boomerang_copyMatrix(void *a0, void *a1);
void boomerang_translateMatrix(f32 a0, f32 a1, f32 a2);
void boomerang_rotateXYZ(void *a0, s16 a1, s16 a2, s16 a3);
u32 boomerang_getArchiveResource(void *a0, void *a1, s32 a2);
void boomerang_showAssert(void *a0, s32 a1, void *a2);
u32 boomerang_createModel(void *a0, s32 a1, s32 a2);
void boomerang_setKeepActor(void *a0, s32 a1);
void boomerang_setLineCheck(void *a0, cXyz *a1, cXyz *a2, daBoomerang_c *a3);
u32 boomerang_lineCross(void *a0, void *a1);
void boomerang_setParticle(void *a0, s32 a1, s32 a2, cXyz *a3, s32 a4, s32 a5, s32 a6, s32 a7,
                           s32 a8, s32 a9, s32 a10, s32 a11);
u32 boomerang_getMaterialSound(void *a0, void *a1);
s32 boomerang_getReverbRegister(s32 a0);
void boomerang_startSound(s32 a0, cXyz *a1, u32 a2, s32 a3);
void boomerang_multiplyMatrixVector(void *a0, void *a1, void *a2);
void boomerang_rotateY(void *a0, s16 a1);
void boomerang_getViewport(void *a0);
void boomerang_projectPosition(cXyz *a0, cXyz *a1, void *a2);
void boomerang_translateMatrixInto(void *a0, f32 a1, f32 a2, f32 a3);
void boomerang_scaleMatrix(f32 a0, f32 a1, f32 a2);
void boomerang_rotateZ(void *a0, s16 a1);
void boomerang_entrySight(void *a0, void *a1, void *a2, void *a3);
void boomerang_entryBlur(void *a0, void *a1, void *a2, cXyz *a3);
u32 boomerang_getEnvironmentLight();
void boomerang_settingTevStruct(void *a0, s32 a1, cXyz *a2, void *a3);
void boomerang_setLightTevColorType(void *a0, void *a1, void *a2);
void boomerang_updateIceModel(void *a0, s32 a1, s32 a2);
void boomerang_modelUpdateDL(void *a0, s32 a1);
void boomerang_updateKeepMatrix(void *a0, void *a1, void *a2);
void boomerang_registerGlobalObject(void *a0);
void boomerang_initializeHeaderByte(void *a0);
void boomerang_initializeHeaderFlag(void *a0);
void boomerang_resetAttackHit(void *a0);
void boomerang_subtractVector(cXyz *a0, cXyz *a1, cXyz *a2);
f32 boomerang_squareMagnitude(cXyz *a0);
f32 boomerang_squareRoot(f32 a0);
s16 boomerang_atan2s(f32 a0, f32 a1);
void boomerang_moveFlight(daBoomerang_c *a0);
u32 boomerang_getCameraBody();
void boomerang_setCapsuleGeometry(cXyz *a0, cXyz *a1, cXyz *a2, f32 a3);
void boomerang_subtractVectorInto(cXyz *a0, cXyz *a1, cXyz *a2);
void boomerang_setCollision(void *a0, void *a1);
void boomerang_pushMatrix();
void boomerang_scaleVector(cXyz *a0, cXyz *a1, f32 a2);
void boomerang_addVector(cXyz *a0, cXyz *a1, cXyz *a2);
void boomerang_popMatrix();
void boomerang_constructRenderObject(void *a0);
void boomerang_constructUniformTransform(void *a0);
void boomerang_clearMemory(void *a0, s32 a1);
void boomerang_allocate_noResult(s32 a0);
void boomerang_constructArray(void *a0, s32 a1, s32 a2, s32 a3);
void boomerang_deleteActor(daBoomerang_c *a0);
void boomerang_startSound_s32_cXyzPtr_s32_s32(s32 a0, cXyz *a1, s32 a2, s32 a3);
void boomerang_divideVector(cXyz *a0, cXyz *a1, f32 a2);
u32 boomerang_returnBoomerang(void *a0);
u32 boomerang_checkAttackHit(void *a0);
u32 boomerang_getHitActor(void *a0);
void boomerang_addCalcAngleS(gabi::be<short> *a0, s16 a1, s32 a2, s32 a3, s32 a4);
void boomerang_rotateZXY(void *a0, s16 a1, s16 a2, s16 a3);
void boomerang_setMassCollision(void *a0, void *a1, s32 a2);
u8 boomerang_setItemWaterEffect(daBoomerang_c *a0, u8 a1, s32 a2);
u32 boomerang_allocate_u32(u32 a0);
void boomerang_constructArray_voidPtr_u32_s32_u32(void *a0, u32 a1, s32 a2, u32 a3);
void boomerang_constructActor(daBoomerang_c *a0);
void boomerang_constructDrawList(void *a0);
void boomerang_constructIndexBuffer(void *a0);
void boomerang_constructTexture(void *a0);
void boomerang_constructSampler(void *a0);
void boomerang_constructPacket(void *a0);
void boomerang_constructKeepActor(void *a0);
void boomerang_constructCollisionStatus(void *a0);
void boomerang_constructGlobalCollisionStatus(void *a0);
void boomerang_constructCollisionObject(void *a0);
void boomerang_constructCapsuleGeometry(void *a0);
void boomerang_constructLineCheck(void *a0);
void boomerang_constructGroundCheck(void *a0);
void boomerang_setupSight(void *a0);
void boomerang_setupBlur(void *a0);
u32 boomerang_entrySolidHeap(daBoomerang_c *a0, s32 a1, s32 a2);
void boomerang_initCollisionStatus(void *a0, s32 a1, s32 a2, daBoomerang_c *a3);
void boomerang_setCollisionCapsule(void *a0, void *a1);
u32 boomerang_getMaterialManager();
s32 boomerang_findResourceIndex(void *a0, void *a1);
void boomerang_initializeMaterialInstance(void *a0, void *a1, s32 a2);
u32 boomerang_getCurrentHeap(void *a0);
void boomerang_setupVertexBuffer(void *a0, void *a1, s32 a2, void *a3);
void boomerang_setupVertexDeclaration(void *a0, void *a1, void *a2, void *a3, s32 a4);
void boomerang_initializeRenderObject(void *a0, s32 a1, s32 a2);
void boomerang_setupIndexBuffer(void *a0, void *a1, s32 a2, s32 a3);
void boomerang_uploadVertexBuffer(void *a0, s32 a1, void *a2);
void boomerang_beginGraphics(void *a0);
void boomerang_initializeTexture(void *a0, void *a1);
void boomerang_endGraphics(void *a0);
void boomerang_copySampler(void *a0, void *a1);
u32 boomerang_getShaderCache(void *a0);
void boomerang_updateMaterial(void *a0, s32 a1);
void boomerang_bindShader(void *a0);
void boomerang_callDisplayList(void *a0, u32 a1);
void boomerang_applyMaterial(void *a0);
void boomerang_getGraphicsState(void *a0);
void boomerang_setGraphicsState(void *a0);
void boomerang_identityMatrix(void *a0);
u32 boomerang_getCameraProjection(void *a0);
void boomerang_copyProjection(void *a0, void *a1);
void boomerang_setRenderMatrices(void *a0, s32 a1, void *a2, void *a3, void *a4);
void boomerang_selectRenderPass(void *a0, s32 a1);
void boomerang_setPixelUniform(s16 a0, void *a1, u32 a2);
void boomerang_setVertexUniform(s16 a0, void *a1, u32 a2);
void boomerang_setGeometryUniform(s16 a0, void *a1, u32 a2);
void boomerang_bindSampler(void *a0, void *a1, s32 a2, s32 a3);
void boomerang_updateUniformTransform(void *a0);
void boomerang_bindVertexDeclaration(void *a0);
void boomerang_drawIndexed(u32 a0, u32 a1, void *a2, u32 a3, s32 a4, s32 a5);
void boomerang_releaseVertexDeclaration(void *a0);
u32 boomerang_findAllocationHeap(void *a0, void *a1);
void boomerang_releaseUniform(void *a0);
void boomerang_destroySampler(void *a0, s32 a1);
void boomerang_destroyIndexBuffer(void *a0, s32 a1);
void boomerang_destroyArray(void *a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
void boomerang_destroyPacket(void *a0, s32 a1);
void boomerang_destroyBackgroundCheck(void *a0, s32 a1);
void boomerang_destroyPolyInfo(void *a0, s32 a1);
void boomerang_destroyCollisionObject(void *a0, s32 a1);
void boomerang_destroyCollisionStatus(void *a0, s32 a1);
void boomerang_destroyKeepActor(void *a0, s32 a1);
void boomerang_destroyDrawList(void *a0, s32 a1);
void boomerang_destroyActor(daBoomerang_c *a0, s32 a1);
void boomerang_deallocate_daBoomerang_cPtr(daBoomerang_c *a0);
void boomerang_uploadVertexBuffer_voidPtr_s32_u32(void *a0, s32 a1, u32 a2);

WWHD_OFFSET(daBoomerang_sightPacket_c, mRenderObjects, 0x5F0);
WWHD_OFFSET(daBoomerang_sightPacket_c, mSampler, 0x1B0C);
WWHD_OFFSET(daBoomerang_blur_c, mRenderObject, 0x23ADC);
WWHD_OFFSET(daBoomerang_blur_c, mSampler, 0x23F18);

struct boomerangSightBuffers_c {
    boomerangRenderPoint_c mPoints[2];
    be<u32> mWriteBuffer;
    be<u32> mDeclaration[3];
    gptr<void> mPreviousResource;
    be<u8> mInitialized;
    u8 _gap[3];
    boomerangIndexBuffer_c mIndexBuffer;
};
WWHD_OFFSET(boomerangSightBuffers_c, mWriteBuffer, 0x4A8);

// HD: draw-list storage follows the shared game-info object.
struct boomerangGameInfo_c {
    dSv_info_c save;
    dComIfG_play_c play;
    dDlstHD::dDlst_list_c mDrawList;
};
WWHD_OFFSET(boomerangGameInfo_c, mDrawList, 0x5D30);
struct boomerangPlayerSight_c {
    be<u32> mVtable;
    be<u8> mDraw;
    u8 _padding[3];
    cXyz mLineTop;
};
struct boomerangCameraBody_c {
    u8 _prefix[0x13C];
    be<u32> mMode;
};
struct boomerangCamera_c {
    u8 _prefix[0xDC];
    cXyz mCenter;
};
struct boomerangImage_c {
    be<u16> mFormat, mWidth, mHeight;
    u8 _prefix[0x20 - 6];
    gptr<void> mpSurface;
};
WWHD_OFFSET(boomerangPlayerSight_c, mLineTop, 8);

struct boomerangShaderOwner_c {
    u8 _prefix[0x28];
    gptr<void> mpShader;
};
struct boomerangShaderProgram_c {
    be<u8> mFlags;
    u8 _padding[3];
    gptr<void> mpDisplayList;
    u8 _gap[4];
    be<u32> mDisplayListSize;
    u8 _state[0x6C];
    gptr<boomerangShaderOwner_c> mpOwner;
};
struct boomerangUniformSlots_c {
    u8 _prefix[12];
    be<s16> mVertex, mPixel, mGeometry;
};
struct boomerangShaderMaterial_c {
    gptr<boomerangShaderProgram_c> mpProgram;
    u8 _prefix[8];
    be<u32> mUniformCount;
    gptr<boomerangUniformSlots_c> mpUniforms;
    be<u32> mSamplerCount;
    gptr<void> mpSamplers;
    u8 _state[4];
    be<u8> mReady;
    u8 _padding[3];
};
struct boomerangShaderArchive_c {
    u8 _prefix[0x1C];
    be<u32> mCount;
    gptr<void> mpEntries;
};
struct boomerangMaterialManager_c {
    u8 _prefix[4];
    gptr<boomerangShaderArchive_c> mpArchive;
    be<u32> mCount;
    gptr<boomerangShaderMaterial_c> mpMaterials;
};
struct boomerangShaderCache_c {
    gptr<void> mpShader;
    gptr<boomerangShaderProgram_c> mpProgram;
};
struct boomerangUniformSelection_c {
    be<u32> mFlags, mSize, mState;
    gptr<void> mpData;
    u8 _tail[12];
};
struct boomerangUniformOutput_c {
    u8 _prefix[16];
    boomerangUniformSelection_c mSelections[2];
    u8 _gap[4];
    be<u32> mSelectionIndex;
};
struct boomerangGraphicsState_c {
    be<u8> mColorEnabled, mAlphaEnabled;
    u8 _prefix[6];
    be<u32> mCullMode, mBlendMode;
    u8 _state[0xD0];
    be<u8> mDepthEnabled;
    u8 _padding[3];
    be<u32> mDepthFunction;
    be<f32> mDepthValue;
    be<u32> mStencilBits;
    u8 _tail[0x2C];
};
struct boomerangSafeStringVtable_c {
    u8 _prefix[0x14];
    gfn mAssureTermination;
};
struct boomerangHeapVtable_c {
    u8 _prefix[0x34];
    gfn mAllocate;
    u8 _gap[4];
    gfn mFree;
};
struct boomerangHeap_c {
    u8 _prefix[12];
    gptr<boomerangHeapVtable_c> mpVtable;
};
struct boomerangVertex_c {
    union {
        cXyz pos;
        be<u32> mPositionWords[3];
    };
    be<f32> u, v;
};
struct boomerangQuad_c {
    union {
        boomerangVertex_c mVertices[4];
        be<f32> mValues[20];
    };
};
struct boomerangTransformVtable_c {
    u8 _prefix[0x14];
    gfn mGetKeepMatrix;
};
struct boomerangVisibilityVtable_c {
    u8 _prefix[0xCC];
    gfn mCheckHide;
};
WWHD_SIZE(boomerangShaderMaterial_c, 0x24);
WWHD_OFFSET(boomerangUniformOutput_c, mSelectionIndex, 0x4C);
WWHD_SIZE(boomerangGraphicsState_c, 0x11C);
WWHD_SIZE(boomerangVertex_c, 0x14);
WWHD_SIZE(boomerangQuad_c, 0x50);

struct boomerangVectorPair_c {
    cXyz mPoints[2];
};
struct boomerangTrailArrayTail_c {
    be<u32> mWriteBuffer, mDeclarationType, mDeclarationStride, mDeclarationFlags;
    gptr<void> mPrevious;
    be<u8> mInitialized;
    u8 _padding[3];
};
struct boomerangCapsuleAttribute_c {
    u8 _prefix[0x18];
    be<u32> mObjectVtable, mShapeVtable;
};
struct boomerangCapsuleGeometry_c {
    cXyz mStart, mEnd;
    be<u32> mVtable;
    be<f32> mRadius;
};
WWHD_SIZE(boomerangTrailArrayTail_c, 0x18);
WWHD_SIZE(boomerangCapsuleAttribute_c, 0x20);
WWHD_SIZE(boomerangCapsuleGeometry_c, 0x20);

struct boomerangVirtualMethod_c {
    be<u32> mAdjustment, mFunction;
};
WWHD_SIZE(boomerangVirtualMethod_c, 8);

// HD: compiler-generated callbacks used by the native resource arrays.
BOOL daBoomerang_IsDelete(daBoomerang_c *actor);
BOOL daBoomerang_Delete(daBoomerang_c *actor);
void boomerangEmptyDestructor(void *object, s32 flags);
void boomerangStringAssureTermination(void *string);
void *boomerangTrailPointCtor(void *object);
void *boomerangSmallCtor(void *object);
BOOL daBoomerang_Execute(daBoomerang_c *actor);
void boomerangRockLineThunk(daBoomerang_c *actor, void *ignored, fopAc_ac_c *target);
void *boomerangBlurPointCtor(void *object);
void boomerangRenderObjectDtor(void *object, s32 flags);
void boomerangTrailPointDtor(void *object, s32 flags);
void boomerangBlurPointDtor(void *object, s32 flags);
BOOL daBoomerang_CreateHeap(daBoomerang_c *actor);
BOOL daBoomerang_Draw(daBoomerang_c *actor);
void boomerangColorToFloat(void *destination, void *color);
void boomerangCopyMatrix(void *destination, void *source);
void __sinit_d_a_boomerang_cpp();
void *boomerangRenderObjectCtor(void *object);
BOOL daBoomerang_Create(daBoomerang_c *actor);
void boomerangBlurDestructor(void *object, s32 flags);
void boomerangDestructor(daBoomerang_c *actor, s32 flags);

// HD: checked sizes of the private rendering and ABI projections.
WWHD_SIZE(boomerangUniformVtable_c, 0x30);
WWHD_SIZE(boomerangUniformBlock_c, 0x1C);
WWHD_SIZE(boomerangRenderEntry_c, 0x23C);
WWHD_SIZE(boomerangVector4_c, 0x10);
WWHD_SIZE(boomerangRenderPoint_c, 0x254);
WWHD_SIZE(boomerangBlurRing_c, 0x11778);
WWHD_SIZE(daBoomerang_sightPacket_c, 0x1CA4);
WWHD_SIZE(daBoomerang_blur_c, 0x24140);
WWHD_SIZE(daBoomerang_c, 0x264C8);
WWHD_SIZE(boomerangSightBuffers_c, 0x4D8);
WWHD_SIZE(boomerangGameInfo_c, 0x62F0);
WWHD_SIZE(boomerangPlayerSight_c, 0x14);
WWHD_SIZE(boomerangCameraBody_c, 0x140);
WWHD_SIZE(boomerangCamera_c, 0xE8);
WWHD_SIZE(boomerangImage_c, 0x24);
WWHD_SIZE(boomerangShaderOwner_c, 0x2C);
WWHD_SIZE(boomerangShaderProgram_c, 0x80);
WWHD_SIZE(boomerangUniformSlots_c, 0x12);
WWHD_SIZE(boomerangShaderArchive_c, 0x24);
WWHD_SIZE(boomerangMaterialManager_c, 0x10);
WWHD_SIZE(boomerangShaderCache_c, 0x8);
WWHD_SIZE(boomerangUniformSelection_c, 0x1C);
WWHD_SIZE(boomerangUniformOutput_c, 0x50);
WWHD_SIZE(boomerangSafeStringVtable_c, 0x18);
WWHD_SIZE(boomerangHeapVtable_c, 0x40);
WWHD_SIZE(boomerangHeap_c, 0x10);
WWHD_SIZE(boomerangTransformVtable_c, 0x18);
WWHD_SIZE(boomerangVisibilityVtable_c, 0xD0);
WWHD_SIZE(boomerangVectorPair_c, 0x18);
