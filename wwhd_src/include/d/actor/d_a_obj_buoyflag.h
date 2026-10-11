/* daObjBuoyflag (flag on a buoy / barrel), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" (every function is a stub). The HD actor keeps the GameCube
 * cloth simulation (Packet_c: 5x7 vertices, double-buffered positions and normals) but draws
 * through HD GPU objects (vertex buffers, shader/uniform blocks, a texture + sampler for the
 * "Cloth" archive), so Packet_c grew from 0xE00 to 0x2C64 bytes and Act_c from 0x112C to 0x3230.
 * Offsets below are measured from the WWHD code (_create, the calc/draw functions). */
#pragma once
#include "gc_api/gc_api.h"

// HD: private GX2 packet objects, absent from the GameCube layout.
struct BuoyGpuMaterial {
    gptr<void> vertices;
    u8 vertexBuffer[0x154];
    u8 shaderObject[0xF4];
    be<u32> allocationCount;
    gptr<void> allocation;
};
WWHD_SIZE(BuoyGpuMaterial, 0x254);
struct BuoyMaterialPool {
    BuoyGpuMaterial materials[4];
    be<u32> current;
    be<u32> attributeCount;
    be<u32> stride;
    gptr<void> attributes;
    be<u32> unused;
    be<u8> initialized;
    u8 padding[3];
};
WWHD_SIZE(BuoyMaterialPool, 0x968);
struct BuoyShaderCollection {
    gptr<void> program;
    be<s32> frontShaderCount;
    gptr<void> frontShaders;
    be<s32> backShaderCount;
    gptr<void> backShaders;
};
WWHD_SIZE(BuoyShaderCollection, 0x14);
WWHD_OFFSET(BuoyShaderCollection, frontShaders, 8);
WWHD_OFFSET(BuoyShaderCollection, backShaders, 0x10);

struct BuoyUniformBindingStorage {
    u8 data[0x1C];
};
struct BuoyUniformObject {
    u8 prefix[12];
    be<u32> vtable;
    BuoyUniformBindingStorage bindings[2];
    u8 padding48[4];
    be<u32> currentLocation;
    u8 tail[0x74 - 0x50];
};
WWHD_SIZE(BuoyUniformObject, 0x74);
struct BuoyDescriptor {
    be<u32> words[4];
};
struct BuoyUniformState {
    be<f32> initialColors[44];
    BuoyDescriptor descriptors[3][2];
    Mtx34 matrices[8];
    BuoyDescriptor finalDescriptors[6];
};
WWHD_SIZE(BuoyUniformState, 0x2F0);
struct BuoyProgramList {
    be<s32> count;
    gptr<BuoyShaderCollection> entries;
};
struct BuoyUniformList {
    be<s32> count;
    gptr<void> objects;
};
struct BuoyIndexBuffer {
    be<u32> indexType, mode, base, count, stride, reserved;
};
struct BuoyTextureResource {
    u8 data[0x90];
};
struct BuoyTexture {
    u8 data[0x198];
};

struct BuoyPoleMaterialPool {
    BuoyGpuMaterial materials[2];
    be<u32> current, attributeCount, stride;
    gptr<void> attributes;
    be<u32> unused;
    be<u8> initialized;
    u8 padding[3];
};
WWHD_SIZE(BuoyPoleMaterialPool, 0x4C0);
struct BuoyFlagGpuState {
    be<u32> programCount;
    BuoyProgramList programs;
    BuoyMaterialPool materialPool;
    BuoyIndexBuffer indexBuffer;
    BuoyUniformList uniforms;
    be<u32> uniformFlags;
    BuoyUniformObject drawUniform;
    Mtx34 modelMatrix;
    be<u32> modelFlags;
    BuoyUniformObject materialUniform;
    BuoyUniformState uniformState;
    be<u8> uniformFlag;
    u8 padding[3];
    BuoyTextureResource primaryResource, alternateResource;
    BuoyTexture primaryTexture, alternateTexture;
};
WWHD_SIZE(BuoyFlagGpuState, 0x11F8);
struct BuoyPoleGpuState {
    be<u32> programCount;
    BuoyProgramList programs;
    BuoyPoleMaterialPool materialPool;
    BuoyIndexBuffer indexBuffer;
    BuoyUniformList uniforms;
    be<u32> uniformFlags;
    BuoyUniformObject drawUniform;
    Mtx34 modelMatrix;
    be<u32> modelFlags;
    BuoyUniformObject materialUniform;
    BuoyUniformState uniformState;
    be<u8> uniformFlag;
    u8 padding[3];
    BuoyTextureResource textureResource;
    BuoyTexture texture;
};
WWHD_SIZE(BuoyPoleGpuState, 0xB28);

