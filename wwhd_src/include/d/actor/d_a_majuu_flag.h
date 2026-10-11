#pragma once
#include "f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
namespace majuu {
struct Vector {
    union {
        be<u32> mWords[3];
        be<f32> mValues[3];
        struct {
            be<f32> x, y, z;
        };
    };
};
struct Vector4 {
    union {
        be<f32> mValues[4];
        be<u32> mWords[4];
    };
};
struct UniformVtable {
    u8 mPrefix[0x2C];
    gfn mBind;
};
struct Transform {
    u8 mPrefix[12];
    gptr<UniformVtable> mpVtable;
    u8 mState[0x64];
    Mtx34 mMatrix;
    be<u32> mFlags;
};
struct LightingTransform {
    u8 mPrefix[12];
    gptr<UniformVtable> mpVtable;
    u8 mState[0x64];
    Vector4 mColors[11];
    Vector4 mVectors[3][2];
    Mtx34 mMatrices[8];
    Vector4 mTailVectors[6];
    be<u8> mEnabled;
    u8 mPadding[3];
};

struct Surface {
    u8 mPrefix[4];
    be<u32> mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat, mAntialias;
    u8 mAllocation[8];
    be<u32> mImage, mMipmapSize, mMipmap, mTileMode, mSwizzle;
    u8 mTail[0x90 - 0x3C];
};
struct Texture {
    Surface mSurface;
    u8 mState[0x44];
    be<u32> mCachedImage;
    u8 mGap[4];
    be<u32> mCachedMipmap;
    u8 mBeforeFilters[0x15C - 0xE0];
    be<u32> mMinFilter, mMagFilter, mMipFilter;
    u8 mBeforeFlags[0x190 - 0x168];
    be<u8> mFlags;
    u8 mTail[7];
};
struct GraphicsState {
    be<u32> mPrefix[2];
    be<u32> mCullMode, mBlendMode;
    u8 mState[0xD0];
    be<u8> mDepthEnabled;
    u8 mPadding[3];
    be<u32> mDepthFunction;
    be<f32> mDepthValue;
    be<u32> mStencilBits;
    u8 mTail[0x30];
};
struct MatrixWords {
    union {
        be<f32> mValues[12];
        be<u32> mWords[12];
    };
};
struct ColorS10 {
    be<s16> mValues[4];
};

struct MaterialInstance {
    u8 mPrefix[0x20];
    be<u8> mReady;
    u8 mTail[3];
};

struct MaterialResource {
    u8 mPrefix[28];
    be<u32> mCount, mpBindings;
};
struct MaterialManager {
    u8 mPrefix[4];
    be<u32> mpResource, mCount, mpInstances;
};
struct ShaderProgramState {
    u8 mPrefix[0x28];
    be<u32> mState;
};
struct ShaderProgram {
    be<u8> mFlags;
    u8 mPadding[3];
    be<u32> mpBinary;
    u8 mGap[4];
    be<u32> mBinarySize;
    u8 mState[0x6C];
    be<u32> mpResource;
};
struct UniformSlots {
    u8 mPrefix[12];
    be<s16> mVertex, mPixel, mGeometry;
};
struct ShaderMaterial {
    be<u32> mpProgram;
    u8 mPrefix[8];
    be<u32> mUniformCount, mpUniforms, mSamplerCount, mpSamplers;
};
struct ShaderCache {
    be<u32> mState, mpProgram;
};
struct UniformRecord {
    u8 mPrefix[4];
    be<u32> mSize;
    u8 mPadding[4];
    be<u32> mpBuffer;
    u8 mTail[12];
};
struct UniformData {
    u8 mPrefix[16];
    UniformRecord mRecords[2];
    u8 mPadding[4];
    be<u32> mRecordIndex;
};
struct DrawObject {
    u8 mPrefix[4];
    be<u32> mpUniformData;
};
struct DrawInfo {
    u8 mPrefix[12];
    be<u32> mPass;
    u8 mPadding[4];
    be<u32> mpDrawObject;
    u8 mTail[0x18];
    be<u32> mpObject;
};

struct ShapeVtable {
    u8 mPrefix[12];
    gfn mDestroy;
};
struct Shape {
    u8 mState[0xF0];
    be<u32> mpVtable;
};
struct ShapeArray {
    be<u32> mCount, mpShapes;
};
struct ShapeGroup {
    be<u32> mpMaterial;
    ShapeArray mShapes[2];
};
struct HeapVtable {
    u8 mPrefix[0x34];
    gfn mAllocate;
    u8 mGap[4];
    gfn mFree;
};
struct Heap {
    u8 mPrefix[12];
    be<u32> mpVtable;
};
struct ResourceImage {
    u8 mPrefix[0x20];
    be<u32> mpSurface;
};
struct ColorU8 {
    be<u8> mValues[4];
};
struct UniformObjectVtable {
    u8 mPrefix[0x2C];
    gfn mBind;
};
struct UniformObjectRoot {
    u8 mPrefix[12];
    be<u32> mpVtable;
};
struct DrawState {
    be<u32> mPrefix[2];
    be<u32> mCullMode, mBlendMode;
    u8 mState[0xD0];
    be<u8> mDepthEnabled;
    u8 mPadding[3];
    be<u32> mDepthFunction;
    be<f32> mDepthValue;
    be<u32> mStencilBits;
    u8 mTail[0x2C];
};
struct IndexBuffer {
    be<u32> mIndexType, mPrimitive, mpIndices, mCount;
    u8 mTail[8];
};
struct Triangle {
    be<u8> mPosition, mNormal, mTexCoord;
};
struct TexCoord {
    be<f32> u, v;
};
struct NormalNeighbors {
    be<s32> mIndices[7];
};
struct SpringNeighbors {
    be<s32> mIndices[6];
};
struct CacheLine {
    be<u32> mWords[8];
};
struct ResourceName {
    be<u32> mpText, mpVtable;
};
struct Light {
    u8 mPrefix[0x24];
    be<f32> mColorScale, mLightScale;
    u8 mGap[0x73];
    be<u8> mAlpha;
};
struct ExtraLight {
    u8 mPrefix[0x16C];
    be<f32> mScale;
};
struct InitialPositionsTable {
    majuu::Vector mValues[21];
};
struct TrianglesTable {
    majuu::Triangle mValues[35];
};
struct TexCoordTable {
    majuu::TexCoord mValues[21];
};
struct IndicesTable {
    be<u8> mValues[140];
};
struct NormalNeighborsTable {
    majuu::NormalNeighbors mValues[21];
};
struct SpringNeighborsTable {
    majuu::SpringNeighbors mValues[21];
};
struct TexturePrefix {
    be<u32> mWords[15];
};

struct LightUniforms {
    u8 mPrefix[0x1C4];
    be<u32> mColors[2][4];
};
struct Vertex {
    union {
        struct {
            Vector mPosition, mNormal;
            be<f32> mTexCoord[2];
        };
        be<f32> mValues[8];
        be<u32> mWords[8];
    };
};
struct SinCosTable {
    be<f32> mValues[8192][2];
};
struct HeaderStatics {
    be<u32> mWords[4];
};
struct DestructorRecord {
    u8 mBytes[12];
};
struct HIO {
    be<u32> mpVtable;
    be<u8> mbUseToonTex;
    be<s8> m05, m06, m07, m08;
    u8 mPad[3];
    be<s32> mWaveSpeed;
    be<f32> mWindSpeed1, mWindSpeed2, mGravity, mSpring, mFlagScale;
    be<u8> mbHasCustomRotation;
    u8 mPadRotation[3];
    Vector mCustomRotation;
    union {
        be<s32> mRotX;
        be<s16> mRotXHalves[2];
    };
    be<f32> mTransZ;
};
} // namespace majuu
// HD: packet 0x1A30 vs GC0x79C; actor 0x1E3C vs GC0xAA0.
// Engine-owned GX2 objects remain opaque; guest pointers and actor-owned arrays
// expose the actual HD layout without host pointers or compiler padding.
struct daMajuuFlagUniformBuffer_c {
    be<u32> mpData;
    struct {
        u8 mPrefix[0x14C];
        be<u32> mSize;
        u8 mTail[4];
    } mUniformObject;
    majuu::Shape mAttributeObject;
    be<u32> mElementCount, mpAllocation;
};
struct daMajuuFlagUniformPairs_c {
    daMajuuFlagUniformBuffer_c mBuffers[4];
    be<u32> mCurrent, mFormat, mStride, mReserved, mFlags;
    be<u8> mReady;
    u8 padding[3];
};
struct daMajuu_Flag_packet_c {
    void setupMaterial();
    void draw(void* context);
    void updateUniforms();
    void setNrmVtx(cXyz* output, s32 vertex);
    void setBackNrm();
    void setNrmMtx();
    u8 mPacketPrefix[12];
    be<u32> mpVtable;
    u8 mPacketState[8];
    be<u32> mpActor;
    u8 mPacketTail[0x7C];
    be<u32> mProgramCount, mMaterialCount, mpMaterials;
    daMajuuFlagUniformPairs_c mUniforms;
    majuu::IndexBuffer mIndexBuffer;
    be<u32> mTransformCount, mpTransforms, mReservedUniform;
    majuu::Transform mTransform;
    majuu::LightingTransform mLighting;
    majuu::Surface mFlagSurface, mToonSurface;
    majuu::Texture mFlagTex, mToonTex;
    majuu::MatrixWords mPosMtx;
    be<u32> mpTevStr;
    u8 reserved12C4[0x80];
    majuu::Vector mpPosArr[2][21], mpNrmArr[2][21], mpNrmArrBack[2][21];
    majuu::Vector mSpeed[21];
    be<s16> mRotateY, m796, m798;
    be<u8> mCurArr, m79B;
};
static_assert(sizeof(daMajuuFlagUniformBuffer_c) == 0x254);
static_assert(sizeof(daMajuuFlagUniformPairs_c) == 0x968);
WWHD_OFFSET(daMajuu_Flag_packet_c, mProgramCount, 0x98);
WWHD_OFFSET(daMajuu_Flag_packet_c, mUniforms, 0xA4);
WWHD_OFFSET(daMajuu_Flag_packet_c, mTransformCount, 0xA24);
WWHD_OFFSET(daMajuu_Flag_packet_c, mTransform.mMatrix, 0xAA4);
WWHD_OFFSET(daMajuu_Flag_packet_c, mLighting, 0xAD8);
WWHD_OFFSET(daMajuu_Flag_packet_c, mFlagSurface, 0xE40);
WWHD_OFFSET(daMajuu_Flag_packet_c, mFlagTex, 0xF60);
WWHD_OFFSET(daMajuu_Flag_packet_c, mPosMtx, 0x1290);
WWHD_OFFSET(daMajuu_Flag_packet_c, mpPosArr, 0x1344);
WWHD_OFFSET(daMajuu_Flag_packet_c, mpNrmArr, 0x153C);
WWHD_OFFSET(daMajuu_Flag_packet_c, mSpeed, 0x192C);
WWHD_OFFSET(daMajuu_Flag_packet_c, mCurArr, 0x1A2E);
struct daMajuu_Flag_c : fopAc_ac_c {
    daMajuu_Flag_packet_c mPacket;
    u8 mPhsCloth[8], mPhsFlag[8];
    u8 mPhasePadding[8];
    be<s16> mWave;
    u8 mWavePadding[6];
    be<f32> mFlagScale;
    be<u8> mFlagType, mTexType, mbUsePlayerTevStr;
    u8 pad1e03;
    majuu::MatrixWords mMtx;
    be<u32> mpParentMtx, mpParentPos;
};
static_assert(sizeof(daMajuu_Flag_packet_c) == 0x1A30);
WWHD_OFFSET(daMajuu_Flag_c, mPacket, 0x3AC);
WWHD_OFFSET(daMajuu_Flag_c, mPhsCloth, 0x1DDC);
WWHD_OFFSET(daMajuu_Flag_c, mPhsFlag, 0x1DE4);
WWHD_OFFSET(daMajuu_Flag_c, mFlagScale, 0x1DFC);
WWHD_OFFSET(daMajuu_Flag_c, mMtx, 0x1E04);
static_assert(sizeof(daMajuu_Flag_c) == 0x1E3C);

