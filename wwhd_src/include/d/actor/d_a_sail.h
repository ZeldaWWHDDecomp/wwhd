/* Pirate ship sail, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
using SailColorScratch = GXColor[4];
// HD: GPU material resources have no GameCube counterpart.
struct SailGpuMaterial {
    gptr<void> vertices;
    u8 vertexBuffer[0x154];
    u8 shaderObject[0xF4];
    be<u32> allocationCount;
    gptr<void> allocation;
};
WWHD_SIZE(SailGpuMaterial, 0x254);
struct SailMaterialPool {
    SailGpuMaterial materials[4];
    be<u32> current;
    be<u32> attributeCount;
    be<u32> stride;
    gptr<void> attributes;
    be<u32> unused;
    be<u8> initialized;
    u8 padding[3];
};
WWHD_SIZE(SailMaterialPool, 0x968);
struct SailShaderCollection {
    gptr<void> program;
    be<s32> frontShaderCount;
    gptr<void> frontShaders;
    be<s32> backShaderCount;
    gptr<void> backShaders;
};
WWHD_SIZE(SailShaderCollection, 0x14);
WWHD_OFFSET(SailShaderCollection, frontShaders, 8);
WWHD_OFFSET(SailShaderCollection, backShaders, 0x10);

struct SailUniformBindingStorage {
    u8 data[0x1C];
};
struct SailUniformObject {
    u8 prefix[12];
    be<u32> vtable;
    SailUniformBindingStorage bindings[2];
    u8 padding48[4];
    be<u32> currentLocation;
    u8 tail[0x74 - 0x50];
};
WWHD_SIZE(SailUniformObject, 0x74);
struct SailDescriptor {
    be<u32> words[4];
};
struct SailUniformState {
    be<f32> initialColors[44];
    SailDescriptor descriptors[3][2];
    Mtx34 matrices[8];
    SailDescriptor finalDescriptors[6];
};
WWHD_SIZE(SailUniformState, 0x2F0);
struct SailProgramList {
    be<s32> count;
    gptr<SailShaderCollection> entries;
};
struct SailUniformList {
    be<s32> count;
    gptr<void> objects;
};
struct SailIndexBuffer {
    be<u32> indexType, mode, base, count, stride, reserved;
};
struct SailTextureResource {
    u8 data[0x90];
};
struct SailTexture {
    u8 data[0x198];
};
/* daSail_HIO_c l_HIO (HD: the vtable after the members) */
struct daSail_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<u8> m06;
    /* 0x03 */ be<u8> m07;
    /* 0x04 */ be<u8> m08;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<u32> __vtbl;
};
WWHD_SIZE(daSail_HIO_c, 0x14);
inline daSail_HIO_c& l_HIO() { return *gabi::at<daSail_HIO_c>(0x1046D78C); }

struct daSail_packet_c {
    /* 0x0000 */ u8 _000[0xC];   /* J3DPacket (HD, constructor 027F1278) */
    /* 0x000C */ be<u32> __vtbl; /* 1003970C */
    // HD: shader and GPU material objects precede the GameCube cloth data.
    u8 gpuPrefix[0x98 - 0x10];
    be<u32> programCount;
    SailProgramList programs;
    SailMaterialPool materialPool;
    SailUniformList uniforms;
    be<u32> uniformFlags;
    SailUniformObject drawUniform;
    Mtx34 modelMatrix;
    be<u32> modelFlags;
    SailUniformObject materialUniform;
    SailUniformState uniformState;
    be<u8> uniformFlag;
    u8 paddingE25[3];
    SailIndexBuffer indexBuffer;
    SailTextureResource sailResource, toonResource;
    SailTexture sailTexture, toonTexture;
    be<u8> textureLoaded;
    u8 padding1291[3];
    /* 0x1294 */ Mtx34 mMtx;
    /* 0x12C4 */ Mtx34 mTexMtx;
    /* 0x12F4 */ Mtx34 mStickMtx;
    /* 0x1324 */ gptr<dKy_tevstr_c> mTevStr;
    /* 0x1328 */ cXyz mPos[2][0x54];
    /* 0x1B08 */ cXyz mPosSpd[0x54];
    /* 0x1EF8 */ cXyz mNrm[2][0x54];
    /* 0x26D8 */ cXyz mBackNrm[2][0x54];
    /* 0x2EB8 */ be<s16> m1C34;
    /* 0x2EBA */ be<s16> m1C36;
    /* 0x2EBC */ be<s16> m1C38;
    /* 0x2EBE */ be<u8> m1C3A;
    /* 0x2EBF */ be<u8> m1C3B;

