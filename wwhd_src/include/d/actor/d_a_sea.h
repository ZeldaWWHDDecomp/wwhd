/* d_a_sea: the sea surface (WWHD layout). 
 *
 * GameCube -> WWHD: daSea_packet_c grew from 0x148 to 0x3A8. Its J3DPacket base is 0x98 bytes
 * (GameCube 0x10), so the CPU fields move by +0x88; the three GXTexObj became 0x40 bytes each
 * (GameCube 0x20), so everything after them moves by +0xE8. HD-only GPU objects (vertex buffers,
 * shader, material) follow at 0x22E..0x3A8. l_cloth is a function-local static (object
 * 0x1046D8D0, guard 0x1046DC78) reached through the pointer at 0x1046D8B0. */
#pragma once
#include "SSystem/SComponent/c_m2d_g_box.h"
#include "bindings.h"

struct daSea_WaterHeightInfo_Mng {
    /* 0x00 */ be<u8> mHeight[9][9];
    /* 0x51 */ u8 m51[3];
    /* 0x54 */ be<u32> __vtbl;

    int Pos2Index(f32, be<f32>*);
    int GetHeight(f32, f32);
    int GetHeight(int, int);
    void GetArea(int, int, be<f32>*, be<f32>*, be<f32>*, be<f32>*);
    void SetInf();
};
WWHD_SIZE(daSea_WaterHeightInfo_Mng, 0x58);

struct daSea_WaveInfoDat {
    /* 0x00 */ be<f32> mHeight;
    /* 0x04 */ be<f32> mKm;
    /* 0x08 */ be<s16> mPhase;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<f32> mScaleX;
    /* 0x10 */ be<f32> mScaleZ;
    /* 0x14 */ be<s32> mCounterMax;
};
WWHD_SIZE(daSea_WaveInfoDat, 0x18);

struct daSea_WaveInfo {
    /* 0x00 */ gptr<daSea_WaveInfoDat> mWaveInfoTable;
    /* 0x04 */ be<f32> m04[4];
    /* 0x14 */ be<s32> mCounters[4];
    /* 0x24 */ be<f32> mCurScale;
    /* 0x28 */ be<u32> __vtbl;

    void AddCounter();
    f32 GetRatio(int);
    f32 GetKm(int);
    f32 GetScale(f32);
};
WWHD_SIZE(daSea_WaveInfo, 0x2C);

struct daSea_packet_c {
    /* 0x000 */ u8 _000[0xC]; /* J3DPacket (HD 0x98) */
    /* 0x00C */ be<u32> __vtbl;
    /* 0x010 */ u8 _010[0x98 - 0x10];
    /* 0x098 */ daSea_WaterHeightInfo_Mng mWaterHeightMgr; /* GameCube 0x010 */
    /* 0x0F0 */ daSea_WaveInfo mWaveInfo;                  /* GameCube 0x068 */
    /* 0x11C */ cXyz mPlayerPos;
    /* 0x128 */ be<s32> mIdxX;
    /* 0x12C */ be<s32> mIdxZ;
    /* 0x130 */ be<f32> mFlatInter;
    /* 0x134 */ be<f32> mFlatTarget;
    /* 0x138 */ be<f32> mFlatInterCounter;
    /* 0x13C */ u8 mTexObj[3][0x40]; /* mTexSea0/mTexSea1/mTexYura (HD GX2 texture objects) */
    /* 0x1FC */ be<f32> mDrawMinX;   /* GameCube 0x114 */
    /* 0x200 */ be<f32> mDrawMinZ;
    /* 0x204 */ be<f32> mDrawMaxX;
    /* 0x208 */ be<f32> mDrawMaxZ;
    /* 0x20C */ be<u32> mpHeightTable; /* f32[65 * 65] */
    /* 0x210 */ cXyz mCurPos;
    /* 0x21C */ be<u32> m_draw_vtx;
    /* 0x220 */ be<u8> mInitFlag;
    /* 0x221 */ be<u8> mCullStopFlag;
    /* 0x222 */ be<u8> m13A;
    /* 0x223 */ be<u8> m13B;
    /* 0x224 */ be<s32> mRoomNo;
    /* 0x228 */ be<s32> mFlags;
    /* 0x22C */ be<s16> mAnimCounter;
    /* 0x22E */ u8 _22E[2];
    /* 0x230 */ be<u32> mpShader; /* HD: "wave_draw" shader */
    /* 0x234 */ be<u32> mIndexFormat;
    /* 0x238 */ be<u32> mIndexPrimitive;
    /* 0x23C */ be<u32> mpIndexBufferData;
    /* 0x240 */ u8 _240[4];
    /* 0x244 */ be<u32> mIndexStride;
    /* 0x248 */ u8 _248[4];
    /* 0x24C */ be<u32> mpIndexData; /* HD: 0x8200-byte buffer */
    /* 0x250 */ be<u32> mpVtxBuf;    /* HD: double-buffered vertex buffer set */
    /* 0x254 */ be<u32> mpLowerRows;
    /* 0x258 */ be<u32> mpLowerClosingRow;
    /* 0x25C */ be<u32> mpUpperRows;
    /* 0x260 */ be<u32> mpUpperClosingRow;
    /* 0x264 */ be<u32> mpLeftRows;
    /* 0x268 */ be<u32> mpLeftClosingRow;
    /* 0x26C */ be<u32> mpRightRows;
    /* 0x270 */ be<u32> mpRightClosingRow;
    /* 0x274 */ be<u32> mpMaterial; /* HD: material/uniform block */
    /* 0x278 */ be<u32> mpPrograms;
    /* 0x27C */ u8 mSeaTexture[0x90];
    /* 0x30C */ u8 mYuraTexture[0x90];
    /* 0x39C */ be<u32> mpSeaSampler;
    /* 0x3A0 */ be<u32> mpYuraSampler;
    /* 0x3A4 */ be<u32> mpSeaSamplerCopy;