// HD: typed external SDK bindings retain the verified callsite ABI.
void majuuApi_rotateMatrixY(void*, s16);
void majuuApi_PSVECSubtract(void*, void*, void*);
unsigned int majuuApi_allocate(int);
void majuuApi_constructVertexBuffer(void*);
void majuuApi_constructAttribute(void*);
void majuuApi_deallocate(void*);
void majuuApi_destroyVertexBuffer(void*, int);
void majuuApi_destroyVertexResource(void*, int);
void majuuApi_PSMTXIdentity(void*);
void majuuApi_PSMTXConcat(void*, void*, void*);
unsigned int majuuApi_getEnvironment();
void majuuApi_dScnKy_env_light_c_settingTevStruct(void*, int, void*, void*);
unsigned int majuuApi_getPlay();
void majuuApi_entryDrawBuffer(void*, void*, int);
void majuuApi_updatePacketUniforms(void*);
void majuuApi_dComIfG_resDelete(void*, void*);
unsigned int majuuApi_dKyw_get_wind_vec();
int majuuApi_cM_atan2s(f32, f32);
void majuuApi_copyMatrix(void*, void*);
void majuuApi_PSMTXMultVecSR(void*, void*, void*);
void majuuApi_cLib_addCalcAngleS2(void*, s32, int, int);
void majuuApi_moveFlagCloth(void*);
void majuuApi_set_mtx(void*);
void majuuApi_mDoMtx_stack_c_transM(f32, f32, f32);
void majuuApi_mDoMtx_YrotM(void*, s16);
void majuuApi_scaleMatrix(f32, f32, f32);
void majuuApi_transformVector(void*, void*, void*);
void majuuApi_MtxTrans(int, f32, f32, f32);
void majuuApi_mDoMtx_ZrotM(void*, s16);
void majuuApi_mDoMtx_XrotM(void*, s16);
void majuuApi_MtxScale(int, f32, f32, f32);
void majuuApi_registerDestructor(void*);
void majuuApi_initializeHeaderFloat(void*);
void majuuApi_initializeHeaderByte(void*);
void majuuApi_constructPacketBase(void*);
void majuuApi_constructArray(void*, int, int, int);
void majuuApi_constructIndexBuffer(void*);
void majuuApi_constructUniformBlock(void*);
void majuuApi_constructMaterialBlock(void*);
void majuuApi_clearMemory(void*, int);
void majuuApi_allocateDiscardResult(int);
void majuuApi_constructSurfaceTransfer(void*);
void majuuApi_constructTexture(void*);
unsigned int majuuApi_findContainingHeap(void*, void*);
void majuuApi_releaseVertexBuffer(void*);
void majuuApi_releaseMaterialAttribute(void*);
void majuuApi_destroyTexture(void*, int);
void majuuApi_destroyMaterialBlock(void*, int);
void majuuApi_destroyUniformBlock(void*, int);
void majuuApi_destroyIndexBuffer(void*, int);
void majuuApi_destroy_arr(void*, int, int, int, int, int);
void majuuApi_destroyPacketBase(void*, int);
void majuuApi_fopAc_ac_c_dtor(void*, int);
void majuuApi_subtractVector(void*, void*, void*);
void majuuApi_crossVector(void*, void*, void*);
void majuuApi_cXyz_normZC(void*, void*);
void majuuApi_addVector(void*, void*, void*);
void majuuApi_MtxPush();
void majuuApi_MtxPosition(void*, void*);
void majuuApi_MtxPull();
void majuuApi_flushVertexBuffer(void*, int, u32);
void majuuApi_setFogTevstr(void*);
void majuuApi_setLightTevstr(void*);
void majuuApi_copyMatrixSnapshot(void*, void*);
void majuuApi_setViewUniforms(void*, int, void*, void*, void*);
void majuuApi_colorS10ToFloat(void*, void*);
void majuuApi_scaleColor(void*, void*, f32);
void majuuApi_submitViewUniforms(void*, int);
void majuuApi_colorU8ToFloat(void*, void*);
void majuuApi_scaleColorInPlace(void*, f32);
void majuuApi_submitMaterialUniforms(void*);
void majuuApi_sampleWind();
float majuuApi_dKyw_get_wind_pow();
void majuuApi_scaleVector(void*, void*, f32);
void majuuApi_mDoMtx_ZrotS(void*, s16);
void majuuApi_dotVector(void*, void*);
void majuuApi_scaleNormalByDotResult(void*, void*);
float majuuApi_squareMagnitude(void*);
float majuuApi_squareRoot(f32);
void majuuApi_setNormalMatrix(void*);
void majuuApi_daMajuu_Flag_packet_c_setNrmVtx(void*, void*, s32);
void majuuApi_daMajuu_Flag_packet_c_setBackNrm(void*);
void majuuApi_fopAc_ac_c_fopAc_ac_c(void*);
void majuuApi_constructFlagPacket(void*);
int majuuApi_dComIfG_resLoad(void*, void*);
float majuuApi_cM_rndFX(f32);
void majuuApi_fopAcM_setCullSizeBox(void*, f32, f32, f32, f32, f32, f32);
void majuuApi_setupFlagPacket(void*);
unsigned int majuuApi_getShaderCache(void*);
void majuuApi_updateShader(void*, int);
void majuuApi_bindShaderProgram(void*);
void majuuApi_bindDisplayList(void*, u32);
void majuuApi_bindShader(void*);
void majuuApi_bindPixelUniform(s16, void*, u32);
void majuuApi_bindVertexUniform(s16, void*, u32);
void majuuApi_bindGeometryUniform(s16, void*, u32);
void majuuApi_drawIndexed(u32, u32, u32, void*, int, int);
void majuuApi_bindDrawMaterial(void*, void*);
void majuuApi_bindTexture(void*, void*, int, int);
void majuuApi_constructRenderState(void*);
void majuuApi_configureMaterialState(unsigned int, void*);
void majuuApi_applyRenderState(void*);
void majuuApi_bindVertexBuffer(void*);
void majuuApi_applyCullMode(void*);
unsigned int majuuApi_getCurrentHeap(void*);
void majuuApi_copyTextureSurface(void*, void*);
unsigned int majuuApi_getMaterialManager();
int majuuApi_findMaterial(void*, void*);
void majuuApi_initializeMaterial(void*, void*, int);
void majuuApi_loadMaterialInstances(void*, void*, int);
void majuuApi_configureVertexBuffer(void*, void*, int, void*);
void majuuApi_configureMaterialAttribute(void*, void*, void*, void*, int);
void majuuApi_allocateUniformBlock(void*, int, int);
void majuuApi_setIndexBuffer(void*, void*, int, u32);
void majuuApi_lockGraphics(void*);
unsigned int majuuApi_findResourceTexture(void*, void*, void*, int);
void majuuApi_decodeTextureSurfaceResource(void*, void*, void*);
unsigned int majuuApi_dRes_control_c_getRes(void*, void*, int);
void majuuApi_assertResource(void*, int, void*);
void majuuApi_decodeImageSurface(void*, void*);
void majuuApi_unlockGraphics(void*);

