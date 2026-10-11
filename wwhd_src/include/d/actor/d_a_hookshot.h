#ifndef D_A_HOOKSHOT_H
#define D_A_HOOKSHOT_H

// Hookshot. HD replaces the GC packet with a GPU renderer and shifts later fields by 0xD25C.
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

struct daHookshot_c : fopAc_ac_c {
    typedef BOOL (daHookshot_c::*ProcFunc)();
    enum Mode { Mode_Wait, Mode_Shot, Mode_Return, Mode_Pull };

    BOOL draw();
    BOOL procWait_init(BOOL);
    BOOL procWait();
    BOOL procShot();
    BOOL procPlayerPull();
    BOOL procReturn();
    BOOL execute();
    BOOL hookshot_delete();
    cPhs_State create();
    void setProc(u32 fn) {
        mCurrProcFunc.d = 0;
        mCurrProcFunc.i = -1;
        mCurrProcFunc.f = fn;
    }
    u32 sightCps() { return gabi::ea(this) + 0xD630; }

    /* 0x03AC */ u8 mShape[0xD0A0]; /* HD daHookshot_shape (user area +0x18) */
    /* 0xD44C */ u8 mRenderObject[0xB0];  /* HD object (constructor 02080404) */
    /* 0xD4FC */ be<u8> m2A0;
    /* 0xD4FD */ be<u8> m2A1;
    /* 0xD4FE */ be<u8> mShipRideFlg;
    /* 0xD4FF */ be<u8> m2A3;
    /* 0xD500 */ be<s16> m2A4;
    /* 0xD502 */ be<u16> m2A6;
    /* 0xD504 */ be<s32> mChainCnt;
    /* 0xD508 */ be<u32> mMtrlSndId;
    /* 0xD50C */ be<s32> mObjHookFlg;
    /* 0xD510 */ csXyz mHookAngle;
    /* 0xD516 */ csXyz m2BA;
    /* 0xD51C */ cXyz mMoveVec;
    /* 0xD528 */ cXyz mObjSightCrossPos;
    /* 0xD534 */ u8 mLinChk[0x6C]; /* dBgS_RopeLinChk (poly info +0x14, cross +0x30, flags +0x4C) */
    /* 0xD5A0 */ u8 mGndChk[0x54]; /* dBgS_ObjGndChk */
    /* 0xD5F4 */ dCcD_Stts mStts;
    /* 0xD630 */ u8 mSightCps[0x138]; /* dCcD_Cps */
    /* 0xD768 */ cXyz mCarryOffset;
    /* 0xD774 */ be<u32> mCarryActorID;
    /* 0xD778 */ Mtx34 mMtx;
    /* 0xD7A8 */ gabi::ptmf<ProcFunc> mCurrProcFunc;
};
WWHD_OFFSET(daHookshot_c, mRenderObject, 0xD44C);
WWHD_OFFSET(daHookshot_c, m2A0, 0xD4FC);
WWHD_OFFSET(daHookshot_c, mChainCnt, 0xD504);
WWHD_OFFSET(daHookshot_c, mHookAngle, 0xD510);
WWHD_OFFSET(daHookshot_c, mLinChk, 0xD534);
WWHD_OFFSET(daHookshot_c, mStts, 0xD5F4);
WWHD_OFFSET(daHookshot_c, mSightCps, 0xD630);
WWHD_OFFSET(daHookshot_c, mCarryOffset, 0xD768);
WWHD_OFFSET(daHookshot_c, mMtx, 0xD778);
WWHD_OFFSET(daHookshot_c, mCurrProcFunc, 0xD7A8);
WWHD_SIZE(daHookshot_c, 0xD7B0);

// HD: private views retain the access widths of the verified inline code.
struct HookshotPlayerView {
    u8 mActor[0x3D0];
    be<u16> mBodyAngleX;
    be<s16> mBodyAngleY;
    u8 mBeforeRoot[0x72F0 - 0x3D4];
    cXyz mHookshotRootPos;
};
WWHD_OFFSET(HookshotPlayerView, mHookshotRootPos, 0x72F0);
WWHD_SIZE(HookshotPlayerView, 0x72FC);