    void fillVtxBuf();       // HD: GPU sea renderer.
    void setUniforms();      // HD: GPU sea renderer.
    void createBuffers();    // HD: GPU sea renderer.
    void createTex();        // HD: GPU sea renderer.
    void draw(void* i_pass); // HD: GPU sea renderer.
    bool create(cXyz*);      /* GameCube: cXyz& */
    void CleanUp();
    void SetFlat();
    void ClrFlat();
    f32 CalcFlatInterTarget(cXyz*); /* GameCube: cXyz& (harness entry takes pointers) */
    void CalcFlatInter();
    void SetCullStopFlag();
    void CheckRoomChange();
    void execute(cXyz*); /* GameCube: cXyz& */
};
WWHD_SIZE(daSea_packet_c, 0x3A8);
WWHD_OFFSET(daSea_packet_c, mWaveInfo, 0xF0);
WWHD_OFFSET(daSea_packet_c, mDrawMinX, 0x1FC);
WWHD_OFFSET(daSea_packet_c, mInitFlag, 0x220);
WWHD_OFFSET(daSea_packet_c, mAnimCounter, 0x22C);
WWHD_OFFSET(daSea_packet_c, mpMaterial, 0x274);

struct sea_class : fopAc_ac_c {};

// HD: sea renderer layout.
struct SeaVertex {
    /* 0x000 */ be<f32> mPosX;
    /* 0x004 */ be<f32> mPosY;
    /* 0x008 */ be<f32> mPosZ;
    /* 0x00C */ be<f32> mNormalX;
    /* 0x010 */ be<f32> mNormalY;
    /* 0x014 */ be<f32> mNormalZ;
    /* 0x018 */ be<f32> mTexU;
    /* 0x01C */ be<f32> mTexV;
};
WWHD_SIZE(SeaVertex, 0x20);
WWHD_OFFSET(SeaVertex, mPosX, 0x0);
WWHD_OFFSET(SeaVertex, mPosY, 0x4);
WWHD_OFFSET(SeaVertex, mPosZ, 0x8);
WWHD_OFFSET(SeaVertex, mNormalX, 0xC);
WWHD_OFFSET(SeaVertex, mNormalY, 0x10);
WWHD_OFFSET(SeaVertex, mNormalZ, 0x14);
WWHD_OFFSET(SeaVertex, mTexU, 0x18);
WWHD_OFFSET(SeaVertex, mTexV, 0x1C);

// HD: sea renderer layout.
struct SeaVertexEntry {
    /* 0x000 */ be<u32> mpData;
    u8 _pad4[0x14C];
    /* 0x150 */ be<u32> mBufferSize;
    u8 _pad154[0xF8];
    /* 0x24C */ be<u32> mVertexCount;
    /* 0x250 */ be<u32> mpAllocatedData;
};
WWHD_SIZE(SeaVertexEntry, 0x254);
WWHD_OFFSET(SeaVertexEntry, mpData, 0x0);
WWHD_OFFSET(SeaVertexEntry, mBufferSize, 0x150);
WWHD_OFFSET(SeaVertexEntry, mVertexCount, 0x24C);
WWHD_OFFSET(SeaVertexEntry, mpAllocatedData, 0x250);

