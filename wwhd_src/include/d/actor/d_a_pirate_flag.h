/* Pirate ship cloth and HD GX2 resources, WWHD layout.
 * GameCube packet geometry is shifted by 0x88; its simulation fields keep the tww names.
 * HD: the packet adds GX2 vertex buffers, shader lists, materials and native texture descriptors.
 */
#pragma once
#include "f_op/f_op_actor.h"
#include "bindings.h"
struct PirateVertexBuffer {
    /* 0x000 */ be<u32> mpVertices;
    /* 0x004 */ u8 mBuffer[0x154];
    /* 0x158 */ u8 mTexture[0xF4];
    /* 0x24C */ be<u32> mCount;
    /* 0x250 */ be<u32> mpStorage;
};
WWHD_SIZE(PirateVertexBuffer, 0x254);
WWHD_OFFSET(PirateVertexBuffer, mpVertices, 0x0);
WWHD_OFFSET(PirateVertexBuffer, mBuffer, 0x4);
WWHD_OFFSET(PirateVertexBuffer, mTexture, 0x158);
WWHD_OFFSET(PirateVertexBuffer, mCount, 0x24C);
WWHD_OFFSET(PirateVertexBuffer, mpStorage, 0x250);
struct PirateClothBuffers {
    /* 0x000 */ PirateVertexBuffer mBuffers[2][2];
    /* 0x950 */ be<u32> mCurrent;
    /* 0x954 */ be<u32> mAttribute;
    /* 0x958 */ be<u32> mStride;
    /* 0x95C */ be<u32> mpDrawCallback;
    /* 0x960 */ be<u32> mCount;
    /* 0x964 */ be<u8> mInitialized;
    /* 0x965 */ u8 _965[0x3];
};
WWHD_SIZE(PirateClothBuffers, 0x968);
WWHD_OFFSET(PirateClothBuffers, mBuffers, 0x0);
WWHD_OFFSET(PirateClothBuffers, mCurrent, 0x950);
WWHD_OFFSET(PirateClothBuffers, mAttribute, 0x954);
WWHD_OFFSET(PirateClothBuffers, mStride, 0x958);
WWHD_OFFSET(PirateClothBuffers, mpDrawCallback, 0x95C);
WWHD_OFFSET(PirateClothBuffers, mCount, 0x960);
WWHD_OFFSET(PirateClothBuffers, mInitialized, 0x964);
struct daPirate_Flag_packet_c {
    void setCorrectNrmAngle(s16, f32);
    void setBackNrm();
    void setNrmVtx(cXyz *, s32, s32);
    void draw();
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ be<u32> __vtbl;
    /* 0x010 */ u8 _010[0x88];
    /* 0x098 */ be<f32> mMtx[12];
    /* 0x0C8 */ gptr<dKy_tevstr_c> mTevStr;
    /* 0x0CC */ cXyz mPos[2][25];
    /* 0x324 */ cXyz mNrm[2][25];
    /* 0x57C */ cXyz mBackNrm[2][25];
    /* 0x7D4 */ cXyz m74C[25];
    /* 0x900 */ be<s16> m878;
    /* 0x902 */ be<s16> m87A;
    /* 0x904 */ be<s16> m87C;
    /* 0x906 */ be<u8> m87E;
    /* 0x907 */ be<u8> m87F;
    /* 0x908 */ be<u32> mShaderCount;
    /* 0x90C */ be<s32> mMaterialCount;
    /* 0x910 */ be<u32> mpMaterials;
    /* 0x914 */ PirateClothBuffers mClothBuffers;
    /* 0x127C */ be<s32> mDrawMaterialCount;
    /* 0x1280 */ be<u32> mpDrawMaterials;
    /* 0x1284 */ u8 _1284[0x4];
    /* 0x1288 */ u8 mShapeMaterial[0xA8];
    /* 0x1330 */ u8 mTevMaterial[0x74];
    /* 0x13A4 */ be<f32> mMaterialParams[44];
    /* 0x1454 */ u8 mMaterialBlocks[3][0x20];
    /* 0x14B4 */ u8 mMatrixBlocks[8][0x30];
    /* 0x1634 */ u8 mColorBlocks[6][0x10];
    /* 0x1694 */ be<u8> mMaterialInitialized;
    /* 0x1695 */ u8 _1695[0x3];
    /* 0x1698 */ be<u32> mPrimitiveType;
    /* 0x169C */ be<u32> mIndexType;
    /* 0x16A0 */ be<u32> mpIndices;
    /* 0x16A4 */ u8 _16A4[0x4];
    /* 0x16A8 */ be<u32> mIndexStride;
    /* 0x16AC */ u8 _16AC[0x4];
    /* 0x16B0 */ u8 mClothTexture[0x90];
    /* 0x1740 */ u8 mShipTexture[0x90];
    /* 0x17D0 */ u8 mClothTextureSampler[0x198];
    /* 0x1968 */ u8 mShipTextureSampler[0x198];
};
WWHD_SIZE(daPirate_Flag_packet_c, 0x1B00);
WWHD_OFFSET(daPirate_Flag_packet_c, __vtbl, 0xC);
WWHD_OFFSET(daPirate_Flag_packet_c, mMtx, 0x98);
WWHD_OFFSET(daPirate_Flag_packet_c, mTevStr, 0xC8);
WWHD_OFFSET(daPirate_Flag_packet_c, mPos, 0xCC);
WWHD_OFFSET(daPirate_Flag_packet_c, mNrm, 0x324);
WWHD_OFFSET(daPirate_Flag_packet_c, mBackNrm, 0x57C);
WWHD_OFFSET(daPirate_Flag_packet_c, m74C, 0x7D4);
WWHD_OFFSET(daPirate_Flag_packet_c, m878, 0x900);
WWHD_OFFSET(daPirate_Flag_packet_c, m87A, 0x902);
WWHD_OFFSET(daPirate_Flag_packet_c, m87C, 0x904);
WWHD_OFFSET(daPirate_Flag_packet_c, m87E, 0x906);
WWHD_OFFSET(daPirate_Flag_packet_c, m87F, 0x907);
WWHD_OFFSET(daPirate_Flag_packet_c, mShaderCount, 0x908);
WWHD_OFFSET(daPirate_Flag_packet_c, mMaterialCount, 0x90C);
WWHD_OFFSET(daPirate_Flag_packet_c, mpMaterials, 0x910);
WWHD_OFFSET(daPirate_Flag_packet_c, mClothBuffers, 0x914);
WWHD_OFFSET(daPirate_Flag_packet_c, mDrawMaterialCount, 0x127C);
WWHD_OFFSET(daPirate_Flag_packet_c, mpDrawMaterials, 0x1280);
WWHD_OFFSET(daPirate_Flag_packet_c, mShapeMaterial, 0x1288);
WWHD_OFFSET(daPirate_Flag_packet_c, mTevMaterial, 0x1330);
WWHD_OFFSET(daPirate_Flag_packet_c, mMaterialParams, 0x13A4);
WWHD_OFFSET(daPirate_Flag_packet_c, mMaterialBlocks, 0x1454);
WWHD_OFFSET(daPirate_Flag_packet_c, mMatrixBlocks, 0x14B4);
WWHD_OFFSET(daPirate_Flag_packet_c, mColorBlocks, 0x1634);
WWHD_OFFSET(daPirate_Flag_packet_c, mMaterialInitialized, 0x1694);
WWHD_OFFSET(daPirate_Flag_packet_c, mPrimitiveType, 0x1698);
WWHD_OFFSET(daPirate_Flag_packet_c, mIndexType, 0x169C);
WWHD_OFFSET(daPirate_Flag_packet_c, mpIndices, 0x16A0);
WWHD_OFFSET(daPirate_Flag_packet_c, mIndexStride, 0x16A8);
WWHD_OFFSET(daPirate_Flag_packet_c, mClothTexture, 0x16B0);
WWHD_OFFSET(daPirate_Flag_packet_c, mShipTexture, 0x1740);
WWHD_OFFSET(daPirate_Flag_packet_c, mClothTextureSampler, 0x17D0);
WWHD_OFFSET(daPirate_Flag_packet_c, mShipTextureSampler, 0x1968);
struct pirate_flag_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs1;
    /* 0x3B4 */ request_of_phase_process_class mPhs2;
    /* 0x3BC */ u8 _3BC[0x8];
    /* 0x3C4 */ be<s16> m2A8;
    /* 0x3C6 */ u8 _3C6[0xE];
    /* 0x3D4 */ daPirate_Flag_packet_c mPacket;
};
WWHD_SIZE(pirate_flag_class, 0x1ED4);
WWHD_OFFSET(pirate_flag_class, mPhs1, 0x3AC);
WWHD_OFFSET(pirate_flag_class, mPhs2, 0x3B4);
WWHD_OFFSET(pirate_flag_class, m2A8, 0x3C4);
WWHD_OFFSET(pirate_flag_class, mPacket, 0x3D4);
struct daPirate_Flag_HIO_c {
    /* 0x000 */ be<s8> mNo;
    /* 0x001 */ be<u8> m05;
    /* 0x002 */ be<u8> m06;
    /* 0x003 */ be<u8> m07;
    /* 0x004 */ be<u32> m08;
    /* 0x008 */ be<f32> m0C;
    /* 0x00C */ be<f32> m10;
    /* 0x010 */ be<f32> m14;
    /* 0x014 */ be<f32> m18;
    /* 0x018 */ be<f32> m1C;
    /* 0x01C */ be<u32> __vtbl;
};
WWHD_SIZE(daPirate_Flag_HIO_c, 0x20);
WWHD_OFFSET(daPirate_Flag_HIO_c, mNo, 0x0);
WWHD_OFFSET(daPirate_Flag_HIO_c, m05, 0x1);
WWHD_OFFSET(daPirate_Flag_HIO_c, m06, 0x2);
WWHD_OFFSET(daPirate_Flag_HIO_c, m07, 0x3);
WWHD_OFFSET(daPirate_Flag_HIO_c, m08, 0x4);
WWHD_OFFSET(daPirate_Flag_HIO_c, m0C, 0x8);
WWHD_OFFSET(daPirate_Flag_HIO_c, m10, 0xC);
WWHD_OFFSET(daPirate_Flag_HIO_c, m14, 0x10);
WWHD_OFFSET(daPirate_Flag_HIO_c, m18, 0x14);
WWHD_OFFSET(daPirate_Flag_HIO_c, m1C, 0x18);
WWHD_OFFSET(daPirate_Flag_HIO_c, __vtbl, 0x1C);