struct HookshotCpsView : dCcD_GObjInf {
    u8 mBounds[0x18];
    be<u32> mBoundsVtable;
    be<u32> mShapeVtable;
    cXyz mStart;
    cXyz mEnd;
    be<u32> mCpsVtable;
    be<f32> mRadius;
};
WWHD_OFFSET(HookshotCpsView, mStart, 0x118);
WWHD_OFFSET(HookshotCpsView, mCpsVtable, 0x130);
WWHD_SIZE(HookshotCpsView, 0x138);

struct HookshotWordVector {
    be<u32> x, y, z;
};
WWHD_SIZE(HookshotWordVector, 12);
struct HookshotFloatMatrix {
    be<f32> m[12];
};
WWHD_SIZE(HookshotFloatMatrix, 48);
struct HookshotFloatColor {
    be<f32> r, g, b, a;
};
WWHD_SIZE(HookshotFloatColor, 16);
struct HookshotSignedColor {
    be<s16> r, g, b, a;
};
WWHD_SIZE(HookshotSignedColor, 8);

template <class T> inline T* hookshotView(u32 address) {
    // HD: preserve wrapped guest addresses, including accesses at zero plus a member offset.
    return reinterpret_cast<T*>(PPC_MEM_BASE + address);
}
inline HookshotCpsView* hookshotCps(u32 address) { return hookshotView<HookshotCpsView>(address); }
inline dBgS_LinChk* hookshotLine(void* line) { return hookshotView<dBgS_LinChk>(gabi::ea(line)); }
inline dBgS_ObjGndChk* hookshotGround(void* ground) {
    return hookshotView<dBgS_ObjGndChk>(gabi::ea(ground));
}

// HD renderer and SDK calls used only by this translation unit.
void hs_prepareDraw();
void hs_setupMaterial(u32 material, s32 index, Mtx34* view, u32 context, u32 matrices);
void hs_scaleColor(void* output, void* input, f32 scale);
void hs_flushMaterial(u32 material, s32 index);
void hs_scaleAmbient(u32 color, f32 scale);
void hs_flushLink(u32 link);
void hs_entryImm(u32 list, void* shape, s32 flags);
void hs_setupPlayerRender(fopAc_ac_c* player, u32 object, void* shape);
u8 hs_setItemWaterEffect(daHookshot_c* actor, u8 state, s32 flags);
void hs_shapeConstruct(void* shape);
void hs_objectConstruct(void* object);
void hs_collisionConstruct(u32 collision);
void hs_capsuleConstruct(u32 capsule);
void hs_capsuleSet(u32 capsule, u32 source);
void hs_setupGanonRender(void* object, s32 flags);
void hs_resetAtHit(u32 collision);
BOOL hs_checkAtHit(u32 collision);
fopAc_ac_c* hs_getHitActor(u32 hit);
void hs_vertexBufferConstruct(u32 buffer);
void hs_uniformBufferConstruct(u32 buffer);
void hs_renderLinkConstruct(u32 link);
void hs_zeroBuffer(u32 buffer, u32 size);
void hs_renderLinkDestruct(u32 link, s32 flags);
void hs_uniformBufferDestruct(u32 buffer, s32 flags);
void hs_vertexBufferDestruct(u32 buffer, s32 flags);
void hs_collisionDestruct(u32 collision, s32 flags);
void hs_groundDestruct(u32 ground, s32 flags);
void hs_lineDestruct(u32 line, s32 flags);
void hs_objectDestruct(u32 object, s32 flags);
void hs_shapeDestruct(void* shape, s32 flags);
void hs_actorDestruct(daHookshot_c* actor, s32 flags);

