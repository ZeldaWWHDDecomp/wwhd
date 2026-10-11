/* Molgera sand floor: GameCube names with the verified WWHD guest layout.
 * HD: the actor base grows by 0x11C, followed by a self pointer; the height field
 * keeps one position/normal array and embeds the HD GPU packet state.
 * Local views describe only the shader, heap and collision fields used here. */
#pragma once
#include "bindings.h"

struct BwdgVertex {
    /* 0x0000 */ cXyz mPosition;
    /* 0x000C */ cXyz mNormal;
    /* 0x0018 */ be<f32> mTexCoord[2];
};
static_assert(sizeof(BwdgVertex) == 0x20);
struct BwdgVertexIndex {
    /* 0x0000 */ be<u16> mPosition;
    /* 0x0002 */ be<u16> mNormal;
    /* 0x0004 */ be<u16> mTexCoord;
};
static_assert(sizeof(BwdgVertexIndex) == 6);
struct BwdgModelInfo {
    /* 0x0000 */ be<u32> mVertexCount;
    /* 0x0004 */ be<u32> mIndexCount;
};
struct BwdgTextureCoordinates {
    /* 0x0000 */ be<f32> mCoords[0x2080][2];
};
struct BwdgModelIndices {
    /* 0x0000 */ BwdgVertexIndex mIndices[0x2080];
};
// HD: additional light values used by the reflection shader.
struct BwdgLightParameters {
    /* 0x0000 */ u8 mLight[0x24];
    /* 0x0024 */ be<f32> mKonstIntensity;
    /* 0x0028 */ be<f32> mColorIntensity;
    /* 0x002C */ u8 mReserved[0x64];
    /* 0x0090 */ GXColorS10 mColor;
    /* 0x0098 */ GXColor mKonstColor;
    /* 0x009C */ u8 mLightState[0xC4];
    /* 0x0160 */ GXColorS10 mAmbientColor;
    /* 0x0168 */ u8 mPadding[4];
    /* 0x016C */ be<f32> mAmbientIntensity;
};
WWHD_OFFSET(BwdgLightParameters, mColor, 0x90);
WWHD_OFFSET(BwdgLightParameters, mAmbientColor, 0x160);
WWHD_OFFSET(BwdgLightParameters, mAmbientIntensity, 0x16C);

struct BwdgVirtualTable;

// HD: GPU objects embedded in the sand-floor packet.
struct BwdgAttributeBuffer {
    /* 0x0000 */ u8 mDescriptor[0x14C];
    /* 0x014C */ be<u32> mSize;
    /* 0x0150 */ u8 mReserved[4];
};
struct BwdgFetchShader {
    /* 0x0000 */ u8 mShader[0xF0];
    /* 0x00F0 */ gptr<BwdgVirtualTable> mpVtable;
};
struct BwdgVertexBuffer {
    /* 0x0000 */ gptr<void> mpData;
    /* 0x0004 */ BwdgAttributeBuffer mAttributeBuffer;
    /* 0x0158 */ BwdgFetchShader mFetchShader;
    /* 0x024C */ be<u32> mAllocationCount;
    /* 0x0250 */ gptr<void> mpAllocation;
};
WWHD_OFFSET(BwdgVertexBuffer, mAttributeBuffer, 4);
WWHD_OFFSET(BwdgVertexBuffer, mFetchShader, 0x158);
WWHD_OFFSET(BwdgVertexBuffer, mAllocationCount, 0x24C);
WWHD_OFFSET(BwdgVertexBuffer, mpAllocation, 0x250);
static_assert(sizeof(BwdgVertexBuffer) == 0x254);

struct BwdgVertexBuffers {
    /* 0x0000 */ BwdgVertexBuffer mBuffers[2];
    /* 0x04A8 */ be<s32> mCurrentBuffer;
    /* 0x04AC */ be<u32> mAttributeCount;
    /* 0x04B0 */ be<u32> mStride;
    /* 0x04B4 */ gptr<void> mpAttributeDescriptor;
    /* 0x04B8 */ be<u32> mFlags;
    /* 0x04BC */ be<u8> mValid;
    /* 0x04BD */ u8 mReserved[3];
};
WWHD_OFFSET(BwdgVertexBuffers, mCurrentBuffer, 0x4A8);
WWHD_OFFSET(BwdgVertexBuffers, mFlags, 0x4B8);
static_assert(sizeof(BwdgVertexBuffers) == 0x4C0);