void pirateAddCalcAngle(void *, s32, u32, u32);
void pirateMtxTrans(u32, f32, f32, f32);
void pirateMtxPosition(void *, void *);
void pirateVectorDifference(void *, void *, void *);
void pirateVectorProduct(void *, void *, f32);
void pirateVectorCross(void *, void *, void *);
void pirateVectorNormalize(void *, void *);
void pirateClothSpringCall(void *, void *, void *, void *, f32);
void pirateClothMoveCall(void *);
void pirateGraphicsInitCall(void *);
void piratePacketConstructCall(void *);
u32 pirateGetPlay();
s32 pirateResLoad(void *, const char *);
void pirateResDelete(void *, const char *);
void pirateFirstLightVector(void *, void *);
void pirateSetFog(void *);
void pirateSetLight(void *);
u32 pirateGetWindVector();
void pirateActorConstruct(void *);
void pirateActorDestroy(void *, u32);
u32 pirateJudgeActor(void *, void *);
void pirateMtxYrotS(void *, s32);
void pirateMtxXrotM(void *, s32);
void pirateMtxYrotM(void *, s32);
void pirateMtxZrotM(void *, s32);
u32 pirateGetResource(void *, void *, u32);
void pirateAssertion(const char *, u32, const char *);
void pirateScaleColor(void *, f32);
void pirateSetScaledColor(void *, void *, f32);
void pirateResourceLock(void *);
void pirateResourceUnlock(void *);
void pirateRasterConstruct(void *);
void pirateRasterApply(void *);
void pirateRasterSetCull(void *);
u32 pirateFindStorageHeap(void *, void *);
u32 pirateGetStorageHeap(void *);
void pirateTextureSetImage(void *, void *);
void pirateIndexBufferConstruct(void *);
void pirateIndexBufferDestroy(void *, u32);
void pirateIndexBufferSet(void *, void *, u32, u32);
void pirateBufferConstruct(void *);
void pirateBufferDestroy(void *, u32);
void pirateBufferFlush(void *, u32, u32);
s32 pirateShaderArchiveFind(void *, void *);
void pirateShaderArchiveBind(void *);
void pirateShaderBind(void *);
void pirateShaderPrepare(void *, u32);
void pirateTextureCopy(void *, void *);
void pirateTextureSamplerConstruct(void *);
void pirateTextureSamplerDestroy(void *, u32);
void pirateTextureSamplerBind(void *, void *, s32, u32);
void pirateTextureResourceConstruct(void *);
void pirateUniformBufferRelease(void *);
void pirateTextureConstruct(void *);
void pirateTextureRelease(void *);
void pirateTextureDestroy(void *, u32);
void pirateTextureBind(void *);
void pirateDrawBufferEntry(void *, void *, u32);
void piratePacketBaseConstruct(void *);
void piratePacketBaseDestroy(void *, u32);
u32 pirateGetShaderCache(void *);
void pirateMaterialConstruct(void *);
void pirateMaterialDestroy(void *, u32);
void pirateMaterialInvalidate(void *);
void pirateDrawMaterialConstruct(void *);
void pirateDrawMaterialDestroy(void *, u32);
void pirateDrawMaterialSetView(void *, u32, void *, void *, void *);
void pirateDrawMaterialUpdate(void *, u32);
void pirateDrawMaterialAllocate(void *, u32, u32);
void pirateVertexBufferSet(void *, void *, u32, void *);
void pirateTextureSetVertexBuffer(void *, void *, void *, void *, u32);
u32 pirateShaderManagerGet(void *);
void pirateRenderContextApply(void *, void *);
void pirateRasterSetMaterial(u32, void *);
void pirateSetShaderOptions(void *, void *, u32);
void pirateShaderRecordConstruct(void *, void *, u32);
void pirateVectorAdd(void *, void *, void *);
void pirateVectorSubtract(void *, void *, void *);
f32 pirateVectorSquareMag(void *);
void pirateVectorScale(void *, void *, f32);
f32 pirateVectorDot(void *, void *);
void pirateMatrixMultVector(void *, void *, void *);
void pirateMatrixIdentity(void *);
void pirateMatrixCopy(void *, void *);
void pirateMatrixConcat(void *, void *, void *);
void pirateHeaderCircleConstruct(void *);
void pirateHeaderAngleConstruct(void *);
void pirateConstructArray(void *, u32, u32, void *);
void pirateDestroyArray(void *, u32, u32, void *, u32, u32);
void pirateRegisterGlobal(void *);
f32 pirateSqrt(f32);
void pirateClearStorage(void *, u32);
void pirateGX2CallDisplayList(void *, u32);
void pirateGX2DrawIndexed(u32, u32, u32, void *, u32, u32);
void pirateGX2SetGeometryUniform(s32, void *, u32);
void pirateGX2SetPixelUniform(s32, void *, u32);
void pirateGX2SetVertexUniform(s32, void *, u32);
void pirateDCStoreRangeNoSync(void *, u32);
void pirateDebugPropertyInit(void *, const char *, u32);