// HD: sea renderer layout.
struct SeaBufferSet {
    u8 _pad0[0x4A8];
    /* 0x4A8 */ be<u32> mCurrentBuffer;
    /* 0x4AC */ be<u32> mFormat;
    /* 0x4B0 */ be<u32> mStride;
    /* 0x4B4 */ be<u32> mpAttributes;
    /* 0x4B8 */ be<u32> mpShader;
    /* 0x4BC */ be<u8> mReady;
    u8 _pad4BD[0x3];
};
WWHD_SIZE(SeaBufferSet, 0x4C0);
WWHD_OFFSET(SeaBufferSet, mCurrentBuffer, 0x4A8);
WWHD_OFFSET(SeaBufferSet, mFormat, 0x4AC);
WWHD_OFFSET(SeaBufferSet, mStride, 0x4B0);
WWHD_OFFSET(SeaBufferSet, mpAttributes, 0x4B4);
WWHD_OFFSET(SeaBufferSet, mpShader, 0x4B8);
WWHD_OFFSET(SeaBufferSet, mReady, 0x4BC);

// HD: sea renderer layout.
struct SeaRowBufferSet {
    u8 _pad0[0x2E90];
    /* 0x2E90 */ be<u32> mCurrentBuffer;
    /* 0x2E94 */ be<u32> mFormat;
    /* 0x2E98 */ be<u32> mStride;
    /* 0x2E9C */ be<u32> mpAttributes;
    /* 0x2EA0 */ be<u32> mpShader;
    /* 0x2EA4 */ be<u8> mReady;
    u8 _pad2EA5[0x3];
};
WWHD_SIZE(SeaRowBufferSet, 0x2EA8);
WWHD_OFFSET(SeaRowBufferSet, mCurrentBuffer, 0x2E90);
WWHD_OFFSET(SeaRowBufferSet, mFormat, 0x2E94);
WWHD_OFFSET(SeaRowBufferSet, mStride, 0x2E98);
WWHD_OFFSET(SeaRowBufferSet, mpAttributes, 0x2E9C);
WWHD_OFFSET(SeaRowBufferSet, mpShader, 0x2EA0);
WWHD_OFFSET(SeaRowBufferSet, mReady, 0x2EA4);

// HD: sea renderer layout.
struct SeaSampler {
    u8 _pad0[0x150];
    /* 0x150 */ be<u32> mWrapS;
    /* 0x154 */ be<u32> mWrapT;
    /* 0x158 */ be<u32> mWrapR;
    /* 0x15C */ be<u32> mMinFilter;
    /* 0x160 */ be<u32> mMagFilter;
    /* 0x164 */ be<u32> mMipFilter;
    u8 _pad168[0x1C];
    /* 0x184 */ be<u32> mAnisotropy;
    u8 _pad188[0x8];
    /* 0x190 */ be<u8> mDirtyFlags;
    u8 _pad191[0x3];
};
WWHD_SIZE(SeaSampler, 0x194);
WWHD_OFFSET(SeaSampler, mWrapS, 0x150);
WWHD_OFFSET(SeaSampler, mWrapT, 0x154);
WWHD_OFFSET(SeaSampler, mWrapR, 0x158);
WWHD_OFFSET(SeaSampler, mMinFilter, 0x15C);
WWHD_OFFSET(SeaSampler, mMagFilter, 0x160);
WWHD_OFFSET(SeaSampler, mMipFilter, 0x164);
WWHD_OFFSET(SeaSampler, mAnisotropy, 0x184);
WWHD_OFFSET(SeaSampler, mDirtyFlags, 0x190);

// HD: sea renderer layout.
struct SeaProgramList {
    /* 0x000 */ be<u32> mCount;
    /* 0x004 */ be<u32> mCapacity;
    /* 0x008 */ be<u32> mpEntries;
};
WWHD_SIZE(SeaProgramList, 0xC);
WWHD_OFFSET(SeaProgramList, mCount, 0x0);
WWHD_OFFSET(SeaProgramList, mCapacity, 0x4);
WWHD_OFFSET(SeaProgramList, mpEntries, 0x8);