struct BwdgFetchShaderArray {
    /* 0x0000 */ be<s32> mCount;
    /* 0x0004 */ gptr<BwdgFetchShader> mpShaders;
};
struct BwdgDrawMode {
    /* 0x0000 */ gptr<void> mpShader;
    /* 0x0004 */ BwdgFetchShaderArray mBuffers[2];
};
WWHD_OFFSET(BwdgDrawMode, mBuffers, 4);
static_assert(sizeof(BwdgDrawMode) == 0x14);

struct BwdgUniformBinding {
    /* 0x0000 */ u8 mState[4];
    /* 0x0004 */ gptr<void> mpData;
    /* 0x0008 */ u8 mReserved[4];
    /* 0x000C */ be<u32> mSize;
    /* 0x0010 */ u8 mTail[0xC];
};
union BwdgColor {
    /* 0x0000 */ be<f32> mValues[4];
    /* 0x0000 */ be<u32> mWords[4];
};
struct BwdgMaterial {
    /* 0x0000 */ u8 mState[0x10];
    /* 0x0010 */ BwdgUniformBinding mBindings[2];
    /* 0x0048 */ u8 mReserved[4];
    /* 0x004C */ be<u32> mCurrentBinding;
    /* 0x0050 */ u8 mShaderState[0x174];
    /* 0x01C4 */ BwdgColor mColor;
    /* 0x01D4 */ BwdgColor mAmbientColor;
    /* 0x01E4 */ u8 mTail[0x58];
};
WWHD_OFFSET(BwdgMaterial, mCurrentBinding, 0x4C);
WWHD_OFFSET(BwdgMaterial, mColor, 0x1C4);
WWHD_OFFSET(BwdgMaterial, mAmbientColor, 0x1D4);
static_assert(sizeof(BwdgMaterial) == 0x23C);

struct BwdgModelUniform {
    /* 0x0000 */ u8 mState[0xC];
    /* 0x000C */ gptr<BwdgVirtualTable> mpVtable;
    /* 0x0010 */ BwdgUniformBinding mBindings[2];
    /* 0x0048 */ u8 mReserved[0x2C];
    /* 0x0074 */ Mtx34 mMatrix;
    /* 0x00A4 */ be<u32> mFlags;
};
WWHD_OFFSET(BwdgModelUniform, mMatrix, 0x74);
static_assert(sizeof(BwdgModelUniform) == 0xA8);
struct BwdgLightUniform {
    /* 0x0000 */ u8 mState[0xC];
    /* 0x000C */ gptr<BwdgVirtualTable> mpVtable;
    /* 0x0010 */ BwdgUniformBinding mBindings[2];
    /* 0x0048 */ u8 mReserved[0x2C];
};
static_assert(sizeof(BwdgLightUniform) == 0x74);
struct BwdgUniformContainer {
    /* 0x0000 */ be<s32> mCount;
    /* 0x0004 */ gptr<BwdgMaterial> mpMaterials;
    /* 0x0008 */ u8 mReserved[4];
    /* 0x000C */ BwdgModelUniform mModel;
    /* 0x00B4 */ BwdgLightUniform mLight;
    /* 0x0128 */ be<f32> mConstants[0xBC];
    /* 0x0418 */ be<u8> mFlags;
    /* 0x0419 */ u8 mTail[3];
};
WWHD_OFFSET(BwdgUniformContainer, mModel, 0xC);
WWHD_OFFSET(BwdgUniformContainer, mLight, 0xB4);
WWHD_OFFSET(BwdgUniformContainer, mConstants, 0x128);
WWHD_OFFSET(BwdgUniformContainer, mFlags, 0x418);
static_assert(sizeof(BwdgUniformContainer) == 0x41C);