// HD: GPU renderer state and allocation ownership.
struct HookshotGpuBuffer {
    /* 0x0000 */ be<u32> mVertexData;
    u8 mPadding4[0x14C];
    /* 0x0150 */ be<u32> mVertexDataSize;
    u8 mPadding154[0xF8];
    /* 0x024C */ be<u32> mAllocationCount;
    /* 0x0250 */ be<u32> mAllocation;
};
WWHD_OFFSET(HookshotGpuBuffer, mVertexData, 0x0);
WWHD_OFFSET(HookshotGpuBuffer, mVertexDataSize, 0x150);
WWHD_OFFSET(HookshotGpuBuffer, mAllocationCount, 0x24C);
WWHD_OFFSET(HookshotGpuBuffer, mAllocation, 0x250);
WWHD_SIZE(HookshotGpuBuffer, 0x254);

struct HookshotGpuBuffers {
    /* 0x0000 */ HookshotGpuBuffer mBuffers[2];
    /* 0x04A8 */ be<u32> mActiveBuffer;
    /* 0x04AC */ be<u32> mAttributeCount;
    /* 0x04B0 */ be<u32> mVertexStride;
    /* 0x04B4 */ be<u32> mAttributes;
    /* 0x04B8 */ be<u32> mState;
    /* 0x04BC */ be<u8> mUpdated;
    u8 mTail[0x3];
};
WWHD_OFFSET(HookshotGpuBuffers, mBuffers, 0x0);
WWHD_OFFSET(HookshotGpuBuffers, mActiveBuffer, 0x4A8);
WWHD_OFFSET(HookshotGpuBuffers, mAttributeCount, 0x4AC);
WWHD_OFFSET(HookshotGpuBuffers, mVertexStride, 0x4B0);
WWHD_OFFSET(HookshotGpuBuffers, mAttributes, 0x4B4);
WWHD_OFFSET(HookshotGpuBuffers, mState, 0x4B8);
WWHD_OFFSET(HookshotGpuBuffers, mUpdated, 0x4BC);
WWHD_SIZE(HookshotGpuBuffers, 0x4C0);

struct HookshotMaterialEntry {
    /* 0x0000 */ be<u32> mShader;
    /* 0x0004 */ be<s32> mVertexBufferCount;
    /* 0x0008 */ be<u32> mVertexBuffers;
    /* 0x000C */ be<s32> mPixelBufferCount;
    /* 0x0010 */ be<u32> mPixelBuffers;
};
WWHD_OFFSET(HookshotMaterialEntry, mShader, 0x0);
WWHD_OFFSET(HookshotMaterialEntry, mVertexBufferCount, 0x4);
WWHD_OFFSET(HookshotMaterialEntry, mVertexBuffers, 0x8);
WWHD_OFFSET(HookshotMaterialEntry, mPixelBufferCount, 0xC);
WWHD_OFFSET(HookshotMaterialEntry, mPixelBuffers, 0x10);
WWHD_SIZE(HookshotMaterialEntry, 0x14);

struct HookshotPrimitive {
    /* 0x0000 */ be<u32> mType;
    /* 0x0004 */ be<u32> mIndices;
    /* 0x0008 */ be<u32> mFormat;
    /* 0x000C */ be<u32> mCount;
    u8 mTail[0x8];
};
WWHD_OFFSET(HookshotPrimitive, mType, 0x0);
WWHD_OFFSET(HookshotPrimitive, mIndices, 0x4);
WWHD_OFFSET(HookshotPrimitive, mFormat, 0x8);
WWHD_OFFSET(HookshotPrimitive, mCount, 0xC);
WWHD_SIZE(HookshotPrimitive, 0x18);