    cXyz* getPos() { return mPos[(u8)m1C3A]; }
    cXyz* getNrm() { return mNrm[(u8)m1C3A]; }
    cXyz* getBackNrm() { return mBackNrm[(u8)m1C3A]; }
    void setCorrectNrmAngle(s16, f32);
    void setNrmMtx();
    void setBackNrm();
    void setNrmVtx(cXyz*, int, int);
    /* HD */
    void update();
    void draw(void* ctx);
};
WWHD_OFFSET(daSail_packet_c, mMtx, 0x1294);
WWHD_OFFSET(daSail_packet_c, mTevStr, 0x1324);
WWHD_OFFSET(daSail_packet_c, mPos, 0x1328);
WWHD_OFFSET(daSail_packet_c, mPosSpd, 0x1B08);
WWHD_OFFSET(daSail_packet_c, mNrm, 0x1EF8);
WWHD_OFFSET(daSail_packet_c, mBackNrm, 0x26D8);
WWHD_OFFSET(daSail_packet_c, m1C34, 0x2EB8);
WWHD_SIZE(daSail_packet_c, 0x2EC0);

struct sail_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mClothPhase;
    /* 0x3B4 */ request_of_phase_process_class mKaizokusenPhase;
    /* 0x3BC */ daSail_packet_c mSailPacket;
    /* 0x327C */ gptr<J3DModel> mStickModel; /* GameCube: packet m1C3C */
    /* 0x3280 */ be<s32> m1C40;
    /* 0x3284 */ be<f32> m1C44;
    /* 0x3288 */ be<f32> m1C48;
    /* 0x328C */ be<f32> m1C4C;
    /* 0x3290 */ be<s16> m1C50;
    /* 0x3292 */ be<s16> m1C52;
};
WWHD_OFFSET(sail_class, mSailPacket, 0x3BC);
WWHD_OFFSET(sail_class, mStickModel, 0x327C);
WWHD_OFFSET(sail_class, m1C44, 0x3284);
WWHD_OFFSET(sail_class, m1C52, 0x3292);
WWHD_SIZE(sail_class, 0x3294);

WWHD_OFFSET(daSail_packet_c, programCount, 0x98);
WWHD_OFFSET(daSail_packet_c, materialPool, 0xA4);
WWHD_OFFSET(daSail_packet_c, uniforms, 0xA0C);
WWHD_OFFSET(daSail_packet_c, drawUniform, 0xA18);
WWHD_OFFSET(daSail_packet_c, uniformState, 0xB34);
WWHD_OFFSET(daSail_packet_c, indexBuffer, 0xE28);
WWHD_OFFSET(daSail_packet_c, sailTexture, 0xF60);
WWHD_OFFSET(daSail_packet_c, textureLoaded, 0x1290);
struct SailDrawState {
    u8 prefix[8];
    be<u32> cullMode, mode;
    u8 padding10[0xE0 - 0x10];
    be<u8> alphaTest;
    u8 paddingE1[3];
    be<u32> compare;
    be<f32> reference;
    be<u32> flags;
    u8 tail[0x118 - 0xF0];
    be<u32> vtable;
};
WWHD_SIZE(SailDrawState, 0x11C);
struct SailVectorWords {
    be<u32> x, y, z;
};
struct SailMatrixWords {
    be<u32> words[12];
};
struct SailFloatColorWords {
    be<u32> r, g, b, a;
};
struct SailColorS16 {
    be<s16> r, g, b, a;
};
struct SailColorU8 {
    be<u8> r, g, b, a;
};
template <class T> inline T* sail_pointer(u32 address) { return gabi::at<T>(address); }
inline SailVectorWords& sail_words(const void* pointer) {
    return gabi::object_at<SailVectorWords>(gabi::ea(pointer));
}

static BOOL daSail_checkCreateHeap(fopAc_ac_c*);
static void* vtx_ct(void*);
static void* elem10_ct(void*);
static void vtx_dt(void*, s32);