struct BwdgIndexBuffer {
    /* 0x0000 */ gptr<void> mpIndices;
    /* 0x0004 */ be<u32> mPrimitive;
    /* 0x0008 */ be<u32> mIndexType;
    /* 0x000C */ be<u32> mCount;
    /* 0x0010 */ u8 mReserved[8];
};
static_assert(sizeof(BwdgIndexBuffer) == 0x18);
struct BwdgAttributeState {
    /* 0x0000 */ be<u32> mSignature[0x24];
};
struct BwdgVertexBindingState {
    /* 0x0000 */ BwdgAttributeState mAttributes;
    /* 0x0090 */ u8 mReserved[0x44];
    /* 0x00D4 */ be<u32> mBuffer;
    /* 0x00D8 */ u8 mPadding[4];
    /* 0x00DC */ be<u32> mBufferSize;
    /* 0x00E0 */ u8 mState[0x7C];
    /* 0x015C */ be<u32> mLayoutDirty;
    /* 0x0160 */ be<u32> mBufferDirty;
    /* 0x0164 */ be<u32> mShaderDirty;
    /* 0x0168 */ u8 mTail[0x28];
    /* 0x0190 */ be<u8> mFlags;
    /* 0x0191 */ u8 mAlign[7];
};
WWHD_OFFSET(BwdgVertexBindingState, mBuffer, 0xD4);
WWHD_OFFSET(BwdgVertexBindingState, mLayoutDirty, 0x15C);
WWHD_OFFSET(BwdgVertexBindingState, mFlags, 0x190);
static_assert(sizeof(BwdgVertexBindingState) == 0x198);
struct BwdgRenderState {
    /* 0x0000 */ be<u32> mModeCount;
    /* 0x0004 */ be<s32> mDrawModeCount;
    /* 0x0008 */ gptr<BwdgDrawMode> mpDrawModes;
    /* 0x000C */ BwdgVertexBuffers mVertices;
    /* 0x04CC */ BwdgUniformContainer mUniforms;
    /* 0x08E8 */ BwdgIndexBuffer mIndices;
    /* 0x0900 */ BwdgVertexBindingState mVertexBinding;
    /* 0x0A98 */ BwdgAttributeState mAttributes;
};
WWHD_OFFSET(BwdgRenderState, mVertices, 0xC);
WWHD_OFFSET(BwdgRenderState, mUniforms, 0x4CC);
WWHD_OFFSET(BwdgRenderState, mIndices, 0x8E8);
WWHD_OFFSET(BwdgRenderState, mVertexBinding, 0x900);
WWHD_OFFSET(BwdgRenderState, mAttributes, 0xA98);
static_assert(sizeof(BwdgRenderState) == 0xB28);

// HD: the packet stores one position/normal array; the original uses two.
struct daBwdg_packet_c {
    /* 0x0000 */ u8 mPacket[0xC];
    /* 0x000C */ be<u32> mVtable;
    /* 0x0010 */ u8 mPacketState[0x88];
    /* 0x0098 */ Mtx34 mMtx;
    /* 0x00C8 */ gptr<dKy_tevstr_c> mpTevStr;
    /* 0x00CC */ cXyz mPos[0x1081];
    /* 0xC6D8 */ cXyz mNrm[0x1081];
    /* 0x18CE4 */ BwdgRenderState mRenderState;

    void prepare();
    void draw(void*);

    cXyz* getPos() { return mPos; }
    cXyz* getNrm() { return mNrm; }
};
WWHD_OFFSET(daBwdg_packet_c, mMtx, 0x98);
WWHD_OFFSET(daBwdg_packet_c, mpTevStr, 0xC8);
WWHD_OFFSET(daBwdg_packet_c, mPos, 0xCC);
WWHD_OFFSET(daBwdg_packet_c, mNrm, 0xC6D8);
static_assert(sizeof(daBwdg_packet_c) == 0x1980C);