// HD: sea renderer layout.
struct SeaProgramEntry {
    /* 0x000 */ be<u32> mpProgram;
    /* 0x004 */ be<u32> mFirstAttributeCount;
    /* 0x008 */ be<u32> mpFirstAttributes;
    /* 0x00C */ be<u32> mSecondAttributeCount;
    /* 0x010 */ be<u32> mpSecondAttributes;
};
WWHD_SIZE(SeaProgramEntry, 0x14);
WWHD_OFFSET(SeaProgramEntry, mpProgram, 0x0);
WWHD_OFFSET(SeaProgramEntry, mFirstAttributeCount, 0x4);
WWHD_OFFSET(SeaProgramEntry, mpFirstAttributes, 0x8);
WWHD_OFFSET(SeaProgramEntry, mSecondAttributeCount, 0xC);
WWHD_OFFSET(SeaProgramEntry, mpSecondAttributes, 0x10);

// HD: sea renderer layout.
struct SeaProgram {
    /* 0x000 */ be<u32> mpShader;
    u8 _pad4[0x8];
    /* 0x00C */ be<u32> mUniformCount;
    /* 0x010 */ be<u32> mpUniforms;
    /* 0x014 */ be<u32> mSamplerCount;
    /* 0x018 */ be<u32> mpSamplers;
    u8 _pad1C[0x4];
    /* 0x020 */ be<u8> mReady;
    u8 _pad21[0x3];
};
WWHD_SIZE(SeaProgram, 0x24);
WWHD_OFFSET(SeaProgram, mpShader, 0x0);
WWHD_OFFSET(SeaProgram, mUniformCount, 0xC);
WWHD_OFFSET(SeaProgram, mpUniforms, 0x10);
WWHD_OFFSET(SeaProgram, mSamplerCount, 0x14);
WWHD_OFFSET(SeaProgram, mpSamplers, 0x18);
WWHD_OFFSET(SeaProgram, mReady, 0x20);

// HD: sea renderer layout.
struct SeaShader {
    /* 0x000 */ be<u8> mFlags;
    u8 _pad1[0x3];
    /* 0x004 */ be<u32> mpDisplayList;
    u8 _pad8[0x4];
    /* 0x00C */ be<u32> mDisplayListSize;
    u8 _pad10[0x6C];
    /* 0x07C */ be<u32> mpFetchShaderInfo;
};
WWHD_SIZE(SeaShader, 0x80);
WWHD_OFFSET(SeaShader, mFlags, 0x0);
WWHD_OFFSET(SeaShader, mpDisplayList, 0x4);
WWHD_OFFSET(SeaShader, mDisplayListSize, 0xC);
WWHD_OFFSET(SeaShader, mpFetchShaderInfo, 0x7C);

// HD: sea renderer layout.
struct SeaShaderState {
    /* 0x000 */ be<u32> mpFetchShader;
    /* 0x004 */ be<u32> mpShader;
};
WWHD_SIZE(SeaShaderState, 0x8);
WWHD_OFFSET(SeaShaderState, mpFetchShader, 0x0);
WWHD_OFFSET(SeaShaderState, mpShader, 0x4);

// HD: sea renderer layout.
struct SeaDrawPass {
    u8 _pad0[0xC];
    /* 0x00C */ be<s32> mPass;
    u8 _pad10[0x20];
    /* 0x030 */ be<u32> mpMaterial;
};
WWHD_SIZE(SeaDrawPass, 0x34);
WWHD_OFFSET(SeaDrawPass, mPass, 0xC);
WWHD_OFFSET(SeaDrawPass, mpMaterial, 0x30);

// HD: sea renderer layout.
struct SeaShaderManager {
    u8 _pad0[0x4];
    /* 0x004 */ be<u32> mpArchive;
    /* 0x008 */ be<u32> mCount;
    /* 0x00C */ be<u32> mpPrograms;
};
WWHD_SIZE(SeaShaderManager, 0x10);
WWHD_OFFSET(SeaShaderManager, mpArchive, 0x4);
WWHD_OFFSET(SeaShaderManager, mCount, 0x8);
WWHD_OFFSET(SeaShaderManager, mpPrograms, 0xC);