WWHD_OFFSET(daMajuu_Flag_c, mWave, 0x1DF4);
WWHD_OFFSET(daMajuu_Flag_packet_c, mpActor, 0x18);

WWHD_SIZE(majuu::HIO, 0x3C);
WWHD_SIZE(majuu::Transform, 0xA8);
WWHD_SIZE(majuu::LightingTransform, 0x368);
WWHD_SIZE(majuu::Surface, 0x90);
WWHD_SIZE(majuu::Texture, 0x198);
WWHD_SIZE(majuu::Vertex, 32);

WWHD_OFFSET(majuu::Light, mAlpha, 0x9F);

void* FlagUniformBuffer__ct(void*);
void* FlagVector4__ct(void*);
void FlagUniformBuffer__dt(void*, u32);

WWHD_SIZE(majuu::DrawState, 0x11C);

WWHD_SIZE(majuu::Vector, 0xC);
WWHD_SIZE(majuu::Vector4, 0x10);
WWHD_SIZE(majuu::UniformVtable, 0x30);
WWHD_SIZE(majuu::GraphicsState, 0x120);
WWHD_SIZE(majuu::MatrixWords, 0x30);
WWHD_SIZE(majuu::ColorS10, 0x8);
WWHD_SIZE(majuu::MaterialInstance, 0x24);
WWHD_SIZE(majuu::MaterialResource, 0x24);
WWHD_SIZE(majuu::MaterialManager, 0x10);
WWHD_SIZE(majuu::ShaderProgramState, 0x2C);
WWHD_SIZE(majuu::ShaderProgram, 0x80);
WWHD_SIZE(majuu::UniformSlots, 0x12);
WWHD_SIZE(majuu::ShaderMaterial, 0x1C);
WWHD_SIZE(majuu::ShaderCache, 0x8);
WWHD_SIZE(majuu::UniformRecord, 0x1C);
WWHD_SIZE(majuu::UniformData, 0x50);
WWHD_SIZE(majuu::DrawObject, 0x8);
WWHD_SIZE(majuu::DrawInfo, 0x34);
WWHD_SIZE(majuu::ShapeVtable, 0x10);
WWHD_SIZE(majuu::Shape, 0xF4);
WWHD_SIZE(majuu::ShapeArray, 0x8);
WWHD_SIZE(majuu::ShapeGroup, 0x14);
WWHD_SIZE(majuu::HeapVtable, 0x40);
WWHD_SIZE(majuu::Heap, 0x10);
WWHD_SIZE(majuu::IndexBuffer, 0x18);
WWHD_SIZE(majuu::Triangle, 0x3);
WWHD_SIZE(majuu::TexCoord, 0x8);
WWHD_SIZE(majuu::NormalNeighbors, 0x1C);
WWHD_SIZE(majuu::SpringNeighbors, 0x18);
WWHD_SIZE(majuu::CacheLine, 0x20);
WWHD_SIZE(majuu::ResourceName, 0x8);
WWHD_SIZE(majuu::Light, 0xA0);
WWHD_SIZE(majuu::ExtraLight, 0x170);
WWHD_SIZE(majuu::ResourceImage, 0x24);
WWHD_SIZE(majuu::ColorU8, 0x4);
WWHD_SIZE(majuu::UniformObjectVtable, 0x30);
WWHD_SIZE(majuu::UniformObjectRoot, 0x10);
WWHD_SIZE(majuu::TexturePrefix, 0x3C);
WWHD_SIZE(majuu::LightUniforms, 0x1E4);
WWHD_SIZE(majuu::SinCosTable, 0x10000);
WWHD_SIZE(majuu::HeaderStatics, 0x10);
WWHD_SIZE(majuu::DestructorRecord, 0xC);
WWHD_OFFSET(majuu::LightingTransform, mColors, 0x74);
WWHD_OFFSET(daMajuu_Flag_packet_c, mLighting, 0xAD8);
WWHD_OFFSET(daMajuu_Flag_packet_c, mToonTex, 0x10F8);
