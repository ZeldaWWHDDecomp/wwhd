/* daGoal_Flag_c (finish-line flag), WWHD layout. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"

// HD: the shader packet replaces the GameCube GX display-list packet and gains native buffers.
// The actor no longer stores the GameCube minigame terminator process ID.

// HD: native shader buffers replace the GameCube display-list packet.
struct GoalTextureVtable {
    u8 _00[0x0C];
    gfn destroy;
};
WWHD_OFFSET(GoalTextureVtable, destroy, 0x0C);
struct GoalTexture {
    u8 mNativeTexture[0xF0];
    gptr<GoalTextureVtable> mVtable;
    void destroy(s32 flags) { gabi::call_ptr(mVtable->destroy.get(), this, flags); }
};
WWHD_SIZE(GoalTexture, 0xF4);
WWHD_OFFSET(GoalTexture, mVtable, 0xF0);
struct GoalVertex {
    union {
        struct {
            be<f32> mPosition[3], mNormal[3], mTexCoord[2];
        };
        be<f32> mComponents[8];
    };
};
struct GoalCacheLine {
    be<u32> mWords[8];
};
WWHD_SIZE(GoalVertex, 0x20);
struct GoalNativeVertexBuffer {
    u8 _00[0x14C];
    be<u32> mFlushSize;
    u8 _150[4];
};
WWHD_SIZE(GoalNativeVertexBuffer, 0x154);
struct GoalVertexBuffer {
    gptr<GoalVertex> mpData;
    GoalNativeVertexBuffer mNativeVertexBuffer;
    GoalTexture mTexture;
    be<u32> mVertexCount;
    gptr<void> mpAllocation;
};
WWHD_SIZE(GoalVertexBuffer, 0x254);
WWHD_OFFSET(GoalVertexBuffer, mNativeVertexBuffer, 4);
WWHD_OFFSET(GoalVertexBuffer, mTexture, 0x158);
WWHD_OFFSET(GoalVertexBuffer, mVertexCount, 0x24C);
WWHD_OFFSET(GoalVertexBuffer, mpAllocation, 0x250);
struct GoalVertexBuffers {
    GoalVertexBuffer mBuffers[2][2];
    be<u32> mCurrentBuffer, mStride, mAlignment;
    gptr<void> mpVertexFormat;
    be<u32> mFlags;
    be<u8> mInitialized;
    u8 _965[3];
};
WWHD_SIZE(GoalVertexBuffers, 0x968);
WWHD_OFFSET(GoalVertexBuffers, mCurrentBuffer, 0x950);
WWHD_OFFSET(GoalVertexBuffers, mStride, 0x954);
WWHD_OFFSET(GoalVertexBuffers, mAlignment, 0x958);
WWHD_OFFSET(GoalVertexBuffers, mFlags, 0x960);
WWHD_OFFSET(GoalVertexBuffers, mInitialized, 0x964);
struct GoalMaterial;
struct GoalTexturePair {
    union {
        be<u32> mMaterialId;
        gptr<GoalMaterial> mpMaterial;
    };
    be<s32> mFrontCount;
    gptr<GoalTexture> mpFrontTextures;
    be<s32> mBackCount;
    gptr<GoalTexture> mpBackTextures;
};
WWHD_SIZE(GoalTexturePair, 0x14);
WWHD_OFFSET(GoalTexturePair, mpFrontTextures, 8);
WWHD_OFFSET(GoalTexturePair, mpBackTextures, 0x10);
struct GoalTextureTable {
    be<s32> mCount;
    gptr<GoalTexturePair> mpPairs;
};
WWHD_SIZE(GoalTextureTable, 8);
inline GoalVertexBuffer* goalFlag_vertexBuffer(void* p) { return gabi::at<GoalVertexBuffer>(gabi::ea(p)); }
inline GoalVertexBuffers* goalFlag_vertexBuffers(void* p) { return gabi::at<GoalVertexBuffers>(gabi::ea(p)); }
inline GoalTexturePair* goalFlag_texturePair(void* p) { return gabi::at<GoalTexturePair>(gabi::ea(p)); }
inline GoalTextureTable* goalFlag_textureTable(void* p) { return gabi::at<GoalTextureTable>(gabi::ea(p)); }
inline GoalTexture* goalFlag_texture(void* p) { return gabi::at<GoalTexture>(gabi::ea(p)); }
// HD: shader-facing texture resources retain descriptor words separately from native bindings.
struct GoalTextureDescription {
    be<u32> mNativeHandle, mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat, mTileMode;
    u8 _20[8];
    gptr<void> mpImage;
    u8 _2C[4];
    gptr<void> mpMipImage;
    be<u32> mPitch, mAlignment;
};
WWHD_SIZE(GoalTextureDescription, 0x3C);
struct GoalTextureResource {
    GoalTextureDescription mDescription;
    u8 _3C[0x54];
};
WWHD_SIZE(GoalTextureResource, 0x90);
struct GoalTextureObject {
    GoalTextureDescription mDescription;
    u8 _3C[0x98];
    gptr<void> mpNativeImage;
    u8 _D8[4];
    gptr<void> mpNativeMipImage;
    u8 _E0[0x7C];
    be<u32> mImageDirty, mMipDirty, mSamplerDirty;
    u8 _168[0x28];
    be<u8> mFlags;
    u8 _191[7];
};
WWHD_SIZE(GoalTextureObject, 0x198);
WWHD_OFFSET(GoalTextureObject, mpNativeImage, 0xD4);
WWHD_OFFSET(GoalTextureObject, mImageDirty, 0x15C);
WWHD_OFFSET(GoalTextureObject, mFlags, 0x190);
struct GoalIndexBuffer {
    be<u32> mType, mMode;
    gptr<u8> mpIndices;
    be<u32> mCount, mStride;
    u8 _14[4];
};
WWHD_SIZE(GoalIndexBuffer, 0x18);
struct GoalUniformBuffer {
    u8 _00[4];
    gptr<void> mpData;
    u8 _08[4];
    be<u32> mSize;
    u8 _10[0xC];
};
WWHD_SIZE(GoalUniformBuffer, 0x1C);
struct GoalUniformVtable {
    u8 _00[0x2C];
    gfn upload;
};
struct GoalUniformBlock {
    u8 _00[0xC];
    gptr<GoalUniformVtable> mVtable;
    GoalUniformBuffer mBuffers[2];
    u8 _48[4];
    be<u32> mCurrentBuffer;
    u8 _50[0x24];
    void upload(void* material) { gabi::call_ptr(mVtable->upload.get(), this, material); }
};
WWHD_SIZE(GoalUniformBlock, 0x74);
WWHD_OFFSET(GoalUniformBlock, mCurrentBuffer, 0x4C);
struct GoalMaterialOwner {
    be<u32> mCount;
    gptr<void> mpMaterials;
    be<u32> mCapacity;
};
WWHD_SIZE(GoalMaterialOwner, 0xC);
struct GoalMaterialUniforms {
    union {
        struct {
            be<f32> mDefaults[16];
            be<f32> mMaterialColor[4], mAmbientColor[4], mEffectColor[4], mRemainingDefaults[16];
        };
        be<f32> mInitialValues[44];
    };
    be<u8> mBufferEntries[3][2][0x10];
    be<f32> mMatrices[8][12];
    be<f32> mVectors[6][4];
};
WWHD_SIZE(GoalMaterialUniforms, 0x2F0);
inline GoalTextureDescription* goalFlag_textureDescription(void* p) {
    return gabi::at<GoalTextureDescription>(gabi::ea(p));
}
inline GoalTextureObject* goalFlag_textureObject(void* p) { return gabi::at<GoalTextureObject>(gabi::ea(p)); }
inline GoalIndexBuffer* goalFlag_indexBuffer(void* p) { return gabi::at<GoalIndexBuffer>(gabi::ea(p)); }
inline GoalUniformBlock* goalFlag_uniformBlock(void* p) { return gabi::at<GoalUniformBlock>(gabi::ea(p)); }
// HD: native shader, draw-pass and heap interfaces used by the packet.
struct GoalUniformDescriptor {
    u8 _00[0xC];
    be<s16> mVertexSlot, mPixelSlot, mGeometrySlot;
};
struct GoalMaterialTexture {
    u8 _00[4];
    u8 mBinding[0x10];
};
WWHD_SIZE(GoalMaterialTexture, 0x14);
struct GoalMaterial {
    gptr<void> mpShader;
    u8 _04[8];
    be<u32> mUniformCount;
    gptr<GoalUniformDescriptor> mpUniforms;
    be<u32> mTextureCount;
    gptr<GoalMaterialTexture> mpTextures;
};
WWHD_OFFSET(GoalMaterial, mpUniforms, 0x10);
struct GoalDrawPass {
    u8 _00[4];
    gptr<void> mpUniformBlock;
};
struct GoalDrawInfo {
    u8 _00[0xC];
    be<s32> mMaterialIndex;
    u8 _10[4];
    gptr<GoalDrawPass> mpPass;
    u8 _18[0x18];
    gptr<GoalUniformBlock> mpExtraUniforms;
};
WWHD_OFFSET(GoalDrawInfo, mpExtraUniforms, 0x30);
struct GoalShaderCache {
    gptr<void> mpNativeShader, mpShader;
};
struct GoalShaderNative {
    u8 _00[0x28];
    gptr<void> mpShader;
};
struct GoalShader {
    be<u8> mFlags;
    u8 _01[3];
    gptr<void> mpFetchData;
    u8 _08[4];
    gptr<void> mpFetchShader;
    u8 _10[0x6C];
    gptr<GoalShaderNative> mpNative;
};
WWHD_OFFSET(GoalShader, mpNative, 0x7C);
struct GoalHeapVtable {
    u8 _00[0x34];
    gfn allocate;
    u8 _38[4];
    gfn free;
};
struct GoalHeap {
    u8 _00[0xC];
    gptr<GoalHeapVtable> mVtable;
    void* allocate(u32 size, u32 alignment) {
        return gabi::call_ptr<void*>(mVtable->allocate.get(), this, size, alignment);
    }
    void free(void* memory) { gabi::call_ptr(mVtable->free.get(), this, memory); }
};
struct GoalRenderState {
    u8 _00[8];
    be<u32> mCullMode, mMode;
    u8 _10[0xD0];
    be<u8> mAlphaEnabled;
    u8 _E1[3];
    be<u32> mAlphaFunction;
    be<f32> mAlphaThreshold;
    be<u32> mFlags;
    u8 _F0[0x28];
    be<u32> mVtable;
};
WWHD_SIZE(GoalRenderState, 0x11C);
WWHD_OFFSET(GoalRenderState, mFlags, 0xEC);
inline GoalMaterial* goalFlag_material(void* p) { return gabi::at<GoalMaterial>(gabi::ea(p)); }
inline GoalDrawInfo* goalFlag_drawInfo(void* p) { return gabi::at<GoalDrawInfo>(gabi::ea(p)); }
inline GoalShader* goalFlag_shader(void* p) { return gabi::at<GoalShader>(gabi::ea(p)); }
inline GoalShaderCache* goalFlag_cache(void* p) { return gabi::at<GoalShaderCache>(gabi::ea(p)); }
inline GoalHeap* goalFlag_heap(void* p) { return gabi::at<GoalHeap>(gabi::ea(p)); }
struct GoalStarter {
    u8 _00[0x18];
    u8 mSequence[0x38];
    be<u8> mReset, mStatus;
};
struct GoalTerminator {
    u8 _00[0x18];
    u8 mSequence[0x4C];
    be<u8> mReset;
};
struct GoalFailure {
    u8 _00[0x60];
    be<u8> mShow;
};
struct GoalMinigameScreens {
    u8 _00[0x1FC];
    gptr<GoalFailure> mpFailure;
    gptr<GoalTerminator> mpTerminator;
    gptr<GoalStarter> mpStarter;
};
WWHD_OFFSET(GoalStarter, mStatus, 0x51);
WWHD_OFFSET(GoalTerminator, mReset, 0x64);
WWHD_OFFSET(GoalMinigameScreens, mpStarter, 0x204);
inline dComIfG_inf_c* goalFlag_gameInfo(u32 address) { return gabi::at<dComIfG_inf_c>(address); }
inline GoalStarter* goalFlag_starter(void* p) { return gabi::at<GoalStarter>(gabi::ea(p)); }
inline GoalTerminator* goalFlag_terminator(void* p) { return gabi::at<GoalTerminator>(gabi::ea(p)); }
inline GoalFailure* goalFlag_failure(void* p) { return gabi::at<GoalFailure>(gabi::ea(p)); }
inline GoalMinigameScreens* goalFlag_screens(u32 address) { return gabi::at<GoalMinigameScreens>(address); }
struct GoalVectorWords {
    be<u32> mComponents[3];
};
inline GoalVectorWords* goalFlag_vectorWords(void* p) { return gabi::at<GoalVectorWords>(gabi::ea(p)); }
struct GoalResource {
    u8 _00[0x20];
    gptr<void> mpTexture;
};
struct GoalShaderEntry {
    u8 _00[0x20];
    be<u8> mInitialized;
    u8 _21[3];
};
WWHD_SIZE(GoalShaderEntry, 0x24);
struct GoalShaderProgram {
    u8 mNativeProgram[0x84];
};
struct GoalShaderRoot {
    u8 _00[0x1C];
    be<u32> mCount;
    gptr<GoalShaderProgram> mpShaders;
};
struct GoalShaderArchive {
    u8 _00[4];
    gptr<GoalShaderRoot> mpRoot;
    be<u32> mCount;
    gptr<GoalShaderEntry> mpEntries;
};
struct GoalTevColors {
    u8 _00[0x24];
    be<f32> mAmbientScale, mMaterialScale;
    u8 _2C[0x64];
    be<s16> mMaterialColor[4];
    be<u8> mAmbientColor[4], mEffectColor[4];
    u8 _A0[0xC0];
    be<s16> mSecondLightColor[4];
    u8 _168[4];
    be<f32> mSecondLightScale;
};
WWHD_OFFSET(GoalTevColors, mMaterialColor, 0x90);
WWHD_OFFSET(GoalTevColors, mSecondLightColor, 0x160);
struct GoalShaderMaterial {
    GoalUniformBlock mUniforms;
    u8 _74[0x150];
    be<u32> mLightColors[2][4];
    u8 _1E4[0x58];
};
WWHD_SIZE(GoalShaderMaterial, 0x23C);
inline GoalTevColors* goalFlag_tevColors(void* p) { return gabi::at<GoalTevColors>(gabi::ea(p)); }
inline GoalResource* goalFlag_resource(void* p) { return gabi::at<GoalResource>(gabi::ea(p)); }
inline GoalShaderArchive* goalFlag_archive(void* p) { return gabi::at<GoalShaderArchive>(gabi::ea(p)); }
struct GoalSafeStringVtable {
    u8 _00[0x14];
    gfn assureTermination;
};
struct GoalCameraView {
    u8 _00[0x240];
    be<f32> mProjection[16];
};
struct GoalJ3DSys {
    u8 _00[0x38];
    Mtx34 mViewMtx;
    u8 _68[0xE0];
    gptr<GoalCameraView> mpView;
    be<f32> mProjection[16];
};
WWHD_OFFSET(GoalJ3DSys, mpView, 0x148);
struct GoalGameAux {
    u8 _00[0x5D2C];
    be<u8> mMiniGameReset;
};
WWHD_OFFSET(GoalGameAux, mMiniGameReset, 0x5D2C);
struct daGFlag_packet_c {
    void setBackNrm();
    void setNrmVtx(cXyz* output, s32 column, s32 row);
    void setTexObj(u32 archive);
    void update();
    void draw(void* drawInfo);
    void init();

    u8 pad0[0xC];
    be<u32> mVtable;
    u8 pad10[0x88];
    be<s16> mNormalFlutterPhase;
    be<s16> mFlagWavePhase;
    be<u32> mMaterialCount;
    be<u32> mTexturePairCount;
    gptr<void> mpTexturePairs;
    GoalVertexBuffers mVertexBuffers;
    be<u32> mShaderMaterialCount;
    gptr<void> mpShaderMaterials;
    be<u32> mShaderMaterialCapacity;
    GoalUniformBlock mMatrixBlock;
    be<f32> mShaderMatrix[12];
    u8 _AC0[4];
    GoalUniformBlock mMaterialBlock;
    GoalMaterialUniforms mMaterialUniforms;
    be<u8> mFlagLoaded;
    u8 _E29[3];
    GoalIndexBuffer mIndices;
    GoalTextureResource mFlagResource, mToonResource;
    GoalTextureObject mFlagTexture, mToonTexture;
    be<u8> mToonLoaded;
    u8 _1295[0x83];
    be<f32> mMtx[12];
    gptr<void> mpTevStr;
    cXyz mPos[2][45];
    cXyz mDPos[2][45];
    cXyz mNrm[2][45];
    cXyz mBackNrm[2][45];
    cXyz mVelocity[45];
    be<u8> mCurrArr;
    u8 pad2649[3];
};
WWHD_OFFSET(daGFlag_packet_c, mMaterialCount, 0x9C);
WWHD_OFFSET(daGFlag_packet_c, mShaderMaterialCount, 0xA10);
WWHD_OFFSET(daGFlag_packet_c, mpTevStr, 0x1348);
WWHD_OFFSET(daGFlag_packet_c, mPos, 0x134C);
WWHD_OFFSET(daGFlag_packet_c, mDPos, 0x1784);
WWHD_OFFSET(daGFlag_packet_c, mNrm, 0x1BBC);
WWHD_OFFSET(daGFlag_packet_c, mBackNrm, 0x1FF4);
WWHD_OFFSET(daGFlag_packet_c, mMtx, 0x1318);
WWHD_OFFSET(daGFlag_packet_c, mVelocity, 0x242C);
WWHD_OFFSET(daGFlag_packet_c, mCurrArr, 0x2648);
WWHD_SIZE(daGFlag_packet_c, 0x264C);
WWHD_OFFSET(daGFlag_packet_c, mMatrixBlock, 0xA1C);
WWHD_OFFSET(daGFlag_packet_c, mShaderMatrix, 0xA90);
WWHD_OFFSET(daGFlag_packet_c, mMaterialBlock, 0xAC4);
WWHD_OFFSET(daGFlag_packet_c, mMaterialUniforms, 0xB38);
WWHD_OFFSET(daGFlag_packet_c, mIndices, 0xE2C);
WWHD_OFFSET(daGFlag_packet_c, mFlagResource, 0xE44);
WWHD_OFFSET(daGFlag_packet_c, mFlagTexture, 0xF64);
WWHD_OFFSET(daGFlag_packet_c, mToonLoaded, 0x1294);
inline daGFlag_packet_c* goalFlag_packet(void* p) { return gabi::at<daGFlag_packet_c>(gabi::ea(p)); }
// Velocity precedes the current-array selector (0x2648).
// HD: the retained rope renderer places its material vtable before the position table.
struct GoalRopeVtable {
    u8 _00[0x14];
    gfn getMaterialType;
};
struct GoalRopeLine {
    u8 _00[0x130];
    gptr<GoalRopeVtable> mVtable;
    u8 _134[0x10];
    gptr<gptr<cXyz>> mpPositions;
    s32 getMaterialType() { return gabi::call_ptr<s32>(mVtable->getMaterialType.get(), this); }
};
WWHD_OFFSET(GoalRopeLine, mVtable, 0x130);
WWHD_OFFSET(GoalRopeLine, mpPositions, 0x144);
WWHD_SIZE(GoalRopeLine, 0x148);
inline GoalRopeLine* goalFlag_ropeLine(void* p) { return gabi::at<GoalRopeLine>(gabi::ea(p)); }
struct daGoal_Flag_c : fopAc_ac_c {
    BOOL CreateHeap();
    BOOL getRacePath(u8 pathId);
    BOOL _execute();
    BOOL CreateBuoyRaces();
    s32 goal_check();
    BOOL RaceStart();
    BOOL RaceEnd();
    BOOL _delete();
    void get_cloth_anim_factor(cXyz* output, cXyz* mPos, cXyz* mNrm, cXyz* wind, s32 column, s32 row);
    void RopeMove();
    BOOL TimerExecute();
    void flag_move();
    BOOL _draw();
    s32 _create();
    s32 getDemoAction(s32 staff);

    daGFlag_packet_c mFlagPacket;
    request_of_phase_process_class mClothPhs;
    request_of_phase_process_class mFlagPhs;
    be<f32> mMtx[12];
    cXyz mGoalFlagPolePos[2];
    be<s16> mWindScalePhase;
    u8 pad2A52[2];
    be<s32> mTimerProcID;
    be<s32> mMgameStartProcID;
    be<f32> mPrevPlayerLineSide;
    be<s16> mRaceEndState;
    be<s16> mCamFramesPassed;
    be<u8> mRaceStartState;
    u8 pad2A65[3];
    gptr<void> mpRopePaths[4];
    be<s32> mNumRopeBuoys[4];
    be<s32> mNumRopes;
    GoalRopeLine mRopeLines[4];
    using ProcFunc = BOOL (daGoal_Flag_c::*)();
    union {
        struct {
            be<s16> mActionAdjustment;
            be<s16> mActionDispatch;
            be<u32> mActionTarget;
        };
        gabi::ptmf<ProcFunc> mCurrProc;
    };
    be<u8> mTimerEnded;
    u8 pad2FB5[3];
};
WWHD_OFFSET(daGoal_Flag_c, mFlagPacket, 0x3AC);
WWHD_OFFSET(daGoal_Flag_c, mGoalFlagPolePos, 0x2A38);
WWHD_OFFSET(daGoal_Flag_c, mPrevPlayerLineSide, 0x2A5C);
WWHD_OFFSET(daGoal_Flag_c, mpRopePaths, 0x2A68);
WWHD_OFFSET(daGoal_Flag_c, mNumRopeBuoys, 0x2A78);
WWHD_OFFSET(daGoal_Flag_c, mNumRopes, 0x2A88);
WWHD_OFFSET(daGoal_Flag_c, mRopeLines, 0x2A8C);
WWHD_OFFSET(daGoal_Flag_c, mActionTarget, 0x2FB0);
WWHD_SIZE(daGoal_Flag_c, 0x2FB8);

// HD: typed bindings for the shader packet and retained full-width game calls.
using GoalArrayCtor = void* (*)(void*);
using GoalArrayDtor = void (*)(void*, s32);
void* goalFlag_ropeLineConstruct(void*);
void goalFlag_ropeLineDestroy(void*, s32);
s32 goalFlag_lineInit(void*, s32, u16, s32);
void goalFlag_vertexBufferConstruct(void*);
void goalFlag_textureConstruct(void*);
void goalFlag_deleteObject(void*);
void goalFlag_textureDestroy(void*, s32);
void goalFlag_vertexBufferDestroy(void*, s32);
u32 goalFlag_playObject();
s32 goalFlag_eventActionIndex(void*, s32, void*, s32, s32, s32);
s32 goalFlag_createChild(s32, u32, s32, void*, s8, s32, s32, s32, s32);
void goalFlag_vectorSubtract(void*, void*, void*);
void goalFlag_vectorNormalizeZP(void*, void*);
f32 goalFlag_vectorSquareMagnitude(void*);
f32 goalFlag_squareRoot(f32);
void goalFlag_vectorScale(void*, void*, f32);
void goalFlag_vectorAdd(void*, void*, void*);
void goalFlag_vectorSubtractPS(void*, void*, void*);
f32 goalFlag_vectorDot(void*, void*);
void goalFlag_eventCutEnd(void*, s32);
s32 goalFlag_eventStaffId(void*, const char*, s32, s32);
void goalFlag_startSequence(void*, void*);
void goalFlag_timerStart(void*, s32);
void goalFlag_bgmStart(u32);
s32 goalFlag_eventEndCheck(void*, const char*);
s32 goalFlag_timerDeleteCheck(void*);
void goalFlag_messageDelete(void*);
s32 goalFlag_triggerA(s32);
s32 goalFlag_triggerB(s32);
s32 goalFlag_triggerStart(s32);
void goalFlag_nextStage(const char*, s32, s32, s32, s32, s32, s32, f32);
void goalFlag_resourceDelete(void*, const char*);
void goalFlag_resetFailureSequence(void*);
void goalFlag_vectorMultiply(void*, void*, f32);
void goalFlag_assertionFail(const char*, s32, const char*);
void goalFlag_vectorSum(void*, void*, void*);
void goalFlag_timerEnd(void*, s32);
void goalFlag_soundStart(s32);
s32 goalFlag_timerRemaining(void*);
void goalFlag_orderOtherEvent(void*, void*, s32, s32);
void goalFlag_timerHide(void*);
void goalFlag_finishSequence(void*, s32);
void goalFlag_vectorCross(void*, void*, void*);
void goalFlag_vectorNormalizeZC(void*, void*);
void goalFlag_matrixPush();
void goalFlag_matrixYRotate(void*, s16);
void goalFlag_matrixXRotate(void*, s16);
void goalFlag_matrixPosition(void*, void*);
void goalFlag_matrixPull();
void goalFlag_matrixYSet(void*, s16);
void goalFlag_vectorNormalize(void*, void*);
void goalFlag_cacheStoreRange(void*, s32);
void goalFlag_registerGlobalObject(void*);
void goalFlag_staticByteConstruct(void*);
void goalFlag_staticByteFlagConstruct(void*);
void goalFlag_settingTevStruct(void*, s32, void*, void*);
void goalFlag_matrixTranslate(s32, f32, f32, f32);
void goalFlag_matrixScale(s32, f32, f32, f32);
void goalFlag_matrixIdentity(void*);
void goalFlag_matrixConcat(void*, void*, void*);
void goalFlag_drawBufferEntry(void*, void*, s32);
void goalFlag_lineUpdate(void*, u16, void*, s32, void*, f32);
void goalFlag_linePacketSetMaterial(void*, void*);
void goalFlag_graphicsLock(void*);
void goalFlag_textureResourceLoad(void*, void*);
void goalFlag_graphicsUnlock(void*);
void goalFlag_textureAssign(void*, void*);
void goalFlag_textureRelease(void*);
void goalFlag_materialOwnerDestroy(void*, s32);
void goalFlag_uniformBufferRelease(void*);
void goalFlag_textureObjectDestroy(void*, s32);
void goalFlag_indexBufferDestroy(void*, s32);
void goalFlag_uniformBlockDestroy(void*, s32);
void goalFlag_arrayDestroy(void*, s32, s32, GoalArrayDtor, s32, s32);
void goalFlag_packetBaseDestroy(void*, s32);
void goalFlag_actorBaseDestroy(void*, s32);
void goalFlag_packetBaseConstruct(void*);
void goalFlag_arrayConstruct(void*, s32, s32, GoalArrayCtor);
void goalFlag_materialOwnerConstruct(void*);
void goalFlag_uniformBlockConstruct(void*);
void goalFlag_memoryClear(void*, s32);
void goalFlag_indexBufferConstruct(void*);
void goalFlag_textureResourceConstruct(void*);
void goalFlag_textureObjectConstruct(void*);
void goalFlag_packetInitialize(void*);
s32 goalFlag_shaderIndex(void*, SafeString*);
void goalFlag_shaderResourceInitialize(void*, void*, s32);
void goalFlag_materialTableInitialize(void*, void*, s32);
void goalFlag_vertexBufferInitialize(void*, void*, s32, void*);
void goalFlag_texturePairInitialize(u32, void*, void*, void*, s32);
void goalFlag_shaderMaterialInitialize(void*, s32, s32);
void goalFlag_indexBufferInitialize(void*, const void*, s32, s32);
void goalFlag_vertexBufferFlush(void*, s32, u32);
void goalFlag_actorBaseConstruct(void*);
s32 goalFlag_resourceLoad(void*, const char*);
s32 goalFlag_entrySolidHeap(void*, void*, s32);
void goalFlag_matrixSetTranslate(void*, f32, f32, f32);
void goalFlag_matrixStackScale(f32, f32, f32);
void goalFlag_matrixCopy(void*, void*);
u32 goalFlag_eventRegister(void*, s32);
s32 goalFlag_timerCreate(s32, s32, u16, s32, s32, s32, f32, f32, f32, f32);
void goalFlag_setFog(void*);
void goalFlag_setLights(void*);
void goalFlag_materialMatrices(void*, s32, void*, void*, void*);
void goalFlag_colorMultiply(void*, void*, f32);
void goalFlag_materialFlush(void*, s32);
void goalFlag_colorScale(void*, f32);
void goalFlag_uniformBlockFlush(void*);
void goalFlag_pixelUniformBlock(s16, u32, void*);
void goalFlag_vertexUniformBlock(s16, u32, void*);
void goalFlag_geometryUniformBlock(s16, u32, void*);
void goalFlag_textureBind(void*, void*, s32, s32);

// HD: typed bindings for the shader packet and retained full-width game calls.
using GoalArrayCtor = void* (*)(void*);
using GoalArrayDtor = void (*)(void*, s32);
void* goalFlag_ropeLineConstruct(void*);
void goalFlag_ropeLineDestroy(void*, s32);
void* goalFlag_roomPath(u8, s8);
void* goalFlag_allocateObject(s32);
void* goalFlag_messageById(s32);
void* goalFlag_windVector();
void* goalFlag_environmentLight();
void* goalFlag_objectResource(void*, SafeString*, s32);
void* goalFlag_findAllocationHeap(void*, void*);
void* goalFlag_currentHeap(void*);
void* goalFlag_shaderArchive(void*);

void* goalFlag_shaderCache(void*);
void goalFlag_shaderRefresh(void*, s32);
void goalFlag_nativeShaderBind(void*);
void goalFlag_fetchShaderBind(void*, void*);
void goalFlag_shaderBind(void*);
void goalFlag_drawInfoPrepare(void*, void*);
void goalFlag_renderStateConstruct(void*);
void goalFlag_renderStateConfigure(s32, void*);
void goalFlag_renderStateApply(void*);
void goalFlag_textureApply(void*);
void goalFlag_drawIndexed(u32, s32, u32, void*, s32, s32);

struct daGFlag_HIO_c {
    be<u32> __vtbl;
    be<s8> m04;
    be<u8> mDrawCelShaded;
    be<s8> m06, m07, m08, m09;
    be<u8> mDebugFlag;
    u8 _0B;
    be<u32> mWindScalePhaseInc, mNormalFlutterPhaseInc, mFlagWavePhaseInc, mTimeLimit;
    be<s32> mPerfectRupeeScore, mEndCamEarlyFrame;
    be<u32> mRopeColorR, mRopeColorG, mRopeColorB;
    be<f32> mWindScale1, mWindScale2, mFlagSagFactor, mClothStiffness;
};
WWHD_SIZE(daGFlag_HIO_c, 0x40);
WWHD_OFFSET(daGFlag_HIO_c, mWindScalePhaseInc, 0x0C);
WWHD_OFFSET(daGFlag_HIO_c, mClothStiffness, 0x3C);
struct GoalPathPoint {
    u8 mArguments[4];
    cXyz m_position;
};
struct GoalPath {
    be<u16> m_num;
    u8 _02;
    be<u8> m_nextID;
    u8 mArguments[4];
    gptr<GoalPathPoint> m_points;
};
WWHD_OFFSET(GoalPathPoint, m_position, 4);
WWHD_SIZE(GoalPathPoint, 0x10);
WWHD_OFFSET(GoalPath, m_nextID, 3);
WWHD_OFFSET(GoalPath, m_points, 8);
WWHD_SIZE(GoalPath, 0xC);
inline GoalPath* goalFlag_path(void* p) { return gabi::at<GoalPath>(gabi::ea(p)); }
inline GoalPathPoint* goalFlag_pathPoints(void* p) { return goalFlag_path(p)->m_points; }

// HD: constructor subobjects may cross the 32-bit guest address boundary.
template <class T> inline T* goalFlag_guestAddress(T* p) { return gabi::at<T>(gabi::ea(p)); }

struct GoalDrawList {
    u8 _00[0x284];
    u8 mLineMaterials[2][0x9C];
};
WWHD_OFFSET(GoalDrawList, mLineMaterials, 0x284);

inline GoalVertex* goalFlag_vertex(u32 address) { return gabi::at<GoalVertex>(address); }

// HD: the save singleton points to a handle prefix before the GameCube save object.
struct GoalSaveRoot {
    u8 mHandle[0x20];
    dSv_info_c mSave;
};
WWHD_OFFSET(GoalSaveRoot, mSave, 0x20);

// Process IDs used by this actor, retained by HD.
enum GoalFlagProcess : s32 { fpcNm_Obj_Buoyrace_e = 0x10C, fpcNm_TIMER_e = 0x1E0 };

// HD interface views: extents describe the fields this packet accesses, not engine ownership.
WWHD_SIZE(GoalTextureVtable, 0x10);
WWHD_SIZE(GoalUniformVtable, 0x30);
WWHD_OFFSET(GoalUniformBuffer, mpData, 4);
WWHD_OFFSET(GoalUniformBuffer, mSize, 0xC);
WWHD_OFFSET(GoalTextureDescription, mpImage, 0x28);
WWHD_OFFSET(GoalTextureDescription, mpMipImage, 0x30);
WWHD_OFFSET(GoalTextureDescription, mPitch, 0x34);
WWHD_OFFSET(GoalTextureDescription, mAlignment, 0x38);
WWHD_SIZE(GoalUniformDescriptor, 0x12);
WWHD_OFFSET(GoalUniformDescriptor, mVertexSlot, 0xC);
WWHD_OFFSET(GoalUniformDescriptor, mPixelSlot, 0xE);
WWHD_OFFSET(GoalUniformDescriptor, mGeometrySlot, 0x10);
WWHD_SIZE(GoalMaterial, 0x1C);
WWHD_OFFSET(GoalMaterial, mUniformCount, 0xC);
WWHD_OFFSET(GoalMaterial, mTextureCount, 0x14);
WWHD_OFFSET(GoalMaterial, mpTextures, 0x18);
WWHD_SIZE(GoalDrawInfo, 0x34);
WWHD_OFFSET(GoalDrawInfo, mMaterialIndex, 0xC);
WWHD_OFFSET(GoalDrawInfo, mpPass, 0x14);
WWHD_SIZE(GoalShaderCache, 8);
WWHD_SIZE(GoalShaderNative, 0x2C);
WWHD_SIZE(GoalShader, 0x80);
WWHD_OFFSET(GoalShader, mpFetchData, 4);
WWHD_OFFSET(GoalShader, mpFetchShader, 0xC);
WWHD_SIZE(GoalHeapVtable, 0x40);
WWHD_SIZE(GoalHeap, 0x10);
WWHD_OFFSET(GoalHeapVtable, allocate, 0x34);
WWHD_OFFSET(GoalHeapVtable, free, 0x3C);
WWHD_OFFSET(GoalRenderState, mCullMode, 8);
WWHD_OFFSET(GoalRenderState, mAlphaEnabled, 0xE0);
WWHD_OFFSET(GoalRenderState, mAlphaFunction, 0xE4);
WWHD_OFFSET(GoalRenderState, mAlphaThreshold, 0xE8);
WWHD_OFFSET(GoalRenderState, mVtable, 0x118);
WWHD_SIZE(GoalResource, 0x24);
WWHD_SIZE(GoalShaderProgram, 0x84);
WWHD_SIZE(GoalShaderRoot, 0x24);
WWHD_SIZE(GoalShaderArchive, 0x10);
WWHD_OFFSET(GoalShaderArchive, mpRoot, 4);
WWHD_OFFSET(GoalShaderArchive, mCount, 8);
WWHD_OFFSET(GoalShaderArchive, mpEntries, 0xC);
WWHD_SIZE(GoalTevColors, 0x170);
WWHD_OFFSET(GoalTevColors, mAmbientScale, 0x24);
WWHD_OFFSET(GoalTevColors, mMaterialScale, 0x28);
WWHD_OFFSET(GoalTevColors, mAmbientColor, 0x98);
WWHD_OFFSET(GoalTevColors, mEffectColor, 0x9C);
WWHD_OFFSET(GoalTevColors, mSecondLightScale, 0x16C);
WWHD_OFFSET(GoalMaterialUniforms, mMaterialColor, 0x40);
WWHD_OFFSET(GoalMaterialUniforms, mAmbientColor, 0x50);
WWHD_OFFSET(GoalMaterialUniforms, mEffectColor, 0x60);
WWHD_OFFSET(GoalMaterialUniforms, mBufferEntries, 0xB0);
WWHD_OFFSET(GoalMaterialUniforms, mMatrices, 0x110);
WWHD_OFFSET(GoalMaterialUniforms, mVectors, 0x290);
WWHD_OFFSET(daGoal_Flag_c, mCurrProc, 0x2FAC);