void sail_vectorCross(const cXyz*, cXyz*, const cXyz*);
void sail_vectorNormalize(const cXyz*, cXyz*);
cXyz* sail_getWindVector();
float sail_getWindPower();
float sail_addCalc(gabi::be<float>*, float, float, float, float);
void sail_getLightVector(cXyz*, cXyz*);
void sail_vectorSubtract(const cXyz*, const cXyz*, cXyz*);
void sail_DCStoreRangeNoSync(void*, unsigned int);
void sail_matrixConcat(const Mtx34*, const Mtx34*, Mtx34*);
void sail_matrixIdentity(Mtx34*);
void sail_entryImm(unsigned int, void*, unsigned int);
void sail_clearStorage(unsigned int, unsigned int);
void sail_construct_array(unsigned int, unsigned int, unsigned int, unsigned int);
void sail_rebuildTexture(unsigned int, unsigned int);
void sail_lockTextureManager(unsigned int);
void sail_loadTexture(unsigned int, unsigned int);
void sail_unlockTextureManager(unsigned int);
void sail_flushVertexBuffer(unsigned int, int, unsigned int);
void sail_updateLighting(unsigned int);
void sail_calculateLightingColors(unsigned int);
void sail_setMaterialView(unsigned int, int, Mtx34*, unsigned int, unsigned int);
void sail_scaleMaterialColor(unsigned int, SailColorScratch*, float);
void sail_flushDrawMaterial(unsigned int, int);
void sail_scaleUniformColor(unsigned int, float);
void sail_flushUniformObject(unsigned int);
unsigned int sail_getDemoActor(unsigned int, unsigned char);
void sail_assureStringTermination(SafeString*);
unsigned int sail_getCurrentHeap(unsigned int);
unsigned int sail_findOwningHeap(unsigned int, unsigned int);
void sail_constructVertexBuffer(unsigned int);
void sail_constructShader(unsigned int);
void sail_destroyShader(unsigned int, int);
void sail_destroyVertexBuffer(unsigned int, int);
void sail_constructPacket(daSail_packet_c*);
void sail_constructMaterialList(unsigned int);
void sail_constructUniformObject(unsigned int);
void sail_constructIndexBuffer(unsigned int);
void sail_constructTextureResource(unsigned int);
void sail_constructTexture(unsigned int);
unsigned int sail_getShaderArchives(unsigned int);
int sail_findShaderArchive(unsigned int, SafeString*);
void sail_initializeShaderArchive(unsigned int, unsigned int, int);
void sail_createShaderCollections(unsigned int, unsigned int, int);
void sail_initializeVertexBuffer(unsigned int, unsigned int, int, unsigned int);
void sail_initializeShader(unsigned int, unsigned int, unsigned int, unsigned int, int);
void sail_createDrawMaterials(unsigned int, int, int);
void sail_initializeIndexBuffer(unsigned int, unsigned int, int, int);
void sail_releaseShader(unsigned int);
void sail_destroyUniformBinding(unsigned int);
void sail_destroyTexture(unsigned int, int);
void sail_destroyIndexBuffer(unsigned int, int);
void sail_releaseUniformObject(unsigned int, int);
void sail_destroyMaterialList(unsigned int, int);
void sail_destroyArray(unsigned int, int, int, unsigned int, int, int);
void sail_destroyMaterialPacket(unsigned int, int);
void sail_destroyActor(sail_class*, int);
void sail_GX2SetPixelUniformBlock(short, unsigned int, unsigned int);
void sail_GX2SetVertexUniformBlock(short, unsigned int, unsigned int);
void sail_GX2SetGeometryUniformBlock(short, unsigned int, unsigned int);
void sail_bindTexture(unsigned int, unsigned int, int, int);
void sail_GX2DrawIndexedEx(unsigned int, int, unsigned int, unsigned int, int, int);
unsigned int sail_getActiveShader(unsigned int);
void sail_updateShaderProgram(unsigned int, int);
void sail_bindShaderGroup(unsigned int);
void sail_GX2CallDisplayList(unsigned int, unsigned int);
void sail_bindShaderProgram(unsigned int);
void sail_applyExtraMaterial(unsigned int, unsigned int);
void sail_initializeRenderState(unsigned int);
void sail_configureRenderState(int, unsigned int);
void sail_applyRenderState(unsigned int);
void sail_bindShader(unsigned int);
void sail_engine2750684(unsigned int);