struct BuoyVectorWords {
    be<u32> x, y, z;
};
struct BuoyMatrixWords {
    be<u32> words[12];
};
struct BuoyFloatColorWords {
    be<u32> r, g, b, a;
};
struct BuoyColorS16 {
    be<s16> r, g, b, a;
};
struct BuoyColorU8 {
    be<u8> r, g, b, a;
};
struct BuoyImageResource {
    u8 prefix[0x20];
    be<u32> texture;
};
struct BuoyTextureView {
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

template <class T> inline T* buoyflag_view(u32 address) { return gabi::at<T>(address); }

// HD: GHS initializes these vectors lazily; guard and component write order is significant.
struct BuoyClothStatics {
    cXyz backNormal;
    cXyz offset;
    be<u32> offsetInitialized;
    be<u32> backNormalInitialized;
    cXyz poleNormals[11];
};
WWHD_OFFSET(BuoyClothStatics, offset, 12);
WWHD_OFFSET(BuoyClothStatics, poleNormals, 32);
inline BuoyClothStatics* buoyflag_clothStatics() { return gabi::at<BuoyClothStatics>(0x10469368); }
inline be<u8>* buoyflag_poleNormalsInitialized() { return gabi::at<be<u8>>(0x101C81E8); }

struct BuoyDrawState {
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
WWHD_SIZE(BuoyDrawState, 0x11C);

struct BuoyVertex {
    cXyz position, normal;
    be<f32> u, v;
};
WWHD_SIZE(BuoyVertex, 0x20);
struct BuoyVertexWords {
    be<u32> words[8];
};
struct BuoyVertexBufferView {
    u8 prefix[0x14C];
    be<u32> byteLength;
};
struct BuoyHeap {
    u8 prefix[12];
    be<u32> vtable;
};
struct BuoyHeapVtable {
    u8 prefix[0x34];
    be<u32> allocate, reserved38, free;
};
struct BuoyMaterialInterface {
    u8 prefix[12];
    be<u32> vtable;
};
struct BuoyMaterialVtable {
    u8 prefix[0x2C];
    be<u32> apply;
};
struct BuoyShader {
    u8 prefix[0xF0];
    be<u32> vtable;
};
struct BuoyShaderVtable {
    u8 prefix[12];
    be<u32> destroy;
};
struct BuoyShaderArray {
    be<s32> count;
    be<u32> array;
};
struct BuoyList {
    be<s32> count;
    be<u32> array;
};
struct BuoyArchiveCatalog {
    be<u32> reserved, root, count, entries;
};
struct BuoyArchiveRoot {
    u8 prefix[0x1C];
    be<u32> programCount, programs;
};
struct BuoyArchiveEntry {
    u8 prefix[0x20];
    be<u8> initialized;
    u8 tail[3];
};
WWHD_SIZE(BuoyArchiveEntry, 0x24);
struct BuoyShaderProgram {
    be<u8> flags;
    u8 padding1[3];
    be<u32> displayList, reserved8, displayListSize;
    u8 prefix10[0x7C - 0x10];
    be<u32> resource;
    u8 tail[4];
};
WWHD_SIZE(BuoyShaderProgram, 0x84);
struct BuoyShaderResource {
    u8 prefix[0x20];
    be<u32> textureData, reserved24, group;
};
struct BuoyShaderObject {
    be<u32> program, reserved4, reserved8, uniformCount, uniformInfo, textureCount, textures;
};
struct BuoyShaderLocations {
    u8 prefix[12];
    be<s16> vertex, pixel, geometry;
};
struct BuoyUniformBinding {
    be<u32> reserved, size, reserved8, data;
    u8 tail[12];
};
struct BuoyActiveShader {
    be<u32> group, program;
};
struct BuoyDrawContext {
    u8 prefix[12];
    be<s32> pass;
    u8 padding10[4];
    be<u32> extra;
    u8 padding18[0x30 - 0x18];
    be<u32> alternateExtra;
};
struct BuoyMaterialExtra {
    be<u32> reserved, material;
};
struct BuoyGraphicsSystem {
    u8 prefix[0x74];
    be<u32> opaqueBuffer, translucentBuffer;
    u8 padding7C[0x18C - 0x7C];
    BuoyDrawState state;
};

struct BuoyLightingParameters {
    u8 prefix[0x24];
    be<f32> colorScale, materialScale;
    u8 padding2C[0x9F - 0x2C];
    be<u8> specialColorAlpha;
    u8 paddingA0[0x16C - 0xA0];
    be<f32> extraScale;
};

struct BuoyLightingColors {
    u8 prefix[0x90];
    BuoyColorS16 ambient;
    BuoyColorU8 material, special;
};
struct BuoyLightingExtraColor {
    u8 prefix[0x160];
    BuoyColorS16 ambient;
};
struct BuoyDrawMaterial {
    u8 prefix[0x1C4];
    BuoyFloatColorWords frontColor, backColor;
    u8 tail[0x23C - 0x1E4];
};
WWHD_SIZE(BuoyDrawMaterial, 0x23C);

// HD: shared TU tables have one owner in the common pending source.
extern be<f32> (&buoyflag_positions)[35][3];
extern be<f32> (&buoyflag_texCoords)[35][2];
extern be<u8> (&buoyflag_strip)[62][3];
extern be<u16> (&buoyflag_indices)[144];
extern be<f32> (&buoyflag_polePositions)[11][3];
extern be<f32> (&buoyflag_poleTexCoords)[7][2];
extern be<u8> (&buoyflag_poleStrip)[35][3];
extern be<u16> (&buoyflag_poleIndices)[45];
extern be<u32>& buoyflag_poleVertexCount;
extern be<u32>& buoyflag_poleIndexCount;
extern be<u32>& buoyflag_vertexCount;
extern be<u32>& buoyflag_indexCount;

inline Mtx34* buoyflag_viewMatrix() { return gabi::at<Mtx34>(0x104B45F8); }

struct BuoyStripVertex {
    be<u8> position, normal, texCoord;
};
struct BuoyTexCoord {
    be<f32> u, v;
};
struct BuoySafeString {
    be<u32> string, vtable;
};
struct BuoyColorWords {
    be<u32> words[4];
};
struct BuoyCacheLine {
    be<u32> words[8];
};
inline be<u32>* buoyflag_textureManager() { return gabi::at<be<u32>>(0x101F8B18); }
inline be<u32>* buoyflag_gpuHeap() { return gabi::at<be<u32>>(0x101F8B4C); }
inline be<u32>* buoyflag_resourceManager() { return gabi::at<be<u32>>(0x101F4F7C); }
inline be<u32>* buoyflag_modelSystem() { return gabi::at<be<u32>>(0x104B4708); }

extern be<u32> (&buoyflag_attributes)[3];
// HD: these callbacks live in another source; ext bindings have no HostFn registry entry.
inline constexpr u32 buoyflag_destroyPoleVertexCallback = 0x0232DCF4;
inline constexpr u32 buoyflag_destroyFlagVertexCallback = 0x0232DD54;

namespace daObjBuoyflag {

struct Act_c;
enum Type_e { Type_UNK0_e, Type_UNK1_e, Type_UNK2_e, Type_UNK3_e, Type_Max };
enum Texture_e { Texture_00_e, Texture_01_e };
// HD: the fourth buoy type is absent from the GameCube header.
enum BuoyResourceIndex { dRes_INDEX_CLOTH_BTI_TOON_e = 3 };

/* the cloth: 5 rows (j) x 7 columns (i) */
enum { ROWS = 5, COLS = 7, VTX = ROWS * COLS };

struct ClothBuf {
    /* 0x000 */ cXyz mPos[ROWS][COLS];
    /* 0x1A4 */ cXyz mNrm[ROWS][COLS];     /* front normals (init: cXyz::BaseZ) */
    /* 0x348 */ cXyz mNrmBack[ROWS][COLS]; /* back normals = -front (init: (0, 0, -1)) */
};
WWHD_SIZE(ClothBuf, 0x4EC);

struct Packet_c {
    /* 0x000 */ u8 _000[0xC];   /* J3DPacket (HD, constructor 027F1278) */
    /* 0x00C */ be<u32> __vtbl; /* 10026860 */
    /* 0x010 */ u8 _010[0x18 - 0x10];
    /* 0x018 */ gptr<Act_c> mpActor;
    /* 0x01C */ u8 _01C[0x98 - 0x1C];
    /* 0x098 */ gptr<dKy_tevstr_c> mpTevStr; /* set by update() */
    /* 0x09C */ ClothBuf mBuf[2];
    /* 0xA74 */ cXyz mSpd[ROWS][COLS];
    /* 0xC18 */ be<s32> mCurBuf;
    /* 0xC1C */ Mtx34 mMtxHasi;     /* pole: setup matrix * actor scale */
    /* 0xC4C */ Mtx34 mMtxHata;     /* flag: ... * trans(0, 60, 0) */
    /* 0xC7C */ Mtx34 mViewMtxHasi; /* view * mMtxHasi (update) */
    /* 0xCAC */ Mtx34 mViewMtxHata; /* view * mMtxHata */
    /* 0xCDC */ cXyz mUp;           /* BaseY in flag space */
    /* 0xCE8 */ cXyz mWind;         /* wind in flag space */
    /* 0xCF4 */ be<s16> mAng[12];
    /* 0xD0C */ cXyz mSpring; /* force accumulator (calc_pos) */
    // HD: independent flag and pole materials, plus a shared toon texture.
    BuoyFlagGpuState flagGpu;
    BuoyPoleGpuState poleGpu;
    BuoyTextureResource toonResource;
    BuoyTexture toonTexture;
    be<u8> textureLoaded;
    u8 padding2C61[3];