struct HookshotShapeView {
    u8 mPadding0[0xC];
    /* 0x000C */ be<u32> mVtable;
    u8 mPadding10[0x8];
    /* 0x0018 */ be<u32> mUserArea;
    u8 mPadding1C[0x7C];
    /* 0x0098 */ be<u32> mMaterialCount;
    /* 0x009C */ be<u32> mEntryCount;
    /* 0x00A0 */ be<u32> mEntries;
    /* 0x00A4 */ HookshotGpuBuffers mBuffers;
    /* 0x0564 */ be<s32> mUniformCount;
    /* 0x0568 */ be<u32> mUniforms;
    u8 mPadding56C[0x10];
    /* 0x057C */ be<u32> mRootLinkVtable;
    u8 mPadding580[0xA4];
    /* 0x0624 */ be<u32> mColorLinkVtable;
    u8 mPadding628[0x84];
    /* 0x06AC */ be<u32> mMaterialColor[4];
    u8 mPadding6BC[0x10];
    /* 0x06CC */ be<f32> mTevColor[4];
    /* 0x06DC */ be<f32> mAmbientColor[4];
    u8 mPadding6EC[0x290];
    /* 0x097C */ be<u8> mRenderFlags;
    u8 mPadding97D[0xC4E3];
    /* 0xCE60 */ HookshotPrimitive mPrimitive;
    u8 mTail[0x228];
};
WWHD_OFFSET(HookshotShapeView, mVtable, 0xC);
WWHD_OFFSET(HookshotShapeView, mUserArea, 0x18);
WWHD_OFFSET(HookshotShapeView, mMaterialCount, 0x98);
WWHD_OFFSET(HookshotShapeView, mEntryCount, 0x9C);
WWHD_OFFSET(HookshotShapeView, mEntries, 0xA0);
WWHD_OFFSET(HookshotShapeView, mBuffers, 0xA4);
WWHD_OFFSET(HookshotShapeView, mUniformCount, 0x564);
WWHD_OFFSET(HookshotShapeView, mUniforms, 0x568);
WWHD_OFFSET(HookshotShapeView, mRootLinkVtable, 0x57C);
WWHD_OFFSET(HookshotShapeView, mColorLinkVtable, 0x624);
WWHD_OFFSET(HookshotShapeView, mMaterialColor, 0x6AC);
WWHD_OFFSET(HookshotShapeView, mTevColor, 0x6CC);
WWHD_OFFSET(HookshotShapeView, mAmbientColor, 0x6DC);
WWHD_OFFSET(HookshotShapeView, mRenderFlags, 0x97C);
WWHD_OFFSET(HookshotShapeView, mPrimitive, 0xCE60);
WWHD_SIZE(HookshotShapeView, 0xD0A0);

