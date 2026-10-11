// Cloth simulation follows the GameCube classes and fields.
// HD: the packet base is 0x98, texture objects and GX2 material/geometry state enlarge
// dCloth_packet_c from 0xFC to 0xA40. Verified argument widths and double distances remain.
#pragma once
#include "bindings.h"
#include "d/d_drawlist.h"
#include "gc_api/d/d_com_inf_game.h"
struct dCloth_packet_c;

namespace cloth {
struct Vector {
    union {
        be<u32> mWords[3];
        be<f32> mValues[3];
        struct {
            be<f32> x, y, z;
        };
    };
};

// HD: GX2 replaces the GameCube GX texture/TEV display-list state.
struct PacketVtable {
    u8 mPrefix[0x34];
    gfn mInit;
    u8 mGapInit[0x14];
    gfn mTexObjInit;
    u8 mGapTex[12];
    gfn mTexObjLoad;
    u8 mGapLoad[4];
    gfn mLoadMaterial;
};
struct PacketBase {
    u8 mPrefix[12];
    gptr<PacketVtable> mpVtable;
    u8 mState[0x88];
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
struct UniformBlock {
    be<u32> mState, mpUniform, mFlags;
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
    u8 mTail[0x198 - 0xE0];
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
struct HeaderStatics {
    be<u32> mWords[4];
};
struct SinCosTable {
    be<f32> mValues[8192][2];
};
struct DestructorRecord {
    u8 mBytes[12];
};

struct SimulationOwner {
    be<u32> cloth_counter;
    gptr<dCloth_packet_c> top_pointer;
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
struct NormalScratch {
    Vector mTemp, mTotal, mPosition, mUnused, mCrossCopy, mHorizontalDifference, mUpLeftDifference,
        mDownLeftDifference, mUpRightDifference, mDownRightDifference, mNormal, mLeftDifference,
        mRightDifference;
};
struct FogUniform {
    u8 mPrefix[0x1C4];
    be<u32> mColors[2][4];
};
struct LightObject {
    be<f32> mPositionAndDirection[6];
    be<u8> mColor[4];
    be<u16> mColorS10[4];
    be<f32> mFactors[8];
    u8 mTail[0x40];
};
struct ResourceImage {
    u8 mPrefix[0x20];
    be<u32> mSurface;
};

struct MaterialInstance {
    u8 mPrefix[0x20];
    be<u8> mReady;
    u8 mTail[3];
};
struct MaterialBinding {
    u8 mResource[132];
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
    be<u32> mpBuffer;
    u8 mPadding[4];
    be<u32> mSize;
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
struct UniformObject {
    u8 mPrefix[12];
    be<u32> mpVtable;
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
struct Vertex {
    union {
        be<f32> mPositionAndNormal[6];
        struct {
            be<f32> mPosition[3], mNormal[3];
        };
    };
    u8 mAttributes[0x20];
    be<f32> mTexCoord[2];
    u8 mTail[0x58];
};
struct VertexPart {
    be<u32> mVertexCount, mpVertices;
    u8 mResource[0x154];
    Shape mBuffer;
};
struct VertexPool {
    be<u32> mCount, mpParts;
};
struct Geometry {
    VertexPool mPools[2];
    be<u32> mRows, mFlags;
    be<u8> mState, mReady;
    u8 mPadding[2];
    be<u32> mPoolIndex;
};
struct HeapVtable {
    u8 mPrefix[0x3C];
    gfn mFree;
};
struct Heap {
    u8 mPrefix[12];
    be<u32> mpVtable;
};
struct StageVtable {
    u8 mPrefix[0x15C];
    gfn mGetStagInfo;
};
struct Stage {
    be<u32> mpVtable;
};
struct StageInfo {
    u8 mPrefix[12];
    be<u32> mParameters;
};
struct SuppliedPositions {
    be<u32> mPositions[2];
};
struct TexturePrefix {
    be<u32> mWords[15];
};
struct SafeString {
    be<u32> mVtable, mpText;
};
} // namespace cloth
struct dCloth_packet_c {
    /* 0x00 */ cloth::PacketBase mPacket;
    /* 0x98 */ be<u32> mFlyGridSize, mHoistGridSize;
    /* 0xA0 */ be<f32> mFlyLength, mHoistLength;
    /* 0xA8 */ be<u32> mpTevstr, mpFactorCheckCB;
    /* 0xB0 */ be<u32> mpPosArr[2], mpNrmArr[2], mpNrmArrBack[2];
    /* 0xC8 */ be<u32> mpSpeedArr;
    /* 0xCC */ cloth::Vector mGlobalWind, mScale;
    /* 0xE4 */ be<f32> mWindSpeed, mWindSpeedWave;
    /* 0xEC */ be<u32> field_0x64;
    /* 0xF0 */ u8 mTexObj[0x40], mToonTex[0x40];
    /* 0x170 */ Mtx34 mMtx;
    /* 0x1A0 */ be<f32> mSpring, mGravity, mDrag, mFlyFlex, mHoistFlex;
    /* 0x1B4 */ be<s16> mWave, mWaveSpeed, field_0xF0, field_0xF2, mRipple, mRotateY;
    /* 0x1C0 */ be<u8> mCurArr;
    u8 mPadding[3];
    /* 0x1C4 */ be<u32> mMaterialCount, mMaterialCapacity, mpMaterials, mpModel;
    /* 0x1D4 */ cloth::UniformBlock mUniformBlock;
    /*0x1E0*/ cloth::Transform mTransform;
    /*0x288*/ cloth::LightingTransform mLighting;
    /*0x5F0*/ cloth::Texture mTexture, mToonTexture;
    /*0x920*/ cloth::Surface mSurface, mToonSurface;
    void setMtx(cloth::MatrixWords* matrix);
    void cloth_draw();
    void TexObjLoad(cloth::GraphicsState* state);
    void setGlobalWind(cloth::Vector* wind);
    void TexObjInit(cloth::ResourceImage* image);
    void TevSetting();
    void init();
    void cloth_move();
    void getFactor(cloth::Vector* output, cloth::Vector* positions, cloth::Vector* normals,
                   cloth::Vector* wind, u32 x, u32 y, f64 horizontal, f64 vertical, f64 diagonal);
    void setNrm();
    void draw();
    void updateVertexBuffers();
    void drawMaterial(cloth::DrawInfo* state);
};
WWHD_SIZE(dCloth_packet_c, 0xA40);
WWHD_OFFSET(dCloth_packet_c, mMtx, 0x170);
WWHD_OFFSET(dCloth_packet_c, mCurArr, 0x1C0);
struct dCloth_packetXlu_c : dCloth_packet_c {
    be<u8> mAlpha;
    u8 mTail[3];
    void TevSetting(cloth::GraphicsState* state);
    void cloth_draw();
    void loadMaterial();
};
WWHD_SIZE(dCloth_packetXlu_c, 0xA44);
struct dClothVobj03_c : dCloth_packet_c {

