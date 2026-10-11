// Chain packet: GameCube chain members with HD shader, GPU and renderer storage.
// Layouts preserve the verified GHS allocation and cleanup sequence.
#pragma once
#include "bindings.h"
struct ChainShaderBlockHD {
    u8 _pad0[0xF0];
    /* 0xF0 */ be<u32> mVtable;
};
WWHD_SIZE(ChainShaderBlockHD, 0xF4);
WWHD_OFFSET(ChainShaderBlockHD, mVtable, 0xF0);
struct ChainResourceHD {
    /* 0x0 */ be<u32> mpData;
    /* 0x4 */ u8 mVertexBuffer[0x154];
    /* 0x158 */ ChainShaderBlockHD mShaderBlock;
    /* 0x24C */ be<u32> mCount;
    /* 0x250 */ be<u32> mpAllocation;
};
WWHD_SIZE(ChainResourceHD, 0x254);
WWHD_OFFSET(ChainResourceHD, mpData, 0x0);
WWHD_OFFSET(ChainResourceHD, mShaderBlock, 0x158);
WWHD_OFFSET(ChainResourceHD, mCount, 0x24C);
WWHD_OFFSET(ChainResourceHD, mpAllocation, 0x250);
struct ChainResourcesHD {
    /* 0x0 */ ChainResourceHD mResource[2];
    /* 0x4A8 */ be<u32> mActive;
    /* 0x4AC */ be<u32> mFormat;
    /* 0x4B0 */ be<u32> mAlignment;
    /* 0x4B4 */ be<u32> mpName;
    /* 0x4B8 */ be<u32> mDirty;
    /* 0x4BC */ be<u8> mReady;
    u8 _pad4BD[0x3];
};
WWHD_SIZE(ChainResourcesHD, 0x4C0);
WWHD_OFFSET(ChainResourcesHD, mActive, 0x4A8);
WWHD_OFFSET(ChainResourcesHD, mFormat, 0x4AC);
WWHD_OFFSET(ChainResourcesHD, mAlignment, 0x4B0);
WWHD_OFFSET(ChainResourcesHD, mpName, 0x4B4);
WWHD_OFFSET(ChainResourcesHD, mDirty, 0x4B8);
WWHD_OFFSET(ChainResourcesHD, mReady, 0x4BC);
struct ChainIndexBufferHD {
    /* 0x0 */ be<u32> mPrimitive;
    /* 0x4 */ be<u32> mIndexType;
    /* 0x8 */ be<u32> mpIndices;
    /* 0xC */ be<u32> mNumIndices;
    u8 _pad10[0x8];
};
WWHD_SIZE(ChainIndexBufferHD, 0x18);
WWHD_OFFSET(ChainIndexBufferHD, mPrimitive, 0x0);
WWHD_OFFSET(ChainIndexBufferHD, mIndexType, 0x4);
WWHD_OFFSET(ChainIndexBufferHD, mpIndices, 0x8);
WWHD_OFFSET(ChainIndexBufferHD, mNumIndices, 0xC);
struct ChainShaderEntryHD {
    /* 0x0 */ be<u32> mpShader;
    /* 0x4 */ be<u32> mFirstCount;
    /* 0x8 */ be<u32> mpFirstBlocks;
    /* 0xC */ be<u32> mSecondCount;
    /* 0x10 */ be<u32> mpSecondBlocks;
};
WWHD_SIZE(ChainShaderEntryHD, 0x14);
WWHD_OFFSET(ChainShaderEntryHD, mpShader, 0x0);
WWHD_OFFSET(ChainShaderEntryHD, mFirstCount, 0x4);
WWHD_OFFSET(ChainShaderEntryHD, mpFirstBlocks, 0x8);
WWHD_OFFSET(ChainShaderEntryHD, mSecondCount, 0xC);
WWHD_OFFSET(ChainShaderEntryHD, mpSecondBlocks, 0x10);
struct ChainDrawPacketHD {
    u8 _pad0[0xC];
    /* 0xC */ be<u32> mVtable;
    u8 _pad10[0x64];
    /* 0x74 */ be<f32> mUniforms[44];
    /* 0x124 */ u8 mColors[0x60];
    u8 _pad184[0x1E0];
};
WWHD_SIZE(ChainDrawPacketHD, 0x364);
WWHD_OFFSET(ChainDrawPacketHD, mVtable, 0xC);
struct ChainShapeHD {
    /* 0x0 */ be<u32> mLayoutWords[16];
    u8 _pad40[0x94];
    /* 0xD4 */ be<u32> mpFirstBinding;
    u8 _padD8[0x4];
    /* 0xDC */ be<u32> mpLastBinding;
    u8 _padE0[0x7C];
    /* 0x15C */ be<u32> mDirtyPosition;
    /* 0x160 */ be<u32> mDirtyNormal;
    /* 0x164 */ be<u32> mDirtyTexCoord;
    u8 _pad168[0x28];
    /* 0x190 */ be<u8> mFlags;
    u8 _pad191[0x7];
};
WWHD_SIZE(ChainShapeHD, 0x198);
WWHD_OFFSET(ChainShapeHD, mpFirstBinding, 0xD4);
WWHD_OFFSET(ChainShapeHD, mpLastBinding, 0xDC);
WWHD_OFFSET(ChainShapeHD, mDirtyPosition, 0x15C);
WWHD_OFFSET(ChainShapeHD, mDirtyNormal, 0x160);
WWHD_OFFSET(ChainShapeHD, mDirtyTexCoord, 0x164);
WWHD_OFFSET(ChainShapeHD, mFlags, 0x190);
struct ChainTemporaryShapeHD {
    /* 0x0 */ be<u32> mLayoutWords[16];
    u8 _pad40[0x50];
};
WWHD_SIZE(ChainTemporaryShapeHD, 0x90);
struct ChainPassHD {
    u8 _pad0[0xC];
    /* 0xC */ be<u32> mMode;
    u8 _pad10[0x4];
    /* 0x14 */ be<u32> mpShape;
    u8 _pad18[0x18];
    /* 0x30 */ be<u32> mpAdditionalPacket;
};
WWHD_SIZE(ChainPassHD, 0x34);
WWHD_OFFSET(ChainPassHD, mMode, 0xC);
WWHD_OFFSET(ChainPassHD, mpShape, 0x14);
WWHD_OFFSET(ChainPassHD, mpAdditionalPacket, 0x30);
struct ChainShaderHD {
    /* 0x0 */ be<u32> mpProgram;
    u8 _pad4[0x8];
    /* 0xC */ be<u32> mAttributeCount;
    /* 0x10 */ be<u32> mpAttributes;
    /* 0x14 */ be<u32> mUniformCount;
    /* 0x18 */ be<u32> mpUniforms;
};
WWHD_SIZE(ChainShaderHD, 0x1C);
WWHD_OFFSET(ChainShaderHD, mpProgram, 0x0);
WWHD_OFFSET(ChainShaderHD, mAttributeCount, 0xC);
WWHD_OFFSET(ChainShaderHD, mpAttributes, 0x10);
WWHD_OFFSET(ChainShaderHD, mUniformCount, 0x14);
WWHD_OFFSET(ChainShaderHD, mpUniforms, 0x18);
struct ChainProgramHD {
    /* 0x0 */ be<u8> mFlags;
    u8 _pad1[0x3];
    /* 0x4 */ be<u32> mpInstructions;
    u8 _pad8[0x4];
    /* 0xC */ be<u32> mInstructionSize;
    u8 _pad10[0x6C];
    /* 0x7C */ be<u32> mpResource;
};
WWHD_SIZE(ChainProgramHD, 0x80);
WWHD_OFFSET(ChainProgramHD, mFlags, 0x0);
WWHD_OFFSET(ChainProgramHD, mpInstructions, 0x4);
WWHD_OFFSET(ChainProgramHD, mInstructionSize, 0xC);
WWHD_OFFSET(ChainProgramHD, mpResource, 0x7C);
struct ChainProgramResourceHD {
    u8 _pad0[0x28];
    /* 0x28 */ be<u32> mpBinary;
};
WWHD_SIZE(ChainProgramResourceHD, 0x2C);
WWHD_OFFSET(ChainProgramResourceHD, mpBinary, 0x28);
struct ChainGraphicsHD {
    /* 0x0 */ be<u32> mpCurrentBinary;
    /* 0x4 */ be<u32> mpCurrentProgram;
    u8 _pad8[0x30];
    /* 0x38 */ u8 mViewMtx[48];
    u8 _pad68[0xE0];
    /* 0x148 */ be<u32> mpCamera;
    /* 0x14C */ u8 mProjection[64];
    u8 _pad18C[0x200];
};
WWHD_SIZE(ChainGraphicsHD, 0x38C);
WWHD_OFFSET(ChainGraphicsHD, mpCurrentBinary, 0x0);
WWHD_OFFSET(ChainGraphicsHD, mpCurrentProgram, 0x4);
WWHD_OFFSET(ChainGraphicsHD, mpCamera, 0x148);
struct ChainRenderState {
    u8 _pad0[0xC];
    /* 0xC */ be<u32> mCullMode;
    u8 _pad10[0xD0];
    /* 0xE0 */ be<u8> mDepthTest;
    u8 _padE1[0xB];
    /* 0xEC */ be<u32> mBlendFlags;
    u8 _padF0[0x2C];
};
WWHD_SIZE(ChainRenderState, 0x11C);
WWHD_OFFSET(ChainRenderState, mCullMode, 0xC);
WWHD_OFFSET(ChainRenderState, mDepthTest, 0xE0);
WWHD_OFFSET(ChainRenderState, mBlendFlags, 0xEC);
struct ChainString {
    /* 0x0 */ be<u32> mpText;
    /* 0x4 */ be<u32> mVtable;
};
WWHD_SIZE(ChainString, 0x8);
WWHD_OFFSET(ChainString, mpText, 0x0);
WWHD_OFFSET(ChainString, mVtable, 0x4);
struct ChainHeapHD {
    u8 _pad0[0xC];
    /* 0xC */ be<u32> mVtable;
};
WWHD_SIZE(ChainHeapHD, 0x10);
WWHD_OFFSET(ChainHeapHD, mVtable, 0xC);
struct ChainHeapVtableHD {
    u8 _pad0[0x34];
    /* 0x34 */ be<u32> mAlloc;
    u8 _pad38[0x4];
    /* 0x3C */ be<u32> mFree;
};
WWHD_SIZE(ChainHeapVtableHD, 0x40);
WWHD_OFFSET(ChainHeapVtableHD, mAlloc, 0x34);
WWHD_OFFSET(ChainHeapVtableHD, mFree, 0x3C);
struct ChainPacketVtableHD {
    u8 _pad0[0xC];
    /* 0xC */ be<u32> mDestroy;
    u8 _pad10[0x1C];
    /* 0x2C */ be<u32> mBindShader;
};
WWHD_SIZE(ChainPacketVtableHD, 0x30);
WWHD_OFFSET(ChainPacketVtableHD, mDestroy, 0xC);
WWHD_OFFSET(ChainPacketVtableHD, mBindShader, 0x2C);
struct ChainShaderManagerHD {
    u8 _pad0[0x4];
    /* 0x4 */ be<u32> mpArchive;
    /* 0x8 */ be<u32> mShaderCount;
    /* 0xC */ be<u32> mpShaders;
};
WWHD_SIZE(ChainShaderManagerHD, 0x10);
WWHD_OFFSET(ChainShaderManagerHD, mpArchive, 0x4);
WWHD_OFFSET(ChainShaderManagerHD, mShaderCount, 0x8);
WWHD_OFFSET(ChainShaderManagerHD, mpShaders, 0xC);
struct ChainShaderArchiveHD {
    u8 _pad0[0x1C];
    /* 0x1C */ be<u32> mCount;
    /* 0x20 */ be<u32> mpEntries;
};
WWHD_SIZE(ChainShaderArchiveHD, 0x24);
WWHD_OFFSET(ChainShaderArchiveHD, mCount, 0x1C);
WWHD_OFFSET(ChainShaderArchiveHD, mpEntries, 0x20);
struct ChainShaderObjectHD {
    u8 _pad0[0x20];
    /* 0x20 */ be<u8> mLoaded;
    u8 _pad21[0x3];
};
WWHD_SIZE(ChainShaderObjectHD, 0x24);
WWHD_OFFSET(ChainShaderObjectHD, mLoaded, 0x20);
struct ChainVertexHD {
    u8 _pad0[0x4C];
    /* 0x4C */ be<u32> mActive;
};
WWHD_SIZE(ChainVertexHD, 0x50);
WWHD_OFFSET(ChainVertexHD, mActive, 0x4C);
struct ChainVertexBufferHD {
    u8 _pad0[0x4];
    /* 0x4 */ be<u32> mStride;
    u8 _pad8[0x4];
    /* 0xC */ be<u32> mpBuffer;
    u8 _pad10[0xC];
};
WWHD_SIZE(ChainVertexBufferHD, 0x1C);
WWHD_OFFSET(ChainVertexBufferHD, mStride, 0x4);
WWHD_OFFSET(ChainVertexBufferHD, mpBuffer, 0xC);
struct ChainAttributeLocationsHD {
    u8 _pad0[0xC];
    /* 0xC */ be<s16> mPosition;
    /* 0xE */ be<s16> mNormal;
    /* 0x10 */ be<s16> mTexCoord;
};
WWHD_SIZE(ChainAttributeLocationsHD, 0x12);
WWHD_OFFSET(ChainAttributeLocationsHD, mPosition, 0xC);
WWHD_OFFSET(ChainAttributeLocationsHD, mNormal, 0xE);
WWHD_OFFSET(ChainAttributeLocationsHD, mTexCoord, 0x10);
struct ChainTevHD {
    u8 _pad0[0x24];
    /* 0x24 */ be<f32> mAlpha;
    /* 0x28 */ be<f32> mBrightness;
    u8 _pad2C[0x64];
    /* 0x90 */ be<s16> mColorC0[4];
    /* 0x98 */ be<u8> mColorK0[4];
    u8 _pad9C[0xC4];
    /* 0x160 */ be<s16> mLightColor[4];
    u8 _pad168[0x4];
    /* 0x16C */ be<f32> mLightBrightness;
};
WWHD_SIZE(ChainTevHD, 0x170);
WWHD_OFFSET(ChainTevHD, mAlpha, 0x24);
WWHD_OFFSET(ChainTevHD, mBrightness, 0x28);
WWHD_OFFSET(ChainTevHD, mLightBrightness, 0x16C);
struct ChainVector {
    be<f32> x, y, z;
};
WWHD_SIZE(ChainVector, 12);
struct ChainColor {
    be<f32> r, g, b, a;
};
WWHD_SIZE(ChainColor, 16);
struct ChainMatrix {
    be<f32> m[12];
};
WWHD_SIZE(ChainMatrix, 48);
struct ChainOwnerHD {
    u8 _pad0[0xC];
    /* 0xC */ be<u32> mVtable;
    u8 _pad10[0x88];
    /* 0x98 */ be<u32> mpTevStr;
    /* 0x9C */ ChainVector mScale;
    /* 0xA8 */ be<u32> mNum;
    /* 0xAC */ be<u32> mPt;
    /* 0xB0 */ be<u32> mShaderCount;
    /* 0xB4 */ be<u32> mShaderCapacity;
    /* 0xB8 */ be<u32> mpShaderEntries;
    /* 0xBC */ be<u32> mPacketCount;
    /* 0xC0 */ be<u32> mpPackets;
    u8 _padC4[0x4];
    /* 0xC8 */ u8 mSegmentPackets[0xA800];
    /* 0xA8C8 */ ChainDrawPacketHD mUniformPacket;
    /* 0xAC2C */ ChainResourcesHD mResources;
    /* 0xB0EC */ ChainIndexBufferHD mIndices;
    /* 0xB104 */ ChainShapeHD mShape;
    /* 0xB29C */ ChainTemporaryShapeHD mTemporaryShape;
};
WWHD_SIZE(ChainOwnerHD, 0xB32C);
WWHD_OFFSET(ChainOwnerHD, mVtable, 0xC);
WWHD_OFFSET(ChainOwnerHD, mpTevStr, 0x98);
WWHD_OFFSET(ChainOwnerHD, mScale, 0x9C);
WWHD_OFFSET(ChainOwnerHD, mNum, 0xA8);
WWHD_OFFSET(ChainOwnerHD, mPt, 0xAC);
WWHD_OFFSET(ChainOwnerHD, mShaderCount, 0xB0);
WWHD_OFFSET(ChainOwnerHD, mShaderCapacity, 0xB4);
WWHD_OFFSET(ChainOwnerHD, mpShaderEntries, 0xB8);
WWHD_OFFSET(ChainOwnerHD, mPacketCount, 0xBC);
WWHD_OFFSET(ChainOwnerHD, mpPackets, 0xC0);
WWHD_OFFSET(ChainOwnerHD, mUniformPacket, 0xA8C8);
WWHD_OFFSET(ChainOwnerHD, mResources, 0xAC2C);
WWHD_OFFSET(ChainOwnerHD, mIndices, 0xB0EC);
WWHD_OFFSET(ChainOwnerHD, mShape, 0xB104);
WWHD_OFFSET(ChainOwnerHD, mTemporaryShape, 0xB29C);

