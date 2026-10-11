/* Magma surface simulation. GameCube member names on the verified HD layout.
 * HD: balls/floors contain additional render resources; the packet owns eight
 * enlarged floors and doubly linked room lists. This header is private to this TU.
 */
#pragma once
#include "bindings.h"

namespace dMagma {
using Mtx = be<f32>[3][4];
struct dMagma_ball_c;
struct dMagma_floor_c;
struct dMagma_room_c;

// HD: these records have no GameCube counterparts. Unknown library payloads
// remain opaque; allocation metadata and the shader's colour/matrix arrays are
// explicit so ownership and construction order remain visible.
struct HdResourceHandleVtable {
    u8 mPrefix[12];
    gabi::gfn destroy;
};
struct HdResourceHandle {
    u8 mPayload[0xF0];
    gptr<HdResourceHandleVtable> __vtbl;
};
WWHD_SIZE(HdResourceHandle, 0xF4);
WWHD_OFFSET(HdResourceHandle, __vtbl, 0xF0);
struct HdVertexBuffer {
    u8 mPayload[0x14C];
    be<u32> mFlushSize;
    u8 mTail[4];
};
WWHD_SIZE(HdVertexBuffer, 0x154);
WWHD_OFFSET(HdVertexBuffer, mFlushSize, 0x14C);
struct HdResourceBuffer {
    gptr<void> mpVertices;
    HdVertexBuffer mVertexBuffer;
    HdResourceHandle mResource;
    be<u32> mCapacity;
    gptr<void> mpStorage;
};
WWHD_SIZE(HdResourceBuffer, 0x254);
WWHD_OFFSET(HdResourceBuffer, mResource, 0x158);
WWHD_OFFSET(HdResourceBuffer, mCapacity, 0x24C);
WWHD_OFFSET(HdResourceBuffer, mpStorage, 0x250);
struct HdResources {
    HdResourceBuffer mBuffer[2];
    be<u32> mActiveBuffer;
    be<u32> mVertexLayout;
    be<u32> mVertexStride;
    be<u32> mHdReserved;
    be<u32> mMode;
    be<u8> mDirty;
    u8 mPadding[3];
};
WWHD_SIZE(HdResources, 0x4C0);
WWHD_OFFSET(HdResources, mActiveBuffer, 0x4A8);
WWHD_OFFSET(HdResources, mMode, 0x4B8);
WWHD_OFFSET(HdResources, mDirty, 0x4BC);
struct HdResourceArray {
    be<s32> mCount;
    gptr<HdResourceHandle> mpItems;
};
WWHD_SIZE(HdResourceArray, 8);
struct HdMaterialRecord {
    gptr<void> mpMaterial;
    HdResourceArray mFirst;
    HdResourceArray mSecond;
};
WWHD_SIZE(HdMaterialRecord, 0x14);
struct HdIndexBuffer {
    be<u32> mIndexCount;
    be<u32> mPrimitiveType;
    be<u32> mIndexOffset;
    gptr<void> mpIndices;
    be<u32> mIndexStride;
    u8 mTail[4];
};
WWHD_SIZE(HdIndexBuffer, 0x18);
struct HdShapeArray {
    be<s32> mCount;
    gptr<void> mpShapes;
    u8 mTail[4];
};
WWHD_SIZE(HdShapeArray, 0xC);
struct HdRenderBuffer {
    be<u32> mFlags;
    be<u32> mSize;
    u8 mPadding[4];
    gptr<void> mpData;
    u8 mPayload[12];
};
WWHD_SIZE(HdRenderBuffer, 0x1C);
WWHD_OFFSET(HdRenderBuffer, mpData, 12);
struct HdRenderObject {
    u8 mPrefix[12];
    be<u32> __vtbl;
    HdRenderBuffer mBuffers[2];
    u8 mPayload[0x2C];
};
WWHD_SIZE(HdRenderObject, 0x74);
WWHD_OFFSET(HdRenderObject, __vtbl, 12);
WWHD_OFFSET(HdRenderObject, mBuffers, 16);
struct HdShapeRecord {
    u8 mPrefix[16];
    HdRenderBuffer mBuffers[2];
    u8 mPadding[4];
    be<u32> mActiveBuffer;
    u8 mPayload[0x1EC];
};
WWHD_SIZE(HdShapeRecord, 0x23C);
WWHD_OFFSET(HdShapeRecord, mActiveBuffer, 0x4C);
struct HdColorVector {
    be<f32> r, g, b, a;
};
WWHD_SIZE(HdColorVector, 0x10);
struct HdShaderParameters {
    HdColorVector mColors[11];
    HdColorVector mColorPairs[3][2];
    Mtx mMatrices[8];
    HdColorVector mVectors[6];
};
WWHD_SIZE(HdShaderParameters, 0x2F0);
WWHD_OFFSET(HdShaderParameters, mColorPairs, 0xB0);
WWHD_OFFSET(HdShaderParameters, mMatrices, 0x110);
WWHD_OFFSET(HdShaderParameters, mVectors, 0x290);
// HD: packed texture descriptors retain their library word representation;
// the verifier checks selected descriptor words and cached sampler bindings.
struct HdTextureHandle {
    be<u32> mDescriptor[0x24];
};
struct HdTextureSampler {
    be<u32> mDescriptor[0x60];
    be<f32> mParameter;
    be<u32> mTail[3];
    be<u8> mFlags;
    u8 mPadding[7];
};
WWHD_SIZE(HdTextureHandle, 0x90);
WWHD_SIZE(HdTextureSampler, 0x198);
struct HdMaterialResources {
    be<u32> mMaterialCount;
    be<u32> mMaterialCapacity;
    gptr<HdMaterialRecord> mpMaterials;
    HdResources mResources;
    HdIndexBuffer mIndexBuffer;
};
WWHD_SIZE(HdMaterialResources, 0x4E4);
WWHD_OFFSET(HdMaterialResources, mResources, 12);
WWHD_OFFSET(HdMaterialResources, mIndexBuffer, 0x4CC);
struct HdBallRenderState {
    HdMaterialResources mMaterial;
    HdShapeArray mShapes;
    HdRenderObject mFirstObject;
    Mtx mModelMtx;
    u8 mMatrixPadding[4];
    HdRenderObject mSecondObject;
    HdShaderParameters mShader;
    be<u8> mShaderFlags;
    u8 mPadding[3];
    HdTextureHandle mColTexture;
    HdTextureHandle mKuroTexture;
    HdTextureSampler mColSampler;
    HdTextureSampler mKuroSampler;
};
WWHD_SIZE(HdBallRenderState, 0xD50);
WWHD_OFFSET(HdBallRenderState, mFirstObject, 0x4F0);
WWHD_OFFSET(HdBallRenderState, mModelMtx, 0x564);
WWHD_OFFSET(HdBallRenderState, mSecondObject, 0x598);
WWHD_OFFSET(HdBallRenderState, mShader, 0x60C);
WWHD_OFFSET(HdBallRenderState, mColTexture, 0x900);
WWHD_OFFSET(HdBallRenderState, mKuroTexture, 0x990);
WWHD_OFFSET(HdBallRenderState, mColSampler, 0xA20);
WWHD_OFFSET(HdBallRenderState, mKuroSampler, 0xBB8);
struct HdFloorRenderState {
    HdMaterialResources mMaterial;
    be<u16> mIndices[4];
    HdShapeArray mShapes;
    HdRenderObject mFirstObject;
    Mtx mModelMtx;
    u8 mMatrixPadding[4];
    HdRenderObject mSecondObject;
    HdShaderParameters mShader;
    be<u8> mShaderFlags;
    u8 mPadding[3];
    HdTextureHandle mColTexture;
    HdTextureSampler mColSampler;
};
WWHD_SIZE(HdFloorRenderState, 0xB30);
WWHD_OFFSET(HdFloorRenderState, mIndices, 0x4E4);
WWHD_OFFSET(HdFloorRenderState, mFirstObject, 0x4F8);
WWHD_OFFSET(HdFloorRenderState, mModelMtx, 0x56C);
WWHD_OFFSET(HdFloorRenderState, mSecondObject, 0x5A0);
WWHD_OFFSET(HdFloorRenderState, mShader, 0x614);
WWHD_OFFSET(HdFloorRenderState, mColTexture, 0x908);
WWHD_OFFSET(HdFloorRenderState, mColSampler, 0x998);

struct dMagma_ball_c {
    s32 rangeCheck(cXyz *pos, be<f32> *dst);
    void prepareRendering();
    void createResources();
    void draw(void *context);
    /* 0x000 */ HdBallRenderState mRender;
    /* 0xD50 */ cXyz mPos;
    /* 0xD5C */ be<f32> mScale;
    /* 0xD60 */ be<f32> mBaseY;
    /* 0xD64 */ be<s16> mWave;
    /* 0xD66 */ u8 mHdPadding[2];
    /* 0xD68 */ Mtx mPosMtx;
    /* 0xD98 */ Mtx mTexProjMtx;
    /* 0xDC8 */ gptr<Mtx> mpFloorTexMtx;
    /* 0xDCC */ be<u32> __vtbl;
    /* 0xDD0 */ be<u8> mWaveTimer;
    /* 0xDD1 */ u8 mHdTail[3];
};
WWHD_SIZE(dMagma_ball_c, 0xDD4);
WWHD_OFFSET(dMagma_ball_c, mPos, 0xD50);
WWHD_OFFSET(dMagma_ball_c, mScale, 0xD5C);
WWHD_OFFSET(dMagma_ball_c, mBaseY, 0xD60);
WWHD_OFFSET(dMagma_ball_c, mWave, 0xD64);
WWHD_OFFSET(dMagma_ball_c, mPosMtx, 0xD68);
WWHD_OFFSET(dMagma_ball_c, mTexProjMtx, 0xD98);
WWHD_OFFSET(dMagma_ball_c, mpFloorTexMtx, 0xDC8);
WWHD_OFFSET(dMagma_ball_c, __vtbl, 0xDCC);
WWHD_OFFSET(dMagma_ball_c, mWaveTimer, 0xDD0);

struct dMagma_ballPath_c : dMagma_ball_c {
    void calc(f32 offsY, u32 pathNo, s32 roomNo);
    void update();
    void setup(f32 offsY, u32 pathNo, s32 roomNo);
};
WWHD_SIZE(dMagma_ballPath_c, 0xDD4);

struct dMagma_floor_c {
    void update();
    void prepareRendering();
    void createResources();
    void calc(s32 room);
    void draw(void *context);
    void remove();
    void *create(void *position, void *scale, s32 path, u32 count, s32 room);
    /* 0x000 */ HdFloorRenderState mRender;
    /* 0xB30 */ gptr<gptr<dMagma_ball_c>> mpBalls;
    /* 0xB34 */ be<u8> mBallNum;
    /* 0xB35 */ be<u8> mPathNo;
    /* 0xB36 */ be<u8> mReleaseTimer;
    /* 0xB37 */ u8 mHdPadding;
    /* 0xB38 */ cXyz mPos;
    /* 0xB44 */ be<f32> mScaleX;
    /* 0xB48 */ be<f32> mScaleZ;
    /* 0xB4C */ Mtx mPosMtx;
    /* 0xB7C */ Mtx mTexMtx0;
    /* 0xBAC */ Mtx mPostMtx0;
    /* 0xBDC */ gptr<dMagma_floor_c> mpNext;
    /* 0xBE0 */ gptr<dMagma_room_c> mpRoom;
};
WWHD_SIZE(dMagma_floor_c, 0xBE4);
WWHD_OFFSET(dMagma_floor_c, mpBalls, 0xB30);
WWHD_OFFSET(dMagma_floor_c, mBallNum, 0xB34);
WWHD_OFFSET(dMagma_floor_c, mPathNo, 0xB35);
WWHD_OFFSET(dMagma_floor_c, mReleaseTimer, 0xB36);
WWHD_OFFSET(dMagma_floor_c, mPos, 0xB38);
WWHD_OFFSET(dMagma_floor_c, mScaleX, 0xB44);
WWHD_OFFSET(dMagma_floor_c, mScaleZ, 0xB48);
WWHD_OFFSET(dMagma_floor_c, mPosMtx, 0xB4C);
WWHD_OFFSET(dMagma_floor_c, mTexMtx0, 0xB7C);
WWHD_OFFSET(dMagma_floor_c, mPostMtx0, 0xBAC);
WWHD_OFFSET(dMagma_floor_c, mpNext, 0xBDC);
WWHD_OFFSET(dMagma_floor_c, mpRoom, 0xBE0);

struct dMagma_room_c {
    void newFloor(dMagma_floor_c *floor);
    void deleteFloor();
    gptr<dMagma_floor_c> mpFirst;
};
WWHD_SIZE(dMagma_room_c, 4);
WWHD_OFFSET(dMagma_room_c, mpFirst, 0);

struct dMagma_packet_c {
    void deleteRoom(u32 roomNo);
    f32 checkYpos(cXyz *pos);
    dMagma_floor_c *newFloor(cXyz *pos, cXyz *scale, u32 roomNo, s32 pathNo);
    void releaseFloors(s32 force);
    void update();
    void draw(void *context);
    void calc();
    /* 0x0000 */ u8 mHdPrefix[12];
    /* 0x000C */ be<u32> __vtbl;
    /* 0x0010 */ u8 mHdPayload[0x88];
    /* 0x0098 */ dMagma_floor_c mFloor[8];
    /* 0x5FB8 */ dMagma_room_c mRoom[64];
    /* 0x60B8 */ GXColor mColor1;
    /* 0x60BC */ be<f32> mTimer;
};
WWHD_SIZE(dMagma_packet_c, 0x60C0);
WWHD_OFFSET(dMagma_packet_c, __vtbl, 12);
WWHD_OFFSET(dMagma_packet_c, mFloor, 0x98);
WWHD_OFFSET(dMagma_packet_c, mRoom, 0x5FB8);
WWHD_OFFSET(dMagma_packet_c, mColor1, 0x60B8);
WWHD_OFFSET(dMagma_packet_c, mTimer, 0x60BC);
} // namespace dMagma