f32 pirateGetWindPower();

struct PirateVectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(PirateVectorWords, 12);

struct ClothTemporary {
    u8 linkage[16];
    cXyz displacement, unit;
};

struct ParentIDTemporary {
    u8 linkage[16];
    be<u32> id;
};

struct NormalTemporary {
    u8 linkage[16];
    cXyz sum, center, side, other, cross, unit, normalized, scratch;
};

struct PacketTemporary {
    u8 linkage[16];
    be<f32> matrix[12], position[12], color[4];
};

struct DrawTemporary {
    u8 linkage[16];
    be<f32> identity[12];
    cXyz light, result, arrows[4];
};

struct ClothMoveTemporary {
    u8 linkage[16];
    cXyz spring, center, delta, wind, light, probe, result, scaled;
    be<u32> stageName, stageVtable, expectedName, expectedVtable;
};

struct ArchiveTemporary {
    u8 linkage[16];
    be<u32> shaderName, shaderVtable, clothName, clothVtable, shipName, shipVtable;
};

struct RasterTemporary {
    u8 linkage[16];
    u8 descriptor[0x120];
};

// HD: the ship prefix used by its attached flag (the complete ship layout lives in its unit).
struct PirateShipView : fopAc_ac_c {
    u8 _3AC[0x38];
    be<u8> mVisible;
    u8 _3E5[3];
    be<u32> mpModel;
};
WWHD_OFFSET(PirateShipView, mVisible, 0x3E4);
WWHD_OFFSET(PirateShipView, mpModel, 0x3E8);
WWHD_SIZE(PirateShipView, 0x3EC);
struct PirateFlagOffset {
    cXyz position;
    be<u32> initialized;
};
WWHD_SIZE(PirateFlagOffset, 0x10);
struct PirateHeaderData {
    be<u32> zero[4];
    be<f32> pi[2];
    u8 angle, circle;
};
WWHD_SIZE(PirateHeaderData, 0x1A);
struct PiratePositionData {
    cXyz points[25];
};
WWHD_SIZE(PiratePositionData, 0x12C);
struct PirateTexCoordData {
    be<f32> uv[25][2];
};
WWHD_SIZE(PirateTexCoordData, 0xC8);