struct bwdg_class : fopAc_ac_c {
    /* 0x03AC */ u8 m00290[0x1C];
    // HD: constructor retains the actor address immediately before its phase.
    /* 0x03C8 */ gptr<bwdg_class> mSelf;
    /* 0x03CC */ request_of_phase_process_class mPhase;
    /* 0x03D4 */ be<s16> m002B4;
    /* 0x03D6 */ u8 m002B6[2];
    /* 0x03D8 */ be<s16> m002B8;
    /* 0x03DA */ u8 m002BA[2];
    /* 0x03DC */ gptr<dBgW> mpBgW;
    /* 0x03E0 */ be<f32> m002C0[0x1081];
    /* 0x45E4 */ daBwdg_packet_c mBwdgPacket;

    cXyz* getPos() { return mBwdgPacket.getPos(); }
    cXyz* getNrm() { return mBwdgPacket.getNrm(); }
    daBwdg_packet_c* packet() { return &mBwdgPacket; }
};
WWHD_OFFSET(bwdg_class, mSelf, 0x3C8);
WWHD_OFFSET(bwdg_class, mPhase, 0x3CC);
WWHD_OFFSET(bwdg_class, m002B4, 0x3D4);
WWHD_OFFSET(bwdg_class, m002B8, 0x3D8);
WWHD_OFFSET(bwdg_class, mpBgW, 0x3DC);
WWHD_OFFSET(bwdg_class, m002C0, 0x3E0);
WWHD_OFFSET(bwdg_class, mBwdgPacket, 0x45E4);
static_assert(sizeof(bwdg_class) == 0x1DDF0);

struct BwdgProgramGroup {
    /* 0x0000 */ u8 mState[0x28];
    /* 0x0028 */ gptr<void> mpGroup;
};
struct BwdgShaderProgram {
    /* 0x0000 */ be<u8> mFlags;
    /* 0x0001 */ u8 mReserved[3];
    /* 0x0004 */ gptr<void> mpDisplayList;
    /* 0x0008 */ u8 mPadding[4];
    /* 0x000C */ be<u32> mDisplayListSize;
    /* 0x0010 */ u8 mState[0x6C];
    /* 0x007C */ gptr<BwdgProgramGroup> mpGroup;
};
WWHD_OFFSET(BwdgShaderProgram, mpGroup, 0x7C);
struct BwdgUniformLocations {
    /* 0x0000 */ u8 mState[0xC];
    /* 0x000C */ be<s16> mVertex;
    /* 0x000E */ be<s16> mPixel;
    /* 0x0010 */ be<s16> mGeometry;
};
struct BwdgShaderVertices {
    /* 0x0000 */ u8 mState[4];
    /* 0x0004 */ BwdgAttributeBuffer mAttributes;
};
struct BwdgShaderShape {
    /* 0x0000 */ gptr<BwdgShaderProgram> mpProgram;
    /* 0x0004 */ u8 mState[8];
    /* 0x000C */ be<u32> mUniformCount;
    /* 0x0010 */ gptr<BwdgUniformLocations> mpUniformLocations;
    /* 0x0014 */ be<u32> mVertexCount;
    /* 0x0018 */ gptr<BwdgShaderVertices> mpVertices;
};
WWHD_OFFSET(BwdgShaderShape, mpUniformLocations, 0x10);
struct BwdgProgramState {
    /* 0x0000 */ gptr<void> mpGroup;
    /* 0x0004 */ gptr<BwdgShaderProgram> mpProgram;
};
struct BwdgVirtualTable {
    /* 0x0000 */ u8 mHeader[0xC];
    /* 0x000C */ gfn mDestroy;
    /* 0x0010 */ u8 mReserved[0x1C];
    /* 0x002C */ gfn mBind;
};
WWHD_OFFSET(BwdgVirtualTable, mBind, 0x2C);
struct BwdgDrawObject {
    /* 0x0000 */ u8 mState[0xC];
    /* 0x000C */ gptr<BwdgVirtualTable> mpVtable;
};
struct BwdgDrawContext {
    /* 0x0000 */ u8 mState[0xC];
    /* 0x000C */ be<s32> mMode;
    /* 0x0010 */ u8 mReserved[0x20];
    /* 0x0030 */ gptr<BwdgDrawObject> mpObject;
};
WWHD_OFFSET(BwdgDrawContext, mpObject, 0x30);
struct bwdg_draw_state {
    /* 0x0000 */ u8 mBlendEnable[4];
    /* 0x0004 */ be<u32> mBlendMode;
    /* 0x0008 */ be<u32> mDepthMode;
    /* 0x000C */ be<u32> mCullMode;
    /* 0x0010 */ u8 mState[0xD0];
    /* 0x00E0 */ be<u8> mPolygonOffsetEnable;
    /* 0x00E1 */ u8 mReserved[3];
    /* 0x00E4 */ be<u32> mPolygonOffsetMode;
    /* 0x00E8 */ be<f32> mPolygonOffset;
    /* 0x00EC */ be<u32> mFlags;
    /* 0x00F0 */ u8 mTail[0x2C];
};
WWHD_OFFSET(bwdg_draw_state, mPolygonOffsetEnable, 0xE0);
WWHD_OFFSET(bwdg_draw_state, mFlags, 0xEC);
static_assert(sizeof(bwdg_draw_state) == 0x11C);