struct SailVertex {
    cXyz position, normal;
    be<f32> u, v;
};
WWHD_SIZE(SailVertex, 0x20);
struct SailVertexWords {
    be<u32> words[8];
};
struct SailVertexBufferView {
    u8 prefix[0x14C];
    be<u32> byteLength;
};
struct SailHeap {
    u8 prefix[12];
    be<u32> vtable;
};
struct SailHeapVtable {
    u8 prefix[0x34];
    be<u32> allocate, reserved38, free;
};
struct SailMaterialInterface {
    u8 prefix[12];
    be<u32> vtable;
};
struct SailMaterialVtable {
    u8 prefix[0x2C];
    be<u32> apply;
};
struct SailShader {
    u8 prefix[0xF0];
    be<u32> vtable;
};
struct SailShaderVtable {
    u8 prefix[12];
    be<u32> destroy;
};
struct SailShaderArray {
    be<s32> count;
    be<u32> array;
};
struct SailList {
    be<s32> count;
    be<u32> array;
};
struct SailArchiveCatalog {
    be<u32> reserved, root, count, entries;
};
struct SailArchiveRoot {
    u8 prefix[0x1C];
    be<u32> programCount, programs;
};
struct SailArchiveEntry {
    u8 prefix[0x20];
    be<u8> initialized;
    u8 tail[3];
};
WWHD_SIZE(SailArchiveEntry, 0x24);
struct SailShaderProgram {
    be<u8> flags;
    u8 padding1[3];
    be<u32> displayList, reserved8, displayListSize;
    u8 prefix10[0x7C - 0x10];
    be<u32> resource;
    u8 tail[4];
};
WWHD_SIZE(SailShaderProgram, 0x84);
struct SailShaderResource {
    u8 prefix[0x20];
    be<u32> textureData, reserved24, group;
};
struct SailShaderObject {
    be<u32> program, reserved4, reserved8, uniformCount, uniformInfo, textureCount, textures;
};
struct SailShaderLocations {
    u8 prefix[12];
    be<s16> vertex, pixel, geometry;
};
struct SailUniformBinding {
    be<u32> reserved, size, reserved8, data;
    u8 tail[12];
};
struct SailActiveShader {
    be<u32> group, program;
};
struct SailDrawContext {
    u8 prefix[12];
    be<s32> pass;
    u8 padding10[4];
    be<u32> extra;
    u8 padding18[0x30 - 0x18];
    be<u32> alternateExtra;
};
struct SailMaterialExtra {
    be<u32> reserved, material;
};
struct SailGraphicsSystem {
    u8 prefix[0x74];
    be<u32> opaqueBuffer, translucentBuffer;
    u8 padding7C[0x18C - 0x7C];
    SailDrawState state;
};
struct SailPlayView {
    u8 prefix[0x5292];
    be<u8> demoRunning;
    u8 padding5293[0x5D70 - 0x5293];
    be<u32> stickOpaqueBuffer, stickTranslucentBuffer, sailOpaqueBuffer, sailTranslucentBuffer;
};
struct SailShipView : fopAc_ac_c {
    u8 prefix3AC[0x3E4 - 0x3AC];
    be<u8> m2CC, padding3E5, m2CE, padding3E7;
    gptr<J3DModel> mModel;
};
struct SailDemoActorView {
    u8 prefix[4];
    be<u16> enabled;
    u8 padding6[0x30 - 6];
    be<f32> animationFrame;
};
struct SailImageResource {
    u8 prefix[0x20];
    be<u32> texture;
};
struct SailTextureView {
    be<u32> reserved, width, height, depth, dimension, format, mipCount, arrayCount;
    u8 padding20[8];
    be<u32> image;
    u8 padding2C[4];
    be<u32> mipOffset, tileMode, swizzle;
    u8 padding3C[0xD4 - 0x3C];
    be<u32> duplicateImage;
    u8 paddingD8[4];
    be<u32> duplicateMipOffset;
    u8 paddingE0[0x15C - 0xE0];
    be<u32> wrapY, wrapX, wrapZ;
    u8 padding168[0x190 - 0x168];
    be<u8> flags;
};
struct SailLightingParameters {
    u8 prefix[0x24];
    be<f32> colorScale, materialScale;
    u8 padding2C[0x9F - 0x2C];
    be<u8> specialColorAlpha;
    u8 paddingA0[0x16C - 0xA0];
    be<f32> extraScale;
};
struct SailStringVtable {
    u8 prefix[0x14];
    be<u32> assureTermination;
};
template <class T> inline T* sail_view(u32 address) { return gabi::at<T>(address); }
inline be<u32>& sail_shipAddress() { return *sail_view<be<u32>>(0x1046D76C); }
inline be<u32>& sail_heapAddress() { return *sail_view<be<u32>>(0x101F8B4C); }
inline be<u32>& sail_textureLock() { return *sail_view<be<u32>>(0x101F8B18); }
inline be<u32>& sail_demoManager() { return *sail_view<be<u32>>(0x101D5FFC); }
inline be<u32>& sail_projectionAddress() { return *sail_view<be<u32>>(0x104B4708); }
inline be<u32>& sail_offsetInitialized() { return *sail_view<be<u32>>(0x1046D7AC); }
inline cXyz& sail_offset() { return *sail_view<cXyz>(0x1046D7A0); }