// HD: sea renderer layout.
struct SeaShaderArchive {
    u8 _pad0[0x1C];
    /* 0x01C */ be<u32> mCount;
    /* 0x020 */ be<u32> mpEntries;
};
WWHD_SIZE(SeaShaderArchive, 0x24);
WWHD_OFFSET(SeaShaderArchive, mCount, 0x1C);
WWHD_OFFSET(SeaShaderArchive, mpEntries, 0x20);

// HD: sea renderer layout.
struct SeaGfxState {
    /* 0x000 */ be<u8> mPolygonDirty;
    /* 0x001 */ be<u8> mDepthDirty;
    u8 _pad2[0x2];
    /* 0x004 */ be<u32> mCullMode;
    /* 0x008 */ be<u32> mPolygonMode;
    /* 0x00C */ be<u32> mDepthMode;
    u8 _pad10[0xD0];
    /* 0x0E0 */ be<u8> mBlendEnable;
    u8 _padE1[0xB];
    /* 0x0EC */ be<u32> mDepthControl;
    u8 _padF0[0x24];
    /* 0x114 */ be<u8> mDepthTest;
    /* 0x115 */ be<u8> mDepthWrite;
    /* 0x116 */ be<u8> mDepthCompare;
    u8 _pad117[0x5];
};
WWHD_SIZE(SeaGfxState, 0x11C);
WWHD_OFFSET(SeaGfxState, mPolygonDirty, 0x0);
WWHD_OFFSET(SeaGfxState, mDepthDirty, 0x1);
WWHD_OFFSET(SeaGfxState, mCullMode, 0x4);
WWHD_OFFSET(SeaGfxState, mPolygonMode, 0x8);
WWHD_OFFSET(SeaGfxState, mDepthMode, 0xC);
WWHD_OFFSET(SeaGfxState, mBlendEnable, 0xE0);
WWHD_OFFSET(SeaGfxState, mDepthControl, 0xEC);
WWHD_OFFSET(SeaGfxState, mDepthTest, 0x114);
WWHD_OFFSET(SeaGfxState, mDepthWrite, 0x115);
WWHD_OFFSET(SeaGfxState, mDepthCompare, 0x116);

// HD: sea renderer layout.
struct SeaPolygonOffset {
    u8 _pad0[0x14];
};
WWHD_SIZE(SeaPolygonOffset, 0x14);

struct SeaColor4f {
    be<f32> r, g, b, a;
};
WWHD_SIZE(SeaColor4f, 0x10);

// HD: sea rendering and stage view.
struct SeaStageMulti {
    /* 0x000 */ be<s32> mCount;
    /* 0x004 */ be<u32> mpEntries;
};
WWHD_SIZE(SeaStageMulti, 0x8);
WWHD_OFFSET(SeaStageMulti, mCount, 0x0);
WWHD_OFFSET(SeaStageMulti, mpEntries, 0x4);

// HD: sea rendering and stage view.
struct SeaStageMultiEntry {
    u8 _pad0[0xA];
    /* 0x00A */ be<u8> mRoomNo;
    /* 0x00B */ be<u8> mWaveMax;
};
WWHD_SIZE(SeaStageMultiEntry, 0xC);
WWHD_OFFSET(SeaStageMultiEntry, mRoomNo, 0xA);
WWHD_OFFSET(SeaStageMultiEntry, mWaveMax, 0xB);

// HD: sea rendering and stage view.
struct SeaStageInfo {
    u8 _pad0[0xC];
    /* 0x00C */ be<u32> mFlags;
};
WWHD_SIZE(SeaStageInfo, 0x10);
WWHD_OFFSET(SeaStageInfo, mFlags, 0xC);

// HD: sea rendering and stage view.
struct SeaStageData {
    /* 0x000 */ be<u32> mpVtable;
};
WWHD_SIZE(SeaStageData, 0x4);
WWHD_OFFSET(SeaStageData, mpVtable, 0x0);

// HD: sea rendering and stage view.
struct SeaStageVtable {
    u8 _pad0[0x20C];
    /* 0x20C */ be<u32> mGetMulti;
};
WWHD_SIZE(SeaStageVtable, 0x210);
WWHD_OFFSET(SeaStageVtable, mGetMulti, 0x20C);

// HD: sea rendering and stage view.
struct SeaHeap {
    u8 _pad0[0xC];
    /* 0x00C */ be<u32> mpVtable;
};
WWHD_SIZE(SeaHeap, 0x10);
WWHD_OFFSET(SeaHeap, mpVtable, 0xC);