struct BwdgSystemTexture {
    /* 0x0000 */ u8 mState[0x240];
    /* 0x0240 */ u8 mReflectionTexture[1];
};
struct BwdgSystem {
    /* 0x0000 */ u8 mState[0x38];
    /* 0x0038 */ Mtx34 mViewMatrix;
    /* 0x0068 */ u8 mReserved[0xC];
    /* 0x0074 */ gptr<void> mpOpaqueDrawBuffer;
    /* 0x0078 */ u8 mDrawBuffers[0xD0];
    /* 0x0148 */ gptr<BwdgSystemTexture> mpTexture;
    /* 0x014C */ Mtx34 mTextureMatrix;
    /* 0x017C */ u8 mPadding[0x10];
    /* 0x018C */ bwdg_draw_state mDrawState;
};
WWHD_OFFSET(BwdgSystem, mpOpaqueDrawBuffer, 0x74);
WWHD_OFFSET(BwdgSystem, mpTexture, 0x148);
WWHD_OFFSET(BwdgSystem, mTextureMatrix, 0x14C);
WWHD_OFFSET(BwdgSystem, mDrawState, 0x18C);

struct BwdgDrawModes {
    /* 0x0000 */ be<s32> mCount;
    /* 0x0004 */ gptr<BwdgDrawMode> mpModes;
};
struct BwdgShaderParameter {
    /* 0x0000 */ u8 mState[0x84];
};
struct BwdgShaderArchive {
    /* 0x0000 */ u8 mState[0x1C];
    /* 0x001C */ be<u32> mParameterCount;
    /* 0x0020 */ gptr<BwdgShaderParameter> mpParameters;
};
struct BwdgShaderEntry {
    /* 0x0000 */ u8 mState[0x20];
    /* 0x0020 */ be<u8> mReady;
    /* 0x0021 */ u8 mReserved[3];
};
static_assert(sizeof(BwdgShaderEntry) == 0x24);
struct BwdgShaderRegistry {
    /* 0x0000 */ u8 mState[4];
    /* 0x0004 */ gptr<BwdgShaderArchive> mpArchive;
    /* 0x0008 */ be<u32> mEntryCount;
    /* 0x000C */ gptr<BwdgShaderEntry> mpEntries;
};

struct BwdgHeapVtable {
    /* 0x0000 */ u8 mState[0x34];
    /* 0x0034 */ gfn mAllocate;
    /* 0x0038 */ u8 mReserved[4];
    /* 0x003C */ gfn mFree;
};
struct BwdgHeap {
    /* 0x0000 */ u8 mState[0xC];
    /* 0x000C */ gptr<BwdgHeapVtable> mpVtable;
};
struct BwdgHeapManager {
    /* 0x0000 */ u8 mState[1];
};
struct BwdgGraphicsDevice {
    /* 0x0000 */ u8 mState[1];
};
struct BwdgResourceManager {
    /* 0x0000 */ u8 mState[1];
};
struct BwdgAttributeDescriptor {
    /* 0x0000 */ u8 mState[0xC];
};
struct BwdgIndexData {
    /* 0x0000 */ be<u16> mIndices[0x2080];
};