u32 chain_constructPacket(u32, u32, u32, f32);
void chain_releaseShaderBlock(u32);
u32 chain_findContainingHeap(u32, u32);
u32 chain_operatorNew(u32);
void chain_packetBaseConstruct(u32);
void chain_packetArrayConstruct(u32);
u32 chain_constructArray(u32, u32, u32, u32);
void chain_drawPacketConstruct(u32);
void chain_clearMemory(u32, u32);
void chain_indexBufferConstruct(u32);
void chain_shapeConstruct(u32);
void chain_temporaryShapeConstruct(u32);
u32 chain_shaderManagerInstance();
s32 chain_findShader(u32, u32);
void chain_shaderObjectInitialize(u32, u32, u32);
void chain_setupShaderEntries(u32, u32, u32);
u32 chain_currentHeap(u32);
void chain_setupVertexBuffer(u32, u32, u32, u32);
void chain_shaderBlockConstruct(u32);
void chain_setupShaderBlock(u32, u32, u32, u32, u32);
void chain_resizePacketArray(u32, u32, u32);
void chain_drawPacketSetup(u32, u32);
void chain_setupIndexBuffer(u32, u32, u32, u32);
void chain_invalidateVertexBuffer(u32, u32, u32);
void chain_enterGraphicsContext(u32);
u32 chain_loadResource(u32, u32, u32, u32);
void chain_readShapeResource(u32, u32, u32);
void chain_copyShape(u32, u32);
void chain_leaveGraphicsContext(u32);
void chain_releasePacketResource(u32);
void chain_destroyArray(u32, u32, u32, u32, u32, u32);
void chain_shapeDestroy(u32, u32);
void chain_indexBufferDestroy(u32, u32);
void chain_drawPacketDestroy(u32, u32);
void chain_packetArrayDestroy(u32, u32);
void chain_packetBaseDestroy(u32, u32);
void chain_operatorDelete(u32);
void chain_bindNormalBuffer(u32, u32, u32);
void chain_bindPositionBuffer(u32, u32, u32);
void chain_bindTexCoordBuffer(u32, u32, u32);
u32 chain_getGraphicsContext(u32);
void chain_updateShaderProgram(u32, u32);
void chain_bindShaderBinary(u32);
void chain_bindShaderInstructions(u32, u32);
void chain_activateShaderProgram(u32);
void chain_bindShape(u32, u32, u32, u32);
void chain_bindAdditionalPass(u32, u32);
void chain_renderStateRead(u32);
void chain_configurePass(u32, u32);
void chain_renderStateApply(u32);
void chain_vectorSubtract(u32, u32, u32);
f32 chain_vectorMagnitudeSquared(u32);
f32 chain_sqrtf(f32);
BOOL chain_vectorNormalizeRS(u32);
void chain_quatRotBaseZ(u32, u32);
void chain_bindUniformBlock(u32);
void chain_drawIndexed(u32, u32, u32, u32, u32, u32);
void chain_vectorScale(u32, u32, f32);
void chain_vectorAdd(u32, u32, u32);
void chain_copyViewMatrix(u32, u32);
void chain_setTevFog(u32);
void chain_preparePacketArray(u32, u32, u32, u32, u32);
void chain_colorToFloat(u32, u32);
void chain_scaleColor(u32, u32, f32);
void chain_updatePacketArray(u32);
void chain_resetTevFog();
void chain_scaleAlpha(u32, f32);
void chain_updateDrawPacket(u32);
void chain_matrixTranslate(u32, f32, f32, f32);
void chain_matrixQuatM(u32);
void chain_matrixZrotM(u32, u32);
void chain_matrixScaleM(f32, f32, f32);
void chain_matrixConcat(u32, u32);
void chain_registerStaticDestructor(u32);
void chain_initializeStaticAngle(u32);
void chain_initializeStaticFlag(u32);
void chain_vertexBufferConstruct(u32);
void chain_shaderBlockDestroy(u32, u32);
void chain_vertexBufferDestroy(u32, u32);