    u8 mTlutObj[12];
    be<u8> mIsStandItem, mIsIndoors;
    u8 mTail[2];
    void cloth_copy();
    void init();
    void cloth_move();
};
WWHD_SIZE(dClothVobj03_c, 0xA50);
struct dClothVobj04_c : dCloth_packet_c {

    u8 mTlutObj[12];
    be<u8> mIsStandItem, mIsIndoors;
    u8 mTail[2];
    void cloth_copy();
    void init();
    void cloth_move();
};
WWHD_SIZE(dClothVobj04_c, 0xA50);
struct dClothVobj05_c : dCloth_packet_c {

    u8 mTlutObj[12];
    be<u8> mIsStandItem, mIsIndoors;
    u8 mTail[2];
    void cloth_copy();
    void init();
    void cloth_move();
};
WWHD_SIZE(dClothVobj05_c, 0xA50);
struct dClothVobj07_0_c : dCloth_packet_c {

    u8 mTlutObj[12];
    be<u8> mIsStandItem, mIsIndoors;
    u8 mTail[2];
    void cloth_copy();
    void init();
    void cloth_move();
};
WWHD_SIZE(dClothVobj07_0_c, 0xA50);

// HD: external SDK and verified-callee bindings, preserving each callsite's ABI.
void clothApi_subtractVector(u32, u32, u32);
void clothApi_scaleNormalByDotResult(u32, u32);
void clothApi_normalizeVector(u32, u32);
void clothApi_springFactor(u32, u32, u32, f64, f32);
void clothApi_setMatrix(u32, u32);
u32 clothApi_constructPacket(u32, u32, u32, u32, u32, u32, f32, f32);
u32 clothApi_constructXluPacket(u32, u32, u32, u32, u32, u32, f32, f32);
u32 clothApi_constructFlag03(u32, u32, u32, u32);
u32 clothApi_constructFlag04(u32, u32, u32, u32);
u32 clothApi_constructFlag05(u32, u32, u32, u32);
u32 clothApi_constructFlag07(u32, u32, u32, u32);
void clothApi_destroyPacket(u32, u32);
void clothApi_setNormals(u32);
void clothApi_initializePacket(u32);
void clothApi_getFactor(u32, u32, u32, u32, u32, u32, u32, f32, f32, f64);
void clothApi_movePacket(u32);
void clothApi_snapshotMatrix(u32, u32);
void clothApi_updateVertices(u32);
void clothApi_colorToFloat(u32, u32);
void clothApi_prepareDraw(u32);
void clothApi_copyFlag03(u32);
void clothApi_copyFlag04(u32);
void clothApi_copyFlag05(u32);
void clothApi_copyFlag07(u32);
void clothApi_entryZSortXluDrawList(u32, u32, u32, u32);
void clothApi_setFogTevstr(u32);
void clothApi_setLightTevstr(u32);
void clothApi_rotateMatrixY(u32, s32);
void clothApi_scaleMatrix(u32, f32, f32, f32);
u32 clothApi_allocate(u32);
void clothApi_deallocate(u32);
void clothApi_scaleColorInPlace(u32, f32);
void clothApi_scaleColor(u32, u32, f32);
void clothApi_lockGraphics(u32);
void clothApi_unlockGraphics(u32);
void clothApi_constructRenderState(u32);
void clothApi_applyRenderState(u32);
void clothApi_applyCullMode(u32);
u32 clothApi_findContainingHeap(u32, u32);
void clothApi_decodeTextureSurface(u32, u32);
void clothApi_destroyVertexResource(u32, u32);
s32 clothApi_findMaterial(u32, u32);
void clothApi_bindShaderProgram(u32);
void clothApi_bindShader(u32);
void clothApi_updateShader(u32, u32);
void clothApi_copyTextureSurface(u32, u32);
void clothApi_constructTexture(u32);
void clothApi_destroyTexture(u32, u32);
void clothApi_bindTexture(u32, u32, u32, u32);
void clothApi_constructSurfaceTransfer(u32);
void clothApi_releaseVertexBuffer(u32);
void clothApi_destroyVertexBuffer(u32, u32);
void clothApi_bindVertexBuffer(u32);
void clothApi_entryDrawBuffer(u32, u32, u32);
void clothApi_constructPacketBase(u32);
void clothApi_destroyPacketBase(u32, u32);
u32 clothApi_getShaderCache(u32);
void clothApi_constructMaterialBlock(u32);
void clothApi_destroyMaterialBlock(u32, u32);
void clothApi_constructUniformBlock(u32);
void clothApi_destroyUniformBlock(u32, u32);
void clothApi_setViewUniforms(u32, u32, u32, u32, u32);
void clothApi_allocateUniformBlock(u32, u32, u32);
void clothApi_submitUniformBlock(u32, u32);
u32 clothApi_constructGeometry(u32);
void clothApi_allocateGeometry(u32, u32, u32, u32);
void clothApi_destroyGeometry(u32, u32);
void clothApi_allocateVertexBuffers(u32, u32, u32, u32);
void clothApi_flushVertexBuffers(u32, u32, u32);
u32 clothApi_getMaterialManager();
void clothApi_bindDrawMaterial(u32, u32);
void clothApi_configureMaterialState(u32, u32);
void clothApi_initializeMaterialVertices(u32, u32, u32, u32);
void clothApi_loadMaterialInstances(u32, u32, u32);
void clothApi_initializeMaterial(u32, u32, u32);
void clothApi_addVector(u32, u32, u32);
f64 clothApi_squareMagnitude(u32);
void clothApi_scaleVector(u32, u32, f32);
f64 clothApi_dotVector(u32, u32);
void clothApi_transformVector(u32, u32, u32);
void clothApi_copyMatrix(u32, u32);
void clothApi_invertMatrix(u32, u32);
void clothApi_initializeHeaderByte(u32);
void clothApi_initializeHeaderFloat(u32);
u32 clothApi_constructArray(u32, u32, u32, u32);
void clothApi_registerDestructor(u32);
f64 clothApi_squareRootFloatInput(f32);
f64 clothApi_squareRoot(f64);
void clothApi_clearMemory(u32, u32);
void clothApi_bindDisplayList(u32, u32);
void clothApi_drawIndexed(u32, u32, u32, u32, u32, u32);
void clothApi_bindGeometryUniform(s32, u32, u32);
void clothApi_bindPixelUniform(s32, u32, u32);
void clothApi_bindVertexUniform(s32, u32, u32);
void clothApi_storeRangeNoSync(u32, u32);
void clothApi_copyMemory(u32, u32, u32);

WWHD_SIZE(cloth::PacketBase, 0x98);
WWHD_SIZE(cloth::Transform, 0xA8);
WWHD_SIZE(cloth::LightingTransform, 0x368);
WWHD_SIZE(cloth::Surface, 0x90);
WWHD_SIZE(cloth::Texture, 0x198);
WWHD_SIZE(cloth::GraphicsState, 0x120);
WWHD_OFFSET(dCloth_packet_c, mTexture, 0x5F0);
WWHD_OFFSET(dCloth_packet_c, mSurface, 0x920);

WWHD_SIZE(cloth::NormalScratch, 156);
WWHD_SIZE(cloth::MatrixWords, 48);
WWHD_SIZE(cloth::Vector, 12);

WWHD_SIZE(cloth::MaterialInstance, 36);
WWHD_SIZE(cloth::ShapeGroup, 20);
WWHD_SIZE(cloth::Shape, 244);
WWHD_SIZE(cloth::VertexPart, 592);
WWHD_SIZE(cloth::Vertex, 152);
WWHD_OFFSET(cloth::Vertex, mTexCoord, 0x38);

void clothApi_crossVector(u32, u32, u32);

WWHD_SIZE(cloth::SafeString, 8);
WWHD_SIZE(cloth::Vector4, 16);

// HD: constructor callback for the GX2 color/vector arrays.
u32 clothVector4__ct(cloth::Vector4*);