struct PirateBufferDescriptor {
    /* 0x000 */ u8 _000[0x14C];
    /* 0x14C */ be<u32> mByteCount;
};
WWHD_SIZE(PirateBufferDescriptor, 0x150);
WWHD_OFFSET(PirateBufferDescriptor, mByteCount, 0x14C);

struct PirateHeapView {
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ be<u32> __vtbl;
};
WWHD_SIZE(PirateHeapView, 0x10);
WWHD_OFFSET(PirateHeapView, __vtbl, 0xC);

struct PirateVirtualFunctions {
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ be<u32> mpDestroyTexture;
    /* 0x010 */ u8 _010[0x4];
    /* 0x014 */ be<u32> mpAssureString;
    /* 0x018 */ u8 _018[0x14];
    /* 0x02C */ be<u32> mpApplyMaterial;
    /* 0x030 */ u8 _030[0x4];
    /* 0x034 */ be<u32> mpAllocate;
    /* 0x038 */ u8 _038[0x4];
    /* 0x03C */ be<u32> mpFree;
};
WWHD_SIZE(PirateVirtualFunctions, 0x40);
WWHD_OFFSET(PirateVirtualFunctions, mpDestroyTexture, 0xC);
WWHD_OFFSET(PirateVirtualFunctions, mpAssureString, 0x14);
WWHD_OFFSET(PirateVirtualFunctions, mpApplyMaterial, 0x2C);
WWHD_OFFSET(PirateVirtualFunctions, mpAllocate, 0x34);
WWHD_OFFSET(PirateVirtualFunctions, mpFree, 0x3C);