struct ChainFloatWordsHD {
    be<f32> mValue[44];
};
struct ChainColorWordsHD {
    be<u32> mValue[4];
};
WWHD_SIZE(ChainColorWordsHD, 16);
struct ChainVertexDataHD {
    be<f32> mPosition[3];
    be<f32> mNormal[3];
    be<f32> mTexCoord[2];
};
WWHD_SIZE(ChainVertexDataHD, 32);
struct ChainVectorArrayHD {
    be<f32> mComponent[3];
};
WWHD_SIZE(ChainVectorArrayHD, 12);
struct ChainTexCoordHD {
    be<f32> mComponent[2];
};
WWHD_SIZE(ChainTexCoordHD, 8);
struct ChainBindingArrayHD {
    be<u32> mCount;
    be<u32> mpData;
};
WWHD_SIZE(ChainBindingArrayHD, 8);
struct ChainDescriptorHD {
    be<u32> mpShader;
    ChainBindingArrayHD mBlocks[2];
};
WWHD_SIZE(ChainDescriptorHD, 20);
struct ChainVertexSizeHD {
    u8 _00[0x150];
    be<u32> mSize;
};
struct ChainColorS10HD {
    be<s16> r, g, b, a;
};
WWHD_SIZE(ChainColorS10HD, 8);
struct dChain_packet_c : ChainOwnerHD {
    void destroy(u32);
    void drawHD(u32);
    void draw();
};
WWHD_SIZE(dChain_packet_c, 0xB32C);
u32 chain_InlinePacketCtor(u32);
u32 chain_InlineColorCtor(u32);
u32 chain_InlineResourceCtor(u32);
void chain_InlineResourceDtor(u32, u32);
void chain_InlinePacketDtor(u32, u32);

namespace chainDetail {
enum StorageFlag { DELETE_STORAGE = 1 };
enum ShaderProgramFlag { PROGRAM_DIRTY = 2 };
enum ShapeFlag { SHAPE_DIRTY = 2 };
enum PassMode { PASS_SHAPE, PASS_UNIFORM, PASS_UNIFORM_ADDITIONAL, PASS_OTHER, PASS_COUNT };
} // namespace chainDetail