    void draw_hata(Act_c*);
    void draw_hasi(Act_c*);
    void draw(Act_c*);
    void init(Act_c*);
    void calc_wind_base(Act_c*);
    void calc_pos_spring_near(const cXyz*, const cXyz*, f32, f32);
    void calc_pos(Act_c*);
    void calc_nrm();
    void calc(Act_c*);
    void update(Act_c*);
    /* HD-only */
    void load_texture();
    void update_hata();
    void update_hasi();
    void gpu_init_hata();
    void gpu_init_hasi();
};
WWHD_OFFSET(Packet_c, mBuf, 0x9C);
WWHD_OFFSET(Packet_c, mSpd, 0xA74);
WWHD_OFFSET(Packet_c, mMtxHasi, 0xC1C);
WWHD_OFFSET(Packet_c, mAng, 0xCF4);
WWHD_OFFSET(Packet_c, mSpring, 0xD0C);
WWHD_SIZE(Packet_c, 0x2C64);
WWHD_OFFSET(Packet_c, flagGpu, 0xD18);
WWHD_OFFSET(Packet_c, poleGpu, 0x1F10);
WWHD_OFFSET(Packet_c, toonResource, 0x2A38);
WWHD_OFFSET(Packet_c, toonTexture, 0x2AC8);
WWHD_OFFSET(Packet_c, textureLoaded, 0x2C60);

/* L_attr_type[4] (0x100267F0): {f32 scale, u8 cc, u8 hata, u8, u8} */
struct Attr_c {
    /* 0x0 */ be<f32> mScale;
    /* 0x4 */ be<u8> mCc;
    /* 0x5 */ be<u8> mHata;
    /* 0x6 */ u8 _6[2];
};

WWHD_SIZE(Attr_c, 8);
extern Attr_c (&L_attr_type)[4];

struct Act_c : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ dCcD_Stts mStts;
    /* 0x3F0 */ dCcD_Cyl mCyl;
    /* 0x520 */ Packet_c mPacket;
    /* 0x3184 */ union { Mtx34 m1090; Mtx34 mMtx; };   /* GameCube m1090 (setup) */
    /* 0x31B4 */ Mtx34 mOldMtx; /* last frame's */
    /* 0x31E4 */ union { Mtx34 m10F0; Mtx34 mJumpMtx; };   /* base rotation of mode_jumpToSea (GameCube m10F0) */
    /* 0x3214 */ be<u32> mType; /* M_type (GameCube m1120) */
    /* 0x3218 */ union { be<s32> m1124; be<s32> mMode; }; /* GameCube m1124 */
    /* 0x321C */ union { be<u8> m1128; be<u8> mbInit; };  /* GameCube m1128: mode_jumpToSea first frame */
    /* 0x321D */ u8 _321D;
    /* 0x321E */ be<s16> mRotAngle;
    /* 0x3220 */ be<f32> mRotSpeed;
    /* 0x3224 */ cXyz mRotAxis;