// HD: sea rendering and stage view.
struct SeaHeapVtable {
    u8 _pad0[0x34];
    /* 0x034 */ be<u32> mAlloc;
    u8 _pad38[0x4];
    /* 0x03C */ be<u32> mFree;
};
WWHD_SIZE(SeaHeapVtable, 0x40);
WWHD_OFFSET(SeaHeapVtable, mAlloc, 0x34);
WWHD_OFFSET(SeaHeapVtable, mFree, 0x3C);

// HD: sea rendering and stage view.
struct SeaFetchShader {
    u8 _pad0[0xF0];
    /* 0x0F0 */ be<u32> mpVtable;
};
WWHD_SIZE(SeaFetchShader, 0xF4);
WWHD_OFFSET(SeaFetchShader, mpVtable, 0xF0);

// HD: sea rendering and stage view.
struct SeaFetchShaderVtable {
    u8 _pad0[0xC];
    /* 0x00C */ be<u32> mDestroy;
};
WWHD_SIZE(SeaFetchShaderVtable, 0x10);
WWHD_OFFSET(SeaFetchShaderVtable, mDestroy, 0xC);

// HD: sea rendering and stage view.
struct SeaBufferAllocation {
    /* 0x000 */ be<u32> mCount;
    /* 0x004 */ be<u32> mpData;
};
WWHD_SIZE(SeaBufferAllocation, 0x8);
WWHD_OFFSET(SeaBufferAllocation, mCount, 0x0);
WWHD_OFFSET(SeaBufferAllocation, mpData, 0x4);

// HD: sea rendering and stage view.
struct SeaAttributeArray {
    /* 0x000 */ be<s32> mCount;
    /* 0x004 */ be<u32> mpEntries;
};
WWHD_SIZE(SeaAttributeArray, 0x8);
WWHD_OFFSET(SeaAttributeArray, mCount, 0x0);
WWHD_OFFSET(SeaAttributeArray, mpEntries, 0x4);

// HD: sea rendering and stage view.
struct SeaTextureResource {
    u8 _pad0[0x20];
    /* 0x020 */ be<u32> mpImage;
};
WWHD_SIZE(SeaTextureResource, 0x24);
WWHD_OFFSET(SeaTextureResource, mpImage, 0x20);

// HD: sea rendering and stage view.
struct SeaFetchShaderInfo {
    u8 _pad0[0x28];
    /* 0x028 */ be<u32> mpShader;
};
WWHD_SIZE(SeaFetchShaderInfo, 0x2C);
WWHD_OFFSET(SeaFetchShaderInfo, mpShader, 0x28);

// HD: sea rendering and stage view.
struct SeaUniform {
    u8 _pad0[0xC];
    /* 0x00C */ be<s16> mVertexLocation;
    /* 0x00E */ be<s16> mPixelLocation;
    /* 0x010 */ be<s16> mGeometryLocation;
    u8 _pad12[0x2];
};
WWHD_SIZE(SeaUniform, 0x14);
WWHD_OFFSET(SeaUniform, mVertexLocation, 0xC);
WWHD_OFFSET(SeaUniform, mPixelLocation, 0xE);
WWHD_OFFSET(SeaUniform, mGeometryLocation, 0x10);

// HD: sea rendering and stage view.
struct SeaUniformBlock {
    u8 _pad0[0x4];
    /* 0x004 */ be<u32> mpData;
    u8 _pad8[0x4];
    /* 0x00C */ be<u32> mSize;
    u8 _pad10[0xC];
};
WWHD_SIZE(SeaUniformBlock, 0x1C);
WWHD_OFFSET(SeaUniformBlock, mpData, 0x4);
WWHD_OFFSET(SeaUniformBlock, mSize, 0xC);

// HD: sea rendering and stage view.
struct SeaPassMaterial {
    u8 _pad0[0x4C];
    /* 0x04C */ be<u32> mCurrentUniformBlock;
};
WWHD_SIZE(SeaPassMaterial, 0x50);
WWHD_OFFSET(SeaPassMaterial, mCurrentUniformBlock, 0x4C);

// HD: sea rendering and stage view.
struct SeaEnvLight {
    u8 _pad0[0x10B8];
    /* 0x10B8 */ be<f32> mSeaColorScale;
    /* 0x10BC */ be<f32> mSeaUnderwaterColorScale;
};
WWHD_SIZE(SeaEnvLight, 0x10C0);
WWHD_OFFSET(SeaEnvLight, mSeaColorScale, 0x10B8);
WWHD_OFFSET(SeaEnvLight, mSeaUnderwaterColorScale, 0x10BC);