struct PirateTextureResource {
    /* 0x000 */ u8 _000[0x20];
    /* 0x020 */ be<u32> mpImage;
};
WWHD_SIZE(PirateTextureResource, 0x24);
WWHD_OFFSET(PirateTextureResource, mpImage, 0x20);

struct PirateShaderManagerView {
    /* 0x000 */ u8 _000[0x4];
    /* 0x004 */ be<u32> mpArchive;
    /* 0x008 */ be<u32> mCount;
    /* 0x00C */ be<u32> mpRecords;
};
WWHD_SIZE(PirateShaderManagerView, 0x10);
WWHD_OFFSET(PirateShaderManagerView, mpArchive, 0x4);
WWHD_OFFSET(PirateShaderManagerView, mCount, 0x8);
WWHD_OFFSET(PirateShaderManagerView, mpRecords, 0xC);

struct PirateShaderArchiveView {
    /* 0x000 */ u8 _000[0x1C];
    /* 0x01C */ be<u32> mCount;
    /* 0x020 */ be<u32> mpPrograms;
    /* 0x024 */ u8 _024[0x4];
    /* 0x028 */ be<u32> mpParentArchive;
};
WWHD_SIZE(PirateShaderArchiveView, 0x2C);
WWHD_OFFSET(PirateShaderArchiveView, mCount, 0x1C);
WWHD_OFFSET(PirateShaderArchiveView, mpPrograms, 0x20);
WWHD_OFFSET(PirateShaderArchiveView, mpParentArchive, 0x28);

struct PirateShaderRecordView {
    /* 0x000 */ be<u32> mpProgram;
    /* 0x004 */ u8 _004[0x1C];
    /* 0x020 */ be<u8> mInitialized;
};
WWHD_SIZE(PirateShaderRecordView, 0x21);
WWHD_OFFSET(PirateShaderRecordView, mpProgram, 0x0);
WWHD_OFFSET(PirateShaderRecordView, mInitialized, 0x20);

struct PirateShaderCacheView {
    /* 0x000 */ be<u32> mpArchive;
    /* 0x004 */ be<u32> mpShader;
};
WWHD_SIZE(PirateShaderCacheView, 0x8);
WWHD_OFFSET(PirateShaderCacheView, mpArchive, 0x0);
WWHD_OFFSET(PirateShaderCacheView, mpShader, 0x4);