    BOOL mode_afl();
    BOOL mode_jumpToSea();
    void mtx_init();
};
WWHD_OFFSET(Act_c, mPacket, 0x520);
WWHD_OFFSET(Act_c, m1090, 0x3184);
// HD: existing Barrel2 consumers retain these byte-identical compatibility names.
WWHD_OFFSET(Act_c, mMtx, 0x3184);
WWHD_OFFSET(Act_c, mJumpMtx, 0x31E4);
WWHD_OFFSET(Act_c, mMode, 0x3218);
WWHD_OFFSET(Act_c, mbInit, 0x321C);
WWHD_OFFSET(Act_c, mType, 0x3214);
WWHD_OFFSET(Act_c, mRotAxis, 0x3224);
WWHD_SIZE(Act_c, 0x3230);

/* L_attr_type with the inline's JUT_ASSERT (each inline instance has its own literals) */
inline Attr_c* attr_type(Act_c* a, const char* file, const char* msg) {
    u32 t = a->mType;
    if (t >= Type_Max) {
        JUT_ASSERT_fail(file, 0xA5A, msg); /* (M_type >= 0) && (M_type < Type_Max) */
        t = a->mType;
    }
    return gabi::at<Attr_c>(gabi::ea(&L_attr_type) + sizeof(Attr_c) * t);
}

/* 0232EDC4 daObj::PrmAbstract<Act_c::Prm_e> */
u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift);

} // namespace daObjBuoyflag