f32 PSVECSquareDistance(const cXyz* a, const cXyz* b);
void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out);
BOOL dBgS_ChkPolyHSStick(dBgS* bgs, void* polyInfo);
s32 dBgS_GetAttributeCode(dBgS* bgs, void* polyInfo);
BOOL cBgS_ChkPolySafe(dBgS* bgs, void* polyInfo);
BOOL dBgS_ChkMoveBG(dBgS* bgs, void* polyInfo);
void dBgS_MoveBgTransPos(dBgS* bgs, void* polyInfo, bool b, cXyz* pos, csXyz* a, csXyz* scale);
void cM3d_CalcVecZAngle(void* v, csXyz* out);
void hs_setCpsEndpoints(u32 capsule, cXyz* start, cXyz* end, f32 radius);
void hs_setCollisionMass(u32 manager, u32 object, u8 type);
struct HookshotActorRenderView {
    u8 mPadding0[0x134];
    /* 0x0134 */ be<f32> mAmbientScale;
    /* 0x0138 */ be<f32> mColorScale;
    u8 mPadding13C[0x134];
    /* 0x0270 */ GXColorS10 mSecondaryColor;
    u8 mPadding278[0x4];
    /* 0x027C */ be<f32> mSecondaryScale;
};
WWHD_OFFSET(HookshotActorRenderView, mAmbientScale, 0x134);
WWHD_OFFSET(HookshotActorRenderView, mColorScale, 0x138);
WWHD_OFFSET(HookshotActorRenderView, mSecondaryColor, 0x270);
WWHD_OFFSET(HookshotActorRenderView, mSecondaryScale, 0x27C);
WWHD_SIZE(HookshotActorRenderView, 0x280);
struct HookshotMaterialView {
    /* 0x0000 */ be<u32> mShader;
    u8 mPadding4[0x8];
    /* 0x000C */ be<u32> mParameterCount;
    /* 0x0010 */ be<u32> mParameters;
    /* 0x0014 */ be<u32> mTextureCount;
    /* 0x0018 */ be<u32> mTextures;
    u8 mPadding1C[0x30];
    /* 0x004C */ be<u32> mActiveBuffer;
    u8 mPadding50[0x174];
    /* 0x01C4 */ be<u32> mColor[4];
    /* 0x01D4 */ be<u32> mSecondaryColor[4];
    u8 mTail[0x58];
};
WWHD_OFFSET(HookshotMaterialView, mShader, 0x0);
WWHD_OFFSET(HookshotMaterialView, mParameterCount, 0xC);
WWHD_OFFSET(HookshotMaterialView, mParameters, 0x10);
WWHD_OFFSET(HookshotMaterialView, mTextureCount, 0x14);
WWHD_OFFSET(HookshotMaterialView, mTextures, 0x18);
WWHD_OFFSET(HookshotMaterialView, mActiveBuffer, 0x4C);
WWHD_OFFSET(HookshotMaterialView, mColor, 0x1C4);
WWHD_OFFSET(HookshotMaterialView, mSecondaryColor, 0x1D4);
WWHD_SIZE(HookshotMaterialView, 0x23C);
struct HookshotRenderContext {
    u8 mPadding0[0x4];
    /* 0x0004 */ be<u32> mPass;
    u8 mPadding8[0x4];
    /* 0x000C */ be<u32> mMode;
    u8 mPadding10[0x4];
    /* 0x0014 */ be<u32> mResource;
    u8 mPadding18[0x18];
    /* 0x0030 */ be<u32> mCustomLink;
};
WWHD_OFFSET(HookshotRenderContext, mPass, 0x4);
WWHD_OFFSET(HookshotRenderContext, mMode, 0xC);
WWHD_OFFSET(HookshotRenderContext, mResource, 0x14);
WWHD_OFFSET(HookshotRenderContext, mCustomLink, 0x30);
WWHD_SIZE(HookshotRenderContext, 0x34);
struct HookshotResourcePackage {
    u8 mPadding0[0x1C];
    /* 0x001C */ be<u32> mShaderCount;
    /* 0x0020 */ be<u32> mShaders;
};
WWHD_OFFSET(HookshotResourcePackage, mShaderCount, 0x1C);
WWHD_OFFSET(HookshotResourcePackage, mShaders, 0x20);
WWHD_SIZE(HookshotResourcePackage, 0x24);
struct HookshotResourceTable {
    u8 mPadding0[0x4];
    /* 0x0004 */ be<u32> mPackage;
    /* 0x0008 */ be<u32> mCount;
    /* 0x000C */ be<u32> mDescriptors;
};
WWHD_OFFSET(HookshotResourceTable, mPackage, 0x4);
WWHD_OFFSET(HookshotResourceTable, mCount, 0x8);
WWHD_OFFSET(HookshotResourceTable, mDescriptors, 0xC);
WWHD_SIZE(HookshotResourceTable, 0x10);
struct HookshotShaderState {
    /* 0x0000 */ be<u32> mProgram;
    /* 0x0004 */ be<u32> mShader;
};
WWHD_OFFSET(HookshotShaderState, mProgram, 0x0);
WWHD_OFFSET(HookshotShaderState, mShader, 0x4);
WWHD_SIZE(HookshotShaderState, 0x8);
struct HookshotRenderLinkView {
    u8 mPadding0[0xC];
    /* 0x000C */ be<u32> mVtable;
    u8 mPadding10[0x3C];
    /* 0x004C */ be<u32> mActiveBuffer;
    u8 mPadding50[0x24];
    /* 0x0074 */ Mtx34 mMatrix;
    u8 mTail[0x4];
};
WWHD_OFFSET(HookshotRenderLinkView, mVtable, 0xC);
WWHD_OFFSET(HookshotRenderLinkView, mActiveBuffer, 0x4C);
WWHD_OFFSET(HookshotRenderLinkView, mMatrix, 0x74);
WWHD_SIZE(HookshotRenderLinkView, 0xA8);
struct HookshotRenderObject {
    u8 mPadding0[0xA4];
    /* 0x00A4 */ be<u32> mTevStr;
    u8 mTail[0x8];
};
WWHD_OFFSET(HookshotRenderObject, mTevStr, 0xA4);
WWHD_SIZE(HookshotRenderObject, 0xB0);
struct HookshotCpsSource {
    u8 mInfo[0x48];
    be<f32> mRadius;
};
WWHD_OFFSET(HookshotCpsSource, mRadius, 0x48);
WWHD_SIZE(HookshotCpsSource, 0x4C);
struct HookshotJ3DSysView {
    u8 mBase[0x38];
    Mtx34 mViewMtx;
    u8 mBeforeDraw[0xE0];
    be<u32> mDrawData;
    u8 mDrawContext[4];
};
WWHD_OFFSET(HookshotJ3DSysView, mDrawData, 0x148);
WWHD_SIZE(HookshotJ3DSysView, 0x150);