struct PirateShaderProgramView {
    /* 0x000 */ be<u32> mpShader;
    /* 0x004 */ u8 _004[0x8];
    /* 0x00C */ be<u32> mUniformCount;
    /* 0x010 */ be<u32> mpUniforms;
    /* 0x014 */ be<u32> mTextureCount;
    /* 0x018 */ be<u32> mpTextures;
};
WWHD_SIZE(PirateShaderProgramView, 0x1C);
WWHD_OFFSET(PirateShaderProgramView, mpShader, 0x0);
WWHD_OFFSET(PirateShaderProgramView, mUniformCount, 0xC);
WWHD_OFFSET(PirateShaderProgramView, mpUniforms, 0x10);
WWHD_OFFSET(PirateShaderProgramView, mTextureCount, 0x14);
WWHD_OFFSET(PirateShaderProgramView, mpTextures, 0x18);

struct PirateShaderView {
    /* 0x000 */ be<u8> mFlags;
    /* 0x001 */ u8 _001[0x3];
    /* 0x004 */ be<u32> mpDisplayList;
    /* 0x008 */ u8 _008[0x4];
    /* 0x00C */ be<u32> mDisplayListSize;
    /* 0x010 */ u8 _010[0x6C];
    /* 0x07C */ be<u32> mpArchive;
};
WWHD_SIZE(PirateShaderView, 0x80);
WWHD_OFFSET(PirateShaderView, mFlags, 0x0);
WWHD_OFFSET(PirateShaderView, mpDisplayList, 0x4);
WWHD_OFFSET(PirateShaderView, mDisplayListSize, 0xC);
WWHD_OFFSET(PirateShaderView, mpArchive, 0x7C);

struct PirateUniformLocations {
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ be<s16> mVertex;
    /* 0x00E */ be<s16> mPixel;
    /* 0x010 */ be<s16> mGeometry;
};
WWHD_SIZE(PirateUniformLocations, 0x12);
WWHD_OFFSET(PirateUniformLocations, mVertex, 0xC);
WWHD_OFFSET(PirateUniformLocations, mPixel, 0xE);
WWHD_OFFSET(PirateUniformLocations, mGeometry, 0x10);

struct PirateUniformView {
    /* 0x000 */ u8 _000[0x4];
    /* 0x004 */ be<u32> mSize;
    /* 0x008 */ u8 _008[0x4];
    /* 0x00C */ be<u32> mpData;
};
WWHD_SIZE(PirateUniformView, 0x10);
WWHD_OFFSET(PirateUniformView, mSize, 0x4);
WWHD_OFFSET(PirateUniformView, mpData, 0xC);

struct PirateMaterialView {
    /* 0x000 */ u8 _000[0x4];
    /* 0x004 */ be<u32> mpDrawMaterial;
    /* 0x008 */ u8 _008[0x4];
    /* 0x00C */ be<u32> __vtbl;
    /* 0x010 */ u8 _010[0x3C];
    /* 0x04C */ be<u32> mCurrentUniform;
};
WWHD_SIZE(PirateMaterialView, 0x50);
WWHD_OFFSET(PirateMaterialView, mpDrawMaterial, 0x4);
WWHD_OFFSET(PirateMaterialView, __vtbl, 0xC);
WWHD_OFFSET(PirateMaterialView, mCurrentUniform, 0x4C);

struct PirateRenderContextView {
    /* 0x000 */ u8 _000[0xC];
    /* 0x00C */ be<u32> mMaterialIndex;
    /* 0x010 */ u8 _010[0x4];
    /* 0x014 */ be<u32> mpMaterial;
    /* 0x018 */ u8 _018[0x18];
    /* 0x030 */ be<u32> mpExtraMaterial;
};
WWHD_SIZE(PirateRenderContextView, 0x34);
WWHD_OFFSET(PirateRenderContextView, mMaterialIndex, 0xC);
WWHD_OFFSET(PirateRenderContextView, mpMaterial, 0x14);
WWHD_OFFSET(PirateRenderContextView, mpExtraMaterial, 0x30);

struct PirateRasterView {
    /* 0x000 */ u8 _000[0x8];
    /* 0x008 */ be<u32> mCullMode;
    /* 0x00C */ be<u32> mDepthMode;
    /* 0x010 */ u8 _010[0xD0];
    /* 0x0E0 */ be<u8> mAlphaEnabled;
    /* 0x0E1 */ u8 _0E1[0x3];
    /* 0x0E4 */ be<u32> mAlphaCompare;
    /* 0x0E8 */ be<f32> mAlphaRef;
    /* 0x0EC */ be<u32> mControl;
};
WWHD_SIZE(PirateRasterView, 0xF0);
WWHD_OFFSET(PirateRasterView, mCullMode, 0x8);
WWHD_OFFSET(PirateRasterView, mDepthMode, 0xC);
WWHD_OFFSET(PirateRasterView, mAlphaEnabled, 0xE0);
WWHD_OFFSET(PirateRasterView, mAlphaCompare, 0xE4);
WWHD_OFFSET(PirateRasterView, mAlphaRef, 0xE8);
WWHD_OFFSET(PirateRasterView, mControl, 0xEC);