using BuoyColorScratch = GXColor[4];
// HD: named guest bindings owned by this unit's pending source.
void buoyflag_vectorDivide(const cXyz*, cXyz*, float);
void buoyflag_vectorCross(const cXyz*, cXyz*, const cXyz*);
int buoyflag_vectorNormalizeRS(cXyz*);
void buoyflag_vectorNormalizeResult(cXyz*, cXyz*);
void buoyflag_matrixInverse(const Mtx34*, Mtx34*);
void buoyflag_matrixConcat(const Mtx34*, const Mtx34*, Mtx34*);
void buoyflag_rotateZXY(Mtx34*, short, short, short);
void buoyflag_quaternionFromAxis(unsigned int, const cXyz*, float);
void buoyflag_stackQuaternion(unsigned int);
void buoyflag_setCullSphere(fopAc_ac_c*, float, float, float, float);
void buoyflag_setCylinder(cM3dGCyl*, const cXyz*, float, float);
void buoyflag_getAllWind(cXyz*, cXyz*);
void buoyflag_call_hasi_nrm_init();
void buoyflag_clearStorage(unsigned int, unsigned int);
void buoyflag_construct_array(unsigned int, unsigned int, unsigned int, unsigned int);
void buoyflag_constructPacket(daObjBuoyflag::Packet_c*);
void buoyflag_constructIndexBuffer(unsigned int);
void buoyflag_constructMaterialList(unsigned int);
void buoyflag_constructUniformObject(unsigned int);
void buoyflag_constructTextureResource(unsigned int);
void buoyflag_constructTexture(unsigned int);
void buoyflag_destroyShader(unsigned int, int);
void buoyflag_destroyVertexBuffer(unsigned int, int);
void buoyflag_constructVertexBuffer(unsigned int);
void buoyflag_constructShader(unsigned int);
void buoyflag_rebuildTexture(unsigned int, unsigned int);
void buoyflag_lockTextureManager(unsigned int);
void buoyflag_loadTexture(unsigned int, unsigned int);
void buoyflag_unlockTextureManager(unsigned int);
void buoyflag_updateLighting(unsigned int);
void buoyflag_calculateLightingColors(unsigned int);
void buoyflag_setMaterialView(unsigned int, int, Mtx34*, unsigned int, unsigned int);
void buoyflag_scaleMaterialColor(BuoyColorScratch*, BuoyColorScratch*, float);
void buoyflag_flushDrawMaterial(unsigned int, int);
void buoyflag_scaleUniformColor(unsigned int, float);
void buoyflag_flushUniformObject(unsigned int);
void buoyflag_flushVertexBuffer(unsigned int, int, unsigned int);
void buoyflag_entryImm(unsigned int, daObjBuoyflag::Packet_c*, int);
unsigned int buoyflag_getActiveShader(unsigned int);
void buoyflag_updateShaderProgram(unsigned int, int);
void buoyflag_bindShaderGroup(unsigned int);
void buoyflag_GX2CallDisplayList(unsigned int, unsigned int);
void buoyflag_bindShaderProgram(unsigned int);
void buoyflag_GX2SetPixelUniformBlock(short, unsigned int, unsigned int);
void buoyflag_GX2SetVertexUniformBlock(short, unsigned int, unsigned int);
void buoyflag_GX2SetGeometryUniformBlock(short, unsigned int, unsigned int);
void buoyflag_bindTexture(unsigned int, unsigned int, int, int);
void buoyflag_configureRenderState(int, unsigned int);
void buoyflag_applyRenderState(unsigned int);
void buoyflag_bindShader(unsigned int);
void buoyflag_GX2DrawIndexedEx(unsigned int, unsigned int, unsigned int, unsigned int, int, int);
void buoyflag_initializeRenderState(unsigned int);
void buoyflag_applyExtraMaterial(unsigned int, unsigned int);
unsigned int buoyflag_getCurrentHeap(unsigned int);
unsigned int buoyflag_findOwningHeap(unsigned int, unsigned int);
unsigned int buoyflag_getShaderArchives(unsigned int);
int buoyflag_findShaderArchive(unsigned int, SafeString*);
void buoyflag_initializeShaderArchive(unsigned int, unsigned int, int);
void buoyflag_createShaderCollections(unsigned int, unsigned int, int);
void buoyflag_initializeVertexBuffer(unsigned int, unsigned int, int, unsigned int);
void buoyflag_initializeShader(unsigned int, unsigned int, unsigned int, unsigned int, int);
void buoyflag_createDrawMaterials(unsigned int, int, int);
void buoyflag_initializeIndexBuffer(unsigned int, unsigned int, int, unsigned int);
unsigned int buoyflag_getArchiveResource(unsigned int, SafeString*, SafeString*, int);
void buoyflag_loadNamedTexture(unsigned int, unsigned int, unsigned int);
void buoyflag_releaseShader(unsigned int);
void buoyflag_destroyUniformBinding(unsigned int);
void buoyflag_destroyTexture(unsigned int, int);
void buoyflag_releaseUniformObject(unsigned int, int);
void buoyflag_destroyMaterialList(unsigned int, int);
void buoyflag_destroyIndexBuffer(unsigned int, int);
void buoyflag_destroyArray(unsigned int, int, int, unsigned int, int, int);
void buoyflag_destroyMaterialPacket(unsigned int, int);
void buoyflag_destroyActor(daObjBuoyflag::Act_c*, int);
void buoyflag_call_Packet_c_draw_hata(daObjBuoyflag::Packet_c*, daObjBuoyflag::Act_c*);
void buoyflag_call_Packet_c_draw_hasi(daObjBuoyflag::Packet_c*, daObjBuoyflag::Act_c*);
void buoyflag_call_Packet_c_draw(daObjBuoyflag::Packet_c*, daObjBuoyflag::Act_c*);
void buoyflag_call_Packet_c_calc_wind_base(daObjBuoyflag::Packet_c*, daObjBuoyflag::Act_c*);
void buoyflag_call_Packet_c_calc_pos_spring_near(daObjBuoyflag::Packet_c*, const cXyz*, const cXyz*, float,
                                                 float);