// HD: sea rendering and stage view.
struct SeaDaiocta {
    u8 _pad0[0x690];
    /* 0x690 */ be<u8> mSwitch;
    u8 _pad691[0x3];
};
WWHD_SIZE(SeaDaiocta, 0x694);
WWHD_OFFSET(SeaDaiocta, mSwitch, 0x690);

// HD: sea rendering and stage view.
struct SeaTextureMatrix {
    u8 _pad0[0x30];
    /* 0x030 */ be<u32> mMode;
};
WWHD_SIZE(SeaTextureMatrix, 0x34);
WWHD_OFFSET(SeaTextureMatrix, mMode, 0x30);

// HD: sea rendering and stage view.
struct SeaIndirectMatrix {
    u8 _pad0[0xC];
    be<u32> mVtable;
    u8 _pad10[0x64];
};
WWHD_SIZE(SeaIndirectMatrix, 0x74);
WWHD_OFFSET(SeaIndirectMatrix, mVtable, 0xC);
struct SeaMaterial {
    u8 _pad0[0x18];
    /* 0x018 */ be<u32> mTextureMatrixVtable;
    u8 _pad1C[0x64];
    /* 0x080 */ SeaTextureMatrix mTextureMatrix;
    /* 0x0B4 */ SeaIndirectMatrix mIndirectMatrix;
    /* 0x128 */ be<f32> mUniforms[0xBC];
    /* 0x418 */ be<u8> mFlags;
    u8 _pad419[3];
};
WWHD_SIZE(SeaMaterial, 0x41C);
WWHD_OFFSET(SeaMaterial, mTextureMatrixVtable, 0x18);
WWHD_OFFSET(SeaMaterial, mIndirectMatrix, 0xB4);
WWHD_OFFSET(SeaMaterial, mUniforms, 0x128);
WWHD_OFFSET(SeaMaterial, mFlags, 0x418);

// HD: sea rendering and stage view.
struct SeaJ3DSys {
    u8 _pad0[0x38];
    /* 0x038 */ be<f32> mViewMtx[12];
    u8 _pad68[0xE0];
    /* 0x148 */ be<u32> mpCamera;
    /* 0x14C */ be<u32> mProjection[16];
};
WWHD_SIZE(SeaJ3DSys, 0x18C);
WWHD_OFFSET(SeaJ3DSys, mpCamera, 0x148);

// HD: active opaque/translucent draw buffers in the renderer.
struct SeaDrawBuffers {
    u8 _pad0[0x74];
    /* 0x074 */ be<u32> mBuffers[2];
};
WWHD_SIZE(SeaDrawBuffers, 0x7C);
WWHD_OFFSET(SeaDrawBuffers, mBuffers, 0x74);
struct SeaPlayDrawLists {
    u8 _pad0[0x5D78];
    /* 0x5D78 */ be<u32> mpOpaque;
    /* 0x5D7C */ be<u32> mpTranslucent;
    u8 _pad5D80[0x20];
    /* 0x5DA0 */ be<u32> mpSky;
};
WWHD_SIZE(SeaPlayDrawLists, 0x5DA4);
WWHD_OFFSET(SeaPlayDrawLists, mpSky, 0x5DA0);
struct SeaDrawIndices {
    /* 0x00 */ be<u32> mFormat;
    /* 0x04 */ be<u32> mPrimitive;
    /* 0x08 */ be<u32> mpData;
};
WWHD_SIZE(SeaDrawIndices, 0xC);
WWHD_OFFSET(SeaDrawIndices, mpData, 8);
struct SeaHeaderStatics {
    /* 0x00 */ be<u32> mCloth;
    /* 0x04 */ be<f32> mAngleMin;
    /* 0x08 */ be<f32> mAngleMax;
    /* 0x0C */ u8 mAngleHelper;
    /* 0x0D */ u8 mRangeHelper;
    u8 _padE[2];
    /* 0x10 */ be<u32> mWords[4];
};
WWHD_SIZE(SeaHeaderStatics, 0x20);
WWHD_OFFSET(SeaHeaderStatics, mWords, 0x10);
enum { fpcNm_DAIOCTA_e = 0xE1 };
enum SeaTextureIndex { SEA_TEXTURE_SEA = 0x6F, SEA_TEXTURE_YURA = 0x70 };