struct BwdgSavedVertices {
    /* 0x0000 */ u8 mWorld[0x90];
    /* 0x0090 */ gptr<cXyz> pm_vtx_tbl;
};
WWHD_OFFSET(BwdgSavedVertices, pm_vtx_tbl, 0x90);
struct BwdgCacheLine {
    /* 0x0000 */ be<u32> mWords[8];
};
static_assert(sizeof(BwdgCacheLine) == 0x20);

enum { TEV_TYPE_BG0_FULL = 5, fpcNm_BWD_e = 0xD9 };
enum dRes_INDEX_BWDG {
    dRes_INDEX_BWDG_DAT_GRIDIDX_e = 4,
    dRes_INDEX_BWDG_DZB_HSAND1_e = 7,
};
using BwdgConstructor = void* (*)(void*);
using BwdgDestructor = void (*)(void*, s32);
using BwdgSearch = void* (*)(void*, void*);

// HD: private GPU and resource bindings used by this packet.
dBgW* bwdgNewHeightField(u32);
BOOL bwdgSetHeightField(dBgW*, cBgD_t*, void*, f32, s32, s32, s32);
void bwdgCopyBackVertices(dBgW*);
void bwdgMoveHeightField(dBgW*);
void bwdgRotateX(Mtx34*, s16);
void bwdgEntryPacket(void*, void*, u16);
void* bwdgSearchActors(BwdgSearch, void*);
void bwdgAttributeBufferCtor(BwdgAttributeBuffer*);
void bwdgFetchShaderCtor(BwdgFetchShader*);
void bwdgFetchShaderDt(BwdgFetchShader*, s32);
void bwdgAttributeBufferDt(BwdgAttributeBuffer*, s32);
BwdgHeap* bwdgFindAllocationHeap(BwdgHeapManager*, void*);
void bwdgReleaseFetchShader(BwdgFetchShader*);
void bwdgReleaseUniformBinding(BwdgUniformBinding*);
void bwdgVertexBindingDt(BwdgVertexBindingState*, s32);
void bwdgIndexBufferDt(BwdgIndexBuffer*, s32);
void bwdgUniformDt(void*, s32);
void bwdgUniformContainerDt(BwdgUniformContainer*, s32);
void bwdgDestroyArray(void*, u32, u32, BwdgDestructor, u32, u32);
void bwdgBasePacketDt(void*, s32);
void bwdgActorDt(fopAc_ac_c*, s32);
void bwdgFlushAttributeBuffer(BwdgAttributeBuffer*, u32, u32);
void bwdgSetFog(dKy_tevstr_c*);
void bwdgSetLight(dKy_tevstr_c*);
void bwdgSetReflectionTexture(BwdgUniformContainer*, u32, Mtx34*, Mtx34*, void*);
void bwdgConvertColor(BwdgColor*, BwdgColor*, f32);
void bwdgScaleColor(be<f32>*, f32);
void bwdgUpdateUniforms(BwdgUniformContainer*, u32);
void bwdgSetPixelUniformBlock(s32, u32, void*);
void bwdgSetVertexUniformBlock(s32, u32, void*);
void bwdgSetGeometryUniformBlock(s32, u32, void*);
BwdgProgramState* bwdgGetProgramState(BwdgSystem*);
void bwdgUpdateShaderProgram(BwdgShaderProgram*, u32);
void bwdgBindProgramGroup(void*);
void bwdgCallDisplayList(void*, u32);
void bwdgBindProgram(BwdgShaderProgram*);
void bwdgApplyDrawContext(BwdgDrawContext*, BwdgShaderShape*);
void bwdgBindVertices(BwdgVertexBindingState*, u32, u32, u32);
void bwdgDrawStateCtor(bwdg_draw_state*);
void bwdgSetDrawMode(u32, bwdg_draw_state*);
void bwdgApplyDrawState(bwdg_draw_state*);
void bwdgBindFetchShader(BwdgFetchShader*);
void bwdgDrawIndexed(u32, u32, void*, u32, u32, u32);
BwdgHeap* bwdgGetCurrentHeap(BwdgHeapManager*);
void bwdgBasePacketCtor(void*);
void bwdgConstructArray(void*, u32, u32, BwdgConstructor);
void bwdgUniformContainerCtor(BwdgUniformContainer*);
void bwdgUniformCtor(void*);
void bwdgClearMemory(void*, u32);
void bwdgIndexBufferCtor(BwdgIndexBuffer*);
void bwdgVertexBindingCtor(BwdgVertexBindingState*);
void bwdgAttributeStateCtor(BwdgAttributeState*);
BwdgShaderRegistry* bwdgGetShaderRegistry();
s32 bwdgFindShader(BwdgShaderArchive*, SafeString*);
void bwdgPrepareShader(BwdgShaderEntry*, BwdgShaderParameter*, u32);
void bwdgSetShaderEntry(BwdgRenderState*, BwdgShaderEntry*, u32);
void bwdgSetVertexBuffer(BwdgAttributeBuffer*, void*, u32, be<u32>*);
void bwdgSetFetchShader(void*, BwdgFetchShader*, BwdgAttributeBuffer*, be<u32>*, u32);
void bwdgInitializeUniforms(BwdgUniformContainer*, u32, u32);
void bwdgSetIndexBuffer(BwdgIndexBuffer*, BwdgIndexData*, u32, u32);
void* bwdgFindResource(BwdgResourceManager*, SafeString*, SafeString*, u32);
void bwdgLockGraphics(BwdgGraphicsDevice*);
void bwdgConfigureAttributes(BwdgAttributeState*, void*, const char*);
void bwdgUnlockGraphics(BwdgGraphicsDevice*);
void bwdgCopyAttributeState(BwdgVertexBindingState*, BwdgAttributeState*);

