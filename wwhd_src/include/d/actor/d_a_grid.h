/* daGrid_c / daHo_packet_c (ship sail), WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
struct GridGpuMaterial {
    gptr<void> vertices;
    u8 vertexBuffer[0x154];
    u8 shaderObject[0xF4];
    be<u32> allocationCount;
    gptr<void> allocation;
};
WWHD_SIZE(GridGpuMaterial, 0x254);
struct GridMaterialPool {
    GridGpuMaterial materials[4];
    be<u32> current;
    be<u32> attributeCount;
    be<u32> stride;
    gptr<void> attributes;
    be<u32> unused;
    be<u8> initialized;
    u8 padding[3];
};
WWHD_SIZE(GridMaterialPool, 0x968);
struct GridShaderCollection {
    gptr<void> program;
    be<s32> frontShaderCount;
    gptr<void> frontShaders;
    be<s32> backShaderCount;
    gptr<void> backShaders;
};
WWHD_SIZE(GridShaderCollection, 0x14);
WWHD_OFFSET(GridShaderCollection, frontShaders, 8);
WWHD_OFFSET(GridShaderCollection, backShaders, 0x10);

struct GridUniformBindingStorage {
    u8 data[0x1C];
};
struct GridUniformObject {
    u8 prefix[12];
    be<u32> vtable;
    GridUniformBindingStorage bindings[2];
    u8 padding48[4];
    be<u32> currentLocation;
    u8 tail[0x74 - 0x50];
};
WWHD_SIZE(GridUniformObject, 0x74);
struct GridDescriptor {
    be<u32> words[4];
};
struct GridUniformState {
    be<f32> initialColors[44];
    GridDescriptor descriptors[3][2];
    Mtx34 matrices[8];
    GridDescriptor finalDescriptors[6];
};
WWHD_SIZE(GridUniformState, 0x2F0);
struct GridCameraView {
    u8 prefix[0x264];
    cXyz eyeBasis;
    u8 padding270[0x7C0 - 0x270];
    cXyz eyeOffset;
};

struct daHo_packet_c {
    void setBackNrm();
    s32 setNrmMtx();
    void setTopNrmVtx(cXyz* out);
    void setNrmVtx(cXyz* out, s32 x, s32 y);
    void updateMaterial();
    void draw(void* drawInfo);

    // HD: GX2 material packet prefix; one position/normal buffer per sail.
    u8 packetBase[0xAC];
    Mtx34 mMtx;
    gptr<void> mpTevStr;
    cXyz mPos[85];
    cXyz mNrm[85];
    cXyz mBackNrm[85];
    be<s16> normalAngle, unusedAngle1, unusedAngle2;
    be<u8> mAlpha;
    u8 paddingCDB;
    be<u32> shaderCount;
    be<u32> shaderCollectionCount;
    gptr<GridShaderCollection> shaderCollections;
    GridMaterialPool materialPool;
    be<u32> drawMaterialCount;
    gptr<void> drawMaterials;
    be<u32> drawMaterialFlags;
    GridUniformObject drawUniform;
    Mtx34 modelMatrix;
    be<u32> modelFlags;
    GridUniformObject materialUniformObject;
    GridUniformState uniformState;
    be<u8> uniformFlag;
    u8 padding1A69[3];
    u8 indexBuffer[0x18];
    u8 frontTexture[0x198];
    u8 frontTextureResource[0x90];
    u8 alternateTexture[0x198];
    u8 alternateTextureResource[0x90];
    u8 toonTexture[0x198];
    u8 toonTextureResource[0x90];
    be<u8> textureInitialized;
    u8 padding20FD[3];
};
WWHD_SIZE(daHo_packet_c, 0x2100);
WWHD_OFFSET(daHo_packet_c, mMtx, 0xAC);
WWHD_OFFSET(daHo_packet_c, mpTevStr, 0xDC);
WWHD_OFFSET(daHo_packet_c, mPos, 0xE0);
WWHD_OFFSET(daHo_packet_c, mNrm, 0x4DC);
WWHD_OFFSET(daHo_packet_c, mBackNrm, 0x8D8);
WWHD_OFFSET(daHo_packet_c, normalAngle, 0xCD4);
WWHD_OFFSET(daHo_packet_c, mAlpha, 0xCDA);
WWHD_OFFSET(daHo_packet_c, shaderCount, 0xCDC);
WWHD_OFFSET(daHo_packet_c, shaderCollectionCount, 0xCE0);
WWHD_OFFSET(daHo_packet_c, shaderCollections, 0xCE4);
WWHD_OFFSET(daHo_packet_c, materialPool, 0xCE8);
WWHD_OFFSET(daHo_packet_c, drawMaterialCount, 0x1650);
WWHD_OFFSET(daHo_packet_c, modelMatrix, 0x16D0);
WWHD_OFFSET(daHo_packet_c, uniformState, 0x1778);
WWHD_OFFSET(daHo_packet_c, indexBuffer, 0x1A6C);
WWHD_OFFSET(daHo_packet_c, frontTexture, 0x1A84);
WWHD_OFFSET(daHo_packet_c, alternateTexture, 0x1CAC);
WWHD_OFFSET(daHo_packet_c, toonTexture, 0x1ED4);
WWHD_OFFSET(daHo_packet_c, textureInitialized, 0x20FC);
struct daGrid_c : fopAc_ac_c {
    bool _delete();
    bool _execute();
    bool _draw();
    s32 _create();

    request_of_phase_process_class mClothPhase;
    request_of_phase_process_class mShipPhase;
    daHo_packet_c mPacket;
    be<u32> wavePhase;
    be<s8> parameter;
    u8 padding24C1;
    be<s16> openingTimer;
    be<s16> waveStepSin;
    be<s16> waveStepCos;
    u8 padding24C8[4];
    be<f32> weights[85];
    u8 unusedClothState[0x558];
    be<f32> furl;
    be<f32> openingGain;
    be<f32> openingStarted;
    be<f32> oscillationGain;
    be<f32> oscillationSign;
    be<s16> oscillationTimer;
    u8 padding2B8E[6];
    be<s16> relativeWindAngle;
    be<s16> twistAngle;
    be<s16> unusedAngle;
    be<s16> mForceWindRelAngle;
    be<u8> mIsForceWindRelAngle;
    u8 padding2B9D[3];
};
WWHD_SIZE(daGrid_c, 0x2BA0); // HD profile101B6B18.
WWHD_OFFSET(daGrid_c, mPacket, 0x3BC);
WWHD_OFFSET(daGrid_c, wavePhase, 0x24BC);
WWHD_OFFSET(daGrid_c, weights, 0x24CC);
WWHD_OFFSET(daGrid_c, furl, 0x2B78);
WWHD_OFFSET(daGrid_c, relativeWindAngle, 0x2B94);
WWHD_OFFSET(daGrid_c, mIsForceWindRelAngle, 0x2B9C);

// HD: integer vector assignment preserves signaling NaN payloads without float loads.
struct GridVectorWords {
    be<u32> x, y, z;
};
inline GridVectorWords& grid_words(const void* vector) {
    return gabi::object_at<GridVectorWords>(gabi::ea(vector));
}
inline daHo_packet_c& grid_packet(const void* packet) {
    return gabi::object_at<daHo_packet_c>(gabi::ea(packet));
}
inline GridMaterialPool& grid_pool(const void* pool) {
    return gabi::object_at<GridMaterialPool>(gabi::ea(pool));
}
inline GridGpuMaterial& grid_material(const void* material) {
    return gabi::object_at<GridGpuMaterial>(gabi::ea(material));
}
inline GridShaderCollection& grid_collection(const void* collection) {
    return gabi::object_at<GridShaderCollection>(gabi::ea(collection));
}
inline cXyz& grid_vector(const void* vector) { return gabi::object_at<cXyz>(gabi::ea(vector)); }

// HD: the debugger interface has its vtable at the end, after the tuning fields.
struct daHo_HIO_c {
    be<s8> mNo;
    be<u8> lightingEnabled, flag2, flag3, useForcedFurl;
    u8 padding5[3];
    be<f32> furlThreshold, forcedFurl, waveAmplitude, gain, minimumGain, openingStep;
    cXyz scale;
    be<u8> farAlpha, nearAlpha;
    u8 padding2E[2];
    be<f32> alphaDistance;
    be<u8> freeze, useCustomFold;
    u8 padding36[2];
    be<f32> rates[13], folds[13];
    be<u32> vtable;
};
WWHD_OFFSET(daHo_HIO_c, scale, 0x20);
WWHD_OFFSET(daHo_HIO_c, vtable, 0xA0);
inline daHo_HIO_c& grid_hio() { return gabi::object_at<daHo_HIO_c>(0x104643AC); }
struct GridSinCos {
    be<f32> sine, cosine;
};
inline GridSinCos& grid_sinCos(u32 angle) {
    return gabi::object_at<GridSinCos>(0x104A44F8 + ((angle & 65535u) >> 3) * 8);
}
inline be<u8>& grid_alternateSail() { return gabi::object_at<be<u8>>(0x10464398); }
inline be<u32>& grid_shipAddress() { return gabi::object_at<be<u32>>(0x1046438C); }
inline be<u32>& grid_matrixAddress() { return gabi::object_at<be<u32>>(0x1018C7B0); }
inline be<f32>& grid_debugFlutter() { return gabi::object_at<be<f32>>(0x1047B9AC); }
inline be<s16>& grid_debugOscillationSpeed() { return gabi::object_at<be<s16>>(0x1047C0AC); }
inline be<f32>& grid_debugRowRate(u32 index) { return gabi::object_at<be<f32>>(0x1047B990 + index * 4); }

struct GridFloatColor {
    be<f32> r, g, b, a;
};
struct GridVertex {
    cXyz position, normal;
    be<f32> u, v;
};
WWHD_SIZE(GridVertex, 32);
struct GridVertexWords {
    be<u32> words[8];
};
struct GridShader {
    u8 prefix[0xF0];
    be<u32> vtable;
};
WWHD_SIZE(GridShader, 0xF4);
struct GridIndexBuffer {
    be<u32> indexType, primitiveMode, indexOffset, indexCount, reserved10, reserved14;
};
WWHD_SIZE(GridIndexBuffer, 0x18);
struct GridDrawInfo {
    u8 prefix[12];
    be<s32> kind;
    u8 padding10[4];
    be<u32> extra;
    u8 padding18[0x30 - 0x18];
    be<u32> alternateExtra;
};
struct GridRenderState {
    be<u8> blendEnabled, colorWrite;
    u8 padding2[6];
    be<u32> cullMode, mode;
    u8 padding10[0xE0 - 0x10];
    be<u8> alphaTest;
    u8 paddingE1[0xEC - 0xE1];
    be<u32> flags;
    u8 tail[0x30];
};
WWHD_SIZE(GridRenderState, 0x120);
struct GridHeap {
    u8 prefix[12];
    be<u32> vtable;
};
struct GridHeapVtable {
    u8 prefix[0x34];
    be<u32> allocate, reserved38, free;
};
struct GridShaderVtable {
    u8 prefix[12];
    be<u32> destroy;
};
struct GridMaterialVtable {
    u8 prefix[0x2C];
    be<u32> apply;
};
struct GridMaterialInterface {
    u8 prefix[12];
    be<u32> vtable;
};
struct GridVertexBufferView {
    u8 prefix[0x14C];
    be<u32> byteLength;
};
struct GridShipView {
    u8 prefix[0x64C];
    be<s16> sailAngle;
};
struct GridPlayView {
    u8 prefix[0x5292];
    be<u8> eventRunning;
    u8 padding5293[0x5AF8 - 0x5293];
    be<u32> camera;
    u8 padding5AFC[0x5D2C - 0x5AFC];
    be<u8> alternateSail;
    u8 padding5D2D[0x5D60 - 0x5D2D];
    be<u32> monotoneBuffer;
    u8 padding5D64[0x5D78 - 0x5D64];
    be<u32> opaqueBuffer;
    u8 padding5D7C[0x5D94 - 0x5D7C];
    be<u32> translucentBuffer;
};
struct GridDrawBuffer {
    u8 prefix[0x1C];
    be<u32> zMatrix;
};

// HD: shader archives and GPU material interfaces have no GameCube counterpart.
struct GridArchiveCatalog {
    be<u32> reserved, root, count, entries;
};
struct GridArchiveRoot {
    u8 prefix[0x1C];
    be<u32> programCount, programs;
};
struct GridArchiveEntry {
    u8 prefix[0x20];
    be<u8> initialized;
    u8 tail[3];
};
struct GridShaderProgram {
    be<u8> flags;
    u8 padding1[3];
    be<u32> displayList, reserved8, displayListSize;
    u8 prefix10[0x7C - 0x10];
    be<u32> resource;
};
struct GridShaderResource {
    u8 prefix[0x20];
    be<u32> textureData, reserved24, group;
};
struct GridShaderObject {
    be<u32> program, reserved4, reserved8, uniformCount, uniformInfo, textureCount, textures;
};
struct GridShaderLocations {
    u8 prefix[12];
    be<s16> vertex, pixel, geometry;
};
struct GridUniformBinding {
    be<u32> reserved, size, reserved8, data;
    u8 tail[12];
};
WWHD_SIZE(GridUniformBinding, 0x1C);
struct GridMaterialBindings {
    u8 prefix[0x4C];
    be<u32> currentLocation;
};
struct GridActiveShader {
    be<u32> group, program;
};
struct GridShaderArray {
    be<s32> count;
    be<u32> array;
};
struct GridShaderArraySelection {
    be<u32> reserved, count, array;
};
struct GridMaterialExtra {
    be<u32> reserved, material;
};
struct GridStringVtable {
    u8 prefix[0x14];
    be<u32> assureTermination;
};
struct GridList {
    be<u32> count, array;
};
struct GridPacketPrefix {
    u8 prefix[12];
    be<u32> vtable;
};
struct GridMaterialSlot {
    be<u32> reserved0, reserved4, reserved8, vtable;
};
struct GridTextureHandle {
    u8 prefix[0x28];
    be<u32> handle;
    u8 padding2C[4];
    be<u32> offset;
    u8 prefix34[0xD4 - 0x34];
    be<u32> duplicateHandle;
    u8 paddingD8[4];
    be<u32> duplicateOffset;
};
inline GridTextureHandle& grid_texture(const void* packet, u32 offset) {
    return gabi::object_at<GridTextureHandle>(gabi::ea(packet) + offset);
}
inline be<u32>& grid_textureWord(const void* packet, u32 offset, u32 wordOffset) {
    return gabi::object_at<be<u32>>(gabi::ea(packet) + offset + wordOffset);
}
inline GridShaderArray& grid_shaderSide(const void* collection, u32 side) {
    return gabi::object_at<GridShaderArray>(gabi::ea(collection) + 4 + side * 8);
}
inline be<u32>& grid_ownedAddress(const void* object, u32 offset) {
    return gabi::object_at<be<u32>>(gabi::ea(object) + offset);
}
inline GridFloatColor& grid_uniformColor(const void* packet, u32 offset) {
    return gabi::object_at<GridFloatColor>(gabi::ea(packet) + offset);
}

struct GridLightObject {
    be<f32> vectors[6];
    be<u8> color[4];
    be<u16> parameters[4];
    be<f32> attenuation[8];
};
WWHD_SIZE(GridLightObject, 0x44);
struct GridActorLighting {
    u8 prefix[0x110];
    GridLightObject primary;
    u8 padding154[0x194 - 0x154];
    GridVectorWords worldPosition;
    be<u16> colorC0[4];
    be<u32> colorK0, colorK1;
    be<u16> fogColor[4];
    be<f32> fogParameters[3];
    be<u8> state[9];
    u8 padding1CD[3];
    GridLightObject secondary;
    u8 padding214[0x254 - 0x214];
    GridLightObject extra;
};
WWHD_OFFSET(GridActorLighting, worldPosition, 0x194);
WWHD_OFFSET(GridActorLighting, extra, 0x254);
struct GridLightAttenuation {
    u8 prefix[0x24];
    be<f32> color, material;
    u8 padding2C[0x16C - 0x2C];
    be<f32> extra;
};
inline dKy_tevstr_c& grid_lighting(const void* lighting) {
    return gabi::object_at<dKy_tevstr_c>(gabi::ea(lighting));
}

inline be<u8>& grid_monotone() { return gabi::object_at<be<u8>>(0x101F4829); }

inline be<u32>& grid_heapManager() { return gabi::object_at<be<u32>>(0x101F8B4C); }

inline be<u32>& grid_textureManager() { return gabi::object_at<be<u32>>(0x101F8B18); }

inline be<u32>& grid_resourceManager() { return gabi::object_at<be<u32>>(0x101F4F28); }

inline be<u32>& grid_archiveManager() { return gabi::object_at<be<u32>>(0x101F4F7C); }

inline be<u32>& grid_projection() { return gabi::object_at<be<u32>>(0x104B4708); }

inline be<u32>& grid_indexCount() { return gabi::object_at<be<u32>>(0x101B5E34); }

inline be<u32>& grid_vertexCount() { return gabi::object_at<be<u32>>(0x101B5E30); }

inline be<f32>& grid_minimumAngle() { return gabi::object_at<be<f32>>(0x10464390); }

inline be<f32>& grid_maximumAngle() { return gabi::object_at<be<f32>>(0x10464394); }

inline be<u32>& grid_commonStatic(u32 offset) { return gabi::object_at<be<u32>>(0x1046439C + offset); }
inline be<u8>& grid_stringByte(u32 address, u32 index) { return gabi::object_at<be<u8>>(address + index); }
struct GridPlayPlayer {
    u8 prefix[0x5B34];
    be<u32> player;
};

// Exact verified widths for named external call boundaries.
void grid_PSVECSubtract(void*, void*, void*);
void grid_dComIfG_resDelete(void*, const char*);
void grid_mDoHIO_subRoot_c__deleteChild(signed char);
void grid_dl(void*);
int grid_daGrid_c___draw(daGrid_c*);
int grid_mDoMtx_YrotS(void*, short);
int grid_daGrid_c___execute(daGrid_c*);
void* grid_allocateObject(int);
void grid_constructVertexBuffer(void*);
void grid_constructShader(void*);
void grid_destroyShader(void*, int);
void grid_destroyVertexBuffer(void*, int);
void* grid_getPlay();
void grid_cXyz____pl(void*, cXyz*, void*);
void grid_cXyz____mi(void*, cXyz*, cXyz*);
float grid_PSVECSquareMag(cXyz*);
float grid_sqrt(float);
void grid_ho_move(daGrid_c*);
int grid_daGrid_c___create(daGrid_c*);
void grid_cXyz____mi_2(void*, cXyz*, void*);
void grid_cXyz__outprod(cXyz*, cXyz*, cXyz*);
void grid_cXyz__normZC(cXyz*, cXyz*);
void grid_MtxPosition(cXyz*, cXyz*);
void grid_PSVECAdd(cXyz*, cXyz*, cXyz*);
int grid_cXyz__normalizeRS(cXyz*);
void grid_MtxPush();
void grid_mDoMtx_YrotM(void*, short);
void grid_MtxPull();
void grid_MtxTrans(float, float, float, int);
void grid_mDoMtx_XrotM(void*, short);
void grid_mDoMtx_ZrotM(void*, short);
void grid_MtxScale(float, float, float, int);
void grid_PSMTXCopy(void*, void*);
void grid_J3DDrawBuffer__entryImm(void*, void*, int);
void grid_updateSailMaterial(void*);
void grid_fopAc_ac_c__fopAc_ac_c(daGrid_c*);
void grid_constructSailPacket(void*);
int grid_dComIfG_resLoad(void*, const char*);
int grid_mDoHIO_subRoot_c__createChild(const char*, void*);
float grid_sin(float);
void* grid_fopAcIt_Judge(void*, gabi::be<short>*);
void grid_register_global_object(void*);
void grid_constructCommonByteA(void*);
void grid_constructCommonByteB(void*);
void* grid_findOwningHeap(void*, void*);
void grid_releaseShader(void*);
void grid_destroyUniformBinding(void*);
void grid_destroyTexture(void*, int);
void grid_destroyIndexBuffer(void*, int);
void grid_releaseUniformObject(void*, int);
void grid_destroyMaterialList(void*, int);
void grid_destroy_arr(void*, int, int, void*, int, int);
void grid_destroyMaterialPacket(void*, int);
void grid_fopAc_ac_c___fopAc_ac_c(daGrid_c*, int);
void grid_dl_2(daGrid_c*);
void grid_lockTextureManager(void*);
void grid_loadTextureResource(void*, void*);
void grid_assignTexture(void*, void*);
void grid_unlockTextureManager(void*);
void grid_flushVertexBuffer(void*, int, unsigned int);
void grid_updateLighting(void*);
void grid_copyViewMatrix(gabi::be<unsigned int>*, void*);
void grid_setMaterialView(void*, int, gabi::be<unsigned int>*, void*, void*);
void grid_signedColorToFloat(gabi::be<unsigned int>*, void*);
void grid_scaleMaterialColor(void*, gabi::be<unsigned int>*, float);
void grid_PSMTXCopy_2(gabi::be<unsigned int>*, void*);
void grid_scaleUniformColor(void*, float);
void grid_byteColorToFloat(void*, void*);
void grid_updateMaterialUniforms(void*, int);
void* grid_dKyw_get_wind_vec();
float grid_dKyw_get_wind_pow();
int grid_cM_atan2s(float, float);
void grid_cLib_addCalcAngleS2(void*, short, int, int);
void grid_mDoMtx_YrotS_2(void*, short);
void grid_cLib_addCalc0(void*, float, float);
void grid_cLib_addCalc2(void*, float, float, float);
void grid_setSailNormalMatrix(void*, cXyz*);
void grid_daHo_packet_c__setNrmVtx(void*, void*, int, int);
void grid_daHo_packet_c__setTopNrmVtx(void*, void*);
void grid_daHo_packet_c__setBackNrm(void*);
void grid_DCStoreRangeNoSync(void*, int);
void* grid_getCurrentHeap(void*);
void grid_constructMaterialPacket(void*);
void grid_construct_array(void*, int, int, void*);
void grid_constructMaterialList(void*);
void grid_constructUniformObject(void*);
void grid_clearStorage(void*, int);
void grid_constructIndexBuffer(void*);
void grid_constructTexture(void*);
void grid_constructTextureResource(void*);
void* grid_getShaderArchives();
int grid_findShaderArchive(void*, gabi::be<unsigned int>*);
void grid_initializeShaderArchive(void*, void*, int);
void grid_createShaderCollections(void*, void*, int);
void grid_initializeVertexBuffer(void*, void*, int, void*);
void grid_initializeShader(void*, void*, void*, void*, int);
void grid_createDrawMaterials(void*, int, int);
void grid_initializeIndexBuffer(void*, void*, int, unsigned int);
void* grid_getArchiveResource(void*, gabi::be<unsigned int>*, gabi::be<unsigned int>*, int);
void grid_loadAlternateTexture(void*, void*, const char*);
void grid_GX2SetPixelUniformBlock(short, unsigned int, unsigned int);
void grid_GX2SetVertexUniformBlock(short, unsigned int, unsigned int);
void grid_GX2SetGeometryUniformBlock(short, unsigned int, unsigned int);
void grid_bindTexture(void*, void*, int, int);
void* grid_getActiveShader(void*);
void grid_updateShaderProgram(void*, int);
void grid_bindShaderGroup(void*);
void grid_GX2CallDisplayList(void*, unsigned int);
void grid_bindShaderProgram(void*);
void grid_initializeRenderState(void*);
void grid_applyExtraMaterial(void*, void*);
void grid_configureRenderState(int, void*);
void grid_applyRenderState(void*);
void grid_bindShader(void*);
void grid_GX2DrawIndexedEx(unsigned int, unsigned int, unsigned int, unsigned int, int, int);
int grid_daGrid_c___delete(daGrid_c*);

struct GridGraphicsSystem {
    u8 prefix[0x18C];
    GridRenderState state;
};

struct GridHeapManager;
struct GridTextureManager;
struct GridResourceManager;
struct GridArchiveManager;
struct GridAllocation;
struct GridTextureData;
struct GridProjection;
struct GridShaderGroup;
struct GridDrawMaterialColors {
    u8 prefix[0x1C4];
    GridFloatColor primary, extra;
};
inline Mtx34& grid_viewMatrix() { return gabi::object_at<Mtx34>(0x104B45F8); }
inline GridProjection& grid_projectionMatrix() { return gabi::object_at<GridProjection>(0x104B470C); }

inline void* grid_actorNameJudge() { return gabi::at<void>(0x025E121C); }

inline void* grid_destructorRecordA() { return gabi::at<void>(0x101B6AD8); }

inline void* grid_destructorRecordB() { return gabi::at<void>(0x101B6AE4); }

inline void* grid_destructorRecordC() { return gabi::at<void>(0x101B6AF0); }

inline void* grid_destructorRecordHio() { return gabi::at<void>(0x101B6AFC); }

inline void* grid_commonByteA() { return gabi::at<void>(0x10464399); }

inline void* grid_commonByteB() { return gabi::at<void>(0x1046439A); }

inline void* grid_callbackPointer(const void* fn) { return gabi::at<void>(gabi::guest_fn(fn)); }

static void materialDestroy(void* p, s32 flags);

static void* materialConstruct(void* p);

static void* vertexDescriptorConstruct(void* p);

struct GridTextureBinding;
struct GridDisplayList;

void* grid_getObjectResource(void*, be<u32>*, int);

// namespace grid_tu
struct GridLightExtraColor {
    u8 prefix[0x160];
    GXColorS10 color;
};
struct GridStartStage {
    u8 prefix[0x5134];
    be<u8> name[8];
};

// Guest pointers retain the null case; object references would imply non-null to the host compiler.
template <class T> inline T* grid_pointer(u32 address) { return gabi::at<T>(address); }