struct PirateTextureSamplerView {
    /* 0x000 */ u8 _000[0x4];
    /* 0x004 */ be<u32> mWidth;
    /* 0x008 */ be<u32> mHeight;
    /* 0x00C */ be<u32> mDepth;
    /* 0x010 */ be<u32> mDimension;
    /* 0x014 */ be<u32> mFormat;
    /* 0x018 */ be<u32> mMipCount;
    /* 0x01C */ be<u32> mUse;
    /* 0x020 */ u8 _020[0x8];
    /* 0x028 */ be<u32> mpImage;
    /* 0x02C */ u8 _02C[0x4];
    /* 0x030 */ be<u32> mpMipmaps;
    /* 0x034 */ be<u32> mAlignment;
    /* 0x038 */ be<u32> mPitch;
    /* 0x03C */ u8 _03C[0x98];
    /* 0x0D4 */ be<u32> mpImageBuffer;
    /* 0x0D8 */ u8 _0D8[0x4];
    /* 0x0DC */ be<u32> mpMipmapBuffer;
    /* 0x0E0 */ u8 _0E0[0x10];
    /* 0x0F0 */ be<u32> __vtbl;
    /* 0x0F4 */ u8 _0F4[0x68];
    /* 0x15C */ be<u32> mWrapX;
    /* 0x160 */ be<u32> mWrapY;
    /* 0x164 */ be<u32> mWrapZ;
    /* 0x168 */ u8 _168[0x28];
    /* 0x190 */ be<u8> mDirty;
};
WWHD_SIZE(PirateTextureSamplerView, 0x191);
WWHD_OFFSET(PirateTextureSamplerView, mWidth, 0x4);
WWHD_OFFSET(PirateTextureSamplerView, mHeight, 0x8);
WWHD_OFFSET(PirateTextureSamplerView, mDepth, 0xC);
WWHD_OFFSET(PirateTextureSamplerView, mDimension, 0x10);
WWHD_OFFSET(PirateTextureSamplerView, mFormat, 0x14);
WWHD_OFFSET(PirateTextureSamplerView, mMipCount, 0x18);
WWHD_OFFSET(PirateTextureSamplerView, mUse, 0x1C);
WWHD_OFFSET(PirateTextureSamplerView, mpImage, 0x28);
WWHD_OFFSET(PirateTextureSamplerView, mpMipmaps, 0x30);
WWHD_OFFSET(PirateTextureSamplerView, mAlignment, 0x34);
WWHD_OFFSET(PirateTextureSamplerView, mPitch, 0x38);
WWHD_OFFSET(PirateTextureSamplerView, mpImageBuffer, 0xD4);
WWHD_OFFSET(PirateTextureSamplerView, mpMipmapBuffer, 0xDC);
WWHD_OFFSET(PirateTextureSamplerView, __vtbl, 0xF0);
WWHD_OFFSET(PirateTextureSamplerView, mWrapX, 0x15C);
WWHD_OFFSET(PirateTextureSamplerView, mWrapY, 0x160);
WWHD_OFFSET(PirateTextureSamplerView, mWrapZ, 0x164);
WWHD_OFFSET(PirateTextureSamplerView, mDirty, 0x190);

struct PiratePlayView {
    /* 0x000 */ u8 _000[0x5292];
    /* 0x5292 */ be<u8> mDemoFlag;
    /* 0x5293 */ u8 _5293[0xADD];
    /* 0x5D70 */ be<u32> mpOpaqueDrawBuffer;
    /* 0x5D74 */ be<u32> mpTranslucentDrawBuffer;
    /* 0x5D78 */ be<u32> mpOpaqueEntryBuffer;
    /* 0x5D7C */ be<u32> mpTranslucentEntryBuffer;
};
WWHD_SIZE(PiratePlayView, 0x5D80);
WWHD_OFFSET(PiratePlayView, mDemoFlag, 0x5292);
WWHD_OFFSET(PiratePlayView, mpOpaqueDrawBuffer, 0x5D70);
WWHD_OFFSET(PiratePlayView, mpTranslucentDrawBuffer, 0x5D74);
WWHD_OFFSET(PiratePlayView, mpOpaqueEntryBuffer, 0x5D78);
WWHD_OFFSET(PiratePlayView, mpTranslucentEntryBuffer, 0x5D7C);