static_assert(sizeof(BwdgModelInfo) == 0x8);
static_assert(sizeof(BwdgTextureCoordinates) == 0x10400);
static_assert(sizeof(BwdgModelIndices) == 0xC300);
static_assert(sizeof(BwdgLightParameters) == 0x170);
static_assert(sizeof(BwdgAttributeBuffer) == 0x154);
static_assert(sizeof(BwdgFetchShader) == 0xF4);
static_assert(sizeof(BwdgFetchShaderArray) == 0x8);
static_assert(sizeof(BwdgUniformBinding) == 0x1C);
static_assert(sizeof(BwdgColor) == 0x10);
static_assert(sizeof(BwdgAttributeState) == 0x90);
static_assert(sizeof(BwdgProgramGroup) == 0x2C);
static_assert(sizeof(BwdgShaderProgram) == 0x80);
static_assert(sizeof(BwdgUniformLocations) == 0x12);
static_assert(sizeof(BwdgShaderVertices) == 0x158);
static_assert(sizeof(BwdgShaderShape) == 0x1C);
static_assert(sizeof(BwdgProgramState) == 0x8);
static_assert(sizeof(BwdgVirtualTable) == 0x30);
static_assert(sizeof(BwdgDrawObject) == 0x10);
static_assert(sizeof(BwdgDrawContext) == 0x34);
static_assert(sizeof(BwdgSystemTexture) == 0x241);
static_assert(sizeof(BwdgSystem) == 0x2A8);
static_assert(sizeof(BwdgDrawModes) == 0x8);
static_assert(sizeof(BwdgShaderParameter) == 0x84);
static_assert(sizeof(BwdgShaderArchive) == 0x24);
static_assert(sizeof(BwdgShaderRegistry) == 0x10);
static_assert(sizeof(BwdgHeapVtable) == 0x40);
static_assert(sizeof(BwdgHeap) == 0x10);
static_assert(sizeof(BwdgHeapManager) == 0x1);
static_assert(sizeof(BwdgGraphicsDevice) == 0x1);
static_assert(sizeof(BwdgResourceManager) == 0x1);
static_assert(sizeof(BwdgAttributeDescriptor) == 0xC);
static_assert(sizeof(BwdgIndexData) == 0x4100);
static_assert(sizeof(BwdgSavedVertices) == 0x94);