struct SailModelView {
    u8 prefix[0xC8];
    SailMatrixWords baseMatrix;
};
struct SailTexCoord {
    be<f32> u, v;
};

struct SailLightObject {
    be<f32> vectors[6];
    be<u8> color[4], parameters[8];
    be<f32> attenuation[8];
};
WWHD_SIZE(SailLightObject, 0x44);
struct SailLightingCopyView {
    SailLightObject primary;
    u8 padding44[0x84 - 0x44];
    be<u8> state[0x30];
    be<u8> flags[9];
    u8 paddingBD[3];
    SailLightObject secondary;
    u8 padding104[0x144 - 0x104];
    SailLightObject extra;
};
struct SailLightingPositionView {
    u8 prefix[0xA8];
    be<f32> position[3];
};

// HD: GPU buffer indices form wrapping 32-bit byte addresses.
inline SailGpuMaterial* sail_materialOffset(u32 pool, u32 offset) {
    return gabi::at<SailGpuMaterial>(pool + offset);
}

inline Mtx34* sail_viewMatrix() { return gabi::at<Mtx34>(0x104B45F8); }

struct SailLightingColors {
    u8 prefix[0x90];
    SailColorS16 ambient;
    SailColorU8 material, special;
};
struct SailLightingExtraColor {
    u8 prefix[0x160];
    SailColorS16 ambient;
};
struct SailDrawMaterial {
    u8 prefix[0x1C4];
    SailFloatColorWords frontColor, backColor;
    u8 tail[0x23C - 0x1E4];
};
WWHD_SIZE(SailDrawMaterial, 0x23C);

enum SailResourceIndex {
    dRes_INDEX_CLOTH_BTI_TOON_e = 3,
    dRes_INDEX_KAIZOKUSEN_BTI_SAIL_e = 10,
    dRes_INDEX_KAIZOKUSEN_BDL_AMAST_e = 13,
};
inline void sail_initializeHeaderStatics() { sinit_header_statics(0x1046D770, 0x101CFDAC); }
inline void sail_registerHio() { __register_global_object(0x101CFDD0); }
WWHD_OFFSET(SailGpuMaterial, vertexBuffer, 0x4);
WWHD_OFFSET(SailGpuMaterial, shaderObject, 0x158);
WWHD_OFFSET(SailGpuMaterial, allocationCount, 0x24C);
WWHD_OFFSET(SailGpuMaterial, allocation, 0x250);
WWHD_OFFSET(SailMaterialPool, current, 0x950);
WWHD_OFFSET(SailMaterialPool, attributeCount, 0x954);
WWHD_OFFSET(SailMaterialPool, attributes, 0x95C);
WWHD_OFFSET(SailUniformObject, bindings, 0x10);
WWHD_OFFSET(SailUniformObject, currentLocation, 0x4C);
WWHD_OFFSET(SailTextureView, image, 0x28);
WWHD_OFFSET(SailTextureView, mipOffset, 0x30);
WWHD_OFFSET(SailTextureView, duplicateImage, 0xD4);
WWHD_OFFSET(SailTextureView, wrapY, 0x15C);
WWHD_OFFSET(SailTextureView, flags, 0x190);
WWHD_OFFSET(SailLightingCopyView, state, 0x84);
WWHD_OFFSET(SailLightingCopyView, flags, 0xB4);
WWHD_OFFSET(SailLightingCopyView, secondary, 0xC0);
WWHD_OFFSET(SailLightingCopyView, extra, 0x144);
WWHD_OFFSET(SailDrawState, alphaTest, 0xE0);
WWHD_OFFSET(SailDrawState, compare, 0xE4);
WWHD_OFFSET(SailDrawState, reference, 0xE8);
WWHD_OFFSET(SailDrawState, flags, 0xEC);
WWHD_OFFSET(SailDrawState, vtable, 0x118);
WWHD_OFFSET(SailShipView, m2CC, 0x3E4);
WWHD_OFFSET(SailShipView, m2CE, 0x3E6);
WWHD_OFFSET(SailShipView, mModel, 0x3E8);