struct HookshotShaderView {
    be<u8> mFlags;
    u8 mPadding1[0x3];
    be<u32> mDisplayList;
    u8 mPadding8[0x4];
    be<u32> mDisplayListSize;
    u8 mPadding10[0x6C];
    be<u32> mProgram;
};
WWHD_OFFSET(HookshotShaderView, mFlags, 0x0);
WWHD_OFFSET(HookshotShaderView, mDisplayList, 0x4);
WWHD_OFFSET(HookshotShaderView, mDisplayListSize, 0xC);
WWHD_OFFSET(HookshotShaderView, mProgram, 0x7C);
WWHD_SIZE(HookshotShaderView, 0x80);
struct HookshotProgramView {
    u8 mPadding0[0x28];
    be<u32> mHandle;
};
WWHD_OFFSET(HookshotProgramView, mHandle, 0x28);
WWHD_SIZE(HookshotProgramView, 0x2C);
struct HookshotShaderParameters {
    u8 mPadding0[0xC];
    be<s16> mVertexBlock;
    be<s16> mPixelBlock;
    be<s16> mGeometryBlock;
};
WWHD_OFFSET(HookshotShaderParameters, mVertexBlock, 0xC);
WWHD_OFFSET(HookshotShaderParameters, mPixelBlock, 0xE);
WWHD_OFFSET(HookshotShaderParameters, mGeometryBlock, 0x10);
WWHD_SIZE(HookshotShaderParameters, 0x12);
struct HookshotUniformView {
    u8 mPadding0[0x4];
    be<u32> mSize;
    u8 mPadding8[0x4];
    be<u32> mData;
    u8 mTail[0xC];
};
WWHD_OFFSET(HookshotUniformView, mSize, 0x4);
WWHD_OFFSET(HookshotUniformView, mData, 0xC);
WWHD_SIZE(HookshotUniformView, 0x1C);
struct HookshotDrawResource {
    u8 mPadding0[0x4];
    be<u32> mUniforms;
};
WWHD_OFFSET(HookshotDrawResource, mUniforms, 0x4);
WWHD_SIZE(HookshotDrawResource, 0x8);
struct HookshotAllocation {
    be<u32> mCount;
    be<u32> mData;
};
WWHD_SIZE(HookshotAllocation, 8);
struct HookshotTextureView {
    be<u32> mSurfaceWords[15];
    u8 mBeforeImage[0x98];
    be<u32> mCachedImage;
    u8 mBeforeMip[4];
    be<u32> mCachedMip;
    u8 mBeforeDirty[0x7C];
    be<u32> mSamplerDirty;
    be<u32> mImageDirty;
    be<u32> mMipDirty;
    u8 mBeforeFlags[0x28];
    be<u8> mFlags;
    u8 mTail[3];
};
WWHD_OFFSET(HookshotTextureView, mCachedImage, 0xD4);
WWHD_OFFSET(HookshotTextureView, mCachedMip, 0xDC);
WWHD_OFFSET(HookshotTextureView, mSamplerDirty, 0x15C);
WWHD_OFFSET(HookshotTextureView, mFlags, 0x190);
WWHD_SIZE(HookshotTextureView, 0x194);

u32 shapeBuf_ct(u32 p);
u32 elem10_ct(u32 p);
u32 chainLink_ct(u32 p);
void chainLink_dt(u32 p, s32 flags);
void shapeBuf_dt(u32 p, s32 flags);
#endif