struct PirateTextureArray {
    be<s32> mCount;
    be<u32> mpTextures;
};
WWHD_SIZE(PirateTextureArray, 8);
struct PirateMaterialRecord {
    be<u32> mpProgram;
    PirateTextureArray mTextures[2];
};
WWHD_SIZE(PirateMaterialRecord, 0x14);
WWHD_OFFSET(PirateMaterialRecord, mTextures, 4);

struct PirateVertex {
    cXyz position, normal;
    be<f32> uv[2];
};
WWHD_SIZE(PirateVertex, 0x20);
struct PirateTevLightView {
    be<f32> positionDirection[6];
    be<u8> color[4];
    be<u16> attenuationShorts[4];
    be<f32> attenuation[8];
};
WWHD_SIZE(PirateTevLightView, 0x44);
struct PirateTevView {
    PirateTevLightView light;
    u8 _044[0x84 - 0x44];
    be<u32> worldPosition[3];
    be<u16> colorC0[4];
    be<u8> colorsK[8];
    be<u16> fogColor[4];
    be<f32> fogBlend[3];
    be<u8> environment[9];
    u8 _0BD[3];
    be<f32> hdLightParameters[17];
    u8 _104[0x144 - 0x104];
    PirateTevLightView hdLight;
    u8 _188[0x1C8 - 0x188];
};
WWHD_SIZE(PirateTevView, 0x1C8);
WWHD_OFFSET(PirateTevView, hdLight, 0x144);
struct PirateTevScaleView {
    u8 _000[0x24];
    be<f32> mColorScaleK, mColorScaleC;
    u8 _02C[0x160 - 0x2C];
    be<s16> mHdColorC[4];
    u8 _168[4];
    be<f32> mHdColorScaleC;
};
WWHD_OFFSET(PirateTevScaleView, mHdColorScaleC, 0x16C);
WWHD_SIZE(PirateTevScaleView, 0x170);

struct PiratePiData {
    be<f32> minimum, maximum;
};
WWHD_SIZE(PiratePiData, 8);
struct PirateZeros {
    be<u32> value[4];
};
WWHD_SIZE(PirateZeros, 16);
struct PirateByteData {
    be<u8> value;
};
WWHD_SIZE(PirateByteData, 1);

struct PirateStringBytes {
    be<u8> characters[0x40001];
};
struct PirateFloatVertex {
    be<f32> component[8];
};
WWHD_SIZE(PirateFloatVertex, 0x20);
struct PirateZeroVector {
    cXyz value;
};
WWHD_SIZE(PirateZeroVector, 12);

void pirate_copy_matrix(void *dest, void *source);

void pirate_color_s16(void *dest, void *source);

void pirate_color_u8(void *dest, void *source);

BOOL daPirate_Flag_IsDelete(void *actor);

BOOL daPirate_Flag_Delete(void *actor);

void pirateSafeString_delete(void *object, u32 flags);

void *pirate_vertex_buffer_construct(void *object);

void *pirate_material_block_allocate(void *object);

void pirate_vertex_buffer_destruct(void *object, u32 flags);

void pirate_empty_material_destructor(void *object);

void daPirate_Flag_HIO_delete(void *object, u32 flags);

void pirateSafeString_assure(void *object);

BOOL daPirate_Flag_Execute(void *actor);

void get_cloth_anim_sub_factor(cXyz *origin, cXyz *neighbor, cXyz *accumulated, f32 restLength);

s32 daPirate_Flag_Create(void *actor);

void __sinit_d_a_pirate_flag_cpp();

void *pirate_packet_construct(void *object);

void pirate_packet_destruct(void *object, u32 flags);

void pirate_actor_destruct(void *object, u32 flags);

BOOL daPirate_Flag_Draw(void *actor);

void pirate_flag_move(void *actor);

void pirate_initialize_graphics(void *object);

void pirate_render_material(void *object, void *context);

// HD: texture indices moved from GameCube Cloth 3 / Kaizokusen 9 to HD 9 / 3.
enum PirateFlagResource_e {
    dRes_INDEX_CLOTH_BTI_CLOTHTOON_e = 9,
    dRes_INDEX_KAIZOKUSEN_BTI_TXA_KAIZOKU_HATA_e = 3,
};