void buoyflag_call_Packet_c_calc_pos(daObjBuoyflag::Packet_c*, daObjBuoyflag::Act_c*);
void buoyflag_call_Packet_c_calc_nrm(daObjBuoyflag::Packet_c*);
void buoyflag_call_Packet_c_calc(daObjBuoyflag::Packet_c*, daObjBuoyflag::Act_c*);
void buoyflag_call_Packet_c_init(daObjBuoyflag::Packet_c*, daObjBuoyflag::Act_c*);
void buoyflag_call_Packet_c_load_texture(daObjBuoyflag::Packet_c*);
void buoyflag_call_Packet_c_update_hata(daObjBuoyflag::Packet_c*);
void buoyflag_call_Packet_c_update_hasi(daObjBuoyflag::Packet_c*);
void buoyflag_call_Packet_c_update(daObjBuoyflag::Packet_c*, daObjBuoyflag::Act_c*);
void buoyflag_call_Packet_c_gpu_init_hata(daObjBuoyflag::Packet_c*);
void buoyflag_call_Packet_c_gpu_init_hasi(daObjBuoyflag::Packet_c*);
void buoyflag_call_Act_c_mtx_init(daObjBuoyflag::Act_c*);
int buoyflag_call_Act_c_mode_afl(daObjBuoyflag::Act_c*);
int buoyflag_call_Act_c_mode_jumpToSea(daObjBuoyflag::Act_c*);
unsigned int buoyflag_call_PrmAbstract(fopAc_ac_c*, int, int);