// HD: private render-library views and mapped data records used by this translation unit.
struct MagmaTuVtables {
    u32 safeString, baseBall, bossBall, pathBall, packet, firstRender, secondRender;
};
WWHD_SIZE(MagmaTuVtables, 0x1C);
struct MagmaFloat {
    be<f32> value;
    operator f32() const { return value; }
};
WWHD_SIZE(MagmaFloat, 0x4);
struct MagmaSinCos {
    be<f32> sine, cosine;
};
WWHD_SIZE(MagmaSinCos, 0x8);
struct MagmaSinCosTable {
    MagmaSinCos entries[8192];
};
WWHD_SIZE(MagmaSinCosTable, 0x10000);
struct MagmaDrawContext {
    u8 mPrefix[12];
    be<u32> mMode;
    u8 mPadding[0x20];
    gptr<dMagma::HdRenderObject> mpExtraObject;
};
WWHD_SIZE(MagmaDrawContext, 0x34);
struct MagmaDrawBufferPointer {
    gptr<void> value;
};
WWHD_SIZE(MagmaDrawBufferPointer, 0x4);
struct MagmaBallVtable {
    u8 mPrefix[12];
    gabi::gfn destroy;
    u8 mReserved0[4];
    gabi::gfn calc;
    u8 mReserved1[4];
    gabi::gfn update;
    u8 mReserved[4];
    gabi::gfn setup;
};
WWHD_SIZE(MagmaBallVtable, 0x28);
struct MagmaWord {
    be<u32> value;
};
WWHD_SIZE(MagmaWord, 0x4);
struct MagmaModuleState {
    be<f32> x, y;
    be<u8> flag, byte;
    u8 mPadding[2];
};
WWHD_SIZE(MagmaModuleState, 0xC);
struct MagmaPackedState {
    be<u32> words[4];
};
WWHD_SIZE(MagmaPackedState, 0x10);
struct MagmaHeapVtable {
    u8 mPrefix[0x34];
    gabi::gfn allocate;
    u8 mReserved[4];
    gabi::gfn free;
};
WWHD_SIZE(MagmaHeapVtable, 0x40);
struct MagmaHeap {
    u8 mPrefix[12];
    gptr<MagmaHeapVtable> __vtbl;
};
WWHD_SIZE(MagmaHeap, 0x10);
struct MagmaMaterialTable {
    be<u32> mCapacity;
    gptr<dMagma::HdMaterialRecord> mpMaterials;
};
WWHD_SIZE(MagmaMaterialTable, 0x8);
struct MagmaKeyColorTable {
    GXColor colors[4];
};
WWHD_SIZE(MagmaKeyColorTable, 0x10);
struct MagmaStringRef {
    be<u32> value, vtable;
};
WWHD_SIZE(MagmaStringRef, 0x8);
struct MagmaStringVtable {
    u8 mPrefix[0x14];
    gabi::gfn resolve;
};
WWHD_SIZE(MagmaStringVtable, 0x18);
struct MagmaCompiledShader {
    u8 mPrefix[0x28];
    gptr<void> mpProgram;
};
WWHD_SIZE(MagmaCompiledShader, 0x2C);
struct MagmaShader {
    be<u8> mFlags;
    u8 mPadding[3];
    gptr<void> mpCommands;
    u8 mReserved[4];
    be<u32> mDisplayList;
    u8 mPayload[0x6C];
    gptr<MagmaCompiledShader> mpCompiled;
};
WWHD_SIZE(MagmaShader, 0x80);
struct MagmaBindingDescriptor {
    u8 mPrefix[12];
    be<s16> mIndexBuffer, mVertexBuffer, mGeometryBuffer;
};
WWHD_SIZE(MagmaBindingDescriptor, 0x12);
struct MagmaMaterialTexture {
    u8 mPrefix[4];
    u8 mTexture[16];
};
WWHD_SIZE(MagmaMaterialTexture, 0x14);
struct MagmaMaterial {
    gptr<MagmaShader> mpShader;
    u8 mPrefix[8];
    be<u32> mBindingCount;
    gptr<MagmaBindingDescriptor> mpBindings;
    be<u32> mTextureCount;
    gptr<MagmaMaterialTexture> mpTextures;
};
WWHD_SIZE(MagmaMaterial, 0x1C);
struct MagmaRenderCache {
    gptr<void> mpProgram;
    gptr<MagmaShader> mpShader;
};
WWHD_SIZE(MagmaRenderCache, 0x8);
struct MagmaRenderVtable {
    u8 mPrefix[0x2C];
    gabi::gfn bindMaterial;
};
WWHD_SIZE(MagmaRenderVtable, 0x30);
struct MagmaGraphicsState {
    u8 mPrefix[8];
    be<u32> mState, mFlags;
    u8 mPayload[0xD0];
    be<u8> mFloor;
    u8 mPadding[3];
    be<u32> mProjectionMode;
    be<f32> mProjectionNear;
    be<u32> mBlendFlags;
    u8 mTail[0x2C];
};
WWHD_SIZE(MagmaGraphicsState, 0x11C);
struct MagmaProjectionState {
    u8 mPrefix[0x240];
    u8 mParameters[4];
};
WWHD_SIZE(MagmaProjectionState, 0x244);
struct MagmaGraphicsSystem {
    u8 mPrefix[0x148];
    gptr<MagmaProjectionState> mpProjection;
    u8 mShapeParameters[4];
};
WWHD_SIZE(MagmaGraphicsSystem, 0x150);
struct MagmaBallStripTable {
    gptr<void> indices[6];
};
WWHD_SIZE(MagmaBallStripTable, 0x18);
struct MagmaShaderRecord {
    u8 mPayload[0x84];
};
WWHD_SIZE(MagmaShaderRecord, 0x84);
struct MagmaShaderEntry {
    u8 mPayload[0x20];
    be<u8> mLoaded;
    u8 mPadding[3];
};
WWHD_SIZE(MagmaShaderEntry, 0x24);
struct MagmaShaderFile {
    u8 mPrefix[0x1C];
    be<u32> mCount;
    gptr<MagmaShaderRecord> mpRecords;
};
WWHD_SIZE(MagmaShaderFile, 0x24);
struct MagmaShaderArchive {
    u8 mPrefix[4];
    gptr<MagmaShaderFile> mpFile;
    be<u32> mCount;
    gptr<MagmaShaderEntry> mpEntries;
};
WWHD_SIZE(MagmaShaderArchive, 0x10);
struct MagmaResourceFile {
    u8 mPrefix[0x20];
    gptr<void> mpImage;
};
WWHD_SIZE(MagmaResourceFile, 0x24);
struct MagmaBallPositions {
    cXyz pos[33];
};
WWHD_SIZE(MagmaBallPositions, 0x18C);
struct MagmaFloorPositions {
    be<f32> pos[4][3];
};
WWHD_SIZE(MagmaFloorPositions, 0x30);
struct MagmaBallIndexData {
    be<u32> words[6];
};
WWHD_SIZE(MagmaBallIndexData, 0x18);

WWHD_OFFSET(MagmaGraphicsState, mFloor, 0xE0);
WWHD_OFFSET(MagmaGraphicsState, mBlendFlags, 0xEC);
WWHD_OFFSET(MagmaShader, mpCompiled, 0x7C);
WWHD_OFFSET(MagmaDrawContext, mpExtraObject, 0x30);

struct MagmaDestructorRecord {
    be<u32> words[3];
};
WWHD_SIZE(MagmaDestructorRecord, 12);