/* ---- local vector/SDK bindings ---- */
#define cXyz_BaseY gabi::at<cXyz>(0x101FFBC0)
#define cXyz_BaseZ gabi::at<cXyz>(0x101FFBCC)
static inline void cXyz_dv(const cXyz* a, cXyz* res, f32 s) { buoyflag_vectorDivide(a, res, s); }
static inline void cXyz_outprod(const cXyz* a, cXyz* res, const cXyz* b) { buoyflag_vectorCross(a, res, b); }
static inline BOOL cXyz_normalizeRS(cXyz* a) { return buoyflag_vectorNormalizeRS(a); }
static inline void cXyz_normalize(cXyz* a, cXyz* res) { buoyflag_vectorNormalizeResult(a, res); }
static inline void PSMTXInverse(const Mtx34* a, Mtx34* b) { buoyflag_matrixInverse(a, b); }
static inline void buoyflag_concatMatrix(const Mtx34* a, const Mtx34* b, Mtx34* ab) {
    buoyflag_matrixConcat(a, b, ab);
}
static inline void mDoMtx_ZXYrotS(Mtx34* m, s16 x, s16 y, s16 z) { buoyflag_rotateZXY(m, x, y, z); }
static inline void C_QUATRotAxisRad(u32 q, const cXyz* axis, f32 rad) {
    buoyflag_quaternionFromAxis(q, axis, rad);
}
static inline void mDoMtx_stack_quatM(u32 q) { buoyflag_stackQuaternion(q); }
static inline u32 mDoMtx_quatStack_get() { return static_cast<u32>(*gabi::at<be<u32>>(0x1048D3FC)); }
static inline void fopAcM_setCullSizeSphere(fopAc_ac_c* a, f32 x, f32 y, f32 z, f32 r) {
    buoyflag_setCullSphere(a, x, y, z, r);
}
/* 0201864C cM3dGCyl::Set(const cXyz& center, f32 r, f32 h) */
static inline void cM3dGCyl_Set(cM3dGCyl* c, const cXyz* center, f32 r, f32 h) {
    buoyflag_setCylinder(c, center, r, h);
}
/* 0257E34C dKyw_get_AllWind_vecpow(cXyz* pos): cXyz through a hidden result pointer */
static inline void dKyw_get_AllWind_vecpow(cXyz* res, cXyz* pos) { buoyflag_getAllWind(res, pos); }
/* bits of a float as an lfs/stfs pair (or an FPR round trip) leaves them: SNaN quieted */
static inline u32 fbits(u32 a) {
    f32 f = static_cast<f32>(*gabi::at<be<f32>>(a));
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
static inline u32 fbits_of(f32 f) {
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
static inline void copy3_u32(u32 dst, u32 src) {
    auto* d = buoyflag_view<BuoyVectorWords>(dst);
    auto* s = buoyflag_view<BuoyVectorWords>(src);
    d->x = static_cast<u32>(s->x);
    d->y = static_cast<u32>(s->y);
    d->z = static_cast<u32>(s->z);
}

WWHD_SIZE(BuoyStripVertex, 3);
WWHD_SIZE(BuoyTexCoord, 8);
WWHD_SIZE(BuoySafeString, 8);
WWHD_SIZE(BuoyIndexBuffer, 0x18);
WWHD_SIZE(BuoyShaderCollection, 0x14);
WWHD_SIZE(BuoyShaderLocations, 0x12);
WWHD_OFFSET(BuoyShaderLocations, vertex, 0xC);
WWHD_OFFSET(BuoyShaderLocations, pixel, 0xE);
WWHD_OFFSET(BuoyShaderLocations, geometry, 0x10);
WWHD_OFFSET(BuoyMaterialPool, current, 0x950);
WWHD_OFFSET(BuoyPoleMaterialPool, current, 0x4A8);
WWHD_OFFSET(BuoyTextureView, flags, 0x190);
WWHD_OFFSET(BuoyGpuMaterial, shaderObject, 0x158);
WWHD_OFFSET(BuoyGpuMaterial, allocation, 0x250);
